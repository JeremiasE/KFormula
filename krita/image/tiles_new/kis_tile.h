/*
 *  Copyright (c) 2004 Casper Boemann <cbr@boemann.dk>
 *  Copyright (c) 2007 Bart Coppens <kde@bartcoppens.be>
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
#ifndef KIS_TILE_H_
#define KIS_TILE_H_

#include <qglobal.h>
#include <QRect>
#include <QMutex>
#include <QTime>
#include <QVector>

#include "krita_export.h"
#include "kis_tilestore.h"
#include "kis_sharedtiledata.h"

class KisTileStoreData;

class KisTiledDataManager;
class KisTiledIterator;


/**
 * Provides abstraction to a tile.  A tile contains
 * a part of a PaintDevice, but only the individual pixels
 * are accessible and that only via iterators.
 *
 * Implicit sharing: they share a same static, non-changing data thing. Tiles writing will auto-detach (as opposed to where they all share the same data, even
 * after writing; this would require also looking at the dirtyRect).
 *
 * ### TODO: Figure out a decent locking policy, and FOLLOW IT! (If necessary, with #defines)
 */
class KRITAIMAGE_EXPORT KisTile
{
public:
    // ### The problem with auto-implicit sharing is that it might be bad for tiles that get copied to get in the KisMemento ...
    KisTile(KisTileStoreSP store, qint32 pixelSize, qint32 col, qint32 row, const quint8 *defPixel);
    KisTile(qint32 pixelSize, qint32 col, qint32 row, KisSharedTileData *data);
    KisTile(const KisTile& rhs, qint32 col, qint32 row); // Implicit sharing (Configurable?? ### )
    KisTile(const KisTile& rhs); // Implicit sharing (Configurable?? ### )
    ~KisTile();

public:
    void release();
    void allocate(KisTileStoreSP store) const; // The tile should either be locked, or not accessible from the outside (during construction)! (const, blegh, but OK since m_sharedData mutable)

    inline quint8 *data() const {
        return m_tileData->data;
    }

    void setData(const quint8 *pixel);

    qint32 refCount() const;
    void ref();

    void detachShared() const; // ### Const?

    inline qint32 getRow() const {
        return m_row;
    }
    inline qint32 getCol() const {
        return m_col;
    }

    inline QRect extent() const {
        return QRect(m_col * WIDTH, m_row * HEIGHT, WIDTH, HEIGHT);
    }

    void setNext(KisTile *);
    inline KisTile *getNext() const {
        return m_nextTile;
    }

    // These are const because they don't change the external data the tile represents,
    // although they do change internal representations. We need to be able to request
    // access to a tile in a const environment (like copyconstructor and so)!
    void addReader() const;

    void removeReader() const;

    //inline qint32 readers() const { return m_nReadlock; }

    inline qint32 pixelSize() const {
        return m_pixelSize;
    }

public:
    const KisSharedTileData::TimeType& lastUseTime() const {
        return m_tileData->lastUse;
    }
    void lock() const {
        m_lock.lock();
    }
    void unlock() const {
        m_lock.unlock();
    }
    //QString trace; // for debugging
private:

    friend class KisTiledIterator;
    friend class KisTiledDataManager;
    friend class KisMemento;
    friend class KisTileStoreMemory;
    friend class KisTileCompressor;
    friend class KisTiledRandomAccessor;
    friend class KisTileSwapper; // for the Mutex, we should de-privatize lock() access perhaps? Also used for m_tileData access!

    KisTile& operator=(const KisTile&);

private:
    mutable KisSharedTileData* m_tileData; // Shared among the tiles

    QByteArray m_compressedData;
    qint32 m_row;
    qint32 m_col;
    qint32 m_pixelSize;
    KisTile *m_nextTile;
    mutable QMutex m_lock;

public:

    static const qint32 WIDTH;
    static const qint32 HEIGHT;
};

#endif // KIS_TILE_H_

