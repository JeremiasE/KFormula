/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTACCOUNTSVIEWCONFIGDIALOG_H
#define KPTACCOUNTSVIEWCONFIGDIALOG_H


#include <kdialog.h>
#include "ui_kptaccountsviewconfigurepanelbase.h"

class QDate;
class QString;
class QWidget;

namespace KPlato
{

class AccountsviewConfigPanel;
class AccountsTreeView;

class AccountsviewConfigurePanelBase : public QWidget, public Ui::AccountsviewConfigurePanelBase
{
public:
  explicit AccountsviewConfigurePanelBase( QWidget *parent ) : QWidget( parent ) {
    setupUi( this );
  }
};


class AccountsviewConfigDialog : public KDialog {
    Q_OBJECT
public:
    AccountsviewConfigDialog( AccountsTreeView *view, QWidget *parent);

public slots:
    void slotOk();
    
private:
    AccountsTreeView *m_view;
    AccountsviewConfigPanel *m_panel;
};

class AccountsviewConfigPanel : public AccountsviewConfigurePanelBase {
    Q_OBJECT
public:
    explicit AccountsviewConfigPanel(QWidget *parent);
    
public slots:
    void slotChanged();

signals:
    void changed(bool);
};

} //KPlato namespace

#endif // KPTACCOUNTSVIEWCONFIGDIALOG_H
