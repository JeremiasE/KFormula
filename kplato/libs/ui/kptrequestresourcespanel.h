/* This file is part of the KDE project
   Copyright (C) 2003 - 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTREQUESTRESOURCESPANEL_H
#define KPTREQUESTRESOURCESPANEL_H

#include "kplatoui_export.h"

#include "ui_kpttaskresourcespanelbase.h"
#include "kptduration.h"

#include <QTreeWidgetItem>
#include <QString>
#include <QTableWidget>
#include <QList>

class QTableWidgetItem;


namespace KPlato
{

class Account;
class Task;
class ResourceGroup;
class Resource;
class ResourceGroupRequest;
class ResourceRequest;
class StandardWorktime;
class MacroCommand;

class ResourceTableItem {
public:
    ResourceTableItem(Resource *resource, ResourceRequest *request, bool check = false);
    ~ResourceTableItem() ;

    void update();
    void insert(QTableWidget *table, int row);
    void ok(ResourceGroupRequest *group);

    bool isChecked() const { return m_checked != Qt::Unchecked; }
    bool isOrigChecked() const { return m_origChecked != Qt::Unchecked; }
    Resource *resource() { return m_resource; }
    ResourceRequest *request() { return m_request; }
    int numRequests() const { return isChecked() ? 1 : 0; }
    int units() const { return m_units; }

    Resource *m_resource;
    int m_units, m_origUnits;
    Qt::CheckState m_checked, m_origChecked;
    QTableWidgetItem *m_checkitem;
    ResourceRequest *m_request;
    int m_curAccountItem;
    QString m_curAccountText;
};

class GroupLVItem : public QTreeWidgetItem {
public:
    GroupLVItem(QTreeWidget *parent, ResourceGroup *group, Task &task);
    ~GroupLVItem();

    void update();
    void insert(QTableWidget *table);
    const QList<ResourceTableItem*> &resources() const { return m_resources; }
    void ok(Task &task);

    int numRequests();
    bool isNull() const;
    
    ResourceGroup *m_group;
    int m_units;
    QList<ResourceTableItem*> m_resources;
    ResourceGroupRequest *m_request;
};

class TaskResourcesPanelBase : public QWidget, public Ui::TaskResourcesPanelBase
{
public:
  explicit TaskResourcesPanelBase( QWidget *parent ) : QWidget( parent ) {
    setupUi( this );
  }
};

class RequestResourcesPanel : public TaskResourcesPanelBase {
    Q_OBJECT
public:
    RequestResourcesPanel(QWidget *parent, Task &task, bool baseline=false);

    MacroCommand *buildCommand();
    
    bool ok();
    
private slots:
    void sendChanged();

    void groupChanged(QTreeWidgetItem *item);
    void groupChanged();
    void resourceChanged(int, int);
    void unitsChanged(int);
    
signals:
    void changed();

private:
    Task &m_task;
    StandardWorktime *m_worktime;
    GroupLVItem *selectedGroup;
    bool m_blockChanged;
    
};

}  //KPlato namespace

#endif
