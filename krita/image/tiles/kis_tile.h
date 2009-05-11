/*
 *  Copyright (c) 2004 Casper Boemann <cbr@boemann.dk>
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
#include <QByteArray>

class KisTiledDataManager;
class KisTiledIterator;

enum TileState {
    UNCOMPRESSED = 0,
    QUEUED,
    COMPRESSED
};



/**
 * Provides abstraction to a tile.  A tile contains
 * a part of a PaintDevice, but only the individual pixels
 * are accesable and that only via iterators.
 */
class KisTile
{
public:
    KisTile(qint32 pixelSize, qint32 col, qint32 row, const quint8 *defPixel);
    KisTile(const KisTile& rhs, qint32 col, qint32 row);
    KisTile(const KisTile& rhs);
    ~KisTile();

public:
    void release();
    void allocate();

    inline quint8 *data() const {
        return m_data;
    }

    void setData(const quint8 *pixel);

    qint32 refCount() const;
    void ref();

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

    inline qint32 readers() {
        return m_nReadlock;
    }

    inline qint32 pixelSize() const {
        return m_pixelSize;
    }

    inline void setTileState(TileState tileState) {
        m_tileState = tileState;
    }
    inline TileState tileState() const {
        return m_tileState;
    }

private:

    friend class KisTiledIterator;
    friend class KisTiledDataManager;
    friend class KisMemento;
    friend class KisTileManager;
    friend class KisTileCompressor;
    friend class KisTiledRandomAccessor;

    KisTile& operator=(const KisTile&);

private:

    quint8 * m_data;
    QByteArray m_compressedData;
    TileState m_tileState;
    mutable qint32 m_nReadlock;
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

