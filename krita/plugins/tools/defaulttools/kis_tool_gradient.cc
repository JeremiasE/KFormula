/*
 *  kis_tool_gradient.cc - part of Krita
 *
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2003 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2004-2007 Adrian Page <adrian@pagenet.plus.com>
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

#include "kis_tool_gradient.h"

#if defined(HAVE_OPENGL) && defined(HAVE_GLEW)
#include <GL/glew.h>
#include <QGLWidget>
#endif

#include <cfloat>

#include <QPainter>
#include <QLabel>
#include <QLayout>
#include <QCheckBox>

#include <kis_debug.h>
#include <klocale.h>
#include <knuminput.h>
#include <kcombobox.h>

#include "KoPointerEvent.h"
#include "KoCanvasBase.h"
#include "KoViewConverter.h"

#include "widgets/kis_cmb_composite.h"
#include "kis_cursor.h"
#include "widgets/kis_double_widget.h"
#include "kis_gradient_painter.h"
#include "kis_painter.h"
#include "KoProgressUpdater.h"
#include "kis_canvas_resource_provider.h"
#include "kis_config.h"
#include "kis_layer.h"
#include <kis_selection.h>

#if defined(HAVE_OPENGL) && defined(HAVE_GLEW)
#include "opengl/kis_opengl.h"
#include "opengl/kis_opengl_gradient_program.h"
#include "opengl/kis_opengl_canvas2.h"
#include "canvas/kis_canvas2.h"
#include "KoSliderCombo.h"
#include "kis_config_notifier.h"
#endif

KisToolGradient::KisToolGradient(KoCanvasBase * canvas)
        : KisToolPaint(canvas, KisCursor::load("tool_gradient_cursor.png", 6, 6)),
        m_dragging(false)
{
    setObjectName("tool_gradient");

    m_startPos = QPointF(0, 0);
    m_endPos = QPointF(0, 0);

    m_reverse = false;
    m_shape = KisGradientPainter::GradientShapeLinear;
    m_repeat = KisGradientPainter::GradientRepeatNone;
    m_antiAliasThreshold = 0.2;

#if defined(HAVE_OPENGL) && defined(HAVE_GLEW)
    m_gradientProgram = 0;
    m_previewOpacityPercent = 75;
#endif
}

KisToolGradient::~KisToolGradient()
{
}

void KisToolGradient::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (m_dragging && m_startPos != m_endPos) {

#if defined(HAVE_OPENGL) && defined(HAVE_GLEW)
        if (m_gradientProgram) {

            QPointF gradientVector = m_endPos - m_startPos;
            double gradientVectorLength = sqrt((gradientVector.x() * gradientVector.x()) + (gradientVector.y() * gradientVector.y()));

            if (gradientVectorLength > DBL_EPSILON) {
                QPointF normalisedGradientVector;

                normalisedGradientVector.rx() = gradientVector.x() / gradientVectorLength;
                normalisedGradientVector.ry() = gradientVector.y() / gradientVectorLength;

                QPointF normalisedGradientVectorStart = m_startPos;
                normalisedGradientVectorStart /= gradientVectorLength;

                KisOpenGLCanvas2 *canvasWidget = dynamic_cast<KisOpenGLCanvas2 *>(m_canvas->canvasWidget());
                Q_ASSERT(canvasWidget);

                if (canvasWidget) {
                    canvasWidget->setPixelToViewTransformation();

                    glMatrixMode(GL_MODELVIEW);
                    glTranslatef(0.5, 0.5, 0.0);

                    glMatrixMode(GL_TEXTURE);
                    glLoadIdentity();
                    glScalef(1 / gradientVectorLength, 1 / gradientVectorLength, 1);

                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                    canvasWidget->makeCurrent();
                    m_gradientProgram->activate(normalisedGradientVectorStart,
                                                normalisedGradientVectorStart + normalisedGradientVector);

//                     glValidateProgramARB(m_gradientProgram->handle());
//                     dbgTools <<"Validate:";
//                     dbgTools << m_gradientProgram->getInfoLog();


                    glBegin(GL_QUADS);

                    glTexCoord2f(0.0, 0.0);
                    glVertex2f(0.0, 0.0);

                    glTexCoord2f(currentImage()->width(), 0.0);
                    glVertex2f(currentImage()->width(), 0.0);

                    glTexCoord2f(currentImage()->width(), currentImage()->height());
                    glVertex2f(currentImage()->width(), currentImage()->height());

                    glTexCoord2f(0.0, currentImage()->height());
                    glVertex2f(0.0, currentImage()->height());

                    glEnd();

                    KisOpenGLProgram::deactivate();

                    glDisable(GL_BLEND);

                    // Unbind the texture otherwise the ATI driver crashes when the canvas context is
                    // made current after the textures are deleted following an image resize.
                    glBindTexture(GL_TEXTURE_1D, 0);
                }
            }
        } else
#endif
        {
            qreal sx, sy;
            converter.zoom(&sx, &sy);
            painter.scale(sx / currentImage()->xRes(), sy / currentImage()->yRes());
            paintLine(painter);
        }
    }
}

void KisToolGradient::mousePressEvent(KoPointerEvent *e)
{
    if (!currentImage()) {
        return;
    }

    QPointF pos = convertToPixelCoord(e);

    if (e->button() == Qt::LeftButton) {
        m_dragging = true;
        m_startPos = pos;
        m_endPos = pos;

#if defined(HAVE_OPENGL) && defined(HAVE_GLEW)
        KisConfig cfg;

        if (cfg.useOpenGL() && cfg.useOpenGLShaders()) {

            KisCanvas2 *canvas = dynamic_cast<KisCanvas2 *>(m_canvas);
            KoColorProfile *monitorProfile = 0;

            Q_ASSERT(canvas);

            if (canvas) {
                monitorProfile = canvas->monitorProfile();
            }

            KisOpenGL::makeContextCurrent();
            m_gradientProgram = new KisOpenGLGradientProgram(currentGradient(),
                    m_shape,
                    m_repeat,
                    m_reverse,
                    currentImage()->colorSpace(),
                    monitorProfile,
                    m_previewOpacityPercent / 100.0);
        }
#endif
    }
}

void KisToolGradient::mouseMoveEvent(KoPointerEvent *e)
{
    if (m_dragging) {
        QPointF pos = convertToPixelCoord(e);

        QRectF bound;
        bound.setTopLeft(m_startPos);
        bound.setBottomRight(m_endPos);
        m_canvas->updateCanvas(convertToPt(bound.normalized()));

        if ((e->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier) {
            m_endPos = straightLine(pos);
        } else {
            m_endPos = pos;
        }

        bound.setTopLeft(m_startPos);
        bound.setBottomRight(m_endPos);
        m_canvas->updateCanvas(convertToPt(bound.normalized()));
    }
}

void KisToolGradient::mouseReleaseEvent(KoPointerEvent *e)
{
    if (!currentNode())
        return;

    if (m_dragging && e->button() == Qt::LeftButton) {

#if defined(HAVE_OPENGL) && defined(HAVE_GLEW)
        delete m_gradientProgram;
        m_gradientProgram = 0;
#endif

        m_dragging = false;

        if (m_startPos == m_endPos) {
            // controller->updateCanvas();
            m_dragging = false;
            return;
        }

        QPointF pos = convertToPixelCoord(e);

        if ((e->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier) {
            m_endPos = straightLine(pos);
        } else {
            m_endPos = pos;
        }

        KisPaintDeviceSP device;

        if (currentImage() && (device = currentNode()->paintDevice())) {

            KisGradientPainter painter(device, currentSelection());

            painter.beginTransaction(i18n("Gradient"));

            painter.setPaintColor(currentFgColor());
            painter.setGradient(currentGradient());
            painter.setOpacity(m_opacity);
            painter.setCompositeOp(m_compositeOp);


            /* KoUpdater *progress = m_subject->progressDisplay();

            if (progress) {
                progress->setSubject(&painter, true, true);
            }*/

            bool painted = painter.paintGradient(m_startPos, m_endPos, m_shape, m_repeat, m_antiAliasThreshold, m_reverse, 0, 0, currentImage()->width(), currentImage()->height());

            if (painted) {
                // does whole thing at moment
                device->setDirty(painter.dirtyRegion());

                notifyModified();

                m_canvas->addCommand(painter.endTransaction());
            }
        }
        QRectF bound;
        bound.setTopLeft(m_startPos);
        bound.setBottomRight(m_endPos);
        m_canvas->updateCanvas(convertToPt(bound.normalized()));
    }
}

