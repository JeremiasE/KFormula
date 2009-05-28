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

#include "kis_chalk_paintop.h"
#include "kis_chalk_paintop_settings.h"

#include <cmath>
#include <math.h>

#include <QRect>
#include <QColor>
//#include <QMutexLocker>

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


KisChalkPaintOp::KisChalkPaintOp(const KisChalkPaintOpSettings *settings, KisPainter * painter, KisImageSP image)
    : KisPaintOp( painter )
    , m_settings( settings )
    , m_image ( image )
{
    m_chalkBrush.setRadius( settings->radius() );
}

KisChalkPaintOp::~KisChalkPaintOp()
{
}

void KisChalkPaintOp::paintAt(const KisPaintInformation& info)
{
    if (!painter()) return;

    dbgPlugins << info;

    dab = cachedDab();
    dab->clear();

    qreal x1, y1;

    x1 = info.pos().x();
    y1 = info.pos().y();

    m_chalkBrush.paint(dab, x1, y1, painter()->paintColor());

    QRect rc = dab->extent();

    painter()->bitBlt(rc.x(), rc.y(), dab, rc.x(), rc.y(), rc.width(), rc.height());

}
