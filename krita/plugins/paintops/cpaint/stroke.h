/*
 *  Copyright (c) 2000 Clara Chan <x@unknown.com>
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

#ifndef STROKE_H
#define STROKE_H

#include <math.h>
#include <stdio.h>

#include <QVector>

#include <kis_types.h>
#include <KoColor.h>
class Brush;
class Sample;

class QRect;


using namespace std;

class Stroke
{


public:

    Stroke(Brush *brush);
    virtual ~Stroke();

    void draw(KisPaintDeviceSP dev);
    void setColor(const KoColor & c);
    void storeOldPath(double, double);
    void storeSample(Sample * sample);

private:
    void drawLine(KisPaintDeviceSP dev, double x1, double y1, double x2, double y2, double width, const KoColor & color);
    void drawWuLine(KisPaintDeviceSP dev, double x1, double y1, double x2, double y2, double width, const KoColor & color);
    void drawGSLine(KisPaintDeviceSP image, int x0, int y0, int x1, int y1, int w1, int w2, const QColor &color);
    int gsfilter(float val);
    // test whether a bristle is touching the paper or not
    // by testing the pressure threshold and test if there is any ink
    bool testThreshold(int, double, double, double);

private:
    KoColor m_color;
    QVector<Sample*> m_samples;

    double m_lastx1, m_lasty1, m_lastx2, m_lasty2;
    QVector< QVector<double> > m_oldPathx;
    QVector< QVector<double> > m_oldPathy;
    QVector< QVector<bool> > m_valid;
    Brush * m_brush;
    int m_numBristles;


};

#endif
