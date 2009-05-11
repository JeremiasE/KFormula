/*
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2006
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

#include "kis_qpainter_canvas.h"


#include <QPaintEvent>
#include <QRect>
#include <QPainter>
#include <QImage>
#include <QBrush>
#include <QColor>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QPixmap>
#include <QApplication>
#include <QMenu>

#include <kis_debug.h>
#include <kxmlguifactory.h>

#include <KoColorProfile.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoShapeManager.h>
#include <KoZoomHandler.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>

#include <kis_image.h>
#include <kis_layer.h>

#include "kis_view2.h"
#include "kis_canvas2.h"
#include "kis_prescaled_projection.h"
#include "kis_config.h"
#include "kis_canvas_resource_provider.h"
#include "kis_doc2.h"
#include "kis_selection_manager.h"
#include "kis_selection.h"
#include "kis_perspective_grid_manager.h"
#include "kis_config_notifier.h"

//#define DEBUG_REPAINT

#if QT_VERSION < 0x040500
#define WORKAROUNT_TABLET_TRACKING_BUG
#endif

class KisQPainterCanvas::Private
{
public:
    Private(const KoViewConverter *vc)
            : toolProxy(0),
            canvas(0),
            viewConverter(vc)
#ifdef WORKAROUNT_TABLET_TRACKING_BUG
            , previousEvent(QEvent::TabletRelease, QPoint(), QPoint(), QPointF(), QTabletEvent::NoDevice, 0, 0.0, 0, 0, 0, 0, 0, Qt::NoModifier, Q_INT64_C(932838457459459))
#endif
            {
    }

    KisPrescaledProjectionSP prescaledProjection;
    KoToolProxy * toolProxy;
    KisCanvas2 * canvas;
    const KoViewConverter * viewConverter;
    QBrush checkBrush;
    /// the offset of the view in the document, expressed in the view reference (not in the document reference)
    QPoint documentOffset;
#ifdef WORKAROUNT_TABLET_TRACKING_BUG
    bool tabletDown;
    QTabletEvent previousEvent;
#endif
    QTimer blockMouseEvent;
};

KisQPainterCanvas::KisQPainterCanvas(KisCanvas2 * canvas, QWidget * parent)
        : QWidget(parent)
        , m_d(new Private(canvas->viewConverter()))
{
    // XXX: Reset pattern size and color when the properties change!

    KisConfig cfg;

    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), SLOT(slotConfigChanged()));

    setAutoFillBackground(true);
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute( Qt::WA_InputMethodEnabled, true);
    setAttribute( Qt::WA_StaticContents );
    setAttribute( Qt::WA_OpaquePaintEvent );


    m_d->canvas =  canvas;
    m_d->toolProxy = canvas->toolProxy();
    m_d->checkBrush = QBrush(checkImage(cfg.checkSize()));
    m_d->blockMouseEvent.setSingleShot(true);
#ifdef WORKAROUNT_TABLET_TRACKING_BUG
    m_d->tabletDown = false;
#endif
}

KisQPainterCanvas::~KisQPainterCanvas()
{
    delete m_d;
}

void KisQPainterCanvas::setPrescaledProjection(KisPrescaledProjectionSP prescaledProjection)
{
    m_d->prescaledProjection = prescaledProjection;
}

void KisQPainterCanvas::paintEvent(QPaintEvent * ev)
{
    KisConfig cfg;

    KisImageSP img = m_d->canvas->image();
    if (img == 0) return;

    setAutoFillBackground(false);

    QPainter gc(this);
    gc.setCompositionMode(QPainter::CompositionMode_Source);

    // Don't draw the checks if we draw a cached pixmap, because we
    // need alpha transparency for checks. The precached pixmap
    // already should contain checks.
    if (!cfg.noXRender()) {

        if (cfg.scrollCheckers()) {

            QRect fillRect = ev->rect();

            if (m_d->documentOffset.x() > 0) {
                fillRect.adjust(0, 0, m_d->documentOffset.x(), 0);
            } else {
                fillRect.adjust(m_d->documentOffset.x(), 0, 0, 0);
            }
            if (m_d->documentOffset.y() > 0) {
                fillRect.adjust(0, 0, 0, m_d->documentOffset.y());
            } else {
                fillRect.adjust(0, m_d->documentOffset.y(), 0, 0);
            }
            gc.save();
            gc.translate(-m_d->documentOffset);
            gc.fillRect(fillRect, m_d->checkBrush);
            gc.restore();
        } else {
            // Checks
            gc.fillRect(ev->rect(), m_d->checkBrush);
        }
    }
    gc.setCompositionMode(QPainter::CompositionMode_SourceOver);

    if (cfg.noXRender()) {
        gc.drawPixmap(ev->rect(), m_d->prescaledProjection->prescaledPixmap(), ev->rect());
    } else {
        gc.drawImage(ev->rect(), m_d->prescaledProjection->prescaledQImage(), ev->rect());
    }

#ifdef DEBUG_REPAINT
    QColor color = QColor(random() % 255, random() % 255, random() % 255, 150);
    gc.fillRect(ev->rect(), color);
#endif
    drawDecorations(gc, true, m_d->documentOffset, ev->rect(), m_d->canvas);

    gc.end();

}

void KisQPainterCanvas::enterEvent(QEvent* e)
{
    QWidget::enterEvent(e);
}

void KisQPainterCanvas::leaveEvent(QEvent* e)
{
#ifdef WORKAROUNT_TABLET_TRACKING_BUG
    if (m_d->tabletDown) {
        m_d->tabletDown = false;
        QTabletEvent* fakeEvent =
            new QTabletEvent(QEvent::TabletRelease, m_d->previousEvent.pos(),
                m_d->previousEvent.globalPos(), m_d->previousEvent.hiResGlobalPos(), m_d->previousEvent.device(),
                m_d->previousEvent.pointerType(), m_d->previousEvent.pressure(), m_d->previousEvent.xTilt(),
                m_d->previousEvent.yTilt(), m_d->previousEvent.tangentialPressure(), m_d->previousEvent.rotation(),
                m_d->previousEvent.z(), m_d->previousEvent.modifiers(), m_d->previousEvent.uniqueId());

        // HACK this fake event is a work around a bug in Qt which stops sending tablet events when the
        // tablet pen move outside the widget (and you get a nasty surprise when the cursor moves back
        // on the widget especially if you have released your tablet as krita is still in drawing mode)
        m_d->toolProxy->tabletEvent(fakeEvent , QPointF());
    }
#endif
    QWidget::leaveEvent(e);
}

void KisQPainterCanvas::mouseMoveEvent(QMouseEvent *e)
{
    if (m_d->blockMouseEvent.isActive()) return;
    m_d->toolProxy->mouseMoveEvent(e, m_d->viewConverter->viewToDocument(e->pos() + m_d->documentOffset));
}

void KisQPainterCanvas::contextMenuEvent(QContextMenuEvent *e)
{
    m_d->canvas->view()->unplugActionList("flake_tool_actions");
    m_d->canvas->view()->plugActionList("flake_tool_actions",
                                        m_d->toolProxy->popupActionList());
    QMenu *menu = dynamic_cast<QMenu*>(m_d->canvas->view()->factory()->container("image_popup", m_d->canvas->view()));
    if (menu)
        menu->exec(e->globalPos());
}

void KisQPainterCanvas::mousePressEvent(QMouseEvent *e)
{
    if (m_d->blockMouseEvent.isActive()) return;
    m_d->toolProxy->mousePressEvent(e, m_d->viewConverter->viewToDocument(e->pos() + m_d->documentOffset));
}

void KisQPainterCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_d->blockMouseEvent.isActive()) return;
    m_d->toolProxy->mouseReleaseEvent(e, m_d->viewConverter->viewToDocument(e->pos() + m_d->documentOffset));
}

void KisQPainterCanvas::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (m_d->blockMouseEvent.isActive()) return;
    m_d->toolProxy->mouseDoubleClickEvent(e, m_d->viewConverter->viewToDocument(e->pos() + m_d->documentOffset));
}

void KisQPainterCanvas::keyPressEvent(QKeyEvent *e)
{
    m_d->toolProxy->keyPressEvent(e);
    if (! e->isAccepted()) {
        if (e->key() == Qt::Key_Backtab
                || (e->key() == Qt::Key_Tab && (e->modifiers() & Qt::ShiftModifier)))
            focusNextPrevChild(false);
        else if (e->key() == Qt::Key_Tab)
            focusNextPrevChild(true);
    }
}

void KisQPainterCanvas::keyReleaseEvent(QKeyEvent *e)
{
    m_d->toolProxy->keyReleaseEvent(e);
}

QVariant KisQPainterCanvas::inputMethodQuery(Qt::InputMethodQuery query) const
{
    return m_d->toolProxy->inputMethodQuery(query, *m_d->viewConverter);
}

void KisQPainterCanvas::inputMethodEvent(QInputMethodEvent *event)
{
    m_d->toolProxy->inputMethodEvent(event);
}

void KisQPainterCanvas::tabletEvent(QTabletEvent *e)
{
    m_d->blockMouseEvent.start(100);
#ifdef WORKAROUNT_TABLET_TRACKING_BUG
    switch (e->type()) {
    case QEvent::TabletPress:
        m_d->tabletDown = true;
        break;
    case QEvent::TabletRelease:
        m_d->tabletDown = false;
        break;
    case QEvent::TabletMove:
        break;
    default:
        ; // ignore the rest.
    }
#endif
    qreal subpixelX = e->hiResGlobalX();
    subpixelX = subpixelX - ((int) subpixelX); // leave only part behind the dot
    qreal subpixelY = e->hiResGlobalY();
    subpixelY = subpixelY - ((int) subpixelY); // leave only part behind the dot
    QPointF pos(e->x() + subpixelX + m_d->documentOffset.x(), e->y() + subpixelY + m_d->documentOffset.y());

#ifdef WORKAROUNT_TABLET_TRACKING_BUG
    m_d->previousEvent = *e;
#endif
    m_d->toolProxy->tabletEvent(e, m_d->viewConverter->viewToDocument(pos));
}

void KisQPainterCanvas::wheelEvent(QWheelEvent *e)
{
    m_d->toolProxy->wheelEvent(e, m_d->viewConverter->viewToDocument(e->pos() + m_d->documentOffset));
}

KoToolProxy * KisQPainterCanvas::toolProxy()
{
    return m_d->toolProxy;
}

void KisQPainterCanvas::documentOffsetMoved(const QPoint & pt)
{
    m_d->documentOffset = pt;
    m_d->prescaledProjection->documentOffsetMoved(pt);
    update();
}

void KisQPainterCanvas::resizeEvent(QResizeEvent *e)
{
    QSize size(e->size());
    if (size.width() <= 0) {
        size.setWidth(1);
    }
    if (size.height() <= 0) {
        size.setHeight(1);
    }
    m_d->prescaledProjection->resizePrescaledImage(size);
}

void KisQPainterCanvas::slotConfigChanged()
{
    KisConfig cfg;

    m_d->checkBrush = QBrush(checkImage(cfg.checkSize()));
}

#include "kis_qpainter_canvas.moc"
