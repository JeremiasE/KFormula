/*
 *  kis_tool_polygon.h - part of Krita
 *
 *  Copyright (c) 2004 Michael Thaler <michael Thaler@physik.tu-muenchen.de>
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

#ifndef KIS_TOOL_POLYGON_H_
#define KIS_TOOL_POLYGON_H_

#include "kis_tool_shape.h"
#include "flake/kis_node_shape.h"

class KoCanvasBase;

class KisToolPolygon : public KisToolShape
{

    Q_OBJECT

public:
    KisToolPolygon(KoCanvasBase *canvas);
    virtual ~KisToolPolygon();

    virtual void mousePressEvent(KoPointerEvent *event);
    virtual void mouseMoveEvent(KoPointerEvent *event);
    virtual void mouseReleaseEvent(KoPointerEvent *event);
    virtual void mouseDoubleClickEvent(KoPointerEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

    virtual void paint(QPainter& gc, const KoViewConverter &converter);

    virtual QString quickHelp() const {
        return i18n("Shift-click will end the polygon.");
    }

private slots:
    void finish();
    void cancel();

protected:
    QRectF dragBoundingRect();

protected:
    QPointF m_dragStart;
    QPointF m_dragEnd;

    bool m_dragging;
private:
    vQPointF m_points;
};


#include "KoToolFactory.h"

class KisToolPolygonFactory : public KoToolFactory
{

public:
    KisToolPolygonFactory(QObject *parent, const QStringList&)
            : KoToolFactory(parent, "KisToolPolygon", i18n("Polygon")) {
        setToolTip(i18n("Draw a polygon. Shift-mouseclick ends the polygon."));
        setToolType(TOOL_TYPE_SHAPE);
        //setActivationShapeId( KIS_NODE_SHAPE_ID );
        setIcon("tool_polygon");
        setPriority(4);
        setInputDeviceAgnostic(false);
    }

    virtual ~KisToolPolygonFactory() {}

    virtual KoTool * createTool(KoCanvasBase *canvas) {
        return new KisToolPolygon(canvas);
    }

};


#endif //__KIS_TOOL_POLYGON_H__
