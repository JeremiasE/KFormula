/*
 *  Copyright (c) 2008 Lukáš Tvrdý <lukast.dev@gmail.com>
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

#include "kis_dyna_paintop_factory.h"
#include "kis_dyna_paintop.h"
#include "kis_dyna_paintop_settings.h"
#include "kis_dyna_paintop_settings_widget.h"

#include <KoInputDevice.h>

#include <kis_image.h>
#include <kis_painter.h>
#include <kis_types.h>


KisDynaPaintOpFactory::KisDynaPaintOpFactory()
    :  m_widget( new KisDynaPaintOpSettingsWidget )
{

}

KisDynaPaintOpFactory::~KisDynaPaintOpFactory()
{
}

KisPaintOp * KisDynaPaintOpFactory::createOp(const KisPaintOpSettingsSP settings, KisPainter * painter, KisImageSP image = 0)
{
    Q_ASSERT( settings->widget() );
    const KisDynaPaintOpSettings *dynaSettings =
        dynamic_cast<const KisDynaPaintOpSettings *>(settings.data());
    Q_ASSERT(settings == 0 || dynaSettings != 0);

    KisPaintOp * op = new KisDynaPaintOp(dynaSettings, painter, image);
    Q_CHECK_PTR(op);
    return op;
}

KisPaintOpSettingsSP KisDynaPaintOpFactory::settings(QWidget * parent, const KoInputDevice& inputDevice, KisImageSP image)
{
    Q_UNUSED( parent );
    Q_UNUSED(inputDevice);
    Q_UNUSED(image);
    return new KisDynaPaintOpSettings(m_widget);
}

KisPaintOpSettingsSP KisDynaPaintOpFactory::settings(KisImageSP image)
{
    Q_UNUSED(image);
    return new KisDynaPaintOpSettings(m_widget);
}
