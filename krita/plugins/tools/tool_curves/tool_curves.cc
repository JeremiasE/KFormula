/*
 *  tool_bezier.cc -- part of Krita
 *
 *  Copyright (c) 2006 Emanuele Tamponi <emanuele@valinor.it>
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

#include "tool_curves.h"


#include <stdlib.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kcomponentdata.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kis_debug.h>
#include <kgenericfactory.h>

#include <kis_global.h>
#include <kis_types.h>
#include <KoToolRegistry.h>


//#include "kis_tool_bezier_paint.h"
//#include "kis_tool_bezier_select.h"
#include "kis_tool_moutline.h"
//#include "kis_tool_example.h"

typedef KGenericFactory<ToolCurves> ToolCurvesFactory;
K_EXPORT_COMPONENT_FACTORY(kritatoolcurves, ToolCurvesFactory("krita"))

ToolCurves::ToolCurves(QObject *parent, const QStringList &)
        : KParts::Plugin(parent)
{
    setComponentData(ToolCurvesFactory::componentData());

    if (parent->inherits("KoToolRegistry")) {
        KoToolRegistry * r = dynamic_cast<KoToolRegistry*>(parent);
        //r->add(new KisToolBezierPaintFactory());
        //r->add(new KisToolBezierSelectFactory());
        r->add(new KisToolMagneticFactory());
        //r->add(new KisToolExampleFactory());
    }

}

ToolCurves::~ToolCurves()
{
}

#include "tool_curves.moc"
