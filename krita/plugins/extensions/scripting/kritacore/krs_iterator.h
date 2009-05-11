
/*
 *  Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
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

#ifndef KROSS_KRITACOREKRS_ITERATOR_H
#define KROSS_KRITACOREKRS_ITERATOR_H

#include <QObject>
#include <QList>
#include <QListIterator>
#include <klocale.h>

#include <KoColorTransformation.h>

#include "krs_paint_device.h"
#include "krs_const_iterator.h"
#include "krs_monitor.h"

#include <kis_paint_device.h>
#include <kis_types.h>

namespace Scripting
{

/**
 * This object allow to change the value of pixel one by one.
 */
class IteratorBase : public QObject
{
    Q_OBJECT
public:
    IteratorBase(QObject* parent) : QObject(parent) {
        setObjectName("KritaIterator");
        // Connect the Monitor to know when the invalidating of iterator is needed
        connect(Monitor::instance(), SIGNAL(signalExecutionFinished()), this, SLOT(invalidateIterator()));
    }
    virtual ~IteratorBase() {}

public slots:

    /**
     * Increment the position, and go to the next pixel. The
     * returned value is true if the iterator reached the
     * end (so, it's like calling next() and after that asking
     * what isDone() returns).
     */
    virtual bool next() = 0;

    /**
     * Return true if the iterator is at the end, what means, that
     * there are no more pixels available.
     */
    virtual bool isDone() = 0;

    /**
     * Return the current column the iterator is on. The value will
     * be always smaller then the width of the image/layer.
     */
    virtual int x() = 0;

    /**
     * Return the current row the iterator is on. The value will
     * be always smaller then the height of the image/layer.
     */
    virtual int y() = 0;

    /**
     * Return the value the current pixel has in the channel
     * number \p channelnr . If the channelnr is out of range
     * (as in >= what \p channelCount() returns) a invalid
     * QVariant is returned.
     */
    virtual QVariant channel(uint channelnr) = 0;

    /**
     * Set the value the current pixel has in the channel
     * number \p channelnr.
     */
    virtual void setChannel(uint channelnr, const QVariant& value) = 0;

    /**
     * Return the number of channels the current pixmap has. As
     * example for RGBA-images 4 is returned cause R is one channel,
     * while G, B and A are other channels what makes 4 total.
     */
    virtual uint channelCount() = 0;

    /**
     * Return the current pixel. The pixel itself may have depending
     * on the used colorspace n colors where each color has it's own
     * channel. So, as example if the colorspace is RGBA, the returned
     * list has exact 4 items. The first one is a value 0-255 for the
     * color red, the second for green and the theird for blue while
     * the forth is the alpha-channel.
     */
    virtual QVariantList pixel() = 0;

    /**
     * Set the current pixel.
     */
    virtual void setPixel(QVariantList pixel) = 0;
    virtual void copyFrom(QObject* obj) = 0;

    /**
     * Invert the color of the current pixel.
     */
    virtual void invertColor() = 0;

    /**
     * Darken a pixel.
     * This functions takes three arguments:
     *  - shade amount use to darken all color channels.
     *  - compensate defines if compensation should be used to limit the darkening.
     *  - compensation to limit the darkening
     */
    virtual void darken(int shade, bool compensate, double compensation) = 0;

protected slots:
    virtual void invalidateIterator() = 0;
};

/**
 * \internal template class that implements \a IteratorBase to provide
 * iterators for walking over the pixels of an image or a layer.
 */
template<class _T_It>
class Iterator : public IteratorBase
{
public:
    Iterator(PaintDevice* layer, _T_It it)
            : IteratorBase(layer)
            , m_it(new _T_It(it))
            , m_layer(layer->paintDevice()) {
    }

    virtual ~Iterator() {
        invalidateIterator();
    }

private:

    bool isDone() {
        return m_it->isDone();
    }

    bool next() {
        ++(*m_it);
        return m_it->isDone();
    }

    int x() {
        return m_it->x();
    }

    int y() {
        return m_it->y();
    }

    QVariant channel(uint channelnr) {
        QList<KoChannelInfo*> channels = m_layer->colorSpace()->channels();
        return channelnr < uint(channels.count()) ? channelValue(channels[channelnr]) : QVariant();
    }

    void setChannel(uint channelnr, const QVariant& value) {
        QList<KoChannelInfo*> channels = m_layer->colorSpace()->channels();
        if (channelnr < uint(channels.count()))
            setChannelValue(channels[channelnr], value);
    }

    uint channelCount() {
        return m_layer->colorSpace()->channels().count();
    }

    QVariantList pixel() {
        QVariantList pixel;
        QList<KoChannelInfo*> channels = m_layer->colorSpace()->channels();
        for (QList<KoChannelInfo*>::iterator itC = channels.begin(); itC != channels.end(); ++itC)
            pixel.push_back(channelValue(*itC));
        return pixel;
    }

    void setPixel(QVariantList pixel) {
        QList<KoChannelInfo *> channels = m_layer->colorSpace()->channels();
        uint i = 0;
        const uint size = pixel.size();
        for (QList<KoChannelInfo *>::iterator itC = channels.begin(); itC != channels.end() && i < size; ++itC, ++i)
            setChannelValue(*itC, pixel[i]);
    }
    void copyFrom(QObject* obj) {
        Iterator<_T_It>* iter = dynamic_cast< Iterator<_T_It>* >(obj);
        if (iter) {
            memcpy(m_it->rawData(), iter->m_it->oldRawData(), m_layer->colorSpace()->pixelSize());
        } else {
            ConstIteratorBase* citer = dynamic_cast< ConstIteratorBase* >(obj);
            if (citer) {
                memcpy(m_it->rawData(), citer->oldRawData(), m_layer->colorSpace()->pixelSize());
            }
        }
    }
    void invertColor() {
        KoColorTransformation* invertTransfo = m_layer->colorSpace()->createInvertTransformation();
        invertTransfo->transform(m_it->oldRawData(), m_it->rawData(), 1);
#ifdef __GNUC__
#warning "Fix me: don't create an invert transform each time!"
#endif

        delete invertTransfo;
    }

    void darken(int shade, bool compensate, double compensation) {
        KoColorTransformation* transfo = m_layer->colorSpace()->createDarkenAdjustment(shade, compensate, compensation);
        transfo->transform(m_it->rawData(), m_it->rawData(), 1);
        delete transfo;
    }

private:

    virtual void invalidateIterator() {
        delete m_it; m_it = 0;
    }

    inline QVariant channelValue(KoChannelInfo* ci) {
        quint8* data = (quint8*)(m_it->rawData() + ci->pos());
        switch (ci->channelValueType()) {
        case KoChannelInfo::UINT8:
            return *data;
        case KoChannelInfo::UINT16:
            return *((quint16*) data);
        case KoChannelInfo::FLOAT32:
            return *((float*) data);
        default:
            dbgScript << "Unsupported data format in script";
            return QVariant();
        }
    }

    inline void setChannelValue(KoChannelInfo* ci, const QVariant& value) {
        quint8* data = (quint8*)(m_it->rawData() + ci->pos());
        switch (ci->channelValueType()) {
        case KoChannelInfo::UINT8:
            *data = value.toUInt();
            break;
        case KoChannelInfo::UINT16:
            *((quint16*) data) = value.toUInt();
            break;
        case KoChannelInfo::FLOAT32:
            *((float*) data) = value.toDouble();
            break;
        default:
            dbgScript << "Unsupported data format in script";
            break;
        }
    }

private:
    _T_It* m_it;
    KisPaintDeviceSP m_layer;
};

}

#endif
