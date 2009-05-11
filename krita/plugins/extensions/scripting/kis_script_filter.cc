/*
 * This file is part of Krita
 *
 * Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
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

#include "kis_script_filter.h"


#include <kis_paint_device.h>

// kdelibs/kross
#include <kross/core/action.h>

class KisScriptFilter::Private
{
public:
    Kross::Action* action;
    explicit Private(Kross::Action* a) : action(a) {}
};

KisScriptFilter::KisScriptFilter(Kross::Action* action) : KisFilter(KoID(action->name(), action->text()), KoID(action->property("categoryId").toString(), i18n(action->property("categoryName").toString().toUtf8().data())), action->text()), d(new Private(action))
{
    d->action->addObject(this, "KritaFilter", Kross::ChildrenInterface::AutoConnectSignals);
    setSupportsPainting(d->action->property("supportsPainting").toBool());
    setSupportsPreview(d->action->property("supportsPreview").toBool());
    setSupportsIncrementalPainting(d->action->property("supportsIncrementalPainting").toBool());
    setSupportsAdjustmentLayers(d->action->property("supportsAdjustmentLayers").toBool());
    setSupportsThreading(false);
    setColorSpaceIndependence(FULLY_INDEPENDENT);
}

KisScriptFilter::~KisScriptFilter()
{
    delete d;
}

QString KisScriptFilter::category() const
{
    //This is an example that demonstrates how to receive properties from within the
    //scripts.rc file. Such properties will be accessible from within scripting too.
    return d->action->property("category").toString();
}

void KisScriptFilter::process(KisConstProcessingInformation srcInfo, KisProcessingInformation dstInfo, const QSize& size, const KisFilterConfiguration* config, KoUpdater*) const
{
    Q_UNUSED(config);
    d->action->trigger();

    emit scriptProcess(new Scripting::ConstPaintDevice(srcInfo.paintDevice(), 0), srcInfo.topLeft(), new Scripting::PaintDevice(dstInfo.paintDevice(), 0), dstInfo.topLeft(), size, 0);
}

#include "kis_script_filter.moc"
