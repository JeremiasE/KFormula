/*
 *  Copyright (c) 2005,2007 Cyrille Berger <cberger@cberger.net>
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

#include "krs_const_paint_device.h"


#include <QBuffer>
#include <klocale.h>

#include <KoColorSpaceRegistry.h>
#include <kis_doc2.h>
#include <kis_layer.h>
#include <kis_iterators_pixel.h>
#include <kis_transaction.h>
#include <kis_math_toolbox.h>
#include <kis_paint_device.h>

#include "krs_image.h"
#include "krs_const_iterator.h"
#include "krs_histogram.h"
#include "krs_painter.h"
#include "krs_wavelet.h"
using namespace Scripting;

ConstPaintDevice::ConstPaintDevice(KisPaintDeviceSP device, KisDoc2* doc)
        : m_device(device)
        , m_doc(doc)
{
    setObjectName("KritaLayer");
}


ConstPaintDevice::~ConstPaintDevice()
{
}

int ConstPaintDevice::width()
{
    QRect r1 = paintDevice()->extent();
    return r1.width();
}

int ConstPaintDevice::height()
{
    QRect r1 = paintDevice()->extent();
    return r1.height();
}

QString ConstPaintDevice::colorSpaceId()
{
    return paintDevice()->colorSpace()->id();
}

QObject* ConstPaintDevice::createRectConstIterator(uint x, uint y, uint width, uint height)
{
    return new ConstIterator<KisRectConstIteratorPixel>(this,
            paintDevice()->createRectConstIterator(x, y, width, height));
}

QObject* ConstPaintDevice::createHLineConstIterator(uint x, uint y, uint width)
{
    return new ConstIterator<KisHLineConstIteratorPixel>(this,
            paintDevice()->createHLineConstIterator(x, y, width));
}

QObject* ConstPaintDevice::createVLineConstIterator(uint x, uint y, uint height)
{
    return new ConstIterator<KisVLineConstIteratorPixel>(this,
            paintDevice()->createVLineConstIterator(x, y, height));
}

QObject* ConstPaintDevice::createHistogram(const QString& histoname, uint typenr)
{
    KoHistogramProducerFactory* factory = KoHistogramProducerFactoryRegistry::instance()->value(histoname);

    /*
    QList<KoID> listID = KisHistogramProducerFactoryRegistry::instance()->listKeys();
    for(QList<KoID>::iterator it = listID.begin(); it != listID.end(); it++)
        dbgScript << (*it).name() <<"" << (*it).id();
    */

    enumHistogramType type ;
    switch (typenr) {
    case 1:
        type = LOGARITHMIC;
        break;
    case 0:
    default:
        type = LINEAR;
        break;
    }

    if (factory && factory->isCompatibleWith(paintDevice()->colorSpace()))
        return new Histogram(this, factory->generate() , type);

    warnScript << QString("An error has occurred in %1\n%2").arg("createHistogram").arg(QString("The histogram %1 is not available").arg(histoname));
    return 0;
}

QObject* ConstPaintDevice::fastWaveletTransformation()
{
    KisMathToolbox* mathToolbox = KisMathToolboxRegistry::instance()->value(paintDevice()->colorSpace()->mathToolboxId().id());
    QRect rect = paintDevice()->exactBounds();
    KisMathToolbox::KisWavelet* wav = mathToolbox->fastWaveletTransformation(paintDevice(), rect);
    return new Wavelet(wav);
}

QObject* ConstPaintDevice::clone()
{
    KisPaintDeviceSP pl = new KisPaintDevice(*paintDevice());
    return new ConstPaintDevice(pl);
}

#if 0
QByteArray ConstPaintDevice::bytes()
{
    qint32 pixelsize = paintDevice()->colorSpace()->pixelSize();
    const int w = width();
    const int h = height();
    const int size = w * h * pixelsize;
    Q_ASSERT(size >= 0);

    QByteArray bytearray;
    QBuffer buffer(&bytearray);
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);

    quint8* data = new quint8[size];
    Q_CHECK_PTR(data);
    paintDevice()->readBytes(data, 0, 0, w, h);
    for (int i = 0; i < size; ++i)
        out << data[i];
    delete [] data;

    return bytearray;
}

bool ConstPaintDevice::setBytes(const QByteArray& bytearray)
{
    qint32 pixelsize = paintDevice()->colorSpace()->pixelSize();
    const int w = width();
    const int h = height();
    const int size = w * h * pixelsize;

    if (size < 0 || bytearray.size() < size)
        return false;

    QBuffer buffer(&bytearray);
    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);

    quint8* data = new quint8[size];
    Q_CHECK_PTR(data);
    for (int i = 0; i < size; ++i)
        in >> data[i];
    paintDevice()->writeBytes(data, 0, 0, w, h);
    delete [] data;

    return true;
}
#endif

#include "krs_const_paint_device.moc"
