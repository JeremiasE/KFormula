/*
 * Copyright (c) 2000 Clara Chan <x@unknown.com>
 * Copyright (c) 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef BRISTLE_H
#define BRISTLE_H

#include <stdio.h>
#include <math.h>

#include <KoColor.h>

class Bristle
{

    friend class Brush;

public:

    Bristle() {
        inkAmount = 0;
    }

    ~Bristle() {}

    void initPos(double);

    void setPos(double, double);

    void setPreThres(double p) {
        pressureThres = p;
    }

    void setTXThres(double tx) {
        txThres = tx;
    }

    void setTYThres(double ty) {
        tyThres = ty;
    }

    void setInitialPosition(double, double);

    int getX() {
        return static_cast<int>(x);
    }

    int getY() {
        return static_cast<int>(y);
    }

    double getPreThres() {
        return pressureThres;
    }

    double getTXThres() {
        return txThres;
    }

    double getTYThres() {
        return tyThres;
    }

    void reposition(double);

    double getThickness() {
        return thickness;
    }

    int getInkAmount() {
        return inkAmount;
    }

    void setInkAmount(int i) {
        inkAmount = i;
    }

    void depleteInk(int i) {
        inkAmount -= i;
    }

    void addInk(int i) {
        inkAmount += i;
    }

    double distanceFromCenter();

    void initializeThickness(int);

private:

    double x, y, lastx, lasty;  // offset from the mouse click position
    double thickness;
    double pressureThres, txThres, tyThres;
    int inkAmount;
    KoColor inkColor;

};

#endif
