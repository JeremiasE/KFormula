/*
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
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

#ifndef _KIS_RECORDED_BEZIER_CURVE_PAINT_ACTIONS_H_
#define _KIS_RECORDED_BEZIER_CURVE_PAINT_ACTIONS_H_

#include "recorder/kis_recorded_action.h"
#include "recorder/kis_recorded_paint_action.h"
#include "kis_types.h"

class KisPaintInformation;
class KisPainter;
class KoColor;
class KoCompositeOp;

#include <krita_export.h>

/**
 * This class will record the painting of a bezier curve.
 */
class KRITAIMAGE_EXPORT KisRecordedBezierCurvePaintAction : public KisRecordedPaintAction
{

public:

    KisRecordedBezierCurvePaintAction(const QString & name,
                                      const KisNodeQueryPath& path,
                                      const KisPaintOpPresetSP paintOpPreset,
                                      KoColor foregroundColor,
                                      KoColor backgroundColor,
                                      int opacity,
                                      bool paintIncremental,
                                      const KoCompositeOp * compositeOp);

    KisRecordedBezierCurvePaintAction(const KisRecordedBezierCurvePaintAction&);

    ~KisRecordedBezierCurvePaintAction();

    void addPoint(const KisPaintInformation& point1,
                  const QPointF& control1,
                  const QPointF& control2,
                  const KisPaintInformation& point2);

    virtual void toXML(QDomDocument& doc, QDomElement& elt) const;

    virtual KisRecordedAction* clone() const;

protected:

    virtual void playPaint(const KisPlayInfo& info, KisPainter* painter) const;

private:

    struct Private;
    Private* const d;
};


class KisRecordedBezierCurvePaintActionFactory : public KisRecordedPaintActionFactory
{
public:
    KisRecordedBezierCurvePaintActionFactory();
    virtual ~KisRecordedBezierCurvePaintActionFactory();
    virtual KisRecordedAction* fromXML(const QDomElement& elt);
};





#endif
