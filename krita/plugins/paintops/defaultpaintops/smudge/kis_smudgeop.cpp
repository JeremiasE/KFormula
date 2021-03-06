/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2004-2008 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2004 Clarence Dang <dang@kde.org>
 *  Copyright (c) 2004 Adrian Page <adrian@pagenet.plus.com>
 *  Copyright (c) 2004 Cyrille Berger <cberger@cberger.net>
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

#include "kis_smudgeop.h"

#include <string.h>

#include <QRect>
#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDomElement>
#include <QHBoxLayout>
#include <qtoolbutton.h>

#include <kis_debug.h>

#include <KoColorSpaceRegistry.h>
#include <KoColorTransformation.h>
#include <KoColor.h>
#include <KoCompositeOp.h>
#include <KoInputDevice.h>

#include <kis_brush.h>
#include <kis_datamanager.h>
#include <kis_global.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_paintop.h>
#include <kis_properties_configuration.h>
#include <kis_selection.h>
#include <kis_brush_option.h>
#include <kis_paintop_options_widget.h>
#include <kis_pressure_opacity_option.h>
#include <kis_pressure_size_option.h>
#include <kis_pressure_rate_option.h>

#include "kis_smudgeop_settings.h"
#include "kis_smudgeop_settings_widget.h"

KisSmudgeOp::KisSmudgeOp(const KisSmudgeOpSettings *settings, KisPainter *painter)
    : KisBrushBasedPaintOp(painter)
    , settings(settings)
    , m_firstRun( true )
    , m_target( 0 )
    , m_srcdev( 0 )
{
    Q_ASSERT(settings);
    Q_ASSERT(painter);
    Q_ASSERT(settings->m_optionsWidget->m_brushOption);
    m_brush = settings->m_optionsWidget->m_brushOption->brush();
    if (settings->node()) {
        m_source = settings->node()->paintDevice();
    }
    else {
        m_source = painter->device();
    }
    m_srcdev = new KisPaintDevice(m_source->colorSpace());
    m_target = new KisPaintDevice(m_source->colorSpace());

}

KisSmudgeOp::~KisSmudgeOp()
{
}

void KisSmudgeOp::paintAt(const KisPaintInformation& info)
{
    if (!painter()->device()) return;

    KisBrushSP brush = m_brush;

    Q_ASSERT(brush);
    if (!brush) return;

    KisPaintInformation adjustedInfo = settings->m_optionsWidget->m_sizeOption->apply(info);
    if (! brush->canPaintFor(adjustedInfo))
        return;

    KisPaintDeviceSP device = painter()->device();
    double pScale = KisPaintOp::scaleForPressure(adjustedInfo.pressure());
    QPointF hotSpot = brush->hotSpot(pScale, pScale);
    QPointF pt = info.pos() - hotSpot;

    // Split the coordinates into integer plus fractional parts. The integer
    // is where the dab will be positioned and the fractional part determines
    // the sub-pixel positioning.
    qint32 x;
    double xFraction;
    qint32 y;
    double yFraction;

    splitCoordinate(pt.x(), &x, &xFraction);
    splitCoordinate(pt.y(), &y, &yFraction);

    KisFixedPaintDeviceSP dab = 0;

    double scale = KisPaintOp::scaleForPressure(adjustedInfo.pressure());

    QRect dabRect = QRect(0, 0, brush->maskWidth(scale, 0.0), brush->maskHeight(scale, 0.0));
    QRect dstRect = QRect(x, y, dabRect.width(), dabRect.height());
    if (dstRect.isNull() || dstRect.isEmpty() || !dstRect.isValid()) return;

    if (brush->brushType() == IMAGE || brush->brushType() == PIPE_IMAGE) {
        dab = brush->image(device->colorSpace(), pScale, 0.0, adjustedInfo, xFraction, yFraction);
        dab->convertTo(KoColorSpaceRegistry::instance()->alpha8());
    } else {
        dab = cachedDab();
        KoColor color = painter()->paintColor();
        dab->convertTo(KoColorSpaceRegistry::instance()->alpha8());
        brush->mask(dab, color, scale, pScale, 0.0, info, xFraction, yFraction);
    }

    qint32 sw = dab->bounds().width();
    qint32 sh = dab->bounds().height();

    /* To smudge, one does the following:
         * at first, initialize a temporary paint device with a copy of the original (dab-sized piece, really).
         * all other times:
             reduce the transparency of the temporary paint device so as to let it mix gradually
         * combine the temp device with the piece the brush currently is 'painting', according to a mix (opacity)
             note that in the first step, this does the actual copying of the data
         * this combination is then composited upon the actual image
       TODO: what happened exactly in 1.6 (and should happen now) when the dab resizes halfway due to pressure?
    */
    int opacity = OPACITY_OPAQUE;
    if (!m_firstRun) {
        opacity = settings->m_optionsWidget->m_rateOption->apply( opacity, sw, sh, m_srcdev, info.pressure() );

        KisRectIterator it = m_srcdev->createRectIterator(0, 0, sw, sh);
        KoColorSpace* cs = m_srcdev->colorSpace();
        while(!it.isDone()) {
            cs->setAlpha(it.rawData(), (cs->alpha(it.rawData()) * opacity) / OPACITY_OPAQUE, 1);
            ++it;
        }

        opacity = OPACITY_OPAQUE - opacity;
    } else {
        m_firstRun = false;
    }

    KisPainter copyPainter(m_srcdev);
    copyPainter.setOpacity(opacity);
    copyPainter.bitBlt(0, 0, device, pt.x(), pt.y(), sw, sh);
    copyPainter.end();

    m_target = new KisPaintDevice(device->colorSpace());

    // Looks hacky, but we lost bltMask, or the ability to easily convert alpha8 paintdev to selection?
    KisSelectionSP dabAsSelection = new KisSelection();
    copyPainter.begin(dabAsSelection);
    copyPainter.setOpacity(OPACITY_OPAQUE);
    copyPainter.setCompositeOp(COMPOSITE_COPY);
    copyPainter.bltFixed(0, 0, dab, 0, 0, sw, sh);
    copyPainter.end();

    copyPainter.begin(m_target);
    copyPainter.setCompositeOp(COMPOSITE_OVER);
    copyPainter.setSelection(dabAsSelection);
    copyPainter.bitBlt(0, 0, m_srcdev, 0, 0, sw, sh);
    copyPainter.end();

    qint32 sx = dstRect.x() - x;
    qint32 sy = dstRect.y() - y;
    sw = dstRect.width();
    sh = dstRect.height();

    painter()->bitBlt(dstRect.x(), dstRect.y(), m_target, sx, sy, sw, sh);

}
