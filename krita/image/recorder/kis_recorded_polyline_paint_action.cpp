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

#include "recorder/kis_recorded_polyline_paint_action.h"
#include <QDomDocument>
#include <QDomElement>

#include <KoColor.h>
#include <KoColorModelStandardIds.h>
#include <KoCompositeOp.h>
#include <KoColorSpace.h>
#include "kis_node.h"
#include "kis_mask_generator.h"
#include "kis_painter.h"
#include "kis_paint_information.h"
#include "kis_paintop_registry.h"
#include "recorder/kis_recorded_action_factory_registry.h"
#include "kis_resource_server_provider.h"
#include "kis_transaction.h"
#include "kis_undo_adapter.h"
#include "kis_paintop_settings.h"
#include "kis_paintop_preset.h"
#include "kis_paint_device.h"
#include "kis_image.h"
#include "kis_layer.h"

struct KisRecordedPolyLinePaintAction::Private {
    QList<KisPaintInformation> infos;
};

KisRecordedPolyLinePaintAction::KisRecordedPolyLinePaintAction(const QString & name,
        const KisNodeQueryPath& path,
        const KisPaintOpPresetSP paintOpPreset,
        KoColor foregroundColor,
        KoColor backgroundColor,
        int opacity,
        bool paintIncremental,
        const KoCompositeOp * compositeOp)
        : KisRecordedPaintAction(name, "PolyLinePaintAction", path, paintOpPreset, foregroundColor,
                                 backgroundColor, opacity, paintIncremental, compositeOp)
        , d(new Private)
{
}

KisRecordedPolyLinePaintAction::KisRecordedPolyLinePaintAction(const KisRecordedPolyLinePaintAction& rhs)
        : KisRecordedPaintAction(rhs), d(new Private(*rhs.d))
{

}

KisRecordedPolyLinePaintAction::~KisRecordedPolyLinePaintAction()
{
    delete d;
}

void KisRecordedPolyLinePaintAction::addPoint(const KisPaintInformation& info)
{
    d->infos.append(info);
}

void KisRecordedPolyLinePaintAction::playPaint(const KisPlayInfo&, KisPainter* painter) const
{
    dbgUI << "play poly line paint with " << d->infos.size() << " points";
    if (d->infos.size() <= 0) return;
    painter->paintAt(d->infos[0]);
    double savedDist = 0.0;
    for (int i = 0; i < d->infos.size() - 1; i++) {
        dbgUI << d->infos[i].pos() << " to " << d->infos[i+1].pos();
        savedDist = painter->paintLine(d->infos[i], d->infos[i+1], savedDist);
    }
}

void KisRecordedPolyLinePaintAction::toXML(QDomDocument& doc, QDomElement& elt) const
{
    KisRecordedPaintAction::toXML(doc, elt);
    QDomElement waypointsElt = doc.createElement("Waypoints");
    foreach(KisPaintInformation info, d->infos) {
        QDomElement infoElt = doc.createElement("Waypoint");
        info.toXML(doc, infoElt);
        waypointsElt.appendChild(infoElt);
    }
    elt.appendChild(waypointsElt);
}

KisRecordedAction* KisRecordedPolyLinePaintAction::clone() const
{
    return new KisRecordedPolyLinePaintAction(*this);
}



KisRecordedPolyLinePaintActionFactory::KisRecordedPolyLinePaintActionFactory() :
        KisRecordedPaintActionFactory("PolyLinePaintAction")
{
}

KisRecordedPolyLinePaintActionFactory::~KisRecordedPolyLinePaintActionFactory()
{

}

KisRecordedAction* KisRecordedPolyLinePaintActionFactory::fromXML(const QDomElement& elt)
{
    Q_UNUSED(elt);
#if 0
    QString name = elt.attribute("name");
    KisNodeSP node = KisRecordedActionFactory::indexPathToNode(img, elt.attribute("node"));
    QString paintOpId = elt.attribute("paintop");
    int opacity = elt.attribute("opacity", "100").toInt();
    bool paintIncremental = elt.attribute("paintIncremental", "1").toInt();

    const KoCompositeOp * compositeOp = node->paintDevice()->colorSpace()->compositeOp(elt.attribute("compositeOp"));
    if (!compositeOp) {
        compositeOp = node->paintDevice()->colorSpace()->compositeOp(COMPOSITE_OVER);
    }


    KisPaintOpSettingsSP settings = 0;
    QDomElement settingsElt = elt.firstChildElement("PaintOpSettings");
    if (!settingsElt.isNull()) {
        settings = settingsFromXML(paintOpId, settingsElt, img);
    } else {
        dbgUI << "No <PaintOpSettings /> found";
    }

    KisBrush* brush = 0;

    QDomElement brushElt = elt.firstChildElement("Brush");
    if (!brushElt.isNull()) {
        brush = brushFromXML(brushElt);
    } else {
        dbgUI << "Warning: no <Brush /> found";
    }

    QDomElement backgroundColorElt = elt.firstChildElement("BackgroundColor");
    KoColor bC;
    if (!backgroundColorElt.isNull()) {
        bC = KoColor::fromXML(backgroundColorElt.firstChildElement(""), Integer8BitsColorDepthID.id(), QHash<QString, QString>());
        bC.setOpacity(255);
        dbgUI << "Background color : " << bC.toQColor();
    } else {
        dbgUI << "Warning: no <BackgroundColor /> found";
    }
    QDomElement foregroundColorElt = elt.firstChildElement("ForegroundColor");
    KoColor fC;
    if (!foregroundColorElt.isNull()) {
        fC = KoColor::fromXML(foregroundColorElt.firstChildElement(""), Integer8BitsColorDepthID.id(), QHash<QString, QString>());
        fC.setOpacity(255);
        dbgUI << "Foreground color : " << fC.toQColor();
    } else {
        dbgUI << "Warning: no <ForegroundColor /> found";
    }

    KisRecordedPolyLinePaintAction* rplpa = new KisRecordedPolyLinePaintAction(name, node, brush, paintOpId, settings, fC, bC, opacity, paintIncremental, compositeOp);

    QDomElement wpElt = elt.firstChildElement("Waypoints");
    if (!wpElt.isNull()) {
        QDomNode nWp = wpElt.firstChild();
        while (!nWp.isNull()) {
            QDomElement eWp = nWp.toElement();
            if (!eWp.isNull() && eWp.tagName() == "Waypoint") {
                rplpa->addPoint(KisPaintInformation::fromXML(eWp));
            }
            nWp = nWp.nextSibling();
        }
    } else {
        dbgUI << "Warning: no <Waypoints /> found";
    }
    return rplpa;
#endif
    return 0;
}
