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

#ifndef _BRISTLE_H_
#define _BRISTLE_H_

#include <cmath>
#include <KoColor.h>

class Bristle
{

public:
    Bristle(float x, float y, float length);
    Bristle();
    ~Bristle();
    float x();
    float y();
    float length();
    void setLength(float length);

    KoColor color();

    void setColor(KoColor inkColor);
    void addInk(float value); // TODO talk with boud ??
    void removeInk(float value); //
    void setInkAmount(float inkAmount);


    float distanceCenter() {
        return std::sqrt((double)(m_x*m_x + m_y*m_y));
    }
    float amount();
private:
    // coordinates of bristle
    float m_x;
    float m_y;
    float m_length; // z - coordinate
    KoColor m_inkColor;
    float m_inkAmount;

};

#endif
