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

#ifndef KPLATO_VIEW
#define KPLATO_VIEW

#include "kplato_export.h"

#include <KoView.h>
#include <KoQueryTrader.h>

#include "kptcontext.h"

#include <QMenu>
#include <QDockWidget>
#include <QTimer>

class QProgressBar;
class QStackedWidget;
class QSplitter;

class KAction;
class KToggleAction;
class QLabel;

class KoView;

namespace KPlato
{

class View;
class ViewBase;
class ViewListItem;
class ViewListWidget;
class AccountsView;
class GanttView;
class PertEditor;
class ResourceView;
class AccountsEditor;
class TaskEditor;
class CalendarEditor;
class ScheduleEditor;
class ScheduleManager;

class ResourceAssignmentView;
class TaskStatusView;
class Calendar;
class Part;
class DocumentChild;
class Node;
class Project;
class MainSchedule;
class Schedule;
class Resource;
class ResourceGroup;
class Relation;
class Context;
class ViewAdaptor;


//-------------
class KPLATO_EXPORT View : public KoView
{
    Q_OBJECT

public:
    explicit View( Part* part, QWidget* parent = 0 );
    ~View();
    /**
     * Support zooming.
     */
    virtual void setZoom( double zoom );

    Part *getPart() const;

    Project& getProject() const;

    QMenu *popupMenu( const QString& name );

    virtual ViewAdaptor* dbusObject();

    virtual bool loadContext();
    virtual void saveContext( QDomElement &context ) const;

    /// Load the workpackage from @p url into @p project. Return true if successful, else false.
    bool loadWorkPackage( Project &project, const KUrl &url );
    
    QWidget *canvas() const;

    //virtual QDockWidget *createToolBox();

    KoDocument *hitTest( const QPoint &viewPos );

    ScheduleManager *currentScheduleManager() const;
    long activeScheduleId() const;
    void setActiveSchedule( long id ) const;
    
