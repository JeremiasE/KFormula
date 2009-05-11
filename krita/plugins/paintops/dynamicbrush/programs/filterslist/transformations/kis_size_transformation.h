/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
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

#ifndef _KIS_SIZE_TRANSFORMATION_
#define _KIS_SIZE_TRANSFORMATION_

#include "kis_dynamic_transformation.h"
#include <klocale.h>

class KisDynamicSensor;

class KisSizeTransformation : public KisDynamicTransformation
{
    Q_OBJECT
public:
    KisSizeTransformation(KisDynamicSensor* hTransfoParameter, KisDynamicSensor* vTransfoParameter);
    virtual ~KisSizeTransformation();
public:
    virtual void transformBrush(KisDynamicShape* dabsrc, const KisPaintInformation& info);
    virtual void transformColoring(KisDynamicColoring* coloringsrc, const KisPaintInformation& info);
    virtual QWidget* createConfigWidget(QWidget* parent);
private slots:
    void setHSensor(KisDynamicSensor*);
    void setVSensor(KisDynamicSensor*);
    void setHMaximum(double v);
    void setVMaximum(double v);
    void setHMinimum(double v);
    void setVMinimum(double v);
private:
    KisDynamicSensor* m_horizTransfoParameter;
    KisDynamicSensor* m_vertiTransfoParameter;
    double m_hmax, m_hmin, m_vmax, m_vmin;
};

#endif
