/*
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
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

#include "kis_dynamic_brush.h"
#include <klocale.h>

#include <KoColorSpaceRegistry.h>

#include <kis_paint_device.h>

#include "kis_dynamic_coloring.h"
#include "kis_dynamic_coloring_program.h"
#include "kis_dynamic_shape.h"
#include "kis_dynamic_shape_program.h"

KisDynamicBrush::KisDynamicBrush(const QString& name)
        : m_name(name)
        , m_shape(0)
        , m_coloring(0)
        , m_shapeProgram(0)
        , m_coloringProgram(0)
{
}

KisDynamicBrush::~KisDynamicBrush()
{

    delete m_shape;
    delete m_coloring;
    delete m_shapeProgram;
    delete m_coloringProgram;
}

void KisDynamicBrush::startPainting(KisPainter* _painter)
{
    Q_ASSERT(m_shape);
    Q_ASSERT(_painter);
    m_shape->startPainting(_painter);
}

void KisDynamicBrush::endPainting()
{
    Q_ASSERT(m_shape);
    m_shape->endPainting();
}

void KisDynamicBrush::setShapeProgram(KisDynamicShapeProgram* p)
{
    Q_ASSERT(p);
    delete m_shapeProgram;
    m_shapeProgram = p;
}
void KisDynamicBrush::setColoringProgram(KisDynamicColoringProgram* p)
{
    Q_ASSERT(p);
    delete m_coloringProgram;
    m_coloringProgram = p;
}

void KisDynamicBrush::setShape(KisDynamicShape* shape)
{
    Q_ASSERT(shape);
    delete m_shape;
    m_shape = shape;
}

void KisDynamicBrush::setColoring(KisDynamicColoring* coloring)
{
    Q_ASSERT(coloring);
    delete m_coloring;
    m_coloring = coloring;
}
