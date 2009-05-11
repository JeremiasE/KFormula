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

#ifndef KROSS_KRITACOREKRS_PAINTER_H
#define KROSS_KRITACOREKRS_PAINTER_H

#include <QObject>


#include <kis_types.h>
#include <kis_paint_layer.h>
#include <kis_painter.h>
#include <kis_fill_painter.h>

//Added by qt3to4:
#include <Q3ValueList>

namespace Scripting
{

class PaintDevice;

/**
 * The painter enables drawing to a \a PaintDevice object.
 *
 * XXX: add setPaintOpPreset
 *
 * Ruby sample that paints a sky :
 * @code
 * require "Krita"
 * image = Krita.image()
 * width = image.width()
 * height = image.height()
 * layer = image.createPaintLayer("Sky",255).paintDevice()
 * layer.beginPainting("sky")
 * painter = layer.createPainter()
 * painter.setStrokeStyle(1)
 * painter.setFillStyle(1)
 * painter.setPaintOp("paintbrush")
 * painter.setPaintColor( Krita.createRGBColor(24,24,24) )
 * painter.paintRect(0.0,0.0, width, height, 0.5)
 * painter.setPaintColor( Krita.createRGBColor(255,255,255) )
 * for i in 1..60
 *   size = rand() * 4
 *   size = 1 if(size < 1)
 *   painter.setBrush(Krita.createCircleBrush(size,size,size/2+1,size/2+1))
 *   painter.paintAt(rand()*width, rand()*height, 0.5)
 * end
 * layer.endPainting()
 * @endcode
 */
class Painter : public QObject
{
    Q_OBJECT
public:
    Painter(PaintDevice* layer);
    virtual ~Painter();

public slots: // Convolution

#if 0
    /**
     * This function apply a convolution kernel to an image.
     * It takes at least three arguments :
     *  - a list of a list with the kernel (all lists need to have the same size)
     *  - factor
     *  - offset
     *
     * The value of a pixel will be given by the following function K*P/factor + offset,
     * where K is the kernel and P is the neighbourhood.
     *
     * It can takes the following optional arguments :
     *  - borderOp control how to convolve the pixels on the border of an image ( 0 use the default color
     * 1 use the pixel on the opposite side of the image 2 use the border pixel 3 avoid border pixels)
     *  - channel ( 1 for color 2 for alpha 3 for both)
     *  - x
     *  - y
     *  - width
     *  - height
     */
    void convolve();
#endif

public slots: // Fill specific

    /**
     * Set the threshold the fill threshold.
     * It takes one argument :
     *  - threshold
     */
    void setFillThreshold(int threshold);

    /**
     * Start filling color.
     * It takes four arguments :
     *  - x
     *  - y
     */
    void fillColor(uint x, uint y, uint w, uint h);

    /**
     * Start filling a pattern.
     * It takes four arguments :
     *  - x
     *  - y
     */
    void fillPattern(uint x, uint y, uint w, uint h);

public slots: // Style operation

    /**
     * This function set the fill style of the Painter.
     * It takes one argument :
     *  - 0 for none 1 for fill with foreground color 2 for fill with
     *    background color 3 for fill with a pattern.
     */
    void setFillStyle(uint style);

    /**
     * This function set the opacity of the painting.
     * It takes one argument :
     *  - opacity in the range 0 to 255
     */
    void setOpacity(uint opacity);

    /**
     * This function set the style of the stroke.
     * It takes one argument :
     *  - 0 for none 1 for brush
     */
    void setStrokeStyle(uint style);

public slots: // Composite operations
    void composeWith(qint32 dx, qint32 dy, const QString& compositeOp, const QObject* source, qint32 opacity, qint32 sx, qint32 sy, qint32 sw, qint32 sh);
public slots: // Painting operations

    /**
     * This function will paint a polyline.
     * It takes two arguments :
     *  - a list of x position
     *  - a list of y position
     */
    void paintPolyline(QVariantList pointsX, QVariantList pointsY);

    /**
     * This function will paint a line.
     * It takes six arguments :
     *  - x1
     *  - y1
     *  - pressure1
     *  - x2
     *  - y2
     *  - pressure2
     */
    void paintLine(double x1, double y1, double p1, double x2, double y2, double p2);

    /**
     * This function will paint a Bezier curve.
     * It takes ten arguments :
     *  - x1
     *  - y1
     *  - p1
     *  - cx1
     *  - cy1
     *  - cx2
     *  - cx2
     *  - x2
     *  - y2
     *  - p2
     *
     * Where (x1,y1) is the start position, p1 is the pressure at the start,
     * (x2,y2) is the ending position, p2 is the pressure at the end. (cx1,cy1)
     * and (cx2,cy2) are the position of the control points.
     */
    void paintBezierCurve(double x1, double y1, double p1, double cx1, double cy1, double cx2, double cy2, double x2, double y2, double p2);

    /**
     * This function will paint an ellipse.
     * It takes five arguments :
     *  - x
     *  - y
     *  - w
     *  - h
     *  - pressure
     *
     * Where x, y, w, and h define the rectangle containing the ellipse.
     */
    void paintEllipse(double x, double y, double w, double h);

    /**
     * This function will paint a polygon.
     * It takes two arguments :
     *  - a list of x position
     *  - a list of y position
     */
    void paintPolygon(QVariantList pointsX, QVariantList pointsY);

    /**
     * This function will paint a rectangle.
     * It takes five arguments :
     *  - x
     *  - y
     *  - width of the rectangle
     *  - height of the rectangle
     *  - pressure
     */
    void paintRect(double x, double y, double width, double height);

    /**
     * This function will paint at a given position.
     * It takes three arguments :
     *  - x
     *  - y
     *  - pressure
     */
    void paintAt(double x, double y, double pressure);

public slots: // Color operations

    /**
     * This functions set the paint color (also called foreground color).
     * It takes one argument :
     *  - a \a Color object e.g. create with \a Module::createRGBColor .
     */
    void setPaintColor(QObject* color);

    /**
     * This functions set the background color.
     * It takes one argument :
     *  - a \a Color object e.g. create with \a Module::createRGBColor .
     */
    void setBackgroundColor(QObject* color);

public slots: // How is painting done operations

    /**
     * This function sets the pattern used for filling.
     * It takes one argument :
     *  - a Pattern object
     */
    void setPattern(QObject* pattern);

protected:
    inline KisPaintDeviceSP paintDevice() {
        return m_layer;
    }

private:
    inline vQPointF createPointsVector(Q3ValueList<QVariant> xs, Q3ValueList<QVariant> ys) {
        vQPointF a;
        Q3ValueList<QVariant>::iterator itx = xs.begin();
        Q3ValueList<QVariant>::iterator ity = ys.begin();
        for (; itx != xs.end(); ++itx, ++ity) {
            a.push_back(QPointF((*itx).toDouble(), (*ity).toDouble()));
        }
        return a;
    }
    inline KisFillPainter* createFillPainter() {
        KisFillPainter* fp = new KisFillPainter(m_painter->device());
        fp->setFillColor(m_painter->fillColor());
        fp->setPaintColor(m_painter->paintColor());
        fp->setFillStyle(m_painter->fillStyle());
        fp->setOpacity(m_painter->opacity());
        fp->setPattern(m_painter->pattern());
        return fp;
    }

private:
    KisPaintDeviceSP m_layer;
    KisPainter* m_painter;
    int m_threshold;
};

}

#endif
