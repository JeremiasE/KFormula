/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2007 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "kptview.h"

#include <kmessagebox.h>

#include "KoDocumentInfo.h"
#include <KoMainWindow.h>
#include <KoToolManager.h>
#include <KoDocumentChild.h>

#include <QApplication>
#include <QLabel>
#include <QString>
#include <QStringList>
#include <qsize.h>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPrinter>
#include <QPrintDialog>
#include <QProgressBar>

#include <kicon.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kmenu.h>
#include <kstandardaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <ktoolbar.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kdeversion.h>
#include <kstatusbar.h>
#include <kxmlguifactory.h>
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <k3command.h>
#include <ktoggleaction.h>
#include <ktemporaryfile.h>
#include <kfiledialog.h>
#include <kparts/event.h>
#include <kparts/partmanager.h>

#include <KoQueryTrader.h>
#include <KoTemplateCreateDia.h>

#include "kptviewbase.h"
#include "kptaccountsview.h"
#include "kptaccountseditor.h"
#include "kptcalendareditor.h"
#include "kptchartview.h"
#include "kptfactory.h"
#include "kptmilestoneprogressdialog.h"
#include "kptnode.h"
#include "kptpart.h"
#include "kptproject.h"
#include "kptmainprojectdialog.h"
#include "kpttask.h"
#include "kptsummarytaskdialog.h"
#include "kpttaskdialog.h"
#include "kpttaskprogressdialog.h"
#include "kptganttview.h"
#include "kpttaskeditor.h"
#include "kptdependencyeditor.h"
#include "kptperteditor.h"
#include "kptdatetime.h"
#include "kptcommand.h"
#include "kptrelation.h"
#include "kptrelationdialog.h"
#include "kptresourceappointmentsview.h"
#include "kptresourceeditor.h"
#include "kptscheduleeditor.h"
#include "kptresourcedialog.h"
#include "kptresource.h"
#include "kptstandardworktimedialog.h"
#include "kptconfigdialog.h"
#include "kptwbsdefinitiondialog.h"
#include "kptwpcontroldialog.h"
#include "kptresourceassignmentview.h"
#include "kpttaskstatusview.h"
#include "kptsplitterview.h"
#include "kptpertresult.h"

#include "kptviewlistdialog.h"
#include "kptviewlistdocker.h"
#include "kptviewlist.h"
#include "kptviewlistcommand.h"

#include "KPtViewAdaptor.h"

#include <assert.h>

namespace KPlato
{

//-------------------------------
View::View( Part* part, QWidget* parent )
        : KoView( part, parent ),
        m_currentEstimateType( Estimate::Use_Expected ),
        m_scheduleActionGroup( new QActionGroup( this ) ),
        m_readWrite( false )
{
    //kDebug();
//    getProject().setCurrentSchedule( Schedule::Expected );

    setComponentData( Factory::global() );
    if ( !part->isReadWrite() )
        setXMLFile( "kplato_readonly.rc" );
    else
        setXMLFile( "kplato.rc" );

    m_dbus = new ViewAdaptor( this );
    QDBusConnection::sessionBus().registerObject( '/' + objectName(), this );

    m_sp = new QSplitter( this );
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin(0);
    layout->addWidget( m_sp );

    if ( part->isEmbedded() || shell() == 0 ) {
        // Don't use docker if embedded
        m_viewlist = new ViewListWidget( part, m_sp );
    } else {
        ViewListDockerFactory vl(this);
        ViewListDocker *docker = dynamic_cast<ViewListDocker *>(createDockWidget(&vl));
        if (docker->view() != this) docker->setView(this);
        m_viewlist = docker->viewList();
    }
    m_tab = new QStackedWidget( m_sp );

////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // Add sub views
    createViews();
    // Add child documents
    createChildDocumentViews();

    connect( m_viewlist, SIGNAL( activated( ViewListItem*, ViewListItem* ) ), SLOT( slotViewActivated( ViewListItem*, ViewListItem* ) ) );
    connect( m_viewlist, SIGNAL( viewListItemRemoved( ViewListItem* ) ), SLOT( slotViewListItemRemoved( ViewListItem* ) ) );
    connect( m_viewlist, SIGNAL( viewListItemInserted( ViewListItem* ) ), SLOT( slotViewListItemInserted( ViewListItem* ) ) );

    connect( m_tab, SIGNAL( currentChanged( int ) ), this, SLOT( slotCurrentChanged( int ) ) );

    // The menu items
    // ------ File
    actionCreateTemplate = new KAction( i18n( "&Create Template From Document..." ), this );
    actionCollection()->addAction("file_createtemplate", actionCreateTemplate );
    connect( actionCreateTemplate, SIGNAL( triggered( bool ) ), SLOT( slotCreateTemplate() ) );
    
    // ------ Edit
    actionCut = actionCollection()->addAction(KStandardAction::Cut,  "edit_cut", this, SLOT( slotEditCut() ));
    actionCopy = actionCollection()->addAction(KStandardAction::Copy,  "edit_copy", this, SLOT( slotEditCopy() ));
    actionPaste = actionCollection()->addAction(KStandardAction::Paste,  "edit_paste", this, SLOT( slotEditPaste() ));

    // ------ View

    actionViewSelector  = new KToggleAction(i18n("Show Selector"), this);
    actionCollection()->addAction("view_show_selector", actionViewSelector );
    connect( actionViewSelector, SIGNAL( triggered( bool ) ), SLOT( slotViewSelector( bool ) ) );

    actionViewResourceAppointments  = new KToggleAction(i18n("Show allocations"), this);
    actionCollection()->addAction("view_resource_appointments", actionViewResourceAppointments );
    connect( actionViewResourceAppointments, SIGNAL( triggered( bool ) ), SLOT( slotViewResourceAppointments() ) );

    // ------ Insert

    // ------ Project
    actionEditMainProject  = new KAction(KIcon( "document-properties" ), i18n("Edit Main Project..."), this);
    actionCollection()->addAction("project_edit", actionEditMainProject );
    connect( actionEditMainProject, SIGNAL( triggered( bool ) ), SLOT( slotProjectEdit() ) );

    actionEditStandardWorktime  = new KAction(KIcon( "document-properties" ), i18n("Edit Standard Worktime..."), this);
    actionCollection()->addAction("project_worktime", actionEditStandardWorktime );
    connect( actionEditStandardWorktime, SIGNAL( triggered( bool ) ), SLOT( slotProjectWorktime() ) );


    // ------ Tools
    actionDefineWBS  = new KAction(KIcon( "configure" ), i18n("Define WBS Pattern..."), this);
    actionCollection()->addAction("tools_define_wbs", actionDefineWBS );
    connect( actionDefineWBS, SIGNAL( triggered( bool ) ), SLOT( slotDefineWBS() ) );

    // ------ Settings
    actionConfigure  = new KAction(KIcon( "configure" ), i18n("Configure KPlato..."), this);
    actionCollection()->addAction("configure", actionConfigure );
    connect( actionConfigure, SIGNAL( triggered( bool ) ), SLOT( slotConfigure() ) );

    // ------ Popup
    actionOpenNode  = new KAction(KIcon( "document-properties" ), i18n("Edit..."), this);
    actionCollection()->addAction("node_properties", actionOpenNode );
    connect( actionOpenNode, SIGNAL( triggered( bool ) ), SLOT( slotOpenNode() ) );
    actionTaskProgress  = new KAction(KIcon( "document-properties" ), i18n("Progress..."), this);
    actionCollection()->addAction("task_progress", actionTaskProgress );
    connect( actionTaskProgress, SIGNAL( triggered( bool ) ), SLOT( slotTaskProgress() ) );
    actionDeleteTask  = new KAction(KIcon( "edit-delete" ), i18n("Delete Task"), this);
    actionCollection()->addAction("delete_task", actionDeleteTask );
    connect( actionDeleteTask, SIGNAL( triggered( bool ) ), SLOT( slotDeleteTask() ) );
            
    actionIndentTask = new KAction(KIcon( "edit-indent" ), i18n("Indent Task"), this);
    actionCollection()->addAction("indent_task", actionIndentTask );
    connect( actionIndentTask, SIGNAL( triggered( bool ) ), SLOT( slotIndentTask() ) );
    actionUnindentTask= new KAction(KIcon( "edit-unindent" ), i18n("Unindent Task"), this);
    actionCollection()->addAction("unindent_task", actionUnindentTask );
    connect( actionUnindentTask, SIGNAL( triggered( bool ) ), SLOT( slotUnindentTask() ) );
    actionMoveTaskUp = new KAction(KIcon( "edit-up" ), i18n("Move Task Up"), this);
    actionCollection()->addAction("move_task_up", actionMoveTaskUp );
    connect( actionMoveTaskUp, SIGNAL( triggered( bool ) ), SLOT( slotMoveTaskUp() ) );
    actionMoveTaskDown = new KAction(KIcon( "edit-down" ), i18n("Move Task Down"), this);
    actionCollection()->addAction("move_task_down", actionMoveTaskDown );
    connect( actionMoveTaskDown, SIGNAL( triggered( bool ) ), SLOT( slotMoveTaskDown() ) );
    
    actionTaskWorkpackage  = new KAction(KIcon( "document-properties" ), i18n("Work Package Control..."), this);
    actionCollection()->addAction("task_workpackagecontrol", actionTaskWorkpackage );
    connect( actionTaskWorkpackage, SIGNAL( triggered( bool ) ), SLOT( slotTaskWorkpackage() ) );

    actionEditResource  = new KAction(KIcon( "document-properties" ), i18n("Edit Resource..."), this);
    actionCollection()->addAction("edit_resource", actionEditResource );
    connect( actionEditResource, SIGNAL( triggered( bool ) ), SLOT( slotEditResource() ) );

    actionEditRelation  = new KAction(KIcon( "document-properties" ), i18n("Edit Dependency..."), this);
    actionCollection()->addAction("edit_dependency", actionEditRelation );
    connect( actionEditRelation, SIGNAL( triggered( bool ) ), SLOT( slotModifyRelation() ) );
    actionDeleteRelation  = new KAction(KIcon( "edit-delete" ), i18n("Delete Dependency"), this);
    actionCollection()->addAction("delete_dependency", actionDeleteRelation );
    connect( actionDeleteRelation, SIGNAL( triggered( bool ) ), SLOT( slotDeleteRelation() ) );

    // Viewlist popup
    connect( m_viewlist, SIGNAL( createView() ), SLOT( slotCreateView() ) );
    connect( m_viewlist, SIGNAL( createKofficeDocument( KoDocumentEntry& ) ), SLOT( slotCreateKofficeDocument( KoDocumentEntry& ) ) );

#ifndef NDEBUG
    //new KAction("Print Debug", CTRL+Qt::SHIFT+Qt::Key_P, this, SLOT( slotPrintDebug()), actionCollection(), "print_debug");
    QAction *action  = new KAction("Print Debug", this);
    actionCollection()->addAction("print_debug", action );
    connect( action, SIGNAL( triggered( bool ) ), SLOT( slotPrintSelectedDebug() ) );
    action->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_P ) );
    action  = new KAction("Print Calendar Debug", this);
    actionCollection()->addAction("print_calendar_debug", action );
    connect( action, SIGNAL( triggered( bool ) ), SLOT( slotPrintCalendarDebug() ) );
    action->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_C ) );
    //     new KAction("Print Test Debug", CTRL+Qt::SHIFT+Qt::Key_T, this, SLOT(slotPrintTestDebug()), actionCollection(), "print_test_debug");


