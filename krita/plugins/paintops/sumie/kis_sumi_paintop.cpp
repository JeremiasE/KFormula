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

#include "kis_sumi_paintop.h"
#include "kis_sumi_paintop_settings.h"

#include <cmath>

#include <QRect>
#include <QList>
#include <QColor>
//#include <QMutexLocker>

#include <KoColor.h>
#include <KoColorSpace.h>
#include <KoInputDevice.h>
#include <KoCompositeOp.h>

#include <kis_image.h>
#include <kis_debug.h>

#include "kis_brush.h"
#include "kis_paint_device.h"
#include "kis_painter.h"
#include <kis_vec.h>

#include "brush.h"
#include "brush_shape.h"


KisSumiPaintOp::KisSumiPaintOp(const
                               KisSumiPaintOpSettings *settings, KisPainter * painter, KisImageSP image)
    : KisPaintOp( painter )
    , m_settings( settings )
    , m_image( image )
    , newStrokeFlag( true )

{
    Q_ASSERT(settings);
    BrushShape brushShape;

    if (settings->brushDimension() == 1) {
        brushShape.fromLine(settings->radius(), settings->sigma());
        brushShape.tresholdBristles(0.1);
    } else if (settings->brushDimension() == 2) {
        brushShape.fromGaussian(settings->radius(), settings->sigma());
        brushShape.tresholdBristles(0.1);
    } else {
        Q_ASSERT(false);
    }

    m_brush.setBrushShape(brushShape);

    m_brush.enableMousePressure(settings->mousePressure());

    m_brush.setInkDepletion(settings->curve());
    m_brush.setInkColor(painter->paintColor());

    m_brush.setScale(settings->scaleFactor());
    m_brush.setRandom(settings->randomFactor());
    m_brush.setShear(settings->shearFactor());

    m_brush.enableWeights(settings->useWeights());
    m_brush.enableOpacity(settings->useOpacity());
    m_brush.enableSaturation(settings->useSaturation());

    if (settings->useWeights()){
        // TODO : improve the way the weights can be set..
        m_brush.setBristleInkAmountWeight( settings->bristleInkAmountWeight()/100.0 );
        m_brush.setBristleLengthWeight( settings->bristleLengthWeight()/100.0 );
        m_brush.setInkDepletionWeight( settings->inkDepletionWeight()/100.0 );
        m_brush.setPressureWeight( settings->pressureWeight()/100.0 );
    }

    if ( !settings->node() ){
        m_dev = 0;
    }else{
        m_dev = settings->node()->paintDevice();
    }

}

KisSumiPaintOp::~KisSumiPaintOp()
{
}

void KisSumiPaintOp::paintAt(const KisPaintInformation& info)
{
    Q_UNUSED(info);
}


double KisSumiPaintOp::paintLine(const KisPaintInformation &pi1, const KisPaintInformation &pi2, double savedDist)
{
//    QMutexLocker locker(&m_mutex);
    Q_UNUSED(savedDist);

    if (!painter()) return 0;

/*    KisPaintDeviceSP device = painter()->device();
    if (!device) return 0;
    m_dev = device;*/

    dab = cachedDab();
    dab->clear();

    m_brush.paintLine(dab, m_dev, pi1, pi2);

    QRect rc = dab->extent();

    painter()->bltSelection(
        rc.x(), rc.y(),
        painter()->compositeOp(),
        dab,
        painter()->opacity(),
        rc.x(), rc.y(),
        rc.width(), rc.height());


   /* painter()->bltSelection(
        rc.x(), rc.y(),
        device->colorSpace()->compositeOp(COMPOSITE_ALPHA_DARKEN),
        dab,
        painter()->opacity(),
        rc.x(), rc.y(),
        rc.width(), rc.height());
    */

    KisVector2D end = toKisVector2D(pi2.pos());
    KisVector2D start = toKisVector2D(pi1.pos());
    KisVector2D dragVec = end - start;
    return  dragVec.norm();
}
