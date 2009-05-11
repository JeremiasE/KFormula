/*
 *  kis_tool_select_contiguous - part of Krayon^WKrita
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
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

#include "kis_tool_select_contiguous.h"
#include <QPainter>
#include <QLayout>
#include <QLabel>
#include <QApplication>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <kis_debug.h>
#include <klocale.h>
#include <knuminput.h>
#include <kcolorbutton.h>

#include "KoPointerEvent.h"

#include "kis_cursor.h"
#include "kis_selection_manager.h"
#include "kis_image.h"
#include "canvas/kis_canvas2.h"
#include "kis_layer.h"
#include "kis_selection_options.h"
#include "kis_paint_device.h"
#include "kis_iterators_pixel.h"
#include "kis_fill_painter.h"
#include "kis_pixel_selection.h"
#include "kis_selection_tool_helper.h"

KisToolSelectContiguous::KisToolSelectContiguous(KoCanvasBase *canvas)
        : KisTool(canvas, KisCursor::load("tool_contiguous_selection_cursor.png", 6, 6))
{
    setObjectName("tool_select_contiguous");
    m_optWidget = 0;
    m_fuzziness = 20;
    m_sampleMerged = false;
    m_selectAction = SELECTION_REPLACE;
}

KisToolSelectContiguous::~KisToolSelectContiguous()
{
}

void KisToolSelectContiguous::activate(bool tmp)
{
    KisTool::activate(tmp);

    if (!m_optWidget)
        return;

    m_optWidget->slotActivated();
}

void KisToolSelectContiguous::mousePressEvent(KoPointerEvent * e)
{
    if (e->button() != Qt::LeftButton)
        return;

    if (m_canvas && currentImage()) {
        QApplication::setOverrideCursor(KisCursor::waitCursor());

        if (!currentNode())
            return;
        KisPaintDeviceSP dev = currentNode()->paintDevice();

        if (!dev || !currentNode()->visible())
            return;

        QPoint pos = convertToIntPixelCoord(e);
        QRect rc = currentImage()->bounds();
        KisFillPainter fillpainter(dev);
        fillpainter.setHeight(rc.height());
        fillpainter.setWidth(rc.width());
        fillpainter.setFillThreshold(m_fuzziness);
        fillpainter.setSampleMerged(m_sampleMerged);
        KisSelectionSP selection =
            fillpainter.createFloodSelection(pos.x(), pos.y(), currentImage()->mergedImage());

        KisCanvas2 * kisCanvas = dynamic_cast<KisCanvas2*>(m_canvas);
        if (!kisCanvas)
            return;

        KisSelectionToolHelper helper(kisCanvas, currentNode(), i18n("Contiguous Area Selection"));
        QUndoCommand* cmd = helper.selectPixelSelection(selection->pixelSelection(), m_selectAction);
        m_canvas->addCommand(cmd);

        QApplication::restoreOverrideCursor();
    }
}

void KisToolSelectContiguous::paint(QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

void KisToolSelectContiguous::slotSetFuzziness(int fuzziness)
{
    m_fuzziness = fuzziness;
}

void KisToolSelectContiguous::slotSetAction(int action)
{
    if (action >= SELECTION_REPLACE && action <= SELECTION_INTERSECT)
        m_selectAction = (selectionAction)action;
// XXX: Fix cursors when then are done.
//     switch(m_selectAction) {
//         case SELECTION_ADD:
//             m_subject->setCanvasCursor(KisCursor::pickerPlusCursor());
//             break;
//         case SELECTION_SUBTRACT:
//             m_subject->setCanvasCursor(KisCursor::pickerMinusCursor());
//     };
}

QWidget* KisToolSelectContiguous::createOptionWidget()
{
    KisCanvas2* canvas = dynamic_cast<KisCanvas2*>(m_canvas);
    Q_ASSERT(canvas);
    m_optWidget = new KisSelectionOptions(canvas);
    Q_CHECK_PTR(m_optWidget);
    m_optWidget->setObjectName(toolId() + " option widget");
    m_optWidget->setWindowTitle(i18n("Contiguous Area Selection"));
    m_optWidget->disableAntiAliasSelectionOption();
    m_optWidget->disableSelectionModeOption();

    QVBoxLayout * l = dynamic_cast<QVBoxLayout*>(m_optWidget->layout());
    Q_ASSERT(l);
    if (l) {
        l->setSpacing(6);

        connect(m_optWidget, SIGNAL(actionChanged(int)), this, SLOT(slotSetAction(int)));

        QHBoxLayout * hbox = new QHBoxLayout();
        Q_CHECK_PTR(hbox);
        l->addLayout(hbox);

        QLabel * lbl = new QLabel(i18n("Fuzziness: "), m_optWidget);
        hbox->addWidget(lbl);

        KIntNumInput * input = new KIntNumInput(m_optWidget);
        Q_CHECK_PTR(input);
        input->setObjectName("fuzziness");

        input->setRange(0, 200, 10, true);
        input->setValue(20);
        hbox->addWidget(input);
        connect(input, SIGNAL(valueChanged(int)), this, SLOT(slotSetFuzziness(int)));

        QCheckBox* samplemerged = new QCheckBox(i18n("Sample merged"), m_optWidget);
        l->addWidget(samplemerged);
        samplemerged->setChecked(m_sampleMerged);
        connect(samplemerged, SIGNAL(stateChanged(int)),
                this, SLOT(slotSetSampleMerged(int)));

        l->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));
    }

    return m_optWidget;
}

QWidget* KisToolSelectContiguous::optionWidget()
{
    return m_optWidget;
}

void KisToolSelectContiguous::slotSetSampleMerged(int state)
{
    if (state == Qt::PartiallyChecked)
        return;
    m_sampleMerged = (state == Qt::Checked);
}

#include "kis_tool_select_contiguous.moc"