#endif
    // Stupid compilers ;)
#ifndef NDEBUG
    /*  Q_UNUSED( actPrintSelectedDebug );
        Q_UNUSED( actPrintCalendarDebug );*/
#endif

    m_progress = 0;
    m_estlabel = new QLabel( "", 0 );
    if ( statusBar() ) {
        addStatusBarItem( m_estlabel, 0, true );
        //m_progress = new QProgressBar();
        //addStatusBarItem( m_progress, 0, true );
        //m_progress->hide();
    }
    m_progressBarTimer.setSingleShot( true );
    connect( &m_progressBarTimer, SIGNAL( timeout() ), this, SLOT( removeProgressBarItems() ) );

    connect( &getProject(), SIGNAL( scheduleChanged( MainSchedule* ) ), SLOT( slotScheduleChanged( MainSchedule* ) ) );

    connect( &getProject(), SIGNAL( scheduleAdded( const MainSchedule* ) ), SLOT( slotScheduleAdded( const MainSchedule* ) ) );
    connect( &getProject(), SIGNAL( scheduleRemoved( const MainSchedule* ) ), SLOT( slotScheduleRemoved( const MainSchedule* ) ) );
    slotPlugScheduleActions();

    m_viewlist->setSelected( m_viewlist->findItem( "TaskEditor" ) );
    
    
    connect( part, SIGNAL( changed() ), SLOT( slotUpdate() ) );
    
    connect( m_scheduleActionGroup, SIGNAL( triggered( QAction* ) ), SLOT( slotViewSchedule( QAction* ) ) );
    
    loadContext();
    
    //kDebug()<<" end";
}

View::~View()
{
    removeStatusBarItem( m_estlabel );
    delete m_estlabel;
}

ViewAdaptor* View::dbusObject()
{
    return m_dbus;
}

void View::slotCreateTemplate()
{
    int width = 60;
    int height = 60;
    QPixmap pix = getPart()->generatePreview(QSize(width, height));

    KTemporaryFile tempFile;
    tempFile.setSuffix( ".kplatot" );
    //Check that creation of temp file was successful
    if ( tempFile.status() != 0 ) {
        qWarning("Creation of temprary file to store template failed.");
        return;
    }

    getPart()->saveNativeFormat( tempFile.name() );
    KoTemplateCreateDia::createTemplate( "kplato_template", Factory::global(), tempFile.name(), pix, this );
}

void View::createViews()
{
    Context *ctx = getPart()->context();
    if ( ctx && ctx->isLoaded() ) {
        kDebug()<<"isLoaded";
        KoXmlNode n = ctx->context().namedItem( "categories" );
        if ( n.isNull() ) {
            kWarning()<<"No categories";
        } else {
            n = n.firstChild();
            for ( ; ! n.isNull(); n = n.nextSibling() ) {
                if ( ! n.isElement() ) {
                    continue;
                }
                KoXmlElement e = n.toElement();
                if (e.tagName() != "category") {
                    continue;
                }
                kDebug()<<"category: "<<e.attribute( "tag" );
                ViewListItem *cat;
                cat = m_viewlist->addCategory( e.attribute( "tag" ), e.attribute( "name" ) );
                KoXmlNode n1 = e.firstChild();
                for ( ; ! n1.isNull(); n1 = n1.nextSibling() ) {
                    if ( ! n1.isElement() ) {
                        continue;
                    }
                    KoXmlElement e1 = n1.toElement();
                    if (e1.tagName() != "view") {
                        continue;
                    }
                    ViewBase *v = 0;
                    QString type = e1.attribute( "viewtype" );
                    QString tag = e1.attribute( "tag" );
                    QString name = e1.attribute( "name" );
                    QString tip = e1.attribute( "tooltip" );
                    //NOTE: type is the same as classname (so if it is changed...)
                    if ( type == "CalendarEditor" ) {
                        v = createCalendarEditor( cat, tag, name, tip );
                    } else if ( type == "AccountsEditor" ) {
                        v = createAccountsEditor( cat, tag, name, tip );
                    } else if ( type == "ResourceEditor" ) {
                        v = createResourcEditor( cat, tag, name, tip );
                    } else if ( type == "TaskEditor" ) {
                        v = createTaskEditor( cat, tag, name, tip );
                    } else if ( type == "DependencyEditor" ) {
                        v = createDependencyEditor( cat, tag, name, tip );
                    } else if ( type == "PertEditor" ) {
                        v = createPertEditor( cat, tag, name, tip );
                    } else if ( type == "ScheduleEditor" ) {
                        v = createScheduleEditor( cat, tag, name, tip );
                    } else if ( type == "ScheduleHandlerView" ) {
                        v = createScheduleHandler( cat, tag, name, tip );
                    } else if ( type == "ProjectStatusView" ) {
                        v = createProjectStatusView( cat, tag, name, tip );
                    } else if ( type == "TaskStatusView" ) {
                        v = createTaskStatusView( cat, tag, name, tip );
                    } else if ( type == "TaskView" ) {
                        v = createTaskView( cat, tag, name, tip );
                    } else if ( type == "GanttView" ) {
                        v = createGanttView( cat, tag, name, tip );
                    } else if ( type == "MilestoneGanttView" ) {
                        v = createMilestoneGanttView( cat, tag, name, tip );
                    } else if ( type == "ResourceAppointmentsView" ) {
                        v = createResourceAppointmentsView( cat, tag, name, tip );
                    } else if ( type == "AccountsView" ) {
                        v = createAccountsView( cat, tag, name, tip );
                    } else if ( type == "ResourceAssignmentView" ) {
                        // Deactivate for koffice 2.0 release
                        //v = createResourceAssignmentView( cat, tag, name, tip );
                    } else if ( type == "ChartView" ) {
                        // Deactivate for koffice 2.0 release
                        //v = createChartView( cat, tag, name, tip );
                    } else if ( type == "PerformanceStatusView" ) {
                        v = createPerformanceStatusView( cat, tag, name, tip );
                    } else  {
                        kWarning()<<"Unknown viewtype: "<<type;
                    }
                    //KoXmlNode settings = e1.namedItem( "settings " ); ????
                    KoXmlNode settings = e1.firstChild();
                    for ( ; ! settings.isNull(); settings = settings.nextSibling() ) {
                        if ( settings.nodeName() == "settings" ) {
                            break;
                        }
                    }
                    if ( v && settings.isElement() ) {
                        kDebug()<<" settings";
                        v->loadContext( settings.toElement() );
                    }
                }
            }
        }
    }
    if ( m_tab->count() == 0 ) {
        kDebug()<<"Default";
        ViewListItem *cat;
        cat = m_viewlist->addCategory( "Editors", i18n( "Editors" ) );
        
        createCalendarEditor( cat, "CalendarEditor", i18n( "Work & Vacation" ), i18n( "Edit working- and vacation days for resources" ) );
        
        createAccountsEditor( cat, "AccountEditor", i18n( "Cost Breakdown Structure" ), i18n( "Edit cost breakdown structure." ) );
        
        createResourcEditor( cat, "ResourceEditor", i18n( "Resources" ), i18n( "Edit resource breakdown structure." ) );

        createTaskEditor( cat, "TaskEditor", i18n( "Tasks" ), i18n( "Edit work breakdown structure" ) );
        
        createDependencyEditor( cat, "DependencyEditor", i18n( "Dependencies (Graphic)" ), i18n( "Edit task dependenies" ) );
        
        createPertEditor( cat, "PertEditor", i18n( "Dependencies (List)" ), i18n( "Edit task dependencies" ) );
        
        createScheduleHandler( cat, "ScheduleHandler", i18n( "Schedules" ), i18n( "Calculate and analyze project schedules" ) );
    
        cat = m_viewlist->addCategory( "Views", i18n( "Views" ) );
        createProjectStatusView( cat, "ProjectStatusView", i18n( "Project Performance Chart" ), i18n( "View project status information" ) );
        
        createPerformanceStatusView( cat, "PerformanceStatusView", i18n( "Tasks Performance Chart" ), i18n( "View tasks performance status information" ) );
        
        createTaskStatusView( cat, "TaskStatusView", i18n( "Task Status" ), i18n( "View task progress information" ) );
        
        createTaskView( cat, "TaskView", i18n( "Task Execution" ), i18n( "View task execution information" ) );
        
        createGanttView( cat, "GanttView", i18n( "Gantt" ), i18n( "View gantt chart" ) );
        
        createMilestoneGanttView( cat, "MilestoneGanttView", i18n( "Milestone Gantt" ), i18n( "View milestone gantt chart" ) );
        
        createResourceAppointmentsView( cat, "ResourceAppointmentsView", i18n( "Resource Assignments" ), i18n( "View resource assignments" ) );

        createAccountsView( cat, "AccountsView", i18n( "Cost Breakdown" ), i18n( "View planned and actual cost" ) );

        // Deactivate for koffice 2.0 release
        //createResourceAssignmentView( cat, "ResourceAssignmentView", i18n( "Tasks by resources" ), i18n( "View task status per resource" ) );

        // Deactivate for koffice 2.0 release
        //createChartView( cat, "PerformanceChart", i18n( "Performance Chart" ), i18n( "Cost and schedule monitoring" ) );
    }
}

ViewBase *View::createResourceAppointmentsView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    ResourceAppointmentsView *v = new ResourceAppointmentsView( getPart(), m_tab );
    m_tab->addWidget( v );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, v, getPart(), "resource_view", index );
    i->setToolTip( 0, tip );

    connect( v, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), v, SLOT( setScheduleManager( ScheduleManager* ) ) );
    
    connect( v, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

    v->setProject( &( getProject() ) );
    v->setScheduleManager( currentScheduleManager() );
    v->updateReadWrite( m_readWrite );
    return v;
}