    ViewBase *createTaskEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createResourcEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createAccountsEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createCalendarEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createScheduleHandler( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ScheduleEditor *createScheduleEditor( QWidget *parent );
    ViewBase *createScheduleEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createDependencyEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createPertEditor( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createProjectStatusView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createPerformanceStatusView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createTaskStatusView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createTaskView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createGanttView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createMilestoneGanttView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createResourceAppointmentsView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createAccountsView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createResourceAssignmentView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );
    ViewBase *createChartView( ViewListItem *cat, const QString tag, const QString &name, const QString &tip, int index = -1 );

    KoPrintJob * createPrintJob();
    
signals:
    void currentScheduleManagerChanged( ScheduleManager *sm );
    
public slots:
    void slotUpdate();
    void slotCreateTemplate();
    void slotAddResource( ResourceGroup *group );
    void slotEditResource();
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotViewSelector( bool show );

    void slotAddTask();
    void slotAddSubTask();
    void slotAddMilestone();
    void slotProjectEdit();
    void slotDefineWBS();

    void slotCreateView();
    void slotCreateKofficeDocument( KoDocumentEntry& );

    void slotConfigure();
    void slotAddRelation( Node *par, Node *child );
    void slotModifyRelation( Relation *rel );
    void slotAddRelation( Node *par, Node *child, int linkType );
    void slotModifyRelation( Relation *rel, int linkType );
    void slotModifyRelation();
    void slotDeleteRelation();

    void slotRenameNode( Node *node, const QString& name );

    void slotPopupMenu( const QString& menuname, const QPoint &pos );
    void slotPopupMenu( const QString& menuname, const QPoint &pos, ViewListItem *item );

protected slots:
    void slotGuiActivated( ViewBase *view, bool );
    void slotViewActivated( ViewListItem*, ViewListItem* );
    void slotPlugScheduleActions();
    void slotViewSchedule( QAction *act );
    void slotScheduleChanged( MainSchedule* );
    void slotScheduleAdded( const MainSchedule * );
    void slotScheduleRemoved( const MainSchedule * );

    void slotAddScheduleManager( Project *project );
    void slotDeleteScheduleManager( Project *project, ScheduleManager *sm );
    void slotCalculateSchedule( Project*, ScheduleManager* );
    void slotBaselineSchedule( Project *project, ScheduleManager *sm );
    void slotProgressChanged( int value );

    void slotProjectWorktime();

    void slotOpenNode();
    void slotOpenNode( Node *node );
    void slotTaskProgress();
    void slotDeleteTask( QList<Node*> lst );
    void slotDeleteTask( Node *node );
    void slotDeleteTask();
    void slotIndentTask();
    void slotUnindentTask();
    void slotMoveTaskUp();
    void slotMoveTaskDown();
    void slotTaskWorkpackage();
    
    void slotConnectNode();

    void slotDeleteResource( Resource *resource );
    void slotDeleteResourceGroup( ResourceGroup *group );
    void slotDeleteResourceObjects( QObjectList );

    void slotCurrentChanged( int );

    void removeProgressBarItems();

#ifndef NDEBUG
    void slotPrintDebug();
    void slotPrintSelectedDebug();
    void slotPrintCalendarDebug();
    void slotPrintTestDebug();
#else
    static void slotPrintDebug() { };
    static void slotPrintSelectedDebug() { };
    static void slotPrintCalendarDebug() { };
    static void slotPrintTestDebug() { };
#endif

protected:
    virtual void guiActivateEvent( KParts::GUIActivateEvent *event );
    virtual void updateReadWrite( bool readwrite );

    QList<QAction*> sortedActionList();
    QAction *addScheduleAction( Schedule *sch );
    void setLabel( ScheduleManager *sm = 0 );
    Node *currentTask();
    Resource *currentResource();
    ResourceGroup *currentResourceGroup();
    Calendar *currentCalendar();
    void updateView( QWidget *widget );

private slots:
    void slotActionDestroyed( QObject *o );
    void slotViewListItemRemoved( ViewListItem *item );
    void slotViewListItemInserted( ViewListItem *item );

private:
    void createViews();
    

    void createChildDocumentViews();
    ViewListItem *createChildDocumentView( DocumentChild *ch );

private:
    QSplitter *m_sp;
    QStackedWidget *m_tab;

    ViewListWidget *m_viewlist;
    ViewListItem *m_viewlistItem; // requested popupmenu item

    //QDockWidget *m_toolbox;

    int m_viewGrp;
    int m_defaultFontSize;
    int m_currentEstimateType;

    bool m_updateAccountsview;
    bool m_updateResourceAssignmentView;
    bool m_updatePertEditor;

    QLabel *m_estlabel;
    QProgressBar *m_progress;
    QLabel *m_text;
    QTimer m_progressBarTimer;

    ViewAdaptor* m_dbus;

    QActionGroup *m_scheduleActionGroup;
    QMap<QAction*, Schedule*> m_scheduleActions;
    
    bool m_readWrite;
    
    // ------ File
    QAction *actionCreateTemplate;
    
    // ------ Edit
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;

    // ------ View
    KToggleAction *actionViewSelector;

    KToggleAction *actionViewResourceAppointments;

    // ------ Insert

    // ------ Project
    KAction *actionEditMainProject;
    KAction *actionEditStandardWorktime;

    // ------ Tools
    KAction *actionDefineWBS;

    // ------ Settings
    KAction *actionConfigure;

    // ------ Popup
    KAction *actionOpenNode;
    KAction *actionTaskProgress;
    KAction *actionDeleteTask;
    KAction *actionTaskWorkpackage;
    KAction *actionIndentTask;
    KAction *actionUnindentTask;
    KAction *actionMoveTaskUp;
    KAction *actionMoveTaskDown;

    KAction *actionEditResource;
    KAction *actionEditRelation;
    KAction *actionDeleteRelation;
    
    //Test
    KAction *actNoInformation;

};

} //Kplato namespace

#endif
