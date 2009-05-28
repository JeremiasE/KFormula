/*
 *  Copyright (c) 2005-2006 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "krs_painter.h"

#include "krs_paint_device.h"
#include "krs_brush.h"
#include "krs_color.h"
#include "krs_pattern.h"

#include <kis_convolution_painter.h>
#include <kis_paint_layer.h>
#include <kis_paintop_registry.h>
#include <kis_paint_information.h>

using namespace Scripting;

Painter::Painter(PaintDevice* layer)
        : QObject(layer)
        , m_layer(layer->paintDevice())
        , m_painter(new KisPainter(layer->paintDevice()))
        , m_threshold(1)
{
    setObjectName("KritaPainter");
}


Painter::~Painter()
{
    delete m_painter;
}

#if 0
void Painter::convolve()
{
    KisConvolutionPainter* cp = new KisConvolutionPainter(m_painter->device());
    QRect rect;
    KisKernel kernel;
    kernel.factor = Kross::Api::Variant::toInt(args->item(1));
    kernel.offset = Kross::Api::Variant::toInt(args->item(2));

    uint borderop = 3;
    if (args.count() > 3)
        borderop = Kross::Api::Variant::toUInt(args->item(3));
    uint channelsFlag = KoChannelInfo::FLAG_COLOR;
    if (args.count() > 4)
        channelsFlag = Kross::Api::Variant::toUInt(args->item(4));
    if (args.count() > 5) {
        uint x = Kross::Api::Variant::toUInt(args->item(5));
        uint y = Kross::Api::Variant::toUInt(args->item(6));
        uint w = Kross::Api::Variant::toUInt(args->item(7));
        uint h = Kross::Api::Variant::toUInt(args->item(8));
        rect = QRect(x, y, w, h);
    } else {
        QRect r1 = PaintDevice()->paintDevice()->extent();
        QRect r2 = PaintDevice()->image()->bounds();
        rect = r1.intersect(r2);
    }

    QList<QVariant> kernelH = Kross::Api::Variant::toList(args->item(0));

    QVariant firstlineVariant = *kernelH.begin();
    if (firstlineVariant.type() != QVariant::List)
        throw Kross::Api::Exception::Ptr(new Kross::Api::Exception(i18n("An error has occurred in %1", QString("applyConvolution"))));

    QList<QVariant> firstline = firstlineVariant.toList();

    kernel.height = kernelH.size();
    kernel.width = firstline.size();
    kernel.data = new qint32[kernel.height * kernel.width];

    uint i = 0;
    for (QList<QVariant>::iterator itK = kernelH.begin(); itK != kernelH.end(); itK++, i ++) {
        QVariant lineVariant = *kernelH.begin();
        if (lineVariant.type() != QVariant::List)
            throw Kross::Api::Exception::Ptr(new Kross::Api::Exception(i18n("An error has occurred in %1", QString("applyConvolution"))));
        QList<QVariant> line = firstlineVariant.toList();
        if (line.size() != kernel.width)
            throw Kross::Api::Exception::Ptr(new Kross::Api::Exception(i18n("An error has occurred in %1", QString("applyConvolution"))));
        uint j = 0;
        for (QList<QVariant>::iterator itLine = line.begin(); itLine != line.end(); itLine++, j ++)
            kernel.data[ j + i * kernel.width ] = (*itLine).toInt();
    }
    cp->applyMatrix(KisKernelSP(&kernel), rect.x(), rect.y(), rect.width(), rect.height(), (KisConvolutionBorderOp)borderop, (KoChannelInfo::enumChannelFlags) channelsFlag);

    delete[] kernel.data;
    return Kross::Api::Object::Ptr(0);
}
#endif

void Painter::setFillThreshold(int threshold)
{
    m_threshold = threshold;
}

void Painter::fillColor(uint x, uint y, uint w, uint h)
{
    KisFillPainter* fp = createFillPainter();
    fp->setWidth(w);
    fp->setHeight(h);
    fp->fillColor(x, y, 0);
    // XXX: Shouldn't we delete the painter?
}

void Painter::fillPattern(uint x, uint y, uint w, uint h)
{
    KisFillPainter* fp = createFillPainter();
    fp->setWidth(w);
    fp->setHeight(h);
    fp->fillPattern(x, y, 0);
    // XXX: Shouldn't we delete the painter?
}

void Painter::setFillStyle(uint style)
{
    KisPainter::FillStyle fillstyle;
    switch (style) {
    case 1:
        fillstyle = KisPainter::FillStyleForegroundColor;
        break;
    case 2:
        fillstyle = KisPainter::FillStyleBackgroundColor;
        break;
    case 3:
        fillstyle = KisPainter::FillStylePattern;
        break;
    default:
        fillstyle = KisPainter::FillStyleNone;
    }
    m_painter->setFillStyle(fillstyle);
}

void Painter::setOpacity(uint opacity)
{
    m_painter->setOpacity((quint8)opacity);
}

void Painter::composeWith(qint32 dx, qint32 dy, const QString& compositeOp, const QObject* source, qint32 opacity, qint32 sx, qint32 sy, qint32 sw, qint32 sh)
{
    const PaintDevice* sourcePL = dynamic_cast< const PaintDevice* >(source);
    if (sourcePL) {
        m_painter->setCompositeOp(compositeOp);
        m_painter->setOpacity(opacity);
        m_painter->bitBlt(dx, dy, sourcePL->paintDevice(), sx, sy, sw, sh);
    }
}


void Painter::setStrokeStyle(uint style)
{
    KisPainter::StrokeStyle strokestyle;
    switch (style) {
    case 1:
        strokestyle = KisPainter::StrokeStyleBrush;
        break;
    default:
        strokestyle = KisPainter::StrokeStyleNone;
    }
    m_painter->setStrokeStyle(strokestyle);
}


void Painter::paintPolyline(QVariantList pointsX, QVariantList pointsY)
{
    if (pointsX.size() != pointsY.size()) {
        warnScript << QString("The two lists of points should have the same size.");
        return;
    }
    m_painter->paintPolyline(createPointsVector(pointsX, pointsY));
}

void Painter::paintLine(double x1, double y1, double p1, double x2, double y2, double p2)
{
    m_painter->paintLine(KisPaintInformation(QPointF(x1, y1), p1), KisPaintInformation(QPointF(x2, y2), p2));
}

void Painter::paintBezierCurve(double x1, double y1, double p1, double cx1, double cy1, double cx2, double cy2, double x2, double y2, double p2)
{
    m_painter->paintBezierCurve(KisPaintInformation(QPointF(x1, y1), p1), QPointF(cx1, cy1), QPointF(cx2, cy2), KisPaintInformation(QPointF(x2, y2), p2));
}

void Painter::paintEllipse(double x, double y, double w, double h)
{
    m_painter->paintEllipse(x, y, w, h);
}

void Painter::paintPolygon(QVariantList pointsX, QVariantList pointsY)
{
    if (pointsX.size() != pointsY.size()) {
        warnScript << "The two lists of points should have the same size.";
        return;
    }
    m_painter->paintPolygon(createPointsVector(pointsX, pointsY));
}

void Painter::paintRect(double x, double y, double width, double height)
{
    m_painter->paintRect(x, y, width, height);
}

void Painter::paintAt(double x, double y, double pressure)
{
    m_painter->paintAt(KisPaintInformation(QPointF(x, y), pressure));
}

void Painter::setPaintColor(QObject* color)
{
    Color* c = dynamic_cast< Color* >(color);
    if (c) m_painter->setPaintColor(KoColor(c->toQColor(), paintDevice()->colorSpace()));
}

void Painter::setBackgroundColor(QObject* color)
{
    Color* c = dynamic_cast< Color* >(color);
    if (c) m_painter->setBackgroundColor(KoColor(c->toQColor(), paintDevice()->colorSpace()));
}

void Painter::setPattern(QObject* pattern)
{
    Pattern* p = dynamic_cast< Pattern* >(pattern);
    if (p) m_painter->setPattern(p->getPattern());
}

#include "krs_painter.moc"
