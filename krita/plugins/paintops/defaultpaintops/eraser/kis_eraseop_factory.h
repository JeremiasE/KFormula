/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2004-2008 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2004 Clarence Dang <dang@kde.org>
 *  Copyright (c) 2004 Adrian Page <adrian@pagenet.plus.com>
 *  Copyright (c) 2004 Cyrille Berger <cberger@cberger.net>
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

#ifndef KIS_ERASEOP_FACTORY_H_
#define KIS_ERASEOP_FACTORY_H_

#include <kis_paintop_factory.h>
#include <kis_types.h>
#include <klocale.h>
#include <QString>

class KisPaintOp;
class KisPainter;
class QWidget;
class KoInputDevice;
class KisEraseOpSettingsWidget;

class KisEraseOpFactory : public KisPaintOpFactory
{

public:
    KisEraseOpFactory();
    virtual ~KisEraseOpFactory();

    virtual KisPaintOp * createOp(const KisPaintOpSettingsSP settings, KisPainter * painter, KisImageSP image);

    virtual QString id() const {
        return "eraser";
    }

    virtual QString name() const {
        return i18n("Pixel Eraser");
    }

    virtual QString pixmap() {
        return "krita-eraser.png";
    }

    virtual KisPaintOpSettingsSP settings(QWidget * parent, const KoInputDevice& inputDevice, KisImageSP image);

    virtual KisPaintOpSettingsSP settings(KisImageSP image);

private:

    KisEraseOpSettingsWidget* m_widget;
};

#endif // KIS_ERASEOP_FACTORY_H_
