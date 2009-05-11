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

#include "kis_darken_transformation.h"
#include "kis_dynamic_coloring.h"
#include "kis_dynamic_sensor.h"

KisDarkenTransformation::KisDarkenTransformation(KisDynamicSensor* transfoParameter)
        : KisDynamicTransformation(KisDynamicTransformation::DarkenTransformationID), m_transfoParameter(transfoParameter)
{

}

KisDarkenTransformation::~KisDarkenTransformation()
{
    delete m_transfoParameter;
}

void KisDarkenTransformation::transformBrush(KisDynamicShape* dabsrc, const KisPaintInformation& info)
{
    Q_UNUSED(dabsrc);
    Q_UNUSED(info);
    // TODO: implement it, but I wonder if it makes sense to support it for the dab ?
}

void KisDarkenTransformation::transformColoring(KisDynamicColoring* coloringsrc, const KisPaintInformation& info)
{
    coloringsrc->darken((qint32)(255  - 75 * m_transfoParameter->parameter(info)));
}

#include "kis_darken_transformation.moc"
