/*
 *  Copyright (c) 2008 Lukas Tvrdy <lukast.dev@gmail.com>
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
#include "kis_spray_shape_option.h"
#include <klocale.h>

#include <QWidget>
#include <QRadioButton>

#include "ui_wdgshapeoptions.h"

class KisShapeOptionsWidget: public QWidget, public Ui::WdgShapeOptions
{
public:
    KisShapeOptionsWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

KisSprayShapeOption::KisSprayShapeOption()
        : KisPaintOpOption(i18n("Particle type"), false)
{
    m_checkable = false;
    m_options = new KisShapeOptionsWidget();
    setConfigurationPage(m_options);
}

KisSprayShapeOption::~KisSprayShapeOption()
{
    // delete m_options; 
}

int KisSprayShapeOption::object() const {
    if (m_options->shapeBtn->isChecked())
        return 0;
    if (m_options->particleBtn->isChecked())
        return 1;
    if (m_options->pixelBtn->isChecked())
        return 2;
    return -1;
}

int KisSprayShapeOption::shape() const {
    return m_options->shapeBox->currentIndex();
}

int KisSprayShapeOption::width() const {
    return m_options->widthSpin->value();
}

int KisSprayShapeOption::height() const {
    return m_options->heightSpin->value();
}

bool KisSprayShapeOption::jitterShapeSize() const {
    return m_options->jitterShape->isChecked();
}

qreal KisSprayShapeOption::heightPerc() const {
    return m_options->heightPro->value();
}

qreal KisSprayShapeOption::widthPerc() const {
    return m_options->widthPro->value(); 
}

bool KisSprayShapeOption::proportional() const {
    return m_options->proportionalBox->isChecked();
}



// TODO
void KisSprayShapeOption::writeOptionSetting(KisPropertiesConfiguration* setting) const
{
//     setting->setProperty( "Spray/diameter", diameter() );
}

// TODO
void KisSprayShapeOption::readOptionSetting(const KisPropertiesConfiguration* setting)
{
/*    m_options->diameterSpinBox->setValue( setting->getInt("Spray/diameter") );
    m_options->coverageSpin->setValue( setting->getDouble("Spray/coverage") );
    m_options->jitterSizeBox->setChecked( setting->getBool("Spray/jitterSize") );*/
}




