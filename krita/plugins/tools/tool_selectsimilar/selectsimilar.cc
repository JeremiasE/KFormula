/*
 * selectsimilar.h -- Part of Krita
 *
 * Copyright (c) 2004 Boudewijn Rempt (boud@valdyas.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "selectsimilar.h"
#include <math.h>

#include <stdlib.h>

#include <QSlider>
#include <QPoint>
#include <QPainter>

#include <klocale.h>
#include <kiconloader.h>
#include <kcomponentdata.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kis_debug.h>
#include <kgenericfactory.h>

#include <KoToolRegistry.h>


#include "kis_tool_selectsimilar.h"

typedef KGenericFactory<SelectSimilar> SelectSimilarFactory;
K_EXPORT_COMPONENT_FACTORY(kritatoolselectsimilar, SelectSimilarFactory("krita"))

SelectSimilar::SelectSimilar(QObject *parent, const QStringList &)
        : KParts::Plugin(parent)
{
    setComponentData(SelectSimilarFactory::componentData());

    KoToolRegistry * r = KoToolRegistry::instance();
    r->add(new KisToolSelectSimilarFactory(r, QStringList()));
}

SelectSimilar::~SelectSimilar()
{
}

#include "selectsimilar.moc"

