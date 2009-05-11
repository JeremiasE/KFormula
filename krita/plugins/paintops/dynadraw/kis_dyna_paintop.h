/*
 *  Copyright (c) 2008 Boudewijn Rempt <boud@valdyas.org>
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

#ifndef KIS_DYNA_PAINTOP_H_
#define KIS_DYNA_PAINTOP_H_

#include <QColor>
//#include <QMutex>

#include <klocale.h>
#include <kis_paintop.h>
#include <kis_types.h>

#include "dyna_brush.h"

#include "kis_dyna_paintop_settings.h"

class QPointF;
class KisPainter;

class KisDynaPaintOp : public KisPaintOp
{

public:

    KisDynaPaintOp(const KisDynaPaintOpSettings *settings, KisPainter * painter, KisImageSP image);
    virtual ~KisDynaPaintOp();

    void paintAt(const KisPaintInformation& info);
    double paintLine(const KisPaintInformation &pi1, const KisPaintInformation &pi2, double savedDist);

    virtual bool incremental() const {
        return true;
    }


    double spacing(double & xSpacing, double & ySpacing, double pressure1, double pressure2) const {
        Q_UNUSED(pressure1);
        Q_UNUSED(pressure2);
        xSpacing = 1;
        ySpacing = 1;
        return 1;
    }


private:
    const KisDynaPaintOpSettings* m_settings;
    KisImageSP m_image;
    KisPaintDeviceSP dab;
    DynaBrush m_dynaBrush;
};

#endif // KIS_DYNA_PAINTOP_H_
