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

#include "kis_bookmarked_filter_configurations_model.h"
#include <QPixmap>

#include <filter/kis_filter.h>
#include <filter/kis_filter_configuration.h>
#include <kis_paint_device.h>

struct KisBookmarkedFilterConfigurationsModel::Private {
    KisPaintDeviceSP thumb;
    KisFilterSP filter;
    QHash<int, QImage> previewCache;
};

KisBookmarkedFilterConfigurationsModel::KisBookmarkedFilterConfigurationsModel(KisPaintDeviceSP thumb, KisFilterSP filter)
        : KisBookmarkedConfigurationsModel(filter->bookmarkManager()), d(new Private)
{
    d->thumb = thumb;
    d->filter = filter;
}

KisBookmarkedFilterConfigurationsModel::~KisBookmarkedFilterConfigurationsModel()
{
    delete d;
}

QVariant KisBookmarkedFilterConfigurationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::DecorationRole) {
        if (!d->previewCache.contains(index.row())) {
            KisPaintDeviceSP target = new KisPaintDevice(*d->thumb);
            d->filter->process(target, QRect(0, 0, 100, 100), configuration(index));
            d->previewCache[index.row()] = target->convertToQImage(0);
        }
        return d->previewCache[index.row()];
    } else {
        return KisBookmarkedConfigurationsModel::data(index, role);
    }
}

KisFilterConfiguration* KisBookmarkedFilterConfigurationsModel::configuration(const QModelIndex &index) const
{
    KisFilterConfiguration* config = dynamic_cast<KisFilterConfiguration*>(KisBookmarkedConfigurationsModel::configuration(index));
    if (config) return config;
    return d->filter->defaultConfiguration(d->thumb);
}
