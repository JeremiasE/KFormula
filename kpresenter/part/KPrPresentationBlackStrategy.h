/* This file is part of the KDE project
 * Copyright (C) 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KPRPRESENTATIONBLACKSTRATEGY_H
#define KPRPRESENTATIONBLACKSTRATEGY_H

#include "KPrPresentationStrategyInterface.h"

class KPrPresentationBlackWidget;

class KPrPresentationBlackStrategy : public KPrPresentationStrategyInterface
{
public:
    KPrPresentationBlackStrategy( KPrPresentationTool * tool );
    virtual ~KPrPresentationBlackStrategy();

    virtual bool keyPressEvent( QKeyEvent * event );

private:
    KPrPresentationBlackWidget * m_blackWidget;
};

#endif /* KPRPRESENTATIONBLACKSTRATEGY_H */