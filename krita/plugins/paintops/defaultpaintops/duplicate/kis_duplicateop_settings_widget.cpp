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

#include "kis_duplicateop_settings_widget.h"
#include "kis_duplicateop_settings.h"
#include "kis_duplicateop_option.h"
                                                
#include <kis_image.h>
#include <kis_properties_configuration.h>
#include <kis_brush_option.h>
#include <kis_paintop_options_widget.h>
#include <kis_pressure_size_option.h>


KisDuplicateOpSettingsWidget::KisDuplicateOpSettingsWidget(QWidget* parent)
    : KisPaintOpOptionsWidget(parent)
{
    setObjectName("brush option widget");

    m_brushOption = new KisBrushOption;
    m_sizeOption = new KisPressureSizeOption;
    m_duplicateOption = new KisDuplicateOpOption;

    addPaintOpOption( m_brushOption );
    addPaintOpOption( m_sizeOption );
    addPaintOpOption( m_duplicateOption );
}

KisDuplicateOpSettingsWidget::~KisDuplicateOpSettingsWidget()
{
    delete m_brushOption;
    delete m_sizeOption;
    delete m_duplicateOption;
}

void KisDuplicateOpSettingsWidget::setConfiguration( const KisPropertiesConfiguration * config)
{
    m_brushOption->readOptionSetting(config);
    m_sizeOption->readOptionSetting(config);
    m_duplicateOption->readOptionSetting( config );
}

KisPropertiesConfiguration* KisDuplicateOpSettingsWidget::configuration() const
{
    KisDuplicateOpSettings *config =
        new KisDuplicateOpSettings(const_cast<KisDuplicateOpSettingsWidget*>( this ), 0);
    m_brushOption->writeOptionSetting(config);
    m_sizeOption->writeOptionSetting(config);
    m_duplicateOption->writeOptionSetting( config );
    return config;
}

void KisDuplicateOpSettingsWidget::writeConfiguration( KisPropertiesConfiguration *config ) const
{
    config->setProperty("paintop", "duplicate"); // XXX: make this a const id string
    m_brushOption->writeOptionSetting(config);
    m_sizeOption->writeOptionSetting(config);
    m_duplicateOption->writeOptionSetting( config );
}

void KisDuplicateOpSettingsWidget::setImage(KisImageSP image)
{
    m_brushOption->setImage(image);
    m_duplicateOption->setImage(image);
}

#include "kis_duplicateop_settings_widget.moc"
