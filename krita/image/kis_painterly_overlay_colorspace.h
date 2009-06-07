/*
 *  Copyright (c) 2007 Boudewijn Rempt <boud@valdyas.org>
 *
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
#ifndef KIS_PAINTERLY_OVERLAY_COLORSPACE
#define KIS_PAINTERLY_OVERLAY_COLORSPACE

#include <klocale.h>
#include <krita_export.h>
#include <QString>
#include <KoIncompleteColorSpace.h>
#include <KoColorSpaceTraits.h>

/**
 *
 * XXX: Maybe split in two: one surface and one artistic medium
 * overlay? Now these properties are mixed up
 *
 * * adsorbency: property of the canvas. How fast wetness is dried
 * * gravity: property of the canvas: which direction the paint is
 * supposed to flow
 * * mixability: property of the paint. How easy it is to mix the paint
 * * height: canvas property. The height of the canvas fibers
 * * pigment_concentration: paint property. How concentrated the color is (similar to alpha)
 * * viscosity: paint property: how fast or slow the paint flows.
 * * volume: paint property: how much paint there is on the canvas.
 * height + volume is total height
 * * wetness: how wet the paint is. Wetter = less sticky
 *
 * reinterpret_cast your quint8* to
 * KisPainterlyOverlayColorSpaceTraits::Cell to have easy access to
 * the individual properties. See the unittest for example code.
 */

template<typename _channels_type_>
struct KisPainterlyOverlayColorSpaceTraits : public KoColorSpaceTrait < _channels_type_, 8, -1 > {

    static const quint8 adsorbency_pos = 0;
    static const quint8 gravity_pos = 1;
    static const quint8 mixability_pos = 2;
    static const quint8 height_pos = 3;
    static const quint8 pigment_concentration_pos = 4;
    static const quint8 viscosity_pos = 5;
    static const quint8 volume_pos = 6;
    static const quint8 wetness_pos = 7;

    struct Cell {
        _channels_type_ adsorbency;
        _channels_type_ gravity;
        _channels_type_ mixability;
        _channels_type_ height;
        _channels_type_ pigment_concentration;
        _channels_type_ viscosity;
        _channels_type_ volume;
        _channels_type_ wetness;
    };

};

typedef KisPainterlyOverlayColorSpaceTraits<float> PainterlyOverlayFloatTraits;
typedef PainterlyOverlayFloatTraits::Cell PropertyCell;

class KRITAIMAGE_EXPORT KisPainterlyOverlayColorSpace
            : public KoIncompleteColorSpace<PainterlyOverlayFloatTraits>
{

public:

    virtual ~KisPainterlyOverlayColorSpace();
    virtual KoID colorModelId() const;
    virtual KoID colorDepthId() const;
    virtual KoColorSpace* clone() const;

    static const KisPainterlyOverlayColorSpace* instance();

private: // This is a singleton

    friend class KisPainterlyOverlayColorSpaceFactory;

    KisPainterlyOverlayColorSpace(const QString &id, const QString &name);
    KisPainterlyOverlayColorSpace(const KisPainterlyOverlayColorSpace&);
    KisPainterlyOverlayColorSpace operator=(const KisPainterlyOverlayColorSpace&);

public:
    void colorToXML(const quint8* , QDomDocument& , QDomElement&) const {}
    void colorFromXML(quint8* , const QDomElement&) const {}

    bool willDegrade(ColorSpaceIndependence independence) const {
        Q_UNUSED(independence);
        return true;
    }

    bool profileIsCompatible(const KoColorProfile* profile) const {
        Q_UNUSED(profile);
        return false;
    }
    void fromQColor(const QColor& c, quint8 *dstU8, const KoColorProfile * profile) const {
        Q_UNUSED(c);
        Q_UNUSED(dstU8);
        Q_UNUSED(profile);
        // Don't call this!
        Q_ASSERT(0 == 1);
    }

    void fromQColor(const QColor& c, quint8 opacity, quint8 *dstU8, const KoColorProfile * profile) const {
        Q_UNUSED(c);
        Q_UNUSED(opacity);
        Q_UNUSED(dstU8);
        Q_UNUSED(profile);
        // Don't call this!
        Q_ASSERT(0 == 1);
    }

    void toQColor(const quint8 *srcU8, QColor *c, const KoColorProfile * profile) const {
        Q_UNUSED(srcU8);
        Q_UNUSED(c);
        Q_UNUSED(profile);
        // Don't call this!
        Q_ASSERT(0 == 1);
    }

    void toQColor(const quint8 *srcU8, QColor *c, quint8 *opacity, const KoColorProfile * profile) const {
        Q_UNUSED(srcU8);
        Q_UNUSED(c);
        Q_UNUSED(opacity);
        Q_UNUSED(profile);
        // Don't call this!
        Q_ASSERT(0 == 1);
    }

    void fromRgbA16(const quint8 * srcU8, quint8 * dstU8, quint32 nPixels) const {
        Q_UNUSED(srcU8);
        Q_UNUSED(dstU8);
        Q_UNUSED(nPixels);
        // Don't call this!
        Q_ASSERT(0 == 1);
    }
    void toRgbA16(const quint8 * srcU8, quint8 * dstU8, quint32 nPixels) const {
        Q_UNUSED(srcU8);
        Q_UNUSED(dstU8);
        Q_UNUSED(nPixels);
        // Don't call this!
        Q_ASSERT(0 == 1);
    }
public:
    struct Private;
    Private* const d;
};
#endif
