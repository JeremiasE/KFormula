/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2004-2008 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2004 Clarence Dang <dang@kde.org>
 *  Copyright (c) 2004 Adrian Page <adrian@pagenet.plus.com>
 *  Copyright (c) 2004 Cyrille Berger <cberger@cberger.net>
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

#include "kis_duplicateop_settings.h"
#include "kis_duplicateop_option.h"
#include "kis_duplicateop_settings_widget.h"

#include <QDomElement>
#include <QDomDocument>

#include <KoPointerEvent.h>

#include <kis_image.h>
#include <kis_brush_option.h>
#include <kis_paintop_options_widget.h>
#include <kis_pressure_darken_option.h>
#include <kis_pressure_opacity_option.h>
#include <kis_pressure_size_option.h>
#include <kis_paint_action_type_option.h>
#include <kis_perspective_grid.h>
#include <KoViewConverter.h>


KisDuplicateOpSettings::KisDuplicateOpSettings( KisDuplicateOpSettingsWidget* widget, KisImageSP image )
    : KisPaintOpSettings( widget )
    , m_image( image )
    , m_isOffsetNotUptodate( true )
{
    Q_ASSERT( widget );
    m_optionsWidget = widget;
    // Initialize with the default settings from the widget
    m_optionsWidget->writeConfiguration( this );

}

KisDuplicateOpSettings::~KisDuplicateOpSettings() {
}

bool KisDuplicateOpSettings::paintIncremental()
{
    return true;
}


QPointF KisDuplicateOpSettings::offset() const
{
    return m_offset;
}

void KisDuplicateOpSettings::mousePressEvent( KoPointerEvent *e )
{
    if (e->modifiers() == Qt::ShiftModifier) {
        m_position = m_image->documentToPixel(e->point);
        m_isOffsetNotUptodate = true;
        e->accept();
    } else {
        if (m_isOffsetNotUptodate) {
            m_offset = m_image->documentToPixel(e->point) - m_position;
            m_isOffsetNotUptodate = false;
        }
        e->ignore();
    }

}

void KisDuplicateOpSettings::activate()
{
    if (m_image->perspectiveGrid()->countSubGrids() != 1) {
        m_optionsWidget->m_duplicateOption->setHealing( false );
        m_optionsWidget->m_duplicateOption->setPerspective( false );
    } else {
        m_optionsWidget->m_duplicateOption->setPerspective( false );;
    }
}

bool KisDuplicateOpSettings::healing() const
{
    return m_optionsWidget->m_duplicateOption->healing();
}

bool KisDuplicateOpSettings::perspectiveCorrection() const
{
    return m_optionsWidget->m_duplicateOption->correctPerspective();
}

void KisDuplicateOpSettings::fromXML(const QDomElement& elt)
{
    // First, call the parent class fromXML to make sure all the
    // properties are saved to the map
    KisPaintOpSettings::fromXML( elt );

    m_offset.setX(elt.attribute("OffsetX", "0.0").toDouble());
    m_offset.setY(elt.attribute("OffsetY", "0.0").toDouble());
    m_isOffsetNotUptodate = false;

    // Then load the properties for all widgets
    m_optionsWidget->setConfiguration( this );
}

void KisDuplicateOpSettings::toXML(QDomDocument& doc, QDomElement& rootElt) const
{

    // First, make sure all the option widgets have saved their state
    // to the property configuration
    KisPropertiesConfiguration * settings = m_optionsWidget->configuration();

    // Then call the parent class fromXML
    settings->KisPropertiesConfiguration::toXML( doc, rootElt );

    rootElt.setAttribute("OffsetX", QString::number(m_offset.x()));
    rootElt.setAttribute("OffsetY", QString::number(m_offset.y()));


    delete settings;
}


KisPaintOpSettingsSP KisDuplicateOpSettings::clone() const {

    KisDuplicateOpSettings* s = dynamic_cast<KisDuplicateOpSettings*>( m_optionsWidget->configuration() );
    s->m_image = m_image;
    s->m_offset = m_offset;
    s->m_isOffsetNotUptodate = m_isOffsetNotUptodate;
    s->m_position = m_position;

    return s;

}

QRectF KisDuplicateOpSettings::duplicateOutlineRect(const QPointF& pos, KisImageSP image) const
{
    // Compute the rectangle for the offset
    QRectF rect2 = QRectF( -5, -5, 10, 10);
    if(m_isOffsetNotUptodate)
    {
        rect2.translate(m_position);
    } else {
        rect2.translate(- m_offset + image->documentToPixel( pos) );
    }
    return image->pixelToDocument(rect2);
}

QRectF KisDuplicateOpSettings::paintOutlineRect(const QPointF& pos, KisImageSP image) const
{
    KisBrushSP brush = m_optionsWidget->m_brushOption->brush();
    QPointF hotSpot = brush->hotSpot(1.0, 1.0);
    QRectF rect = image->pixelToDocument(QRect(0,0, brush->width(), brush->height()) );
    rect.translate( pos - hotSpot + QPoint(1,1) );
    rect |= duplicateOutlineRect(pos, image);
    return rect;
}

void KisDuplicateOpSettings::paintOutline(const QPointF& pos, KisImageSP image, QPainter &painter, const KoViewConverter &converter) const
{
    KisBrushSP brush = m_optionsWidget->m_brushOption->brush();
    QPointF hotSpot = brush->hotSpot(1.0, 1.0);
    painter.setPen(Qt::black);
    painter.setBackground(Qt::black);
    painter.drawEllipse( converter.documentToView( image->pixelToDocument(QRect(0,0, brush->width(), brush->height()) ).translated( pos - hotSpot + QPoint(1,1) ) ) );
    
    QRectF rect2 = converter.documentToView( duplicateOutlineRect( pos, image ) );
    painter.drawLine(rect2.topLeft(), rect2.bottomRight() );
    painter.drawLine(rect2.topRight(), rect2.bottomLeft() );

}

#include "kis_duplicateop_settings.moc"
