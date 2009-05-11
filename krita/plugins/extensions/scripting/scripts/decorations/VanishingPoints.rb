#  Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

def drawDecoration( gc, documentOffset, area, converter)
    x_c = 800
    y_c = 600
    painter = Qt::Internal.kross2smoke( gc, Qt::Painter )
    painter.setPen( Qt::Pen.new( Qt::Brush.new(Qt::black), 1, Qt::SolidLine ) )
    painter.drawLine(0, y_c, 1000, y_c)
    painter.drawLine(0, 0, x_c, y_c)
    painter.drawLine(0, 1000, x_c, y_c)
    painter.drawLine(1000, 0, x_c, y_c)
    painter.drawLine(1000, 1000, x_c, y_c)
end