ViewBase *View::createResourcEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    ResourceEditor *resourceeditor = new ResourceEditor( getPart(), m_tab );
    m_tab->addWidget( resourceeditor );
    resourceeditor->draw( getProject() );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, resourceeditor, getPart(), "resource_editor", index );
    i->setToolTip( 0, tip );

    connect( resourceeditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( resourceeditor, SIGNAL( addResource( ResourceGroup* ) ), SLOT( slotAddResource( ResourceGroup* ) ) );
    connect( resourceeditor, SIGNAL( deleteObjectList( QObjectList ) ), SLOT( slotDeleteResourceObjects( QObjectList ) ) );

    connect( resourceeditor, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    resourceeditor->updateReadWrite( m_readWrite );
    return resourceeditor;
}

ViewBase *View::createTaskEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    TaskEditor *taskeditor = new TaskEditor( getPart(), m_tab );
    m_tab->addWidget( taskeditor );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, taskeditor, getPart(), "task_editor", index );
    i->setToolTip( 0, tip );

    taskeditor->draw( getProject() );
    taskeditor->setScheduleManager( currentScheduleManager() );

    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), taskeditor, SLOT( setScheduleManager( ScheduleManager* ) ) );
    
    connect( taskeditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( taskeditor, SIGNAL( addTask() ), SLOT( slotAddTask() ) );
    connect( taskeditor, SIGNAL( addMilestone() ), SLOT( slotAddMilestone() ) );
    connect( taskeditor, SIGNAL( addSubtask() ), SLOT( slotAddSubTask() ) );
    connect( taskeditor, SIGNAL( deleteTaskList( QList<Node*> ) ), SLOT( slotDeleteTask( QList<Node*> ) ) );
    connect( taskeditor, SIGNAL( moveTaskUp() ), SLOT( slotMoveTaskUp() ) );
    connect( taskeditor, SIGNAL( moveTaskDown() ), SLOT( slotMoveTaskDown() ) );
    connect( taskeditor, SIGNAL( indentTask() ), SLOT( slotIndentTask() ) );
    connect( taskeditor, SIGNAL( unindentTask() ), SLOT( slotUnindentTask() ) );
    


    connect( taskeditor, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    taskeditor->updateReadWrite( m_readWrite );
    return taskeditor;
}

ViewBase *View::createAccountsEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    AccountsEditor *ae = new AccountsEditor( getPart(), m_tab );
    m_tab->addWidget( ae );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, ae, getPart(), "accounts_editor", index );
    i->setToolTip( 0, tip );

    ae->draw( getProject() );

    connect( ae, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );
    ae->updateReadWrite( m_readWrite );
    return ae;
}

ViewBase *View::createCalendarEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    CalendarEditor *calendareditor = new CalendarEditor( getPart(), m_tab );
    m_tab->addWidget( calendareditor );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, calendareditor, getPart(), "calendar_editor", index );
    i->setToolTip( 0, tip );

    calendareditor->draw( getProject() );

    connect( calendareditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( calendareditor, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    calendareditor->updateReadWrite( m_readWrite );
    return calendareditor;
}

ViewBase *View::createScheduleHandler( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    ScheduleHandlerView *handler = new ScheduleHandlerView( getPart(), m_tab );
    m_tab->addWidget( handler );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, handler, getPart(), "schedule_editor", index );
    i->setToolTip( 0, tip );

    connect( handler->scheduleEditor(), SIGNAL( addScheduleManager( Project* ) ), SLOT( slotAddScheduleManager( Project* ) ) );
    connect( handler->scheduleEditor(), SIGNAL( deleteScheduleManager( Project*, ScheduleManager* ) ), SLOT( slotDeleteScheduleManager( Project*, ScheduleManager* ) ) );

    connect( handler->scheduleEditor(), SIGNAL( calculateSchedule( Project*, ScheduleManager* ) ), SLOT( slotCalculateSchedule( Project*, ScheduleManager* ) ) );

    connect( handler->scheduleEditor(), SIGNAL( baselineSchedule( Project*, ScheduleManager* ) ), SLOT( slotBaselineSchedule( Project*, ScheduleManager* ) ) );


    connect( handler, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), handler, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ) );
    
    connect( handler, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

    handler->draw( getProject() );
    handler->updateReadWrite( m_readWrite );
    return handler;
}

ScheduleEditor *View::createScheduleEditor( QWidget *parent )
{
    ScheduleEditor *scheduleeditor = new ScheduleEditor( getPart(), parent );
    
    connect( scheduleeditor, SIGNAL( addScheduleManager( Project* ) ), SLOT( slotAddScheduleManager( Project* ) ) );
    connect( scheduleeditor, SIGNAL( deleteScheduleManager( Project*, ScheduleManager* ) ), SLOT( slotDeleteScheduleManager( Project*, ScheduleManager* ) ) );

    connect( scheduleeditor, SIGNAL( calculateSchedule( Project*, ScheduleManager* ) ), SLOT( slotCalculateSchedule( Project*, ScheduleManager* ) ) );

    connect( scheduleeditor, SIGNAL( baselineSchedule( Project*, ScheduleManager* ) ), SLOT( slotBaselineSchedule( Project*, ScheduleManager* ) ) );

    scheduleeditor->updateReadWrite( m_readWrite );
    return scheduleeditor;
}

ViewBase *View::createScheduleEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    ScheduleEditor *scheduleeditor = new ScheduleEditor( getPart(), m_tab );
    m_tab->addWidget( scheduleeditor );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, scheduleeditor, getPart(), "schedule_editor", index );
    i->setToolTip( 0, tip );

    scheduleeditor->setProject( &( getProject() ) );

    connect( scheduleeditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( scheduleeditor, SIGNAL( addScheduleManager( Project* ) ), SLOT( slotAddScheduleManager( Project* ) ) );
    
    connect( scheduleeditor, SIGNAL( deleteScheduleManager( Project*, ScheduleManager* ) ), SLOT( slotDeleteScheduleManager( Project*, ScheduleManager* ) ) );

    connect( scheduleeditor, SIGNAL( calculateSchedule( Project*, ScheduleManager* ) ), SLOT( slotCalculateSchedule( Project*, ScheduleManager* ) ) );
    
    connect( scheduleeditor, SIGNAL( baselineSchedule( Project*, ScheduleManager* ) ), SLOT( slotBaselineSchedule( Project*, ScheduleManager* ) ) );
    
    scheduleeditor->updateReadWrite( m_readWrite );
    return scheduleeditor;
}


ViewBase *View::createDependencyEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    DependencyEditor *editor = new DependencyEditor( getPart(), m_tab );
    m_tab->addWidget( editor );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, editor, getPart(), "task_editor", index );
    i->setToolTip( 0, tip );

    editor->draw( getProject() );

    connect( editor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( editor, SIGNAL( addRelation( Node*, Node*, int ) ), SLOT( slotAddRelation( Node*, Node*, int ) ) );
    connect( editor, SIGNAL( modifyRelation( Relation*, int ) ), SLOT( slotModifyRelation( Relation*, int ) ) );
    connect( editor, SIGNAL( modifyRelation( Relation* ) ), SLOT( slotModifyRelation( Relation* ) ) );

    connect( editor, SIGNAL( editNode( Node * ) ), SLOT( slotOpenNode( Node * ) ) );
    connect( editor, SIGNAL( addTask() ), SLOT( slotAddTask() ) );
    connect( editor, SIGNAL( addMilestone() ), SLOT( slotAddMilestone() ) );
    connect( editor, SIGNAL( addSubtask() ), SLOT( slotAddSubTask() ) );
    connect( editor, SIGNAL( deleteTaskList( QList<Node*> ) ), SLOT( slotDeleteTask( QList<Node*> ) ) );

    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), editor, SLOT( setScheduleManager( ScheduleManager* ) ) );
    
    connect( editor, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    editor->updateReadWrite( m_readWrite );
    editor->setScheduleManager( currentScheduleManager() );
    return editor;
}

ViewBase *View::createPertEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    PertEditor *perteditor = new PertEditor( getPart(), m_tab );
    m_tab->addWidget( perteditor );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, perteditor, getPart(), "task_editor", index );
    i->setToolTip( 0, tip );

    perteditor->draw( getProject() );

    connect( perteditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );
    m_updatePertEditor = true;
    perteditor->updateReadWrite( m_readWrite );
    return perteditor;
}

ViewBase *View::createProjectStatusView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    ProjectStatusView *v = new ProjectStatusView( getPart(), m_tab );
    m_tab->addWidget( v );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, v, getPart(), "status_view", index );
    i->setToolTip( 0, tip );

    connect( v, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), v, SLOT( setScheduleManager( ScheduleManager* ) ) );
    
    v->updateReadWrite( m_readWrite );
    v->setProject( &getProject() );
    v->setScheduleManager( currentScheduleManager() );
    return v;
}

ViewBase *View::createPerformanceStatusView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    PerformanceStatusView *v = new PerformanceStatusView( getPart(), m_tab );
    m_tab->addWidget( v );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, v, getPart(), "status_view", index );
    i->setToolTip( 0, tip );

    connect( v, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), v, SLOT( setScheduleManager( ScheduleManager* ) ) );
    
    connect( v, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

    v->updateReadWrite( m_readWrite );
    v->setProject( &getProject() );
    v->setScheduleManager( currentScheduleManager() );
    return v;
}


ViewBase *View::createTaskStatusView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    TaskStatusView *taskstatusview = new TaskStatusView( getPart(), m_tab );
    m_tab->addWidget( taskstatusview );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, taskstatusview, getPart(), "status_view", index );
    i->setToolTip( 0, tip );

    connect( taskstatusview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), taskstatusview, SLOT( setScheduleManager( ScheduleManager* ) ) );
    
    connect( taskstatusview, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    
    taskstatusview->updateReadWrite( m_readWrite );
    taskstatusview->draw( getProject() );
    taskstatusview->setScheduleManager( currentScheduleManager() );
    return taskstatusview;
}

ViewBase *View::createTaskView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    TaskView *v = new TaskView( getPart(), m_tab );
    m_tab->addWidget( v );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, v, getPart(), "task_view", index );
    i->setToolTip( 0, tip );

    v->draw( getProject() );
    v->setScheduleManager( currentScheduleManager() );
    
    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), v, SLOT( setScheduleManager( ScheduleManager* ) ) );
    
    connect( v, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( v, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    v->updateReadWrite( m_readWrite );
    return v;
}

ViewBase *View::createGanttView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    GanttView *ganttview = new GanttView( getPart(), m_tab, getPart()->isReadWrite() );
    m_tab->addWidget( ganttview );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, ganttview, getPart(), "gantt_chart", index );
    i->setToolTip( 0, tip );

    ganttview->setProject( &( getProject() ) );
    ganttview->setScheduleManager( currentScheduleManager() );

    connect( ganttview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );
/*  TODO: Review these
    connect( ganttview, SIGNAL( addRelation( Node*, Node*, int ) ), SLOT( slotAddRelation( Node*, Node*, int ) ) );
    connect( ganttview, SIGNAL( modifyRelation( Relation*, int ) ), SLOT( slotModifyRelation( Relation*, int ) ) );
    connect( ganttview, SIGNAL( modifyRelation( Relation* ) ), SLOT( slotModifyRelation( Relation* ) ) );
    connect( ganttview, SIGNAL( itemDoubleClicked() ), SLOT( slotOpenNode() ) );
    connect( ganttview, SIGNAL( itemRenamed( Node*, const QString& ) ), this, SLOT( slotRenameNode( Node*, const QString& ) ) );*/
    
    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), ganttview, SLOT( setScheduleManager( ScheduleManager* ) ) );
    
    connect( ganttview, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    ganttview->updateReadWrite( m_readWrite );
    return ganttview;
}

