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

#include "kis_tool_select_brush.h"
#include <QEvent>
#include <QLabel>
#include <QLayout>
#include <QWidget>
#include <QRect>
#include <QVBoxLayout>

#include <kis_debug.h>
#include <kactioncollection.h>
#include <kaction.h>
#include <klocale.h>

#include "KoPointerEvent.h"

#include "widgets/kis_cmb_composite.h"
#include "kis_cursor.h"
#include "kis_doc2.h"
#include "kis_paintop.h"
#include "kis_paintop_registry.h"
#include "kis_painter.h"
#include "kis_selection.h"
#include "kis_types.h"
#include "kis_layer.h"
#include "kis_selection_options.h"
#include "kis_selected_transaction.h"
#include "kis_undo_adapter.h"

KisToolSelectBrush::KisToolSelectBrush(KoCanvasBase *canvas)
        : KisToolFreehand(canvas, KisCursor::load("tool_brush_selection_cursor.png", 5, 5),
                          i18n("Selection Brush"))
{
    setObjectName("tool_select_brush");
    m_paintOnSelection = true;
}

KisToolSelectBrush::~KisToolSelectBrush()
{
}

void KisToolSelectBrush::activate(bool tmp)
{
    KisToolFreehand::activate(tmp);
}

void KisToolSelectBrush::initPaint(KoPointerEvent* /*e*/)
{
    if (!currentImage() || !currentNode() || !currentNode()->paintDevice()) return;

    m_mode = PAINT;
    m_dragDist = 0;

    // Create painter
    KisPaintDeviceSP dev = currentNode()->paintDevice();
    if (m_painter)
        delete m_painter;
#if 0 // XXX_SELECTION
    bool hasSelection = currentSelection();

    if (currentImage()->undo())
        m_transaction = new KisSelectedTransaction(i18n("Selection Brush"), dev);

#endif
    KisSelectionSP selection = currentSelection();
    Q_ASSERT(selection);

    m_target = selection.data();
    m_painter = new KisPainter(selection);
    Q_CHECK_PTR(m_painter);
    m_painter->setPaintColor(KoColor(Qt::black, selection->colorSpace()));
    m_painter->setOpacity(OPACITY_OPAQUE);//m_currentFgColor.colorSpace()->intensity8(m_currentFgColor));
    m_painter->setCompositeOp(selection->colorSpace()->compositeOp(COMPOSITE_OVER));
    m_painter->setPaintOpPreset(currentPaintOpPreset(), currentImage());

    // Set the cursor -- ideally. this should be a mask created from the brush,
    // now that X11 can handle colored cursors.
#if 0
    // Setting cursors has no effect until the tool is selected again; this
    // should be fixed.
    useCursor(KisCursor::brushCursor());
#endif
}

void KisToolSelectBrush::endPaint()
{
    m_mode = HOVER;
    if (currentImage() && currentNode()) {
        if (currentImage()->undo() && m_painter) {
            // If painting in mouse release, make sure painter
            // is destructed or end()ed
            currentImage()->undoAdapter()->addCommand(m_transaction);
        }
        delete m_painter;
        m_painter = 0;
        //notifyModified();
    }
}

#include "kis_tool_select_brush.moc"
