/*
 * Copyright (c) 2007 Cyrille Berger (cberger@cberger.net)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. */

#include "kis_images_blender.h"

#include <Eigen/LU>
#include <QPolygon>

#include <kis_paint_device.h>
#include <kis_iterators_pixel.h>
#include <kis_random_sub_accessor.h>

void KisImagesBlender::blend(QList<LayerSource> sources, KisPaintDeviceSP device)
{
    Eigen::Vector3d v, v1;
    v(2) = 1.0;
    QRect resultRect;
    // Compute the bounding of each image
    for (int i = 0; i < sources.size(); i++) {
        QPolygon pa(4);
        QRect r = sources[i].rect;
        v(0) = r.topLeft().x();
        v(1) = r.topLeft().y();
        v1 = sources[i].homography * v;
        v1 /= v1(2);
        pa.setPoint(0, QPoint((int)v1(0), (int)v1(1)));

        v(0) = r.topRight().x();
        v(1) = r.topRight().y();
        v1 = sources[i].homography * v;
        v1 /= v1(2);
        pa.setPoint(1, QPoint((int)v1(0), (int)v1(1)));

        v(0) = r.bottomRight().x();
        v(1) = r.bottomRight().y();
        v1 = sources[i].homography * v;
        v1 /= v1(2);
        pa.setPoint(2, QPoint((int)v1(0), (int)v1(1)));

        v(0) = r.bottomLeft().x();
        v(1) = r.bottomLeft().y();
        v1 = sources[i].homography * v;
        v1 /= v1(2);
        pa.setPoint(3, QPoint((int)v1(0), (int)v1(1)));

        dbgPlugins << pa[0] << pa[1] << pa[2] << pa[3];
        sources[i].boundingBox = pa;
        resultRect |= sources[i].boundingBox.boundingRect();
        sources[i].accessor = new KisRandomSubAccessorPixel(sources[i].layer->createRandomSubAccessor());
        sources[i].homography.computeInverse(&sources[i].invHomography);
    }
    dbgPlugins << "resultRect = " << resultRect;
    KisHLineIteratorPixel hitDevice = device->createHLineIterator(resultRect.left(), resultRect.top(), resultRect.width());
    Eigen::Vector3d vec1, vec2;
    vec1(2) = 1.0;
    int previousReport = -1;
    for (int y = resultRect.top(); y <= resultRect.bottom(); y++) {
        int report = ((100 * (y - resultRect.top())) / resultRect.height());
        if (report != previousReport) {
            dbgPlugins << report;
            previousReport = report;
        }
        while (!hitDevice.isDone()) {
            QPoint p(hitDevice.x(), hitDevice.y());
            for (int i = 0; i < sources.size(); i++) {
                if (sources[i].boundingBox.contains(p)) {
                    // Compute the position of the original point
                    vec1(0) = hitDevice.x();
                    vec1(1) = hitDevice.y();
                    vec2 = sources[i].invHomography * vec1;
                    vec2 /= vec2(2);
                    double x2 = vec2(0) - sources[i].xc2;
                    double y2 = vec2(1) - sources[i].yc2;
                    double r2 = (x2 * x2 + y2 * y2) * sources[i].norm;
                    double xsrc = poly(sources[i].a, sources[i].b, sources[i].c, r2) * x2 + sources[i].xc1;
                    double ysrc = poly(sources[i].a, sources[i].b, sources[i].c, r2) * y2 + sources[i].yc1;
                    sources[i].accessor->moveTo(xsrc, ysrc);
                    sources[i].accessor->sampledRawData(hitDevice.rawData());
//           dbgPlugins << xsrc <<"" << ysrc <<" = src dst =" << hitDevice.x() <<"" << hitDevice.y() <<" a =" << sources[i].a <<"; b =" << sources[i].b <<"; c=" << sources[i].c <<"; v2 = [" << vec2(0) <<";" << vec2(1) <<"; 1.0 ]; r2 ="<< r2;
                    if (hitDevice.rawData()[0] != 0)
                        break;
                }
            }
            ++hitDevice;
        }
        hitDevice.nextRow();
    }
}
