/*
 *  Copyright (c) 2008 Lukas Tvrdy <lukast.dev@gmail.com>
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

#include "chalk_brush.h"
#include "brush_shape.h"

#include <KoColor.h>
#include <KoColorSpace.h>
#include <KoColorTransformation.h>

#include <QVariant>
#include <QHash>

#include "kis_random_accessor.h"
#include <cmath>

#ifdef _WIN32
#define srand48 srand
#define drand48 rand
#endif

ChalkBrush::ChalkBrush(const BrushShape &initialShape, KoColor inkColor)
{
    m_initialShape = initialShape;
    m_inkColor = inkColor;
    m_counter = 0;
}

ChalkBrush::ChalkBrush()
{
    m_radius = 5;
    m_counter = 0;
    init();
}


void ChalkBrush::init(){
    BrushShape bs;
    // some empiric values 
    bs.fromGaussian(m_radius, 1.0f, 0.9f);
    m_bristles = bs.getBristles();
    srand48( time(0) );
}

void ChalkBrush::paint(KisPaintDeviceSP dev, qreal x, qreal y, const KoColor &color)
{

    m_inkColor = color;
    m_counter++;
    qreal decr = (m_counter * m_counter * m_counter) / 1000000.0f;

    Bristle *bristle;

    qint32 pixelSize = dev->colorSpace()->pixelSize();
    KisRandomAccessor accessor = dev->createRandomAccessor((int)x, (int)y);

    qreal dirt, result;

    //count decrementing of saturation and alpha
    result = log( ( qreal )m_counter);
    result = -(result * 10) / 100.0;

    QHash<QString, QVariant> params;
    params["h"] = 0.0;
    params["s"] = result;
    params["v"] = 0.0;

    KoColorTransformation* transfo = dev->colorSpace()->createColorTransformation("hsv_adjustment", params);
    transfo->transform(m_inkColor.data(), m_inkColor.data(), 1);

    int opacity = static_cast<int>((1.0f + result) * 255.0);
    m_inkColor.setOpacity(opacity);

    for (int i = 0;i < m_bristles.size();i++) {
        bristle = &m_bristles[i];

        // let's call that noise from ground to chalk :)
        dirt = drand48();
        if (bristle->distanceCenter() > m_radius || dirt < 0.5) {
            continue;
        }

        int dx, dy;
        dx = (int)(x + bristle->x());
        dy = (int)(y + bristle->y());

        accessor.moveTo(dx, dy);
        memcpy(accessor.rawData(), m_inkColor.data(), pixelSize);
        bristle->setInkAmount(1.0f - decr);
    }
}

ChalkBrush::~ChalkBrush()
{
}
