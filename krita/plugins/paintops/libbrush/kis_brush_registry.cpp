/*
 *  Copyright (c) 2008 Boudewijn Rempt <boud@valdyas.org>
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

#include "kis_brush_registry.h"

#include <QString>

#include <kaction.h>
#include <klocale.h>
#include <kparts/plugin.h>
#include <kservice.h>
#include <kservicetypetrader.h>
#include <kparts/componentfactory.h>

#include <KoPluginLoader.h>

#include <kis_debug.h>

#include "kis_brush_server.h"
#include "kis_brush_factory.h"
#include "kis_brush_registry.h"
#include "kis_auto_brush_factory.h"
#include "kis_gbr_brush_factory.h"
#include "kis_text_brush_factory.h"

KisBrushRegistry *KisBrushRegistry::m_singleton = 0;

KisBrushRegistry::KisBrushRegistry()
{
    Q_ASSERT(KisBrushRegistry::m_singleton == 0);
    KisBrushRegistry::m_singleton = this;

    // Initialize the brush server. This loads gbr and gih brushes.
    KisBrushServer::instance();
}

KisBrushRegistry::~KisBrushRegistry()
{
}

KisBrushRegistry* KisBrushRegistry::instance()
{
    if (KisBrushRegistry::m_singleton == 0) {
        KisBrushRegistry::m_singleton = new KisBrushRegistry();

        KisBrushRegistry::m_singleton->add( new KisAutoBrushFactory() );
        KisBrushRegistry::m_singleton->add( new KisGbrBrushFactory() );
        KisBrushRegistry::m_singleton->add( new KisTextBrushFactory() );

        Q_CHECK_PTR( KisBrushRegistry::m_singleton );
        KoPluginLoader::instance()->load( "Krita/brush", "Type == 'Service' and ([X-Krita-Version] == 3)" );
    }
    return KisBrushRegistry::m_singleton;
}


KisBrushSP KisBrushRegistry::getOrCreateBrush( const QDomElement& element )
{
    QString brushType = element.attribute( "brush_type" );

    if ( brushType.isEmpty() ) return 0;

    KisBrushFactory* factory = get(brushType);
    if (!factory) return 0;

    KisBrushSP brush = factory->getOrCreateBrush(element);
    return brush;
}

#include "kis_brush_registry.moc"
