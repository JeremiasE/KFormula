/*
 *  Copyright (c) 2007 Sven Langkamp <sven.langkamp@gmail.com>
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

#include "kis_tool_path.h"
#include <klocale.h>
#include <KoPathShape.h>
#include <KoCanvasBase.h>
#include <KoColorSpace.h>
#include <KoCompositeOp.h>

#include "kis_image.h"
#include "kis_painter.h"
#include "kis_layer.h"
#include "canvas/kis_canvas2.h"
#include "kis_view2.h"
#include "kis_canvas_resource_provider.h"
#include "kis_paintop_registry.h"
#include "kis_selection.h"

KisToolPath::KisToolPath(KoCanvasBase * canvas)
        : KoCreatePathTool(canvas)
{
}

KisToolPath::~KisToolPath()
{
}

void KisToolPath::addPathShape()
{
    KisNodeSP currentNode =
        m_canvas->resourceProvider()->resource(KisCanvasResourceProvider::CurrentKritaNode).value<KisNodeSP>();
    if (!currentNode || !currentNode->paintDevice()) {
        delete m_shape;
        m_shape = 0;
        return;
    }

    KisPaintDeviceSP dev = currentNode->paintDevice();

    KisCanvas2 *canvas = dynamic_cast<KisCanvas2 *>(m_canvas);
    if(!canvas)
        return;

    KisImageSP image = canvas->view()->image();
    KisSelectionSP selection = canvas->view()->selection();

    KisPainter painter(dev, selection);
    painter.beginTransaction(i18n("Path"));
    painter.setPaintColor(KoColor(Qt::black, dev->colorSpace()));
    painter.setFillStyle(KisPainter::FillStyleForegroundColor);
    painter.setStrokeStyle(KisPainter::StrokeStyleNone);
    painter.setOpacity(OPACITY_OPAQUE);
    painter.setCompositeOp(dev->colorSpace()->compositeOp(COMPOSITE_OVER));
    KisPaintOpPresetSP preset = m_canvas->resourceProvider()->
                                resource(KisCanvasResourceProvider::CurrentPaintOpPreset).value<KisPaintOpPresetSP>();
    painter.setPaintOpPreset(preset, image);

    QMatrix matrix;
    matrix.scale(image->xRes(), image->yRes());
    matrix.translate(m_shape->position().x(), m_shape->position().y());
    painter.paintPainterPath(matrix.map(m_shape->outline()));
    QRegion dirtyRegion = painter.dirtyRegion();
    dev->setDirty(dirtyRegion);
    image->setModified();

    m_canvas->addCommand(painter.endTransaction());
    
    delete m_shape;
    m_shape = 0;
}

#include "kis_tool_path.moc"
