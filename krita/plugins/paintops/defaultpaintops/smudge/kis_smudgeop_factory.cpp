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

#include "kis_smudgeop_factory.h"

#include <KoInputDevice.h>
#include <kis_painter.h>
#include <kis_paintop_settings.h>
#include <kis_image.h>

#include "kis_smudgeop_settings_widget.h"
#include "kis_smudgeop_settings.h"
#include "kis_smudgeop.h"

KisSmudgeOpFactory::KisSmudgeOpFactory()
    : m_widget( new KisSmudgeOpSettingsWidget )
{
}


KisSmudgeOpFactory::~KisSmudgeOpFactory()
{
    // XXX? Delete our widget?
}


KisPaintOp * KisSmudgeOpFactory::createOp(const KisPaintOpSettingsSP settings,
                                         KisPainter * painter,
                                         KisImageSP image)
{
    Q_UNUSED(image);
    Q_ASSERT( settings->widget() );

    const KisSmudgeOpSettings *smudgeopSettings = dynamic_cast<const KisSmudgeOpSettings *>(settings.data());
    Q_ASSERT(settings != 0 && smudgeopSettings != 0);

    KisPaintOp * op = new KisSmudgeOp(smudgeopSettings, painter);
    Q_CHECK_PTR(op);
    return op;
}

KisPaintOpSettingsSP KisSmudgeOpFactory::settings(QWidget * parent, const KoInputDevice& inputDevice, KisImageSP image)
{
    // XXX: store widgets per inputDevice?
    Q_UNUSED( parent );
    Q_UNUSED(inputDevice);
    m_widget->setImage(image);
    return new KisSmudgeOpSettings(m_widget);
}

KisPaintOpSettingsSP KisSmudgeOpFactory::settings(KisImageSP image)
{
    m_widget->setImage(image);
    return new KisSmudgeOpSettings(m_widget);
}
