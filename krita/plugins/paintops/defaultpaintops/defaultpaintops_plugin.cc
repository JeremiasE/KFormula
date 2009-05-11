/*
 * defaultpaintops_plugin.cc -- Part of Krita
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

#include "defaultpaintops_plugin.h"
#include <klocale.h>
#include <kiconloader.h>
#include <kcomponentdata.h>
#include <kstandarddirs.h>
#include <kis_debug.h>
#include <kgenericfactory.h>

#include "kis_airbrushop_factory.h"
#include "kis_brushop_factory.h"
#include "kis_duplicateop_factory.h"
#include "kis_eraseop_factory.h"
#include "kis_penop_factory.h"
#include "kis_smudgeop_factory.h"
#include "kis_global.h"
#include "kis_paintop_registry.h"

typedef KGenericFactory<DefaultPaintOpsPlugin> DefaultPaintOpsPluginFactory;
K_EXPORT_COMPONENT_FACTORY(kritadefaultpaintops, DefaultPaintOpsPluginFactory("krita"))


DefaultPaintOpsPlugin::DefaultPaintOpsPlugin(QObject *parent, const QStringList &)
        : KParts::Plugin(parent)
{
    setComponentData(DefaultPaintOpsPluginFactory::componentData());

    KisPaintOpRegistry *r = KisPaintOpRegistry::instance();
    r->add(new KisAirbrushOpFactory);
    r->add(new KisBrushOpFactory);
    r->add(new KisDuplicateOpFactory);
    r->add(new KisEraseOpFactory);
    r->add(new KisPenOpFactory);
    r->add(new KisSmudgeOpFactory);
}

DefaultPaintOpsPlugin::~DefaultPaintOpsPlugin()
{
}

#include "defaultpaintops_plugin.moc"
