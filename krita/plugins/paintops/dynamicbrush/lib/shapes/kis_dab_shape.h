/*
 *  Copyright (c) 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */


#ifndef _KIS_DAB_SHAPE_H_
#define _KIS_DAB_SHAPE_H_

#include "dynamicbrush_export.h"

#include <kis_types.h>
#include <QRect>

class KisAutobrushShape;
class KisDynamicColoring;
class KisPaintInformation;

#include <kis_brush.h>
#include "kis_dynamic_shape.h"

class DYNAMIC_BRUSH_EXPORT KisDabShape : public KisDynamicShape
{
public:
    KisDabShape(KisBrushSP brush);
    virtual ~KisDabShape();
    virtual void paintAt(const QPointF &pos, const KisPaintInformation& info, KisDynamicColoring* coloringsrc);
    virtual void resize(double xs, double ys);
    virtual void rotate(double r);
    virtual QRect rect() const;
    virtual KisDynamicShape* clone() const;
private:
    double m_scaleX, m_scaleY, m_rotate;
    KisPaintDeviceSP m_dab;
    KisBrushSP m_brush;
};

#endif
