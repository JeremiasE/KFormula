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

#ifndef KIS_BRUSHOP_SETTINGS_H_
#define KIS_BRUSHOP_SETTINGS_H_

#include <kis_paintop_settings.h>
#include <kis_types.h>

class QDomElement;
class KisBrushOpSettingsWidget;

class KisBrushOpSettings : public QObject, public KisPaintOpSettings
{
    Q_OBJECT

public:
    using KisPaintOpSettings::fromXML;
    using KisPaintOpSettings::toXML;

    KisBrushOpSettings( KisBrushOpSettingsWidget* widget );
    virtual ~KisBrushOpSettings();

    bool paintIncremental();

    void fromXML(const QDomElement& elt);
    void toXML(QDomDocument& doc, QDomElement& rootElt) const;

    KisPaintOpSettingsSP clone() const;

    virtual QRectF paintOutlineRect(const QPointF& pos, KisImageSP image ) const;
    virtual void paintOutline(const QPointF& pos, KisImageSP image, QPainter &painter, const KoViewConverter &converter) const;
public:

    KisBrushOpSettingsWidget *m_optionsWidget;

};


#endif // KIS_BRUSHOP_SETTINGS_H_
