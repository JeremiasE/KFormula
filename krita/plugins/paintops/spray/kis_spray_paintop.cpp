/*
 *  Copyright (c) 2008 Lukáš Tvrdý <lukast.dev@gmail.com>
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

#include "kis_spray_paintop.h"
#include "kis_spray_paintop_settings.h"

#include <cmath>

#include <QRect>
#include <QColor>
#include <QList>

#include <KoColor.h>
#include <KoColorSpace.h>

#include <kis_image.h>
#include <kis_debug.h>

#include <kis_brush.h>
#include <kis_global.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_types.h>
#include <kis_paintop.h>
#include <kis_selection.h>
#include <kis_random_accessor.h>
#include "metaball.h"
#include <qapplication.h>

KisSprayPaintOp::KisSprayPaintOp(const KisSprayPaintOpSettings *settings, KisPainter * painter, KisImageSP image)
    : KisPaintOp( painter )
    , m_settings( settings )
    , m_image ( image )
{
    m_sprayBrush.setDiameter( settings->diameter() );
    m_sprayBrush.setJitterSize( settings->jitterSize() );
    m_sprayBrush.setJitterMovement( settings->jitterMovement() );

    m_sprayBrush.setObject( settings->object() );
    m_sprayBrush.setShape( settings->shape() );
    m_sprayBrush.setJitterShapeSize(settings->jitterShapeSize());

    if (settings->proportional()){
        m_sprayBrush.setObjectDimension( settings->widthPerc()  / 100.0 * settings->diameter() * settings->scale(), 
                                          settings->heightPerc() / 100.0 * settings->diameter() * settings->scale());
    } else
    {
        m_sprayBrush.setObjectDimension( settings->width(), settings->height() );
    }

    m_sprayBrush.setAmount( settings->amount() );
    m_sprayBrush.setScale( settings->scale() );


    m_sprayBrush.setCoverity( settings->coverage() / 100.0 );
    m_sprayBrush.setUseDensity( settings->useDensity() );
    m_sprayBrush.setParticleCount( settings->particleCount() );

    // spacing 
    if ( (settings->diameter() * 0.5) > 1)
    {
        m_ySpacing = m_xSpacing = settings->diameter() * 0.5 * settings->spacing();
    } else
    {
        m_ySpacing = m_xSpacing = 1.0;
    }
    m_spacing = m_xSpacing;

}

KisSprayPaintOp::~KisSprayPaintOp()
{
}

double KisSprayPaintOp::spacing(double & xSpacing, double & ySpacing, double pressure1, double pressure2) const {
        Q_UNUSED(pressure1);
        Q_UNUSED(pressure2);
        xSpacing = m_xSpacing;
        ySpacing = m_ySpacing;

        return m_spacing;
}


void KisSprayPaintOp::paintAt(const KisPaintInformation& info)
{
    if (!painter()) return;

    dab = cachedDab();
    dab->clear();
    
    m_sprayBrush.paint(dab, info, painter()->paintColor());
        
    QRect rc = dab->extent();

    painter()->bltSelection(
        rc.x(), rc.y(),
        painter()->compositeOp(),
        dab,
        painter()->opacity(),
        rc.x(), rc.y(),
        rc.width(), rc.height());

}

