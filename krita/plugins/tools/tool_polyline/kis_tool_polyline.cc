/*
 *  Copyright (c) 2004 Michael Thaler <michael.thaler@physik.tu-muenchen.de>
 *  Copyright (c) 2008 Boudewijn Rempt <boud@valdyas.org>
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

#include "kis_tool_polyline.h"

#include <math.h>

#include <QPainter>
#include <QSpinBox>
#include <QKeyEvent>
#include <QMenu>

#include <kaction.h>
#include <kicon.h>
#include <klocale.h>
#include <kis_debug.h>
#include <knuminput.h>

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>

#include <kis_selection.h>
#include <kis_painter.h>
#include <kis_cursor.h>
#include <kis_paint_information.h>
#include <kis_paintop_preset.h>

KisToolPolyline::KisToolPolyline(KoCanvasBase * canvas)
        : KisToolPaint(canvas, KisCursor::load("tool_polyline_cursor.png", 6, 6)),
        m_dragging(false)
{
    setObjectName("tool_polyline");

    KAction *action = new KAction(i18n("&Finish Polyline"), this);
    addAction("finish_polyline", action);
    connect(action, SIGNAL(triggered()), this, SLOT(finish()));
    action = new KAction(KIcon("dialog-cancel"), i18n("&Cancel"), this);
    addAction("cancel_polyline", action);
    connect(action, SIGNAL(triggered()), this, SLOT(cancel()));


    QList<QAction*> list;
    list.append(this->action("finish_polyline"));
    list.append(this->action("cancel_polyline"));
    setPopupActionList(list);
}

KisToolPolyline::~KisToolPolyline()
{
}

void KisToolPolyline::mousePressEvent(KoPointerEvent *event)
{
    if (currentImage()) {
        if (event->button() == Qt::LeftButton && event->modifiers() != Qt::ShiftModifier) {

            m_dragging = true;

            if (m_points.isEmpty()) {
                m_dragStart = convertToPixelCoord(event);
                m_dragEnd = convertToPixelCoord(event);
                m_points.append(m_dragStart);
                m_boundingRect = QRectF(m_dragStart.x(), m_dragStart.y(), 0, 0);
            } else {
                m_dragStart = m_dragEnd;
                m_dragEnd = convertToPixelCoord(event);
            }
        } else if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ShiftModifier) {
            finish();
        }
    }
}

void KisToolPolyline::deactivate()
{
    m_points.clear();
    m_dragging = false;
}

void KisToolPolyline::finish()
{
    m_dragging = false;

    if (!currentNode())
        return;

    KisPaintDeviceSP device = currentNode()->paintDevice();
    if (!device) return;

    KisPainter painter(device, currentSelection());
    painter.beginTransaction(i18n("Polyline"));

    painter.setPaintColor(currentFgColor());
    painter.setOpacity(m_opacity);
    painter.setCompositeOp(m_compositeOp);
    painter.setPaintOpPreset(currentPaintOpPreset(), currentImage()); // Painter takes ownership

    QPointF start, end;
    KoPointVector::iterator it;
    for (it = m_points.begin(); it != m_points.end(); ++it) {
        if (it == m_points.begin()) {
            start = (*it);
        } else {
            end = (*it);
            painter.paintLine(start, end);
            start = end;
        }
    }
    m_points.clear();

    device->setDirty(painter.dirtyRegion());
    notifyModified();

    m_canvas->addCommand(painter.endTransaction());
}

void KisToolPolyline::cancel()
{
    m_dragging = false;
    m_points.clear();
}

void KisToolPolyline::mouseMoveEvent(KoPointerEvent *event)
{
    if (m_dragging) {
        //Erase old lines
        m_canvas->updateCanvas(m_boundingRect.unite(QRectF(m_dragStart.x(), m_dragStart.y(), m_dragEnd.x() - m_dragStart.x(), m_dragEnd.y() - m_dragStart.y())));

        // get current mouse position
        m_dragEnd = convertToPixelCoord(event);
    }
    m_canvas->updateCanvas(m_boundingRect.unite(QRectF(m_dragStart.x(), m_dragStart.y(), m_dragEnd.x() - m_dragStart.x(), m_dragEnd.y() - m_dragStart.y())));
}

void KisToolPolyline::mouseReleaseEvent(KoPointerEvent *event)
{
    if (!m_canvas || !currentImage())
        return;

    if (m_dragging && event->button() == Qt::LeftButton)  {
        m_dragging = false;
        m_points.append(m_dragEnd);
        m_boundingRect = m_boundingRect.unite(QRectF(m_dragStart.x(), m_dragStart.y(), m_dragEnd.x() - m_dragStart.x(), m_dragEnd.y() - m_dragStart.y()));
    }
    m_canvas->updateCanvas(m_boundingRect);
}


void KisToolPolyline::mouseDoubleClickEvent(KoPointerEvent *)
{
    finish();
}

void KisToolPolyline::paint(QPainter& gc, const KoViewConverter &converter)
{
    if (!m_canvas || !currentImage())
        return;

    qreal sx, sy;
    converter.zoom(&sx, &sy);

    gc.scale(sx / currentImage()->xRes(), sy / currentImage()->yRes());


    QPen pen(Qt::SolidLine);
    gc.setPen(pen);
    //gc.setRasterOp(Qt::XorROP);

    QPointF start, end;
    QPointF startPos;
    QPointF endPos;

    if (m_dragging) {
        startPos = m_dragStart;
        endPos = m_dragEnd;
        gc.drawLine(startPos, endPos);
    }
    for (KoPointVector::iterator it = m_points.begin(); it != m_points.end(); ++it) {

        if (it == m_points.begin()) {
            start = (*it);
        } else {
            end = (*it);

            startPos = start;
            endPos = end;

            gc.drawLine(startPos, endPos);

            start = end;
        }
    }
}

QString KisToolPolyline::quickHelp() const
{
    return i18n("Press shift-mouseclick to end the polyline.");
}

void KisToolPolyline::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        cancel();
}

#include "kis_tool_polyline.moc"
