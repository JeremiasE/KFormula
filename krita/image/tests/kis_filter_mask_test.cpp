/*
 *  Copyright (c) 2007 Boudewijn Rempt <boud@valdyas.org>
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

#include "kis_filter_mask_test.h"
#include <qtest_kde.h>

#include <KoColorSpaceRegistry.h>

#include "filter/kis_filter.h"
#include "filter/kis_filter_configuration.h"
#include "kis_filter_mask.h"
#include "filter/kis_filter_registry.h"
#include "kis_group_layer.h"
#include "kis_paint_device.h"
#include "kis_paint_layer.h"
#include "kis_types.h"
#include "kis_image.h"


#include "testutil.h"

void KisFilterMaskTest::testCreation()
{
    KisFilterMaskSP mask = new KisFilterMask();
}

void KisFilterMaskTest::testProjectionNotSelected()
{
    const KoColorSpace * cs = KoColorSpaceRegistry::instance()->colorSpace("RGBA", 0);

    QImage qimg(QString(FILES_DATA_DIR) + QDir::separator() + "hakonepa.png");
    QImage inverted(QString(FILES_DATA_DIR) + QDir::separator() + "inverted_hakonepa.png");

    KisFilterSP f = KisFilterRegistry::instance()->value("invert");
    Q_ASSERT(f);
    KisFilterConfiguration * kfc = f->defaultConfiguration(0);
    Q_ASSERT(kfc);

    KisFilterMaskSP mask = new KisFilterMask();
    mask->setFilter(kfc);

    // Check basic apply(). Shouldn't do anything, since nothing is selected yet.
    KisPaintDeviceSP projection = new KisPaintDevice(cs);
    projection->convertFromQImage(qimg, 0, 0, 0);
    mask->createNodeProgressProxy();
    mask->apply(projection, QRect(0, 0, qimg.width(), qimg.height()));
    mask->select(qimg.rect(), MIN_SELECTED);

    QPoint errpoint;
    if (!TestUtil::compareQImages(errpoint, qimg, projection->convertToQImage(0, 0, 0, qimg.width(), qimg.height()))) {
        projection->convertToQImage(0, 0, 0, qimg.width(), qimg.height()).save("filtermasktest1.png");
        QFAIL(QString("Failed to create identical image, first different pixel: %1,%2 ").arg(errpoint.x()).arg(errpoint.y()).toAscii());
    }
}

void KisFilterMaskTest::testProjectionSelected()
{
    const KoColorSpace * cs = KoColorSpaceRegistry::instance()->colorSpace("RGBA", 0);

    QImage qimg(QString(FILES_DATA_DIR) + QDir::separator() + "hakonepa.png");
    QImage inverted(QString(FILES_DATA_DIR) + QDir::separator() + "inverted_hakonepa.png");

    KisFilterSP f = KisFilterRegistry::instance()->value("invert");
    Q_ASSERT(f);
    KisFilterConfiguration * kfc = f->defaultConfiguration(0);
    Q_ASSERT(kfc);

    KisFilterMaskSP mask = new KisFilterMask();
    mask->setFilter(kfc);
    mask->createNodeProgressProxy();

    KisPaintDeviceSP projection = new KisPaintDevice(cs);
    projection->convertFromQImage(qimg, 0, 0, 0);
    mask->select(qimg.rect(), MAX_SELECTED);
    mask->apply(projection, QRect(0, 0, qimg.width(), qimg.height()));
    QCOMPARE(mask->exactBounds(), QRect(0, 0, qimg.width(), qimg.height()));

    QPoint errpoint;
    if (!TestUtil::compareQImages(errpoint, inverted, projection->convertToQImage(0, 0, 0, qimg.width(), qimg.height()))) {
        projection->convertToQImage(0, 0, 0, qimg.width(), qimg.height()).save("filtermasktest2.png");
        QFAIL(QString("Failed to create inverted image, first different pixel: %1,%2 ").arg(errpoint.x()).arg(errpoint.y()).toAscii());
    }

}

void KisFilterMaskTest::testInImage()
{
    const KoColorSpace * cs = KoColorSpaceRegistry::instance()->colorSpace("RGBA", 0);

    QImage qimg(QString(FILES_DATA_DIR) + QDir::separator() + "hakonepa.png");
    QImage inverted(QString(FILES_DATA_DIR) + QDir::separator() + "inverted_hakonepa.png");

    KisFilterSP f = KisFilterRegistry::instance()->value("invert");
    Q_ASSERT(f);
    KisFilterConfiguration * kfc = f->defaultConfiguration(0);
    Q_ASSERT(kfc);

    KisFilterMaskSP mask = new KisFilterMask();
    mask->createNodeProgressProxy();
    mask->setFilter(kfc);
    mask->select(qimg.rect(), MAX_SELECTED);

    // Check in image stack
    KisImageSP image = new KisImage(0, qimg.width(), qimg.height(), cs, "merge test");

    KisPaintLayerSP layer = new KisPaintLayer(image, "test", OPACITY_OPAQUE);
    layer->paintDevice()->convertFromQImage(qimg, 0, 0, 0);

    image->addNode(layer.data());
    image->addNode(mask.data(), layer.data());
    layer->setDirty(qimg.rect());

    QTest::qSleep(250); // allow the action to do its job.
    QCoreApplication::processEvents(); // allow the actions 'gui' stuff to run.

    KisPaintDeviceSP pd = image->projection();

    QPoint errpoint;
    if (!TestUtil::compareQImages(errpoint, inverted,
                                  pd->convertToQImage(0, 0, 0, qimg.width(), qimg.height()))) {
        pd->convertToQImage(0, 0, 0, qimg.width(), qimg.height()).save("filtermasktest3.png");
        QFAIL(QString("Failed to create inverted image, first different pixel: %1,%2 ").arg(errpoint.x()).arg(errpoint.y()).toAscii());
    }

    delete kfc;
}

QTEST_KDEMAIN(KisFilterMaskTest, GUI)
#include "kis_filter_mask_test.moc"
