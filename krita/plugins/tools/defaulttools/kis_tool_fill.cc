/*
*  kis_tool_fill.cc - part of Krayon
*
*  Copyright (c) 2000 John Califf <jcaliff@compuzone.net>
*  Copyright (c) 2004 Boudewijn Rempt <boud@valdyas.org>
*  Copyright (c) 2004 Bart Coppens <kde@bartcoppens.be>
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

#include "kis_tool_fill.h"

#include <kis_debug.h>
#include <klocale.h>
#include <QLabel>
#include <QLayout>
#include <QCheckBox>

#include <QVector>
#include <QRect>
#include <QColor>

#include "knuminput.h"

#include "kis_layer.h"
#include "kis_cursor.h"
#include "kis_painter.h"
#include "kis_tool_brush.h"
#include "widgets/kis_cmb_composite.h"

#include "KoColorSpace.h"
#include "KoCanvasBase.h"
#include "KoPointerEvent.h"
#include "kis_pattern.h"
#include "kis_fill_painter.h"
#include "KoProgressUpdater.h"
#include "kis_selection.h"

KisToolFill::KisToolFill(KoCanvasBase * canvas)
        : KisToolPaint(canvas, KisCursor::load("tool_fill_cursor.png", 6, 6))
{
    setObjectName("tool_fill");
    m_fillPainter = 0;
    m_painter = 0;
    m_oldColor = 0;
    m_threshold = 15;
    m_usePattern = false;
    m_unmerged = false;
    m_fillOnlySelection = false;

}

KisToolFill::~KisToolFill()
{
}

bool KisToolFill::flood(int startX, int startY)
{

    KisPaintDeviceSP device = currentNode()->paintDevice();
    if (!device) return false;
    KisSelectionSP selection = currentSelection();

    if (m_fillOnlySelection && selection) {
#ifdef __GNUC__
#warning Port the fixes for filling the selection from 1.6!
#endif

        QRect rc = selection->selectedRect();
        KisPaintDeviceSP filled = new KisPaintDevice(device->colorSpace());
        delete m_fillPainter;
        m_fillPainter = new KisFillPainter(filled);
        Q_CHECK_PTR(m_fillPainter);

        //KisFillPainter painter(filled);
        // really filled.
        if (m_usePattern)
            m_fillPainter->fillRect(0, 0,
                                    currentImage()->width(),
                                    currentImage()->height(),
                                    currentPattern());
        else
            m_fillPainter->fillRect(0, 0,
                                    currentImage()->width(),
                                    currentImage()->height(),
                                    currentFgColor(),
                                    m_opacity);

        QRegion dirty = m_fillPainter->dirtyRegion();

        delete m_fillPainter;
        m_fillPainter = 0;

        m_painter = new KisPainter(device, currentSelection());
        Q_CHECK_PTR(m_painter);


        m_painter->beginTransaction(i18n("Fill"));

        QVector<QRect> rects = dirty.rects();

        QVector<QRect>::iterator it = rects.begin();
        QVector<QRect>::iterator end = rects.end();

        m_painter->setCompositeOp(m_compositeOp);
        m_painter->setOpacity(m_opacity);
        
        while (it != end) {
            QRect rc = *it;
            m_painter->bitBlt(rc.topLeft(), filled, rc);
            ++it;
        }

        device->setDirty(dirty);

        m_canvas->addCommand(m_painter->endTransaction());
    } else {

        delete m_fillPainter;
        m_fillPainter = new KisFillPainter(device, currentSelection());
        Q_CHECK_PTR(m_fillPainter);

        m_fillPainter->beginTransaction(i18n("Flood Fill"));
        setupPainter(m_fillPainter);
        m_fillPainter->setOpacity(m_opacity);
        m_fillPainter->setFillThreshold(m_threshold);
        m_fillPainter->setCompositeOp(m_compositeOp);
        m_fillPainter->setSampleMerged(!m_unmerged);
        m_fillPainter->setCareForSelection(true);
        m_fillPainter->setWidth(currentImage()->width());
        m_fillPainter->setHeight(currentImage()->height());
        // Enable this code again when I know how progress works
        //  KoUpdater *progress = ??
//     if (progress) {
//  progress->setSubject(m_fillPainter, true, true);
//     }

        if (m_usePattern)
            m_fillPainter->fillPattern(startX, startY, currentImage()->mergedImage());
        else
            m_fillPainter->fillColor(startX, startY, currentImage()->mergedImage());

        QRegion dirtyRegion = m_fillPainter->dirtyRegion();
        device->setDirty(dirtyRegion);

        m_canvas->addCommand(m_fillPainter->endTransaction());

        delete m_fillPainter;
        m_fillPainter = 0;
    }
    return true;
}

void KisToolFill::mousePressEvent(KoPointerEvent *e)
{
    QPointF pos = convertToPixelCoord(e);
    m_startPos = pos;
}

void KisToolFill::mouseReleaseEvent(KoPointerEvent *e)
{

    if (!m_canvas) return;
    if (!currentNode()) return;
    if (!currentImage() || !currentNode()->paintDevice()) return;
    if (e->button() == Qt::LeftButton) {
        int x, y;

        x = static_cast<int>(m_startPos.x());
        y = static_cast<int>(m_startPos.y());

        if (!currentImage()->bounds().contains(x, y)) {
            return;
        }

        flood(x, y);
        notifyModified();
    } else {
        KisToolPaint::mouseReleaseEvent(e);
    }
}

QWidget* KisToolFill::createOptionWidget()
{
    //QWidget *widget = KisToolPaint::createOptionWidget(parent);
    QWidget *widget = KisToolPaint::createOptionWidget();
    widget->setObjectName(toolId() + " option widget");
    m_lbThreshold = new QLabel(i18n("Threshold: "), widget);
    m_slThreshold = new KIntNumInput(widget);
    m_slThreshold->setObjectName("int_widget");
    m_slThreshold->setRange(1, 100);
    m_slThreshold->setSteps(3, 3);
    m_slThreshold->setValue(m_threshold);
    connect(m_slThreshold, SIGNAL(valueChanged(int)), this, SLOT(slotSetThreshold(int)));

    m_checkUsePattern = new QCheckBox(i18n("Use pattern"), widget);
    m_checkUsePattern->setToolTip(i18n("When checked do not use the foreground color, but the gradient selected to fill with"));
    m_checkUsePattern->setChecked(m_usePattern);
    connect(m_checkUsePattern, SIGNAL(toggled(bool)), this, SLOT(slotSetUsePattern(bool)));

    m_checkSampleMerged = new QCheckBox(i18n("Limit to current layer"), widget);
    m_checkSampleMerged->setChecked(m_unmerged);
    connect(m_checkSampleMerged, SIGNAL(toggled(bool)), this, SLOT(slotSetSampleMerged(bool)));

    m_checkFillSelection = new QCheckBox(i18n("Fill entire selection"), widget);
    m_checkFillSelection->setToolTip(i18n("When checked do not look at the current layer colors, but just fill all of the selected area"));
    m_checkFillSelection->setChecked(m_fillOnlySelection);
    connect(m_checkFillSelection, SIGNAL(toggled(bool)), this, SLOT(slotSetFillSelection(bool)));

    addOptionWidgetOption(m_slThreshold, m_lbThreshold);

    addOptionWidgetOption(m_checkFillSelection);
    addOptionWidgetOption(m_checkSampleMerged);
    addOptionWidgetOption(m_checkUsePattern);

    return widget;
}

void KisToolFill::slotSetThreshold(int threshold)
{
    m_threshold = threshold;
}

void KisToolFill::slotSetUsePattern(bool state)
{
    m_usePattern = state;
}

void KisToolFill::slotSetSampleMerged(bool state)
{
    m_unmerged = state;
}

void KisToolFill::slotSetFillSelection(bool state)
{
    m_fillOnlySelection = state;
    m_slThreshold->setEnabled(!state);
    m_checkSampleMerged->setEnabled(!state);
}

#include "kis_tool_fill.moc"
