/* This file is part of the KDE project
   Copyright 2007 Boudewijn Rempt <boud@valdyas.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KritaShapeFactory.h"
#include <QStringList>

#include <kurl.h>
#include <kgenericfactory.h>
#include <klocale.h>

#include <KoProperties.h>
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>

#include "KritaShape.h"
#include "KritaShapeToolFactory.h"

K_EXPORT_COMPONENT_FACTORY(kritashape, KGenericFactory<KritaShapePlugin>("krita"))

KritaShapePlugin::KritaShapePlugin(QObject * parent,  const QStringList & list)
{
    KoShapeRegistry::instance()->add(new KritaShapeFactory(parent));
    KoToolRegistry::instance()->add(new KritaShapeToolFactory(parent, list));
}


KritaShapeFactory::KritaShapeFactory(QObject* parent)
        : KoShapeFactory(parent, KritaShapeId, i18n("KritaShape Shape"))
{
    setToolTip(i18n("A color managed, multi-layer raster image"));
    setIcon("kritashape");

}

KoShape* KritaShapeFactory::createDefaultShape() const
{
    KritaShape* shape = new KritaShape(KUrl(), "sRGB built-in - (lcms internal)");
    return shape;
}

KoShape* KritaShapeFactory::createShape(const KoProperties* params) const
{
    Q_UNUSED(params);
    // XXX; Get the url and profile name from the params
    KritaShape* shape = new KritaShape(KUrl(), "sRGB built-in - (lcms internal)");
    return shape;
}

#include "KritaShapeFactory.moc"

