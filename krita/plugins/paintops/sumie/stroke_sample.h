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
#ifndef _STROKE_SAMPLE_H_
#define _STROKE_SAMPLE_H_
class StrokeSample
{


public:
    StrokeSample(float x, float y, float pressure, float tiltX, float tiltY, float rotation);
    StrokeSample();
    ~StrokeSample();

    float x();
    float y();
    float pressure();
    float tiltX();
    float tiltY();
    float rotation();

private:
    float m_x;
    float m_y;
    float m_pressure;
    float m_tiltX;
    float m_tiltY;
    float m_rotation;
};

#endif // _STROKE_SAMPLE_H_

