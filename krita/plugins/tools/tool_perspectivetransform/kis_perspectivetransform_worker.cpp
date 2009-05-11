/*
 * This file is part of Krita
 *
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_perspectivetransform_worker.h"


#include "kis_iterators_pixel.h"
#include "kis_paint_device.h"
#include "kis_perspective_math.h"
#include "KoProgressUpdater.h"
#include "kis_random_sub_accessor.h"
#include "kis_selection.h"

KisPerspectiveTransformWorker::KisPerspectiveTransformWorker(KisPaintDeviceSP dev, const QPointF& topLeft, const QPointF& topRight, const QPointF& bottomLeft, const QPointF& bottomRight, KoUpdater *progress)
        : m_dev(dev), m_progressUpdater->interrupted()(false), m_progress(progress)

{
    QRect m_r;
    if (m_dev->hasSelection())
        m_r = m_dev->selection()->selectedExactRect();
    else
        m_r = m_dev->exactBounds();
    /*    if(m_dev->hasSelection())
            m_dev->selection()->clear();*/

    double* b = KisPerspectiveMath::computeMatrixTransfoToPerspective(topLeft, topRight, bottomLeft, bottomRight, m_r);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_matrix[i][j] = b[3*i+j];
        }
    }
    delete b;
}


KisPerspectiveTransformWorker::~KisPerspectiveTransformWorker()
{
}

double norm2(const QPointF& p)
{
    return sqrt(p.x() * p.x() + p.y() * p.y());
}

void KisPerspectiveTransformWorker::run()
{

    //TODO: understand why my caching of the rect didn't work...
    if (m_dev->hasSelection()) {
        m_r = m_dev->selection()->selectedExactRect();
    } else {
        m_r = m_dev->exactBounds();
    }
    KoColorSpace * cs = m_dev->colorSpace();

    KisRectIteratorPixel dstIt = m_dev->createRectIterator(m_r.x(), m_r.y(), m_r.width(), m_r.height());
    KisPaintDeviceSP srcdev = new KisPaintDevice(*m_dev.data());
    { // ensure that the random sub accessor is deleted first
        KisRandomSubAccessorPixel srcAcc = srcdev->createRandomSubAccessor();
        // Initialise progress
        m_lastProgressReport = 0;
        m_progressStep = 0;
        m_progressTotalSteps = m_r.width() * m_r.height();
        //Action
        while (!dstIt.isDone()) {
            if (dstIt.isSelected()) {
                QPointF p;
                double sf = (dstIt.x() * m_matrix[2][0] + dstIt.y() * m_matrix[2][1] + 1.0);
                sf = (sf == 0.) ? 1. : 1. / sf;
                p.setX((dstIt.x() * m_matrix[0][0] + dstIt.y() * m_matrix[0][1] + m_matrix[0][2]) * sf);
                p.setY((dstIt.x() * m_matrix[1][0] + dstIt.y() * m_matrix[1][1] + m_matrix[1][2]) * sf);

                srcAcc.moveTo(p);
                srcAcc.sampledOldRawData(dstIt.rawData());

                // TODO: Should set alpha = alpha*(1-selectedness)
//                 cs->setAlpha( dstIt.rawData(), 255, 1);
            } else {
//                 cs->setAlpha( dstIt.rawData(), 0, 1);
            }
            m_progressStep ++;
            if (m_lastProgressReport != (m_progressStep * 100) / m_progressTotalSteps) {
                m_lastProgressReport = (m_progressStep * 100) / m_progressTotalSteps;
                m_progressUpdater->setProgress(m_lastProgressReport);
            }
            if (m_progressUpdater->interrupted()) {
                break;
            }
            ++dstIt;
        }
    }
}
#include "kis_perspectivetransform_worker.moc"
