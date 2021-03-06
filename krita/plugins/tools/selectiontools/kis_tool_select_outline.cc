/*
 *  kis_tool_select_freehand.h - part of Krayon^WKrita
 *
 *  Copyright (c) 2000 John Califf <jcaliff@compuzone.net>
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2004 Boudewijn Rempt <boud@valdyas.org>
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

#include "kis_tool_select_outline.h"

#include <QApplication>
#include <QPainter>
#include <QWidget>
#include <QLayout>
#include <QVBoxLayout>

#include <kis_debug.h>
#include <klocale.h>

#include <KoPointerEvent.h>
#include <KoShapeController.h>
#include <KoPathShape.h>
#include <KoColorSpace.h>
#include <KoCompositeOp.h>

#include <kis_layer.h>
#include <kis_selection_options.h>
#include <kis_cursor.h>
#include <kis_image.h>

#include "kis_selected_transaction.h"
#include "kis_painter.h"
#include "kis_paintop_registry.h"
#include "canvas/kis_canvas2.h"
#include "kis_pixel_selection.h"
#include "kis_selection_tool_helper.h"

KisToolSelectOutline::KisToolSelectOutline(KoCanvasBase * canvas)
        : KisTool(canvas, KisCursor::load("tool_outline_selection_cursor.png", 5, 5))
{
    m_dragging = false;
    m_optWidget = 0;
    m_selectAction = SELECTION_REPLACE;
    m_selectionMode = PIXEL_SELECTION;
}

KisToolSelectOutline::~KisToolSelectOutline()
{
}

void KisToolSelectOutline::activate(bool temporary)
{
    KisTool::activate(temporary);

    if (!m_optWidget)
        return;

    m_optWidget->slotActivated();
}


void KisToolSelectOutline::mousePressEvent(KoPointerEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_points.clear();
        m_points.append(convertToPixelCoord(event));
    }
}

void KisToolSelectOutline::mouseMoveEvent(KoPointerEvent *event)
{
    if (m_dragging) {
        m_points.append(convertToPixelCoord(event));
        updateFeedback();
    }
}

void KisToolSelectOutline::mouseReleaseEvent(KoPointerEvent *event)
{
    if (!m_canvas)
        return;

    if (m_dragging && event->button() == Qt::LeftButton) {
        m_dragging = false;
        deactivate();

        if (currentImage()) {
            QApplication::setOverrideCursor(KisCursor::waitCursor());

            KisCanvas2 * kisCanvas = dynamic_cast<KisCanvas2*>(m_canvas);
            if (!kisCanvas)
                return;

            if (!currentNode())
                return;

            KisSelectionToolHelper helper(kisCanvas, currentNode(), i18n("Outline Selection"));

            if (m_selectionMode == PIXEL_SELECTION) {

                KisPixelSelectionSP tmpSel = KisPixelSelectionSP(new KisPixelSelection());

                KisPainter painter(tmpSel);
                painter.setBounds(currentImage()->bounds());
                painter.setPaintColor(KoColor(Qt::black, tmpSel->colorSpace()));
                painter.setFillStyle(KisPainter::FillStyleForegroundColor);
                painter.setStrokeStyle(KisPainter::StrokeStyleNone);
                painter.setOpacity(OPACITY_OPAQUE);
                painter.setPaintOpPreset(currentPaintOpPreset(), currentImage());
                painter.setAntiAliasPolygonFill(m_optWidget->antiAliasSelection());
                painter.setCompositeOp(tmpSel->colorSpace()->compositeOp(COMPOSITE_OVER));
                painter.paintPolygon(m_points);

                QUndoCommand* cmd = helper.selectPixelSelection(tmpSel, m_selectAction);
                m_canvas->addCommand(cmd);
            } else {

                if (m_points.count() > 1) {
                    KoPathShape* path = new KoPathShape();
                    path->setShapeId(KoPathShapeId);

                    QMatrix resolutionMatrix;
                    resolutionMatrix.scale(1 / currentImage()->xRes(), 1 / currentImage()->yRes());
                    path->moveTo(resolutionMatrix.map(m_points[0]));
                    for (int i = 1; i < m_points.count(); i++)
                        path->lineTo(resolutionMatrix.map(m_points[i]));
                    path->close();
                    path->normalize();

                    helper.addSelectionShape(path);
                }
            }
            QApplication::restoreOverrideCursor();
        }

        m_points.clear();
    }
}

#define FEEDBACK_LINE_WIDTH 1

void KisToolSelectOutline::paint(QPainter& gc, const KoViewConverter &converter)
{
    Q_UNUSED(converter);

    if (m_dragging && m_points.count() > 1) {

        QPen pen(Qt::white, FEEDBACK_LINE_WIDTH, Qt::DotLine);
        gc.save();
        gc.setPen(pen);

        for (qint32 pointIndex = 0; pointIndex < m_points.count() - 1; pointIndex++) {
            QPointF startPos = pixelToView(m_points[pointIndex]);
            QPointF endPos = pixelToView(m_points[pointIndex + 1]);
            gc.drawLine(startPos, endPos);
        }

        gc.restore();
    }
}

void KisToolSelectOutline::updateFeedback()
{
    if (m_points.count() > 1) {
        qint32 lastPointIndex = m_points.count() - 1;

        QRectF updateRect = QRectF(m_points[lastPointIndex - 1], m_points[lastPointIndex]).normalized();
        updateRect.adjust(-FEEDBACK_LINE_WIDTH, -FEEDBACK_LINE_WIDTH, FEEDBACK_LINE_WIDTH, FEEDBACK_LINE_WIDTH);

        updateCanvasPixelRect(updateRect);
    }
}

void KisToolSelectOutline::deactivate()
{
    if (m_canvas) {
        updateCanvasPixelRect(image()->bounds());
    }
}

QWidget* KisToolSelectOutline::createOptionWidget()
{
    KisCanvas2* canvas = dynamic_cast<KisCanvas2*>(m_canvas);
    Q_ASSERT(canvas);
    m_optWidget = new KisSelectionOptions(canvas);
    Q_CHECK_PTR(m_optWidget);
    m_optWidget->setObjectName(toolId() + " option widget");
    
    m_optWidget->setWindowTitle(i18n("Outline Selection"));

    connect(m_optWidget, SIGNAL(actionChanged(int)), this, SLOT(slotSetAction(int)));
    connect(m_optWidget, SIGNAL(modeChanged(int)), this, SLOT(slotSetSelectionMode(int)));

    QVBoxLayout * l = dynamic_cast<QVBoxLayout*>(m_optWidget->layout());
    Q_ASSERT(l);
    if (l) {
        l->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));
    }

    return m_optWidget;
}

QWidget* KisToolSelectOutline::optionWidget()
{
    return m_optWidget;
}

void KisToolSelectOutline::slotSetAction(int action)
{
    if (action >= SELECTION_REPLACE && action <= SELECTION_INTERSECT)
        m_selectAction = (selectionAction)action;
}

void KisToolSelectOutline::slotSetSelectionMode(int mode)
{
    m_selectionMode = (selectionMode)mode;
}

#include "kis_tool_select_outline.moc"