ViewBase *View::createMilestoneGanttView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    MilestoneGanttView *ganttview = new MilestoneGanttView( getPart(), m_tab, getPart()->isReadWrite() );
    m_tab->addWidget( ganttview );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, ganttview, getPart(), "gantt_chart", index );
    i->setToolTip( 0, tip );

    ganttview->setProject( &( getProject() ) );
    ganttview->setScheduleManager( currentScheduleManager() );

    connect( ganttview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );
    
    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), ganttview, SLOT( setScheduleManager( ScheduleManager* ) ) );
    
    connect( ganttview, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    ganttview->updateReadWrite( m_readWrite );
    return ganttview;
}


ViewBase *View::createAccountsView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    AccountsView *accountsview = new AccountsView( &getProject(), getPart(), m_tab );
    m_tab->addWidget( accountsview );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, accountsview, getPart(), "accounts", index );
    i->setToolTip( 0, tip );

    accountsview->setScheduleManager( currentScheduleManager() );

    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), accountsview, SLOT( setScheduleManager( ScheduleManager* ) ) );
    
    connect( accountsview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );
    accountsview->updateReadWrite( m_readWrite );
    return accountsview;
}

ViewBase *View::createResourceAssignmentView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    ResourceAssignmentView *resourceAssignmentView = new ResourceAssignmentView( getPart(), m_tab );
    m_tab->addWidget( resourceAssignmentView );
    m_updateResourceAssignmentView = true;

    ViewListItem *i = m_viewlist->addView( cat, tag, name, resourceAssignmentView, getPart(), "resource_assignment", index );
    i->setToolTip( 0, tip );

    resourceAssignmentView->draw( getProject() );

    connect( resourceAssignmentView, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( resourceAssignmentView, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    resourceAssignmentView->updateReadWrite( m_readWrite );
    return resourceAssignmentView;
}

ViewBase *View::createChartView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index )
{
    ChartView *v = new ChartView( getPart(), m_tab );
    m_tab->addWidget( v );

    ViewListItem *i = m_viewlist->addView( cat, tag, name, v, getPart(), "chart", index );
    i->setToolTip( 0, tip );

    v->setProject( &( getProject() ) );
    
    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), v, SLOT( setScheduleManager( ScheduleManager* ) ) );

    connect( v, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );
    v->updateReadWrite( m_readWrite );
    return v;

}

Project& View::getProject() const
{
    return getPart() ->getProject();
}

void View::setZoom( double )
{
    //TODO
}

KoPrintJob * View::createPrintJob()
{
    KoView *v = qobject_cast<KoView*>( canvas() );
    if ( v == 0 ) {
        return 0;
    }
    return v->createPrintJob();
}

void View::slotEditCut()
{
    //kDebug();
}

void View::slotEditCopy()
{
    //kDebug();
}

void View::slotEditPaste()
{
    //kDebug();
}

void View::slotViewSelector( bool show )
{
    //kDebug();
    m_viewlist->setVisible( show );
}


void View::slotProjectEdit()
{
    MainProjectDialog * dia = new MainProjectDialog( getProject() );
    if ( dia->exec()  == QDialog::Accepted) {
        QUndoCommand * cmd = dia->buildCommand();
        if ( cmd ) {
            getPart() ->addCommand( cmd );
        }
    }
    delete dia;
}

void View::slotProjectWorktime()
{
    StandardWorktimeDialog * dia = new StandardWorktimeDialog( getProject() );
    if ( dia->exec()  == QDialog::Accepted) {
        QUndoCommand * cmd = dia->buildCommand();
        if ( cmd ) {
            //kDebug()<<"Modifying calendar(s)";
            getPart() ->addCommand( cmd ); //also executes
        }
    }
    delete dia;
}

QList<QAction*> View::sortedActionList()
{
    QMap<QString, QAction*> lst;
    foreach ( QAction *a, m_scheduleActions.keys() ) {
        lst.insert( a->objectName(), a );
    }
    return lst.values();
}

void View::slotScheduleRemoved( const MainSchedule *sch )
{
    kDebug()<<sch<<sch->name();
    QAction *a = 0;
    QAction *checked = m_scheduleActionGroup->checkedAction();
    QMapIterator<QAction*, Schedule*> i( m_scheduleActions );
    while (i.hasNext()) {
        i.next();
        if ( i.value() == sch ) {
            a = i.key();
            break;
        }
    }
    if ( a ) {
        unplugActionList( "view_schedule_list" );
        delete a;
        plugActionList( "view_schedule_list", sortedActionList() );
        if ( checked && checked != a ) {
            checked->setChecked( true );
        } else if ( ! m_scheduleActions.isEmpty() ) {
            m_scheduleActions.keys().first()->setChecked( true );
        }
    }
    slotViewSchedule( m_scheduleActionGroup->checkedAction() );
}

void View::slotScheduleAdded( const MainSchedule *sch )
{
    if ( sch->type() != Schedule::Expected ) {
        return; // Only view expected
    }
    MainSchedule *s = const_cast<MainSchedule*>( sch ); // FIXME
    //kDebug()<<sch->name()<<" deleted="<<sch->isDeleted();
    QAction *checked = m_scheduleActionGroup->checkedAction();
    if ( ! sch->isDeleted() && sch->isScheduled() ) {
        unplugActionList( "view_schedule_list" );
        QAction *act = addScheduleAction( s );
        plugActionList( "view_schedule_list", sortedActionList() );
        if ( checked ) {
            checked->setChecked( true );
        } else if ( act ) {
            act->setChecked( true );
        } else if ( ! m_scheduleActions.isEmpty() ) {
            m_scheduleActions.keys().first()->setChecked( true );
        }
    }
    slotViewSchedule( m_scheduleActionGroup->checkedAction() );
}

void View::slotScheduleChanged( MainSchedule *sch )
{
    //kDebug()<<sch->name()<<" deleted="<<sch->isDeleted();
    if ( sch->isDeleted() || ! sch->isScheduled() ) {
        slotScheduleRemoved( sch );
        return;
    }
    if ( m_scheduleActions.values().contains( sch ) ) {
        slotScheduleRemoved( sch ); // hmmm, how to avoid this?
    }
    slotScheduleAdded( sch );
}

QAction *View::addScheduleAction( Schedule *sch )
{
    QAction *act = 0;
    if ( ! sch->isDeleted() ) {
        QString n = sch->name();
        act = new KToggleAction( n, this);
        actionCollection()->addAction(n, act );
        m_scheduleActions.insert( act, sch );
        m_scheduleActionGroup->addAction( act );
        //kDebug()<<"Add:"<<n;
        connect( act, SIGNAL(destroyed( QObject* ) ), SLOT( slotActionDestroyed( QObject* ) ) );
    }
    return act;
}

void View::slotViewSchedule( QAction *act )
{
    //kDebug()<<act;
    ScheduleManager *sm = 0;
    if ( act != 0 ) {
        Schedule *sch = m_scheduleActions.value( act, 0 );
        sm = sch->manager();
    }
    setLabel( sm );
    emit currentScheduleManagerChanged( sm );
}

void View::slotActionDestroyed( QObject *o )
{
    //kDebug()<<o->name();
    m_scheduleActions.remove( static_cast<QAction*>( o ) );
//    slotViewSchedule( m_scheduleActionGroup->checkedAction() );
}

void View::slotPlugScheduleActions()
{
    //kDebug()<<activeScheduleId();
    long id = activeScheduleId();
    unplugActionList( "view_schedule_list" );
    foreach( QAction *act, m_scheduleActions.keys() ) {
        m_scheduleActionGroup->removeAction( act );
        delete act;
    }
    m_scheduleActions.clear();
    QAction *ca = 0;
    foreach( ScheduleManager *sm, getProject().allScheduleManagers() ) {
        Schedule *sch = sm->expected();
        if ( sch == 0 ) {
            continue;
        }
        QAction *act = addScheduleAction( sch );
        if ( act && id == sch->id() ) {
            ca = act;
        }
    }
    plugActionList( "view_schedule_list", sortedActionList() );
    if ( ca == 0 && m_scheduleActionGroup->actions().count() > 0 ) {
        ca = m_scheduleActionGroup->actions().first();
    }
    if ( ca ) {
        ca->setChecked( true );
    }
    slotViewSchedule( ca );
}

void View::slotProgressChanged( int )
{
}

void View::slotCalculateSchedule( Project *project, ScheduleManager *sm )
{
    if ( project == 0 || sm == 0 ) {
        return;
    }
    if ( m_progressBarTimer.isActive() ) {
        m_progressBarTimer.stop();
        removeProgressBarItems();
    }
    m_text = new QLabel( i18n( "%1: Calculating...", sm->name() ) );
    addStatusBarItem( m_text, 0, true );
    m_progress = new QProgressBar();
    m_progress->setMaximumHeight(statusBar()->fontMetrics().height());
    addStatusBarItem( m_progress, 0, true );
    connect( project, SIGNAL( maxProgress( int ) ), m_progress, SLOT( setMaximum( int ) ) );
    connect( project, SIGNAL( sigProgress( int ) ), m_progress, SLOT( setValue( int ) ) );
    QApplication::setOverrideCursor( Qt::WaitCursor );
    CalculateScheduleCmd *cmd =  new CalculateScheduleCmd( *project, *sm, i18n( "Calculate %1", sm->name() ) );
    getPart() ->addCommand( cmd );
    QApplication::restoreOverrideCursor();
    slotUpdate();
    m_text->setText( i18n( "%1: Calculating done", sm->name() ) );
    disconnect( project, SIGNAL( sigProgress( int ) ), m_progress, SLOT(setValue( int ) ) );
    disconnect( project, SIGNAL( maxProgress( int ) ), m_progress, SLOT( setMaximum( int ) ) );
    m_progressBarTimer.start( 2000 );
}

void View::removeProgressBarItems()
{
    removeStatusBarItem( m_progress );
    removeStatusBarItem( m_text );
    addStatusBarItem( m_estlabel, 0, true );
    delete m_progress;
    m_progress = 0;
    delete m_text;
    m_text = 0;
}

void View::slotBaselineSchedule( Project *project, ScheduleManager *sm )
{
    if ( project == 0 || sm == 0 ) {
        return;
    }
    if ( ! sm->isBaselined() && project->isBaselined() ) {
        KMessageBox::sorry( this, i18n( "Cannot baseline. The project is already baselined." ) );
        return;
    }
    QUndoCommand *cmd;
    if ( sm->isBaselined() ) {
        int res = KMessageBox::warningContinueCancel( this, i18n( "This schedule is baselined. Do you want to remove the baseline?" ) );
        if ( res == KMessageBox::Cancel ) {
            return;
        }
        cmd = new ResetBaselineScheduleCmd( *sm, i18n( "Reset Baseline %1", sm->name() ) );
    } else {
        cmd = new BaselineScheduleCmd( *sm, i18n( "Baseline %1", sm->name() ) );
    }
    getPart() ->addCommand( cmd );
}

