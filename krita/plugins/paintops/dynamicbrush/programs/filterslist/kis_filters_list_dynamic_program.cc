/*
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
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

#include "kis_filters_list_dynamic_program.h"
#include <QDomNode>
#include <QWidget>

#include <klocale.h>

// Dynamic Brush lib includes
#include "kis_dynamic_coloring.h"
#include "kis_dynamic_program_factory_registry.h"
#include "kis_dynamic_shape.h"
#include "kis_dynamic_transformation.h"
#include "kis_dynamic_transformations_factory.h"

// Filterslist program includes
#include "kis_filters_list_dynamic_program_editor.h"

class Factory
{
public:
    Factory() {
        KisDynamicProgramFactoryRegistry::instance()->add(new KisFiltersListDynamicProgramFactory);
    }
};

static Factory factory;

KisFiltersListDynamicProgram::~KisFiltersListDynamicProgram()
{
    for (QList<KisDynamicTransformation*>::iterator transfo = beginTransformation();
            transfo != endTransformation(); ++transfo) {
        delete * transfo;
    }
}

void KisFiltersListDynamicProgram::apply(KisDynamicShape* shape, KisDynamicColoring* coloringsrc, const KisPaintInformation& adjustedInfo)
{
    // First apply the transfo to the dab source
    for (QList<KisDynamicTransformation*>::iterator transfo = beginTransformation();
            transfo != endTransformation(); ++transfo) {
        (*transfo)->transformBrush(shape, adjustedInfo);
    }

    // Then to the coloring source
    for (QList<KisDynamicTransformation*>::iterator transfo = beginTransformation();
            transfo != endTransformation(); ++transfo) {
        (*transfo)->transformColoring(coloringsrc, adjustedInfo);
    }

}

void KisFiltersListDynamicProgram::appendTransformation(KisDynamicTransformation* transfo)
{
    dbgPlugins << "Append transfo : " << transfo->name();
    m_transformations.append(transfo);
    emit(programChanged());
}

QWidget* KisFiltersListDynamicProgram::createEditor(QWidget* /*parent*/)
{
    return new KisFiltersListDynamicProgramEditor(this);
}

void KisFiltersListDynamicProgram::fromXML(const QDomElement& elt)
{
    KisDynamicProgram::fromXML(elt);
    QDomNode n = elt.firstChild();
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
            if (e.tagName() == "Filters") {
                QDomNode nFilter = e.firstChild();
                while (!nFilter.isNull()) {
                    QDomElement eFilter = nFilter.toElement();
                    if (eFilter.tagName() == "Filter") {
                        KisDynamicTransformation* transfo = KisDynamicTransformationsFactory::createFromXML(eFilter);
                        if (transfo) {
                            appendTransformation(transfo);
                        }
                    }
                    nFilter = nFilter.nextSibling();
                }
            }
        }
        n = n.nextSibling();
    }

}

void KisFiltersListDynamicProgram::toXML(QDomDocument& doc, QDomElement& rootElt) const
{
    KisDynamicProgram::toXML(doc, rootElt);

    QDomElement filtersElt = doc.createElement("Filters");
    for (QList<KisDynamicTransformation*>::const_iterator it = m_transformations.begin();
            it != m_transformations.end(); ++it) {
        QDomElement filterElt = doc.createElement("Filter");
        (*it)->toXML(doc, filterElt);
        filtersElt.appendChild(filterElt);
    }
    rootElt.appendChild(filtersElt);

}

//--- KisFiltersListDynamicProgramFactory ---//

KisFiltersListDynamicProgramFactory::KisFiltersListDynamicProgramFactory() :
        KisDynamicProgramFactory("filterslist", i18n("Filters list"))
{
}

KisDynamicProgram* KisFiltersListDynamicProgramFactory::program(QString name)
{
    return new KisFiltersListDynamicProgram(name);
}

