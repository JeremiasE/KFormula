/*
 *  Copyright (c) 2007 Emanuele Tamponi <emanuele@valinor.it>
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

#include "colorspot.h"

ColorSpot::ColorSpot(QWidget *parent, const KoColor &color) : QToolButton(parent), m_color(color)
{
    QRgb c = m_color.toQColor().rgba();
    setPalette(QPalette(c, c));
    setAutoFillBackground(true);
    setAutoRepeat(true);
}

ColorSpot::~ColorSpot()
{

}

void ColorSpot::setColor(const KoColor &color)
{
    m_color = color;
    QRgb c = m_color.toQColor().rgba();
    setPalette(QPalette(c, c));
}


#include "colorspot.moc"
