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

#ifndef _BRUSH_SHAPE_H_
#define _BRUSH_SHAPE_H_

#include <QVector>
#include "bristle.h"

class BrushShape
{

public:
    BrushShape();
    ~BrushShape();
    void fromGaussian(int radius, float maxLength, float sigma);
    QVector<Bristle> getBristles();
    int width();
    int height();

private:
    int m_width;
    int m_height;
    QVector<Bristle> m_bristles;

};

#endif