void View::slotAddScheduleManager( Project *project )
{
    if ( project == 0 ) {
        return;
    }
    ScheduleManager *sm = project->createScheduleManager();
    AddScheduleManagerCmd *cmd =  new AddScheduleManagerCmd( *project, sm, i18n( "Add Schedule %1", sm->name() ) );
    getPart() ->addCommand( cmd );
}

void View::slotDeleteScheduleManager( Project *project, ScheduleManager *sm )
{
    if ( project == 0 || sm == 0) {
        return;
    }
    DeleteScheduleManagerCmd *cmd =  new DeleteScheduleManagerCmd( *project, sm, i18n( "Delete Schedule %1", sm->name() ) );
    getPart() ->addCommand( cmd );
}

void View::slotAddSubTask()
{
    // If we are positionend on the root project, then what we really want to
    // do is to add a first project. We will silently accept the challenge
    // and will not complain.
    Task * node = getProject().createTask( getPart() ->config().taskDefaults(), currentTask() );
    TaskAddDialog *dia = new TaskAddDialog( *node, getProject().accounts() );
    if ( dia->exec()  == QDialog::Accepted) {
        Node * currNode = currentTask();
        if ( currNode ) {
            QUndoCommand *m = dia->buildCommand();
            m->redo(); // do changes to task
            delete m;
            SubtaskAddCmd *cmd = new SubtaskAddCmd( &( getProject() ), node, currNode, i18n( "Add Subtask" ) );
            getPart() ->addCommand( cmd ); // add task to project
            delete dia;
            return ;
        } else
            kDebug() <<"Cannot insert new project. Hmm, no current node!?";
    }
    delete node;
    delete dia;
}

void View::slotAddTask()
{
    Task * node = getProject().createTask( getPart() ->config().taskDefaults(), currentTask() );
    TaskAddDialog *dia = new TaskAddDialog( *node, getProject().accounts() );
    if ( dia->exec()  == QDialog::Accepted) {
        Node * currNode = currentTask();
        if ( currNode ) {
            QUndoCommand * m = dia->buildCommand();
            m->redo(); // do changes to task
            delete m;
            TaskAddCmd *cmd = new TaskAddCmd( &( getProject() ), node, currNode, i18n( "Add Task" ) );
            getPart() ->addCommand( cmd ); // add task to project
            delete dia;
            return ;
        } else
            kDebug() <<"Cannot insert new task. Hmm, no current node!?";
    }
    delete node;
    delete dia;
}

void View::slotAddMilestone()
{
    Task * node = getProject().createTask( currentTask() );
    node->estimate() ->clear();

    TaskAddDialog *dia = new TaskAddDialog( *node, getProject().accounts() );
    if ( dia->exec() == QDialog::Accepted ) {
        Node * currNode = currentTask();
        if ( currNode ) {
            QUndoCommand * m = dia->buildCommand();
            m->redo(); // do changes to task
            delete m;
            TaskAddCmd *cmd = new TaskAddCmd( &( getProject() ), node, currNode, i18n( "Add Milestone" ) );
            getPart() ->addCommand( cmd ); // add task to project
            delete dia;
            return ;
        } else
            kDebug() <<"Cannot insert new milestone. Hmm, no current node!?";
    }
    delete node;
    delete dia;
}

void View::slotDefineWBS()
{
    //kDebug();
    Project &p = getProject();
    WBSDefinitionDialog dia( p, p.wbsDefinition() );
    if ( dia.exec() == QDialog::Accepted ) {
        QUndoCommand *cmd = dia.buildCommand();
        if ( cmd ) {
            getPart()->addCommand( cmd );
        }
    }
}

void View::slotConfigure()
{
    //kDebug();
    ConfigDialog * dia = new ConfigDialog( getPart(), getPart() ->config() );
    dia->exec();
    delete dia;
}


Calendar *View::currentCalendar()
{
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v == 0 ) {
        return 0;
    }
    return v->currentCalendar();
}

Node *View::currentTask()
{
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v == 0 ) {
        return 0;
    }
    Node * task = v->currentNode();
    if ( 0 != task ) {
        return task;
    }
    return &( getProject() );
}

Resource *View::currentResource()
{
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v == 0 ) {
        return 0;
    }
    return v->currentResource();
}

ResourceGroup *View::currentResourceGroup()
{
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v == 0 ) {
        return 0;
    }
    return v->currentResourceGroup();
}


void View::slotOpenNode()
{
    //kDebug();
    Node * node = currentTask();
    slotOpenNode( node );
}

