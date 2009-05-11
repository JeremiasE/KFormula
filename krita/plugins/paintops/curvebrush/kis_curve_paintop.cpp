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

#include "kis_curve_paintop.h"
#include "kis_curve_paintop_settings.h"

#include <cmath>

#include <QRect>
#include <QList>
#include <QColor>
//#include <QMutexLocker>

#include <qdebug.h>

#include <KoColor.h>
#include <KoColorSpace.h>
#include <KoInputDevice.h>
#include <KoCompositeOp.h>

#include <kis_image.h>
#include <kis_debug.h>

#include "kis_brush.h"
#include "kis_global.h"
#include "kis_paint_device.h"
#include "kis_painter.h"
#include "kis_types.h"
#include "kis_paintop.h"
#include "kis_selection.h"
#include "kis_random_accessor.h"

#include "kis_datamanager.h"


KisCurvePaintOp::KisCurvePaintOp(const KisCurvePaintOpSettings *settings, KisPainter * painter, KisImageSP image)
        : KisPaintOp(painter)
{
    Q_ASSERT(settings);
    m_curveBrush.setImage(image);
    m_curveBrush.setPainter(painter);

    m_curveBrush.setInkColor( painter->paintColor() );
    m_curveBrush.setMode( settings->curveAction() );
    m_curveBrush.setMinimalDistance( settings->minimalDistance() );
    m_curveBrush.setInterval( settings->interval() );
}

KisCurvePaintOp::~KisCurvePaintOp()
{
}

void KisCurvePaintOp::paintAt(const KisPaintInformation& info)
{
    Q_UNUSED(info);
}


double KisCurvePaintOp::paintLine(const KisPaintInformation &pi1, const KisPaintInformation &pi2, double savedDist)
{
//    QMutexLocker locker(&m_mutex);

    if (!painter()) return 0;
    m_dev = painter()->device();
    if (!m_dev) return 0;

    dab = cachedDab();
    dab->clear();

    //write device, read device, position
    m_curveBrush.paintLine(dab,m_dev, pi1, pi2);

    QRect rc = dab->extent();

    painter()->bltSelection(
        rc.x(), rc.y(),
        painter()->compositeOp(),
        dab,
        painter()->opacity(),
        rc.x(), rc.y(),
        rc.width(), rc.height());

    KisVector2D end = toKisVector2D(pi2.pos());
    KisVector2D start = toKisVector2D(pi1.pos());
    KisVector2D dragVec = end - start;
    return  dragVec.norm();

}
