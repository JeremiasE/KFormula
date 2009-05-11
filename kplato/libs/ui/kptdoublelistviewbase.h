/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <kplato@kde.org>

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

#ifndef KPTDOUBLELISTVIEWBASE_H
#define KPTDOUBLELISTVIEWBASE_H

#include "kplatoui_export.h"

#include <QMap>
#include <QSplitter>
#include <QTreeWidget>

class QColor;
class QStringList;
class QTreeWidgetItem;
class QTreeWidgetItem;

class QPrinter;

namespace KPlato
{

class KPLATOUI_EXPORT ListView : public QTreeWidget
{
public:
    explicit ListView(QWidget *parent)
    : QTreeWidget(parent)
    {}
    
    virtual void paintToPrinter(QPainter * p, int cx, int cy, int cw, int ch);
};

/**
 * The class DoubleListViewBase provides a double listview
 * where the right listview (the slave) containes columns of
 * double values and the left listview (the master) is the 'item' listview
 * and also provides for a sum total column of the values in the slave listview.
 * This makes it possible to scroll the slave listview and still see the values
 * in the master listview.
 */
class KPLATOUI_EXPORT DoubleListViewBase : public QSplitter
{
    Q_OBJECT
public:

    DoubleListViewBase(QWidget *parent, bool description=false);

    //~DoubleListViewBase();

    ListView *masterListView() const { return m_masterList; }
    ListView *slaveListView() const { return m_slaveList; }
    void setOpen(QTreeWidgetItem *item, bool open);
    
    void setNameHeader(const QString& text);
    void setTotalHeader(const QString& text);
    void setDescriptionHeader(const QString& text);
    void setSlaveLabels(const QStringList &text);
    virtual void print(QPrinter &printer);

    virtual void calculate();
    void clearLists();
    virtual void createSlaveItems();
    void clearSlaveList();
    void setFormat(int fieldwidth=0, char fmt='f', int prec=0);
    void setMasterFormat(int fieldwidth=0, char fmt='f', int prec=0);
    void setSlaveFormat(int fieldwidth=0, char fmt='f', int prec=0);
    virtual QSize sizeHint() const;
    
    class MasterListItem;
    class SlaveListItem : public QTreeWidgetItem {
    public:
        SlaveListItem(MasterListItem *master, QTreeWidget *parent, QTreeWidgetItem *after, bool highlight=false);
        SlaveListItem(MasterListItem *master, QTreeWidgetItem *parent, QTreeWidgetItem *after, bool highlight=false);
        ~SlaveListItem();
        void masterItemDeleted() { m_masterItem = 0; }
        
        virtual void setColumn(int col, double value);
        virtual void clearColumn(int col);
        
        double value(int col) const { return m_valueMap[col]; }
        void setLimit(int col, double limit);
        void setHighlight(bool on) { m_highlight = on; }

        virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
        
        void setFormat(int fieldwidth=0, char fmt='f', int prec=0);
        
    protected:
        MasterListItem *m_masterItem;
        double m_value;
        bool m_highlight;

        QMap<int, double> m_valueMap;
        QMap<int, double> m_limitMap;
        
        int m_fieldwidth;
        char m_fmt;
        int m_prec;
    };

    class MasterListItem : public QTreeWidgetItem {
    public:
        explicit MasterListItem(QTreeWidget *parent, bool highlight=false);
        explicit MasterListItem(QTreeWidget *parent, const QString& text, bool highlight=false);
        MasterListItem(QTreeWidgetItem *parent, bool highlight=false);
        MasterListItem(QTreeWidgetItem *parent, const QString& text, bool highlight=false);
        ~MasterListItem();
        
        /// Creates slaveitems for myself and my children
        void createSlaveItems(QTreeWidget *lv, QTreeWidgetItem *after=0);
        void slaveItemDeleted();
        void setSlaveOpen(bool on);
        SlaveListItem *slaveItem() const { return m_slaveItem; }
        double value() const { return m_value; }
        
        void setTotal(double tot);
        double calcTotal();
        void addToTotal(double v);
        void setSlaveItem(int col, double value);
        void setSlaveLimit(int col, double limit);
        void setLimit(double limit) { m_limit = limit; }
        void setHighlight(bool on) { m_highlight = on; }
        void setSlaveHighlight(bool on);
        void clearColumn(int col);
        void calcSlaveItems();
        virtual double calcSlaveItems(int col);

        virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);    

        void setFormat(int fieldwidth=0, char fmt='f', int prec=0);
        
    protected:
        SlaveListItem *m_slaveItem;
        double m_value;
        double m_limit;
        bool m_highlight;
        
        QMap<int, double> m_valueMap;
    
        int m_fieldwidth;
        char m_fmt;
        int m_prec;
    };

public:
    QList<QTreeWidgetItem*> masterItems() { 
        return m_masterList->findItems("*", Qt::MatchWildcard|Qt::MatchRecursive); 
    }
    QList<QTreeWidgetItem*> slaveItems() { 
        return m_slaveList->findItems("*", Qt::MatchWildcard|Qt::MatchRecursive); 
    }
    virtual void paintContents(QPainter *p);

protected slots:
    void slotExpanded(QTreeWidgetItem* item);
    void slotCollapsed(QTreeWidgetItem* item);

private:

private:
    ListView *m_masterList;
    ListView *m_slaveList;
    
    int m_fieldwidth;
    char m_fmt;
    int m_prec;
};

}  //KPlato namespace

#endif