void View::slotOpenNode( Node *node )
{
    //kDebug();
    if ( !node )
        return ;

    switch ( node->type() ) {
        case Node::Type_Project: {
                Project * project = dynamic_cast<Project *>( node );
                MainProjectDialog *dia = new MainProjectDialog( *project );
                if ( dia->exec()  == QDialog::Accepted) {
                    QUndoCommand * m = dia->buildCommand();
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        case Node::Type_Subproject:
            //TODO
            break;
        case Node::Type_Task: {
            Task *task = dynamic_cast<Task *>( node );
                Q_ASSERT( task );
                TaskDialog *dia = new TaskDialog( *task, getProject().accounts() );
                if ( dia->exec()  == QDialog::Accepted) {
                    QUndoCommand * m = dia->buildCommand();
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        case Node::Type_Milestone: {
                // Use the normal task dialog for now.
                // Maybe milestone should have it's own dialog, but we need to be able to
                // enter a duration in case we accidentally set a tasks duration to zero
                // and hence, create a milestone
                Task *task = dynamic_cast<Task *>( node );
                Q_ASSERT( task );
                TaskDialog *dia = new TaskDialog( *task, getProject().accounts() );
                if ( dia->exec()  == QDialog::Accepted) {
                    QUndoCommand * m = dia->buildCommand();
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        case Node::Type_Summarytask: {
                Task *task = dynamic_cast<Task *>( node );
                Q_ASSERT( task );
                SummaryTaskDialog *dia = new SummaryTaskDialog( *task );
                if ( dia->exec()  == QDialog::Accepted) {
                    QUndoCommand * m = dia->buildCommand();
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        default:
            break; // avoid warnings
    }
}

ScheduleManager *View::currentScheduleManager() const
{
    Schedule *s = m_scheduleActions.value( m_scheduleActionGroup->checkedAction() );
    return s == 0 ? 0 : s->manager();
}

long View::activeScheduleId() const
{
    Schedule *s = m_scheduleActions.value( m_scheduleActionGroup->checkedAction() );
    return s == 0 ? -1 : s->id();
}

void View::setActiveSchedule( long id ) const
{
    if ( id != -1 ) {
        QMap<QAction*, Schedule*>::const_iterator it = m_scheduleActions.constBegin();
        for (; it != m_scheduleActions.constEnd(); ++it ) {
            if ( it.value()->id() == id ) {
                it.key()->setChecked( true );
                break;
            }
        }
    }
}

void View::slotTaskProgress()
{
    //kDebug();
    Node * node = currentTask();
    if ( !node )
        return ;

    switch ( node->type() ) {
        case Node::Type_Project: {
                break;
            }
        case Node::Type_Subproject:
            //TODO
            break;
        case Node::Type_Task: {
                Task *task = dynamic_cast<Task *>( node );
                Q_ASSERT( task );
                TaskProgressDialog *dia = new TaskProgressDialog( *task, currentScheduleManager(),  getProject().standardWorktime() );
                if ( dia->exec()  == QDialog::Accepted) {
                    QUndoCommand * m = dia->buildCommand();
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        case Node::Type_Milestone: {
                Task *task = dynamic_cast<Task *>( node );
                MilestoneProgressDialog *dia = new MilestoneProgressDialog( *task );
                if ( dia->exec()  == QDialog::Accepted) {
                    QUndoCommand * m = dia->buildCommand();
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        case Node::Type_Summarytask: {
                // TODO
                break;
            }
        default:
            break; // avoid warnings
    }
}

void View::slotDeleteTask( QList<Node*> lst )
{
    //kDebug();
    foreach ( Node *n, lst ) {
        if ( n->isScheduled() ) {
            int res = KMessageBox::warningContinueCancel( this, i18n( "A task that has been scheduled will be deleted. This will invalidate the schedule." ) );
            if ( res == KMessageBox::Cancel ) {
                return;
            }
            break;
        }
    }
    if ( lst.count() == 1 ) {
        getPart()->addCommand( new NodeDeleteCmd( lst.takeFirst(), i18n( "Delete Task" ) ) );
        return;
    }
    int num = 0;
    MacroCommand *cmd = new MacroCommand( i18n( "Delete Tasks" ) );
    while ( !lst.isEmpty() ) {
        Node *node = lst.takeFirst();
        if ( node == 0 || node->parentNode() == 0 ) {
            kDebug() << ( node ?"Task is main project" :"No current task" );
            continue;
        }
        bool del = true;
        foreach ( Node *n, lst ) {
            if ( node->isChildOf( n ) ) {
                del = false; // node is going to be deleted when we delete n
                break;
            }
        }
        if ( del ) {
            //kDebug()<<num<<": delete:"<<node->name();
            cmd->addCommand( new NodeDeleteCmd( node, i18n( "Delete Task" ) ) );
            num++;
        }
    }
    if ( num > 0 ) {
        getPart()->addCommand( cmd );
    } else {
        delete cmd;
    }
}

void View::slotDeleteTask( Node *node )
{
    //kDebug();
    if ( node == 0 || node->parentNode() == 0 ) {
        kDebug() << ( node ?"Task is main project" :"No current task" );
        return ;
    }
    if ( node->isScheduled() ) {
        int res = KMessageBox::warningContinueCancel( this, i18n( "This task has been scheduled. This will invalidate the schedule." ) );
        if ( res == KMessageBox::Cancel ) {
            return;
        }
    }
    NodeDeleteCmd *cmd = new NodeDeleteCmd( node, i18n( "Delete Task" ) );
    getPart() ->addCommand( cmd );
}

void View::slotDeleteTask()
{
    //kDebug();
    return slotDeleteTask( currentTask() );
}

void View::slotTaskWorkpackage()
{
    //kDebug();
    Node *node = currentTask();
    if ( node == 0 || node->type() != Node::Type_Task ) {
        return;
    }
    Task *task = static_cast<Task*>( node );
    WPControlDialog *dlg = new WPControlDialog( this, *task, this );
    dlg->exec();
//    getPart()->saveWorkPackageUrl( KUrl( "workpackage.kplatowork" ), node, activeScheduleId() );
}

void View::slotIndentTask()
{
    //kDebug();
    Node * node = currentTask();
    if ( node == 0 || node->parentNode() == 0 ) {
        kDebug() << ( node ?"Task is main project" :"No current task" );
        return ;
    }
    if ( getProject().canIndentTask( node ) ) {
        NodeIndentCmd * cmd = new NodeIndentCmd( *node, i18n( "Indent Task" ) );
        getPart() ->addCommand( cmd );
    }
}

void View::slotUnindentTask()
{
    //kDebug();
    Node * node = currentTask();
    if ( node == 0 || node->parentNode() == 0 ) {
        kDebug() << ( node ?"Task is main project" :"No current task" );
        return ;
    }
    if ( getProject().canUnindentTask( node ) ) {
        NodeUnindentCmd * cmd = new NodeUnindentCmd( *node, i18n( "Unindent Task" ) );
        getPart() ->addCommand( cmd );
    }
}

void View::slotMoveTaskUp()
{
    //kDebug();

    Node * task = currentTask();
    if ( 0 == task ) {
        // is always != 0. At least we would get the Project, but you never know who might change that
        // so better be careful
        kError() << "No current task" << endl;
        return ;
    }

    if ( Node::Type_Project == task->type() ) {
        kDebug() <<"The root node cannot be moved up";
        return ;
    }
    if ( getProject().canMoveTaskUp( task ) ) {
        NodeMoveUpCmd * cmd = new NodeMoveUpCmd( *task, i18n( "Move Task Up" ) );
        getPart() ->addCommand( cmd );
    }
}

void View::slotMoveTaskDown()
{
    //kDebug();

    Node * task = currentTask();
    if ( 0 == task ) {
        // is always != 0. At least we would get the Project, but you never know who might change that
        // so better be careful
        return ;
    }

    if ( Node::Type_Project == task->type() ) {
        kDebug() <<"The root node cannot be moved down";
        return ;
    }
    if ( getProject().canMoveTaskDown( task ) ) {
        NodeMoveDownCmd * cmd = new NodeMoveDownCmd( *task, i18n( "Move Task Down" ) );
        getPart() ->addCommand( cmd );
    }
}

void View::slotAddRelation( Node *par, Node *child )
{
    //kDebug();
    Relation * rel = new Relation( par, child );
    AddRelationDialog *dia = new AddRelationDialog( getProject(), rel, this );
    if ( dia->exec()  == QDialog::Accepted) {
        QUndoCommand * cmd = dia->buildCommand();
        if ( cmd )
            getPart() ->addCommand( cmd );
    } else {
        delete rel;
    }
    delete dia;
}

void View::slotAddRelation( Node *par, Node *child, int linkType )
{
    //kDebug();
    if ( linkType == Relation::FinishStart ||
            linkType == Relation::StartStart ||
            linkType == Relation::FinishFinish ) {
        Relation * rel = new Relation( par, child, static_cast<Relation::Type>( linkType ) );
        getPart() ->addCommand( new AddRelationCmd( getProject(), rel, i18n( "Add Relation" ) ) );
    } else {
        slotAddRelation( par, child );
    }
}

void View::slotModifyRelation( Relation *rel )
{
    //kDebug();
    ModifyRelationDialog * dia = new ModifyRelationDialog( getProject(), rel, this );
    if ( dia->exec()  == QDialog::Accepted) {
        if ( dia->relationIsDeleted() ) {
            getPart() ->addCommand( new DeleteRelationCmd( getProject(), rel, i18n( "Delete Relation" ) ) );
        } else {
            QUndoCommand *cmd = dia->buildCommand();
            if ( cmd ) {
                getPart() ->addCommand( cmd );
            }
        }
    }
    delete dia;
}

void View::slotModifyRelation( Relation *rel, int linkType )
{
    //kDebug();
    if ( linkType == Relation::FinishStart ||
            linkType == Relation::StartStart ||
            linkType == Relation::FinishFinish ) {
        getPart() ->addCommand( new ModifyRelationTypeCmd( rel, static_cast<Relation::Type>( linkType ) ) );
    } else {
        slotModifyRelation( rel );
    }
}

void View::slotModifyRelation()
{
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v == 0 ) {
        return;
    }
    Relation *rel = v->currentRelation();
    if ( rel ) {
        slotModifyRelation( rel );
    }
}

void View::slotDeleteRelation()
{
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v == 0 ) {
        return;
    }
    Relation *rel = v->currentRelation();
    if ( rel ) {
        getPart()->addCommand( new DeleteRelationCmd( getProject(), rel, i18n( "Delete Task Dependency" ) ) );
    }
}

void View::slotAddResource( ResourceGroup *group )
{
    //kDebug();
    if ( group == 0 ) {
        return;
    }
    Resource *r = new Resource();
    ResourceDialog *dia = new ResourceDialog( getProject(), r );
    if ( dia->exec()  == QDialog::Accepted) {
        MacroCommand *m = new MacroCommand( i18n( "Add resource" ) );
        m->addCommand( new AddResourceCmd( group, r ) );
        QUndoCommand * cmd = dia->buildCommand();
        if ( cmd ) {
            m->addCommand( cmd );
        }
        getPart()->addCommand( m );
        delete dia;
        return;
    }
    delete r;
    delete dia;
}

void View::slotEditResource()
{
    //kDebug();
    Resource * r = currentResource();
    if ( r == 0 ) {
        return ;
    }
    ResourceDialog *dia = new ResourceDialog( getProject(), r );
    if ( dia->exec()  == QDialog::Accepted) {
        QUndoCommand * cmd = dia->buildCommand();
        if ( cmd )
            getPart() ->addCommand( cmd );
    }
    delete dia;
}

void View::slotDeleteResource( Resource *resource )
{
    getPart()->addCommand( new RemoveResourceCmd( resource->parentGroup(), resource, i18n( "Delete Resource" ) ) );
}

void View::slotDeleteResourceGroup( ResourceGroup *group )
{
    getPart()->addCommand( new RemoveResourceGroupCmd( group->project(), group, i18n( "Delete Resourcegroup" ) ) );
}

void View::slotDeleteResourceObjects( QObjectList lst )
{
    //kDebug();
    foreach ( QObject *o, lst ) {
        Resource *r = qobject_cast<Resource*>( o );
        if ( r && r->isScheduled() ) {
            int res = KMessageBox::warningContinueCancel( this, i18n( "A resource that has been scheduled will be deleted. This will invalidate the schedule." ) );
            if ( res == KMessageBox::Cancel ) {
                return;
            }
            break;
        }
        ResourceGroup *g = qobject_cast<ResourceGroup*>( o );
        if ( g && g->isScheduled() ) {
            int res = KMessageBox::warningContinueCancel( this, i18n( "A resource that has been scheduled will be deleted. This will invalidate the schedule." ) );
            if ( res == KMessageBox::Cancel ) {
                return;
            }
            break;
        }
    }
    if ( lst.count() == 1 ) {
        Resource *r = qobject_cast<Resource*>( lst.first() );
        if ( r ) {
            slotDeleteResource( r );
        } else {
            ResourceGroup *g = qobject_cast<ResourceGroup*>( lst.first() );
            if ( g ) {
                slotDeleteResourceGroup( g );
            }
        }
        return;
    }
    int num = 0;
    MacroCommand *cmd = 0, *rc = 0, *gc = 0;
    foreach ( QObject *o, lst ) {
        Resource *r = qobject_cast<Resource*>( o );
        if ( r ) {
            if ( rc == 0 )  rc = new MacroCommand( "" );
            rc->addCommand( new RemoveResourceCmd( r->parentGroup(), r ) );
            continue;
        }
        ResourceGroup *g = qobject_cast<ResourceGroup*>( o );
        if ( g ) {
            if ( gc == 0 )  gc = new MacroCommand( "" );
            gc->addCommand( new RemoveResourceGroupCmd( g->project(), g ) );
        }
    }
    if ( rc || gc ) {
        cmd = new MacroCommand( i18n( "Delete Resource Objects" ) );
    }
    if ( rc )
        cmd->addCommand( rc );
    if ( gc )
        cmd->addCommand( gc );
    if ( cmd )
        getPart()->addCommand( cmd );
}


void View::updateReadWrite( bool readwrite )
{
    m_readWrite = readwrite;
    m_viewlist->setReadWrite( readwrite );
}

Part *View::getPart() const
{
    return ( Part * ) koDocument();
}

void View::slotConnectNode()
{
    //kDebug();
    /*    NodeItem *curr = ganttview->currentItem();
        if (curr) {
            kDebug()<<"node="<<curr->getNode().name();
        }*/
}

QMenu * View::popupMenu( const QString& name )
{
    //kDebug();
    Q_ASSERT( factory() );
    if ( factory() )
        return ( ( QMenu* ) factory() ->container( name, this ) );
    return 0L;
}

void View::slotUpdate()
{
    //kDebug()<<"calculate="<<calculate;

//    m_updateResourceview = true;
    m_updateResourceAssignmentView = true;
    m_updatePertEditor = true;
    updateView( m_tab->currentWidget() );
}

void View::slotGuiActivated( ViewBase *view, bool activate )
{
    //FIXME: Avoid unplug if possible, it flashes the gui
    // always unplug, in case they already are plugged
    foreach( QString name, view->actionListNames() ) {
        //kDebug()<<"deactivate"<<name;
        unplugActionList( name );
    }
    if ( activate ) {
        foreach( QString name, view->actionListNames() ) {
            //kDebug()<<"activate"<<name<<","<<view->actionList( name ).count();
            plugActionList( name, view->actionList( name ) );
        }
    }
}

void View::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    kDebug()<<ev->activated();
    KoView::guiActivateEvent( ev );
    if ( ev->activated() ) {
        // plug my own actionlists, they may be gone
        slotPlugScheduleActions();
    }
    // propagate to sub-view
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v ) {
        v->setGuiActive( ev->activated() );
    }
}

KoDocument *View::hitTest( const QPoint &pos )
{
    //TODO: test this with embedded koffice parts
    //kDebug()<<pos;
    // pos is in m_tab->currentWidget() coordinates
    QPoint gl = m_tab->currentWidget()->mapToGlobal(pos);
    kDebug()<<pos<<gl;
    if ( m_tab->currentWidget()->frameGeometry().contains( m_tab->currentWidget()->mapFromGlobal( gl ) ) ) {
        // Check if own subview
        ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
        if ( v ) {
            kDebug()<<"Hit on:"<<v;
            v = v->hitView( gl );
            v->setGuiActive( true );
            return koDocument();
        }
        SplitterView *sp = dynamic_cast<SplitterView*>( m_tab->currentWidget() );
        if ( sp ) {
            // Check which subview has actually been hit (can aslo be the splitter)
            v = sp->findView( pos );
            if ( v ) {
                kDebug()<<"Hit on:"<<sp<<" -> "<<v;
                v->hitView( gl );
                v->setGuiActive( true );
                return koDocument();
            }
        }
    }
    // check child documents
    return koDocument()->hitTest( pos, this );
}

void View::createChildDocumentViews()
{
    foreach ( KoDocumentChild *ch, getPart()->children() ) {
        if ( ! ch->isDeleted() ) {
            DocumentChild *c = static_cast<DocumentChild*>( ch );
            QTreeWidgetItem *cat = m_viewlist->findItem( c->category(), 0 );
            if ( cat == 0 ) {
                kDebug()<<"New category: Documents";
                cat = m_viewlist->addCategory( "Documents", i18n( "Documents" ) );
                cat->setIcon( 0, KIcon( "koshell" ) );
            }
            ViewListItem *i = createChildDocumentView( c );
            cat->insertChild( cat->childCount(), i );
        }
    }
}

ViewListItem *View::createChildDocumentView( DocumentChild *ch )
{
    kDebug()<<ch->title();
    KoDocument *doc = ch->document();

    QString title = ch->title();
    if ( title.isEmpty() && doc->documentInfo() ) {
        title = doc->documentInfo()->aboutInfo( "title" );
    }
    if ( title.isEmpty() ) {
        title = doc->url().pathOrUrl();
    }
    if ( title.isEmpty() ) {
        title = "Untitled";
    }
    KoView *v = doc->createView( this );
    ch->setGeometry( geometry(), true );
    m_tab->addWidget( v );
    ViewListItem *i = m_viewlist->createChildDocumentView( doc->objectName(), title, v, ch );
    if ( ! ch->icon().isEmpty() ) {
        i->setIcon( 0, KIcon( ch->icon() ) );
    }
    return i;
}

void View::slotViewListItemRemoved( ViewListItem *item )
{
    if ( item->documentChild() ) {
        // This restores basic ui
        item->documentChild()->setActivated( false, this );
    }
    m_tab->removeWidget( item->view() );
    if ( item->type() == ViewListItem::ItemType_SubView ) {
        delete item->view();
        delete item;
    }
}

void View::slotViewListItemInserted( ViewListItem *item )
{
    m_tab->addWidget( item->view() );
}

void View::slotCreateView()
{
    ViewListDialog dlg( this, *m_viewlist );
    dlg.exec();
}

void View::slotCreateKofficeDocument( KoDocumentEntry &entry)
{
    QString e;
    KoDocument *doc = entry.createDoc( &e, getPart() );
    if ( doc == 0 ) {
        return;
    }
    if ( ! doc->showEmbedInitDialog( this ) ) {
        delete doc;
        return;
    }
    QTreeWidgetItem *cat = m_viewlist->addCategory( "Documents", i18n( "Documents" ) );
    cat->setIcon( 0, KIcon( "koshell" ) );

    DocumentChild *ch = getPart()->createChild( doc );
    ch->setIcon( entry.service()->icon() );
    ViewListItem *i = createChildDocumentView( ch );
    getPart()->addCommand( new InsertEmbeddedDocumentCmd( m_viewlist, i, cat, i18n( "Insert Document" ) ) );
    m_viewlist->setSelected( i );
}

void View::slotViewActivated( ViewListItem *item, ViewListItem *prev )
{
    //kDebug() <<"item=" << item <<","<<prev;
    if ( prev && prev->type() != ViewListItem::ItemType_ChildDocument ) {
        // Remove sub-view specific gui
        //kDebug()<<"Deactivate:"<<prev;
        ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
        if ( v ) {
            v->setGuiActive( false );
        }
    }
    if ( item->type() == ViewListItem::ItemType_SubView ) {
        //kDebug()<<"Activate:"<<item;
        m_tab->setCurrentWidget( item->view() );
        if (  prev && prev->type() != ViewListItem::ItemType_SubView ) {
            // Put back my own gui (removed when (if) viewing different doc)
            getPart()->activate( this );
        }
        // Add sub-view specific gui
        ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
        if ( v ) {
            v->setGuiActive( true );
        }
        return;
    }
    if ( item->type() == ViewListItem::ItemType_ChildDocument ) {
        //kDebug()<<"Activated:"<<item->view();
        // changing doc also takes care of all gui
        m_tab->setCurrentWidget( item->view() );
        item->documentChild()->setActivated( true, item->view() );
        return;
    }
}

QWidget *View::canvas() const
{
    return m_tab->currentWidget();//KoView::canvas();
}

void View::slotCurrentChanged( int )
{
    kDebug()<<m_tab->currentIndex();
    ViewListItem *item = m_viewlist->findItem( m_tab->currentWidget() );
    if ( item == 0 ) {
        return;
    }
    kDebug()<<item->text(0);
    item->setSelected( true );
}

void View::updateView( QWidget * )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    //setScheduleActionsEnabled();

    QWidget *widget2;

    widget2 = m_viewlist->findView( "ResourceAssignmentView" );
    if ( widget2 && m_updateResourceAssignmentView )
        static_cast<ViewBase*>( widget2 ) ->draw( getProject() );
    m_updateResourceAssignmentView = false;

    QApplication::restoreOverrideCursor();
}

void View::slotRenameNode( Node *node, const QString& name )
{
    //kDebug()<<name;
    if ( node ) {
        NodeModifyNameCmd * cmd = new NodeModifyNameCmd( *node, name, i18n( "Modify Name" ) );
        getPart() ->addCommand( cmd );
    }
}

void View::slotPopupMenu( const QString& menuname, const QPoint & pos )
{
    QMenu * menu = this->popupMenu( menuname );
    if ( menu ) {
        //kDebug()<<menu<<":"<<menu->actions().count();
        ViewBase *v = qobject_cast<ViewBase*>( m_tab->currentWidget() );
        kDebug()<<v<<menuname;
        QList<QAction*> lst;
        if ( v ) {
            lst = v->contextActionList();
            kDebug()<<lst;
            if ( ! lst.isEmpty() ) {
                menu->addSeparator();
                foreach ( QAction *a, lst ) {
                    menu->addAction( a );
                }
            }
        }
        menu->exec( pos );
        foreach ( QAction *a, lst ) {
            menu->removeAction( a );
        }
    }
}

void View::slotPopupMenu( const QString& menuname, const QPoint &pos, ViewListItem *item )
{
    //kDebug()<<menuname;
    m_viewlistItem = item;
    slotPopupMenu( menuname, pos );
}

bool View::loadContext()
{
    kDebug();
    Context *ctx = getPart()->context();
    if ( ctx == 0 || ! ctx->isLoaded() ) {
        return true;
    }
    KoXmlElement n = ctx->context();
    QString cv = n.attribute( "current-view" );
    if ( ! cv.isEmpty() ) {
        m_viewlist->setSelected( m_viewlist->findItem( cv ) );
    } else kDebug()<<"No current view";
    
    long id = n.attribute( "current-schedule", "-1" ).toLong();
    if ( id != -1 ) {
        setActiveSchedule( id );
    } else kDebug()<<"No current schedule";

    return true;
}

void View::saveContext( QDomElement &me ) const
{
    //kDebug();
    long id = activeScheduleId();
    if ( id != -1 ) {
        me.setAttribute( "current-schedule", (qlonglong)id );
    }
    ViewListItem *item = m_viewlist->findItem( m_tab->currentWidget() );
    if ( item ) {
        me.setAttribute("current-view", item->tag() );
    }
    m_viewlist->save( me );
}

bool View::loadWorkPackage( Project &project, const KUrl &url )
{
    return getPart()->loadWorkPackage( project, url );
}

void View::setLabel( ScheduleManager *sm )
{
    //kDebug();
    Schedule *s = sm == 0 ? 0 : sm->expected();
    if ( s && !s->isDeleted() && s->isScheduled() ) {
        m_estlabel->setText( sm->name() );
        return;
    }
    m_estlabel->setText( i18n( "Not scheduled" ) );
}

#ifndef NDEBUG
void View::slotPrintDebug()
{
    kDebug() <<"-------- Debug printout: Node list";
    /*    Node *curr = ganttview->currentNode();
        if (curr) {
            curr->printDebug(true,"");
        } else*/
    getPart() ->getProject().printDebug( true, "" );
}
void View::slotPrintSelectedDebug()
{

    if ( m_tab->currentWidget() == m_viewlist->findView( "TaskEditor" ) ) {
        Node *curr = static_cast<ViewBase*>( m_tab->currentWidget() )->currentNode();
        if ( curr ) {
            kDebug() <<"-------- Debug printout: Selected node";
            curr->printDebug( true, "" );
        } else
            slotPrintDebug();
        return;
    } else if ( m_tab->currentWidget() == m_viewlist->findView( "ResourceEditor" ) ) {
        Resource *r = static_cast<ViewBase*>( m_tab->currentWidget() )->currentResource();
        if ( r ) {
            kDebug() <<"-------- Debug printout: Selected resource";
            r->printDebug("  !");
            return;
        }
        ResourceGroup *g = static_cast<ViewBase*>( m_tab->currentWidget() )->currentResourceGroup();
        if ( g ) {
            kDebug() <<"-------- Debug printout: Selected group";
            g->printDebug("  !");
            return;
        }
    }
    slotPrintDebug();
}
void View::slotPrintCalendarDebug()
{
    //kDebug() <<"-------- Debug printout: Calendars";
    /*    Node *curr = ganttview->currentNode();
        if (curr) {
            curr->printDebug(true,"");
        } else*/
    getPart() ->getProject().printCalendarDebug( "" );
}
void View::slotPrintTestDebug()
{
    const QStringList & lst = getPart() ->xmlLoader().log();

    for ( QStringList::ConstIterator it = lst.constBegin(); it != lst.constEnd(); ++it ) {
        kDebug() << *it;
    }
    //     kDebug()<<"------------Test 1---------------------";
    //     {
    //     DateTime d1(QDate(2006,1,2), QTime(8,0,0));
    //     DateTime d2 = d1.addSecs(3600);
    //     Duration d = d2 - d1;
    //     bool b = d==Duration(0,0,0,3600);
    //     kDebug()<<"1: Success="<<b<<""<<d2.toString()<<"-"<<d1.toString()<<"="<<d.toString();
    //     d = d1 - d2;
    //     b = d==Duration(0,0,0,3600);
    //     kDebug()<<"2: Success="<<b<<""<<d1.toString()<<"-"<<d2.toString()<<"="<<d.toString();
    //     d2 = d2.addDays(-2);
    //     d = d1 - d2;
    //     b = d==Duration(2,0,0)-Duration(0,0,0,3600);
    //     kDebug()<<"3: Success="<<b<<""<<d1.toString()<<"-"<<d2.toString()<<"="<<d.toString();
    //     d = d2 - d1;
    //     b = d==Duration(2,0,0)-Duration(0,0,0,3600);
    //     kDebug()<<"4: Success="<<b<<""<<d2.toString()<<"-"<<d1.toString()<<"="<<d.toString();
    //     kDebug();
    //     b = (d2 + d)==d1;
    //     kDebug()<<"5: Success="<<b<<""<<d2<<"+"<<d.toString()<<"="<<d1;
    //     b = (d1 - d)==d2;
    //     kDebug()<<"6: Success="<<b<<""<<d1<<"-"<<d.toString()<<"="<<d2;
    //     } // end test 1
    //     kDebug();
    //     kDebug()<<"------------Test 2 Single calendar-----------------";
    //     {
    //     Calendar *t = new Calendar("Test 2");
    //     QDate wdate(2006,1,2);
    //     DateTime before = DateTime(wdate.addDays(-1));
    //     DateTime after = DateTime(wdate.addDays(1));
    //     QTime t1(8,0,0);
    //     QTime t2(10,0,0);
    //     DateTime wdt1(wdate, t1);
    //     DateTime wdt2(wdate, t2);
    //     CalendarDay *day = new CalendarDay(QDate(2006,1,2), CalendarDay::Working);
    //     day->addInterval(TimeInterval(t1, t2));
    //     if (!t->addDay(day)) {
    //         kDebug()<<"Failed to add day";
    //         delete day;
    //         delete t;
    //         return;
    //     }
    //     kDebug()<<"Added     date="<<day->date().toString()<<""<<day->startOfDay().toString()<<" -"<<day->endOfDay();
    //     kDebug()<<"Found     date="<<day->date().toString()<<""<<day->startOfDay().toString()<<" -"<<day->endOfDay();
    //
    //     CalendarDay *d = t->findDay(wdate);
    //     bool b = (day == d);
    //     kDebug()<<"1: Success="<<b<<"      Find same day";
    //
    //     DateTime dt = t->firstAvailableAfter(after, after.addDays(10));
    //     b = !dt.isValid();
    //     kDebug()<<"2: Success="<<b<<"      firstAvailableAfter("<<after<<"): ="<<dt;
    //
    //     dt = t->firstAvailableBefore(before, before.addDays(-10));
    //     b = !dt.isValid();
    //     kDebug()<<"3: Success="<<b<<"       firstAvailableBefore("<<before.toString()<<"): ="<<dt;
    //
    //     dt = t->firstAvailableAfter(before, after);
    //     b = dt == wdt1;
    //     kDebug()<<"4: Success="<<b<<"      firstAvailableAfter("<<before<<"): ="<<dt;
    //
    //     dt = t->firstAvailableBefore(after, before);
    //     b = dt == wdt2;
    //     kDebug()<<"5: Success="<<b<<"      firstAvailableBefore("<<after<<"): ="<<dt;
    //
    //     b = t->hasInterval(before, after);
    //     kDebug()<<"6: Success="<<b<<"      hasInterval("<<before<<","<<after<<")";
    //
    //     b = !t->hasInterval(after, after.addDays(1));
    //     kDebug()<<"7: Success="<<b<<"      !hasInterval("<<after<<","<<after.addDays(1)<<")";
    //
    //     b = !t->hasInterval(before, before.addDays(-1));
    //     kDebug()<<"8: Success="<<b<<"      !hasInterval("<<before<<","<<before.addDays(-1)<<")";
    //
    //     Duration e1(0, 2, 0); // 2 hours
    //     Duration e2 = t->effort(before, after);
    //     b = e1==e2;
    //     kDebug()<<"9: Success="<<b<<"      effort"<<e1.toString()<<" ="<<e2.toString();
    //
    //     delete t;
    //     }// end test 2
    //
    //     kDebug();
    //     kDebug()<<"------------Test 3 Parent calendar-----------------";
    //     {
    //     Calendar *t = new Calendar("Test 3");
    //     Calendar *p = new Calendar("Test 3 parent");
    //     t->setParent(p);
    //     QDate wdate(2006,1,2);
    //     DateTime before = DateTime(wdate.addDays(-1));
    //     DateTime after = DateTime(wdate.addDays(1));
    //     QTime t1(8,0,0);
    //     QTime t2(10,0,0);
    //     DateTime wdt1(wdate, t1);
    //     DateTime wdt2(wdate, t2);
    //     CalendarDay *day = new CalendarDay(QDate(2006,1,2), CalendarDay::Working);
    //     day->addInterval(TimeInterval(t1, t2));
    //     if (!p->addDay(day)) {
    //         kDebug()<<"Failed to add day";
    //         delete day;
    //         delete t;
    //         return;
    //     }
    //     kDebug()<<"Added     date="<<day->date().toString()<<""<<day->startOfDay().toString()<<" -"<<day->endOfDay().toString();
    //     kDebug()<<"Found     date="<<day->date().toString()<<""<<day->startOfDay().toString()<<" -"<<day->endOfDay().toString();
    //
    //     CalendarDay *d = p->findDay(wdate);
    //     bool b = (day == d);
    //     kDebug()<<"1: Success="<<b<<"      Find same day";
    //
    //     DateTime dt = t->firstAvailableAfter(after, after.addDays(10));
    //     b = !dt.isValid();
    //     kDebug()<<"2: Success="<<b<<"      firstAvailableAfter("<<after.toString()<<"): ="<<!b;
    //
    //     dt = t->firstAvailableBefore(before, before.addDays(-10));
    //     b = !dt.isValid();
    //     kDebug()<<"3: Success="<<b<<"       firstAvailableBefore("<<before.toString()<<"): ="<<!b;
    //
    //     dt = t->firstAvailableAfter(before, after);
    //     b = dt == wdt1;
    //     kDebug()<<"4: Success="<<b<<"      firstAvailableAfter("<<before.toString()<<"): ="<<dt.toString();
    //
    //     dt = t->firstAvailableBefore(after, before);
    //     b = dt == wdt2;
    //     kDebug()<<"5: Success="<<b<<"      firstAvailableBefore("<<after.toString()<<"): ="<<dt.toString();
    //
    //     b = t->hasInterval(before, after);
    //     kDebug()<<"6: Success="<<b<<"      hasInterval("<<before.toString()<<","<<after<<")";
    //
    //     b = !t->hasInterval(after, after.addDays(1));
    //     kDebug()<<"7: Success="<<b<<"      !hasInterval("<<after.toString()<<","<<after.addDays(1)<<")";
    //
    //     b = !t->hasInterval(before, before.addDays(-1));
    //     kDebug()<<"8: Success="<<b<<"      !hasInterval("<<before.toString()<<","<<before.addDays(-1)<<")";
    //     Duration e1(0, 2, 0); // 2 hours
    //     Duration e2 = t->effort(before, after);
    //     b = e1==e2;
    //     kDebug()<<"9: Success="<<b<<"      effort"<<e1.toString()<<"=="<<e2.toString();
    //
    //     delete t;
    //     delete p;
    //     }// end test 3
    //     kDebug();
    //     kDebug()<<"------------Test 4 Parent calendar/weekdays-------------";
    //     {
    //     QTime t1(8,0,0);
    //     QTime t2(10,0,0);
    //     Calendar *p = new Calendar("Test 4 parent");
    //     CalendarDay *wd1 = p->weekday(0); // monday
    //     if (wd1 == 0) {
    //         kDebug()<<"Failed to get weekday";
    //     }
    //     wd1->setState(CalendarDay::NonWorking);
    //
    //     CalendarDay *wd2 = p->weekday(2); // wednesday
    //     if (wd2 == 0) {
    //         kDebug()<<"Failed to get weekday";
    //     }
    //     wd2->addInterval(TimeInterval(t1, t2));
    //     wd2->setState(CalendarDay::Working);
    //
    //     Calendar *t = new Calendar("Test 4");
    //     t->setParent(p);
    //     QDate wdate(2006,1,2); // monday jan 2
    //     DateTime before = DateTime(wdate.addDays(-4)); //Thursday dec 29
    //     DateTime after = DateTime(wdate.addDays(4)); // Friday jan 6
    //     DateTime wdt1(wdate, t1);
    //     DateTime wdt2(QDate(2006, 1, 4), t2); // Wednesday
    //     CalendarDay *day = new CalendarDay(QDate(2006,1,2), CalendarDay::Working);
    //     day->addInterval(TimeInterval(t1, t2));
    //     if (!p->addDay(day)) {
    //         kDebug()<<"Failed to add day";
    //         delete day;
    //         delete t;
    //         return;
    //     }
    //     kDebug()<<"Added     date="<<day->date().toString()<<""<<day->startOfDay().toString()<<" -"<<day->endOfDay().toString();
    //     kDebug()<<"Found     date="<<day->date().toString()<<""<<day->startOfDay().toString()<<" -"<<day->endOfDay().toString();
    //
    //     CalendarDay *d = p->findDay(wdate);
    //     bool b = (day == d);
    //     kDebug()<<"1: Success="<<b<<"      Find same day";
    //
    //     DateTime dt = t->firstAvailableAfter(after, after.addDays(10));
    //     b = (dt.isValid() && dt == DateTime(QDate(2006,1,11), t1));
    //     kDebug()<<"2: Success="<<b<<"      firstAvailableAfter("<<after<<"): ="<<dt;
    //
    //     dt = t->firstAvailableBefore(before, before.addDays(-10));
    //     b = (dt.isValid() && dt == DateTime(QDate(2005, 12, 28), t2));
    //     kDebug()<<"3: Success="<<b<<"       firstAvailableBefore("<<before.toString()<<"): ="<<dt;
    //
    //     dt = t->firstAvailableAfter(before, after);
    //     b = dt == wdt1; // We find the day jan 2
    //     kDebug()<<"4: Success="<<b<<"      firstAvailableAfter("<<before.toString()<<"): ="<<dt.toString();
    //
    //     dt = t->firstAvailableBefore(after, before);
    //     b = dt == wdt2; // We find the weekday (wednesday)
    //     kDebug()<<"5: Success="<<b<<"      firstAvailableBefore("<<after.toString()<<"): ="<<dt.toString();
    //
    //     b = t->hasInterval(before, after);
    //     kDebug()<<"6: Success="<<b<<"      hasInterval("<<before.toString()<<","<<after<<")";
    //
    //     b = !t->hasInterval(after, after.addDays(1));
    //     kDebug()<<"7: Success="<<b<<"      !hasInterval("<<after.toString()<<","<<after.addDays(1)<<")";
    //
    //     b = !t->hasInterval(before, before.addDays(-1));
    //     kDebug()<<"8: Success="<<b<<"      !hasInterval("<<before.toString()<<","<<before.addDays(-1)<<")";
    //     Duration e1(0, 4, 0); // 2 hours
    //     Duration e2 = t->effort(before, after);
    //     b = e1==e2;
    //     kDebug()<<"9: Success="<<b<<"      effort"<<e1.toString()<<"="<<e2.toString();
    //
    //     DateTimeInterval r = t->firstInterval(before, after);
    //     b = r.first == wdt1; // We find the monday jan 2
    //     kDebug()<<"10: Success="<<b<<"      firstInterval("<<before<<"): ="<<r.first<<","<<r.second;
    //     r = t->firstInterval(r.second, after);
    //     b = r.first == DateTime(QDate(2006, 1, 4),t1); // We find the wednesday jan 4
    //     kDebug()<<"11: Success="<<b<<"      firstInterval("<<r.second<<"): ="<<r.first<<","<<r.second;
    //
    //     delete t;
    //     delete p;
    //     }// end test 4
}
#endif

}  //KPlato namespace

#include "kptview.moc"
