/*
 *  kis_tool_select_brush.cc - part of Krita
 *
 *  Copyright (c) 2004 Boudewijn Rempt <boud@valdyas.org>
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

#include "kis_tool_select_eraser.h"
#include <QEvent>
#include <QLabel>
#include <QLayout>
#include <QWidget>
#include <QVBoxLayout>

#include <kis_debug.h>
#include <klocale.h>

#include "KoPointerEvent.h"

#include "kis_layer.h"
#include "kis_paintop.h"
#include "kis_paintop_registry.h"
#include "kis_cursor.h"
#include "kis_doc2.h"
#include "kis_painter.h"
#include "kis_selection.h"
#include "kis_types.h"
#include "kis_selection_options.h"
#include "kis_undo_adapter.h"

KisToolSelectEraser::KisToolSelectEraser(KoCanvasBase *canvas)
        : KisToolFreehand(canvas, KisCursor::load("tool_eraser_selection_cursor.png", 5, 5),
                  i18n("Selection Eraser"))
{
    setObjectName("tool_select_eraser");
    m_paintOnSelection = true;
}

KisToolSelectEraser::~KisToolSelectEraser()
{
}

void KisToolSelectEraser::activate(bool tmp)
{
    KisToolFreehand::activate(tmp);

    if (!m_optWidget)
        return;

    m_optWidget->slotActivated();
}

void KisToolSelectEraser::initPaint(KoPointerEvent */*e*/)
{
    if (!currentImage() || !currentNode()->paintDevice()) return;

    m_mode = PAINT;
    m_dragDist = 0;

    // Create painter
    KisPaintDeviceSP dev = currentNode()->paintDevice();

    if (dev.isNull()) return;

    if (m_painter)
        delete m_painter;
    KisSelectionSP selection = currentSelection();

    m_target = selection;
    m_painter = new KisPainter(selection);
    Q_CHECK_PTR(m_painter);
    m_painter->beginTransaction(i18n("Selection Eraser"));
    m_painter->setPaintColor(KoColor(Qt::white, selection->colorSpace()));
    m_painter->setOpacity(OPACITY_OPAQUE);
    m_painter->setCompositeOp(selection->colorSpace()->compositeOp(COMPOSITE_ERASE));
    m_painter->setPaintOpPreset(currentPaintOpPreset(), currentImage());

    // Set the cursor -- ideally. this should be a mask created from the brush,
    // now that X11 can handle colored cursors.
#if 0
    // Setting cursors has no effect until the tool is selected again; this
    // should be fixed.
    useCursor(KisCursor::eraserCursor());
#endif
}

void KisToolSelectEraser::endPaint()
{
    KisToolFreehand::endPaint();
}

#include "kis_tool_select_eraser.moc"

