/*
 *  Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "trianglecolorselector.h"

#include <kcomponentdata.h>
#include <kgenericfactory.h>

#include <KoDockFactory.h>
#include <KoDockRegistry.h>
#include "kis_debug.h"

#include "kis_triangle_color_selector_dock.h"

typedef KGenericFactory<TriangleColorSelectorPlugin> TriangleColorSelectorPluginFactory;
K_EXPORT_COMPONENT_FACTORY(kritatrianglecolorselector, TriangleColorSelectorPluginFactory("krita"))

class KisTriangleColorSelectorDockFactory : public KoDockFactory
{
public:
    KisTriangleColorSelectorDockFactory() {
    }

    virtual QString id() const {
        return QString("KisTriangleColorSelector");
    }

    virtual Qt::DockWidgetArea defaultDockWidgetArea() const {
        return Qt::RightDockWidgetArea;
    }

    virtual QDockWidget* createDockWidget() {
        KisTriangleColorSelectorDock * dockWidget = new KisTriangleColorSelectorDock();

        dockWidget->setObjectName(id());

        return dockWidget;
    }

    KoDockFactory::DockPosition defaultDockPosition() const {
        return DockMinimized;
    }
};


TriangleColorSelectorPlugin::TriangleColorSelectorPlugin(QObject *parent, const QStringList &)
        : KParts::Plugin(parent)
{
    dbgPlugins << "TriangleColorSelectorPlugin";

    setComponentData(TriangleColorSelectorPluginFactory::componentData()); 
    KoDockRegistry::instance()->add( new KisTriangleColorSelectorDockFactory() );
}

TriangleColorSelectorPlugin::~TriangleColorSelectorPlugin()
{
}

#include "trianglecolorselector.moc"
