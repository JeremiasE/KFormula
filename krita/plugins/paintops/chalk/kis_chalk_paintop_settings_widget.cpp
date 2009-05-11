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

#include "kis_chalk_paintop_settings_widget.h"

#include <KoColorSpaceRegistry.h>

#include <kis_image.h>
#include <kis_debug.h>

#include <kis_paintop_registry.h>
#include <kis_painter.h>
#include <kis_paint_device.h>
#include <kis_paint_information.h>

#include <KoColor.h>

#include "kis_chalkop_option.h"
#include "kis_chalk_paintop_settings.h"

#include <kis_paintop_options_widget.h>
#include <kis_paint_action_type_option.h>

KisChalkPaintOpSettingsWidget:: KisChalkPaintOpSettingsWidget(QWidget* parent)
    : KisPaintOpOptionsWidget(parent)
{
    m_paintActionTypeOption = new KisPaintActionTypeOption();
    m_chalkOption =  new KisChalkOpOption();

    addPaintOpOption(m_chalkOption);
    addPaintOpOption(m_paintActionTypeOption);
}

KisChalkPaintOpSettingsWidget::~ KisChalkPaintOpSettingsWidget()
{
    delete m_chalkOption;
    delete m_paintActionTypeOption;
}

void  KisChalkPaintOpSettingsWidget::setConfiguration( const KisPropertiesConfiguration * config)
{
    m_chalkOption->readOptionSetting(config);
    m_paintActionTypeOption->readOptionSetting(config);
}

KisPropertiesConfiguration*  KisChalkPaintOpSettingsWidget::configuration() const
{
    KisChalkPaintOpSettings* config = new KisChalkPaintOpSettings( const_cast<KisChalkPaintOpSettingsWidget*>( this ) );

    m_chalkOption->writeOptionSetting(config);
    m_paintActionTypeOption->writeOptionSetting(config);
    return config;
}

void KisChalkPaintOpSettingsWidget::writeConfiguration( KisPropertiesConfiguration* config ) const
{
    config->setProperty("paintop", "chalkbrush"); // XXX: make this a const id string
    m_chalkOption->writeOptionSetting(config);
    m_paintActionTypeOption->writeOptionSetting(config);
}
