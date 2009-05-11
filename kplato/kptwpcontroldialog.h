/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTWPCONTROLDIALOG_H
#define KPTWPCONTROLDIALOG_H

#include "kplato_export.h"

#include <kpagedialog.h>


namespace KPlato
{

class DocumentsPanel;
class WPControlPanel;
class View;
class Task;

class KPLATO_EXPORT WPControlDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit WPControlDialog( View *view, Task &task, QWidget *parent=0);

private:
    WPControlPanel *m_wp;
    DocumentsPanel *m_docs;
};

} //KPlato namespace

#endif // KPTWPCONTROLDIALOG_H
