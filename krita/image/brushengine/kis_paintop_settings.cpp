/*
 *  Copyright (c) 2007 Boudewijn Rempt <boud@valdyas.org>
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

#include "kis_paintop_settings.h"

#include <QWidget>
#include <QImage>
#include <QColor>

#include <KoPointerEvent.h>
#include <KoColor.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include "kis_node.h"
#include "kis_paint_layer.h"
#include "kis_image.h"
#include "kis_painter.h"
#include "kis_paint_device.h"
#include "kis_paintop_registry.h"
#include "kis_paint_information.h"
#include "kis_paintop_settings_widget.h"

struct KisPaintOpSettings::Private {
    KisNodeSP node;
    KisPaintOpSettingsWidget *settingsWidget;
};

KisPaintOpSettings::KisPaintOpSettings(KisPaintOpSettingsWidget *settingsWidget)
    : d(new Private)
{
    d->settingsWidget = settingsWidget;
}

KisPaintOpSettings::~KisPaintOpSettings()
{
    delete d;
}

KisPaintOpSettingsWidget* KisPaintOpSettings::widget() const
{
    return d->settingsWidget;
}

void KisPaintOpSettings::mousePressEvent(KoPointerEvent *e)
{
    e->ignore();
}

void KisPaintOpSettings::activate()
{
}

void KisPaintOpSettings::setNode(KisNodeSP node)
{
    d->node = node;
}

KisNodeSP KisPaintOpSettings::node() const
{
    return d->node;
}

QImage KisPaintOpSettings::sampleStroke(const QSize& size )
{
    const KoColorSpace * cs = KoColorSpaceRegistry::instance()->rgb8();
    int width = size.width();
    int height = size.height();

    KisImageSP img = new KisImage(0, width, height, cs, "temporary for stroke sample");
    KisLayerSP layer = new KisPaintLayer(img, "temporary for stroke sample", OPACITY_OPAQUE, cs);
    
    
    KisPainter painter(layer->paintDevice());
    painter.setPaintColor(KoColor(Qt::black, cs) );

    KisPaintOpPresetSP preset = new KisPaintOpPreset();
    preset->setSettings( this ); // This clones
    preset->settings()->setNode( layer );
    
    painter.setPaintOpPreset( preset, img);

    QPointF p1(0                , 7.0/12.0 * height);
    QPointF p2(1.0/2.0 * width  , 7.0/12.0 * height);
    QPointF p3( width - 4.0, height - 4.0);

    KisPaintInformation pi1(p1, 0.0);
    KisPaintInformation pi2(p2, 0.95);
    KisPaintInformation pi3(p3, 0.0);

    QPointF c1( 1.0/4.0* width , height - 2.0);
    QPointF c2(c1);
    painter.paintBezierCurve(pi1, c1, c2, pi2, 0);

    c1.setX( 3.0/4.0 * width);
    c1.setY( 0);
    c2.setX( c1.x() );
    c2.setY( c1.y() );
    painter.paintBezierCurve(pi2, c1, c2, pi3, 0);

    return layer->paintDevice()->convertToQImage(0);
}

QRectF KisPaintOpSettings::paintOutlineRect(const QPointF& pos, KisImageSP image) const
{
    return QRectF();
}

void KisPaintOpSettings::paintOutline(const QPointF& pos, KisImageSP image, QPainter &painter, const KoViewConverter &converter) const
{
    Q_UNUSED(pos);
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}
