/* This file is part of the KDE project
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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

#include "DefaultFactory.h"
#include "Property.h"
/*
#include "customproperty.h"*/
#include "booledit.h"
#include "combobox.h"
/*
#include "coloredit.h"*/
#include "cursoredit.h"
/*#include "dateedit.h"
#include "datetimeedit.h"
#include "dummywidget.h"
//TODO #include "linestyleedit.h"*/
#include "pixmapedit.h"
#include "pointedit.h"
#include "fontedit.h"
#include "rectedit.h"
#include "sizeedit.h"
#include "sizepolicyedit.h"
#include "spinbox.h"
/*#include "stringlistedit.h"*/
#include "stringedit.h"
/*#include "symbolcombo.h"
#include "timeedit.h"
#include "urledit.h"
*/
#include <kdebug.h>
#include <kglobal.h>

using namespace KoProperty;

DefaultFactory::DefaultFactory()
 : Factory()
{
    addEditor( KoProperty::Bool, new BoolDelegate );
    addEditor( KoProperty::Cursor, new CursorDelegate );
    addEditor( KoProperty::Double, new DoubleSpinBoxDelegate );
    addEditor( KoProperty::Font, new FontDelegate );
//! @todo addEditor( KoProperty::LongLong, new LongLongSpinBoxDelegate );
    addEditor( KoProperty::Pixmap, new PixmapDelegate );
    addEditor( KoProperty::Point, new PointDelegate );
    addEditor( KoProperty::Rect, new RectDelegate );
    addEditor( KoProperty::Size, new SizeDelegate );
    addEditor( KoProperty::SizePolicy, new SizePolicyDelegate );
    addEditor( KoProperty::Int, new IntSpinBoxDelegate );
    addEditor( KoProperty::String, new StringDelegate );
    addEditor( KoProperty::UInt, new IntSpinBoxDelegate );
//! @todo addEditor( KoProperty::ULongLong, new LongLongSpinBoxDelegate );
    addEditor( KoProperty::ValueFromList, new ComboBoxDelegate );
}

DefaultFactory::~DefaultFactory()
{
}
