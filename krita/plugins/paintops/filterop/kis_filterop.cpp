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

#include "kis_filterop.h"

#include <string.h>

#include <QRect>
#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDomElement>
#include <QHBoxLayout>
#include <qtoolbutton.h>

#include <kis_image.h>
#include <kis_debug.h>

#include <KoColorTransformation.h>
#include <KoColor.h>
#include <KoInputDevice.h>

#include <kis_processing_information.h>
#include <filter/kis_filter_registry.h>
#include <filter/kis_filter.h>
#include <filter/kis_filter_configuration.h>
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
#include <kis_pressure_size_option.h>
#include <kis_filter_option.h>
#include <kis_filterop_settings.h>
#include <kis_filterop_settings_widget.h>


KisFilterOp::KisFilterOp(const KisFilterOpSettings *settings, KisPainter *painter)
        : KisBrushBasedPaintOp(painter)
        , settings(settings)
{
    Q_ASSERT(settings);
    Q_ASSERT(painter);
    Q_ASSERT(settings->m_optionsWidget->m_brushOption);
    m_brush = settings->m_optionsWidget->m_brushOption->brush();
    Q_ASSERT( m_brush );
    m_tmpDevice = new KisPaintDevice(source()->colorSpace());
}

KisFilterOp::~KisFilterOp()
{
}

void KisFilterOp::paintAt(const KisPaintInformation& info)
{
    if (!painter()) {
        return;
    }

    KisFilterSP filter = settings->filter();
    if (!filter) {
        return;
    }

    if (!source()) {
        return;
    }

    KisBrushSP brush = m_brush;;
    if (!brush) return;

    KisPaintInformation adjustedInfo = settings->m_optionsWidget->m_sizeOption->apply(info);
    if (! brush->canPaintFor(adjustedInfo))
        return;

    double pScale = KisPaintOp::scaleForPressure(adjustedInfo.pressure());   // TODO: why is there scale and pScale that seems to contains the same things ?
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

    double scale = KisPaintOp::scaleForPressure(adjustedInfo.pressure());

    qint32 maskWidth = brush->maskWidth(scale, 0.0);
    qint32 maskHeight = brush->maskHeight(scale, 0.0);

    // Filter the paint device
    filter->process(KisConstProcessingInformation(source(), QPoint(x, y)),
                    KisProcessingInformation(m_tmpDevice, QPoint(0, 0)),
                    QSize(maskWidth, maskHeight),
                    settings->filterConfig(), 0);

    // Apply the mask on the paint device (filter before mask because edge pixels may be important)

    KisFixedPaintDeviceSP fixedDab = new KisFixedPaintDevice(m_tmpDevice->colorSpace());
    fixedDab->setRect(m_tmpDevice->extent());
    fixedDab->initialize();

    m_tmpDevice->readBytes(fixedDab->data(), fixedDab->bounds());
    brush->mask(fixedDab, scale, scale, 0.0, info, xFraction, yFraction);
    m_tmpDevice->writeBytes(fixedDab->data(), fixedDab->bounds());

    if (!settings->ignoreAlpha()) {
        KisHLineIteratorPixel itTmpDev = m_tmpDevice->createHLineIterator(0, 0, maskWidth);
        KisHLineIteratorPixel itSrc = source()->createHLineIterator(x, y, maskWidth);
        const KoColorSpace* cs = m_tmpDevice->colorSpace();
        for (int y = 0; y < maskHeight; ++y) {
            while (!itTmpDev.isDone()) {
                quint8 alphaTmpDev = cs->alpha(itTmpDev.rawData());
                quint8 alphaSrc = cs->alpha(itSrc.rawData());

                cs->setAlpha(itTmpDev.rawData(), qMin(alphaTmpDev, alphaSrc), 1);
                ++itTmpDev;
                ++itSrc;
            }
            itTmpDev.nextRow();
            itSrc.nextRow();
        }
    }

    // Blit the paint device onto the layer
    QRect dabRect = QRect(0, 0, maskWidth, maskHeight);
    QRect dstRect = QRect(x, y, dabRect.width(), dabRect.height());

    if (painter()->bounds().isValid()) {
        dstRect &= painter()->bounds();
    }
    if (dstRect.isNull() || dstRect.isEmpty() || !dstRect.isValid()) return;

    qint32 sx = dstRect.x() - x;
    qint32 sy = dstRect.y() - y;
    qint32 sw = dstRect.width();
    qint32 sh = dstRect.height();

    painter()->bitBlt(dstRect.x(), dstRect.y(), m_tmpDevice, sx, sy, sw, sh);

}

double KisFilterOp::paintLine(const KisPaintInformation &pi1,
                           const KisPaintInformation &pi2,
                           double savedDist)
{
    KisPaintInformation adjustedInfo1(pi1);
    KisPaintInformation adjustedInfo2(pi2);
    if (!settings->m_optionsWidget->m_sizeOption->isChecked()) {
        adjustedInfo1.setPressure(PRESSURE_DEFAULT);
        adjustedInfo2.setPressure(PRESSURE_DEFAULT);
    }
    return KisPaintOp::paintLine(adjustedInfo1, adjustedInfo2, savedDist);
}
