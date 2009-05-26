/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <koffice-devel@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KPTTASKNOTESPANEL_H
#define KPTTASKNOTESPANEL_H

#include "kplatoui_export.h"

#include "ui_kpttasknotespanelbase.h"

#include <QWidget>

#include <KDialog>

namespace KPlato
{

class TaskNotesPanel;
class Task;
class MacroCommand;
        
class TaskNotesPanelImpl : public QWidget, public Ui_TaskNotesPanelBase
{
    Q_OBJECT
public:
    TaskNotesPanelImpl( Task &task, QWidget *parent );
        
public slots:
    virtual void slotChanged();

signals:
    void textChanged( bool );

protected:
    Task &m_task;
};

class TaskNotesPanel : public TaskNotesPanelImpl
{
    Q_OBJECT
public:
    explicit TaskNotesPanel( Task &task, QWidget *parent=0 );

    MacroCommand *buildCommand();

    bool ok();

    void setStartValues( Task &task );

};

class KPLATOUI_EXPORT TaskNotesDialog : public KDialog
{
    Q_OBJECT
public:
    /**
     * The constructor for the task settings dialog.
     * @param task the task to show
     * @param parent parent widget
     */
    TaskNotesDialog( Task &task, QWidget *parent=0 );

    MacroCommand *buildCommand();

protected slots:
    void slotButtonClicked( int button );

protected:
    TaskNotesPanel *m_notesTab;
};

} //KPlato namespace

#endif // KPTTASKNOTESPANEL_H