QPointF KisToolGradient::straightLine(QPointF point)
{
    QPointF comparison = point - m_startPos;
    QPointF result;

    if (fabs(comparison.x()) > fabs(comparison.y())) {
        result.setX(point.x());
        result.setY(m_startPos.y());
    } else {
        result.setX(m_startPos.x());
        result.setY(point.y());
    }

    return result;
}

void KisToolGradient::paintLine(QPainter& gc)
{
    if (m_canvas) {
        QPen old = gc.pen();
        QPen pen(Qt::SolidLine);
        QPointF start;
        QPointF end;

        start = m_startPos;
        end = m_endPos;
        gc.setPen(pen);
        start = QPoint(static_cast<int>(start.x()), static_cast<int>(start.y()));
        end = QPoint(static_cast<int>(end.x()), static_cast<int>(end.y()));
        gc.drawLine(start, end);
        gc.setPen(old);
    }
}

QWidget* KisToolGradient::createOptionWidget()
{
    QWidget *widget = KisToolPaint::createOptionWidget();
    Q_CHECK_PTR(widget);
    widget->setObjectName(toolId() + " option widget");
    
    m_lbShape = new QLabel(i18n("Shape:"), widget);
    m_lbRepeat = new QLabel(i18n("Repeat:"), widget);

    m_ckReverse = new QCheckBox(i18nc("the gradient will be drawn with the color order reversed", "Reverse"), widget);
    m_ckReverse->setObjectName("reverse_check");
    connect(m_ckReverse, SIGNAL(toggled(bool)), this, SLOT(slotSetReverse(bool)));

    m_cmbShape = new KComboBox(widget);
    m_cmbShape->setObjectName("shape_combo");
    connect(m_cmbShape, SIGNAL(activated(int)), this, SLOT(slotSetShape(int)));
    m_cmbShape->addItem(i18nc("the gradient will be drawn linearly", "Linear"));
    m_cmbShape->addItem(i18nc("the gradient will be drawn bilinearly", "Bi-Linear"));
    m_cmbShape->addItem(i18nc("the gradient will be drawn radially", "Radial"));
    m_cmbShape->addItem(i18nc("the gradient will be drawn in a square around a centre", "Square"));
    m_cmbShape->addItem(i18nc("the gradient will be drawn as an assymmetric cone", "Conical"));
    m_cmbShape->addItem(i18nc("the gradient will be drawn as a symmetric cone", "Conical Symmetric"));

    m_cmbRepeat = new KComboBox(widget);
    m_cmbRepeat->setObjectName("repeat_combo");
    connect(m_cmbRepeat, SIGNAL(activated(int)), this, SLOT(slotSetRepeat(int)));
    m_cmbRepeat->addItem(i18nc("The gradient will not repeat", "None"));
    m_cmbRepeat->addItem(i18nc("The gradient will repeat forwards", "Forwards"));
    m_cmbRepeat->addItem(i18nc("The gradient will repeat alternatingly", "Alternating"));

    addOptionWidgetOption(m_cmbShape, m_lbShape);

    addOptionWidgetOption(m_cmbRepeat, m_lbRepeat);

    addOptionWidgetOption(m_ckReverse);

    m_lbAntiAliasThreshold = new QLabel(i18n("Anti-alias threshold:"), widget);

    m_slAntiAliasThreshold = new KDoubleNumInput(widget);
    m_slAntiAliasThreshold->setObjectName("threshold_slider");
    m_slAntiAliasThreshold->setMinimum(0);
    m_slAntiAliasThreshold->setMaximum(1);
    m_slAntiAliasThreshold->setValue(m_antiAliasThreshold);
    connect(m_slAntiAliasThreshold, SIGNAL(valueChanged(double)), this, SLOT(slotSetAntiAliasThreshold(double)));

    addOptionWidgetOption(m_slAntiAliasThreshold, m_lbAntiAliasThreshold);

#if defined(HAVE_OPENGL) && defined(HAVE_GLEW)
    m_lbPreviewOpacity = new QLabel(i18n("Preview opacity:"), widget);
    m_slPreviewOpacity = new KoSliderCombo(widget);
    m_slPreviewOpacity->setDecimals(0);
    m_slPreviewOpacity->setValue(m_previewOpacityPercent);
    connect(m_slPreviewOpacity, SIGNAL(valueChanged(int,bool)), this, SLOT(slotSetPreviewOpacity(int,bool)));

    addOptionWidgetOption(m_slPreviewOpacity, m_lbPreviewOpacity);

    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), SLOT(slotConfigChanged()));
    slotConfigChanged();
#endif

    return widget;
}

void KisToolGradient::slotSetShape(int shape)
{
    m_shape = static_cast<KisGradientPainter::enumGradientShape>(shape);
}

void KisToolGradient::slotSetRepeat(int repeat)
{
    m_repeat = static_cast<KisGradientPainter::enumGradientRepeat>(repeat);
}

void KisToolGradient::slotSetReverse(bool state)
{
    m_reverse = state;
}

void KisToolGradient::slotSetAntiAliasThreshold(double value)
{
    m_antiAliasThreshold = value;
}

#if defined(HAVE_OPENGL) && defined(HAVE_GLEW)
void KisToolGradient::slotSetPreviewOpacity(int value, bool final)
{
    m_previewOpacityPercent = value;
}

void KisToolGradient::slotConfigChanged()
{
    KisConfig cfg;
    bool enablePreviewOpacity = cfg.useOpenGL() && cfg.useOpenGLShaders();

    m_slPreviewOpacity->setEnabled(enablePreviewOpacity);
    m_lbPreviewOpacity->setEnabled(enablePreviewOpacity);
}
#endif

#include "kis_tool_gradient.moc"

