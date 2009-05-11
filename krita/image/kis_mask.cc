/*
 *  Copyright (c) 2006 Boudewijn Rempt <boud@valdyas.org>
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

#include "kis_mask.h"


#include <kis_debug.h>
#include <klocale.h>

#include <KoColorSpaceRegistry.h>
#include <KoProperties.h>

#include "kis_types.h"
#include "kis_paint_device.h"
#include "kis_layer.h"
#include "kis_selection.h"
#include "kis_pixel_selection.h"

struct KisMask::Private {
    KisSelectionSP selection;
};

KisMask::KisMask(const QString & name)
        : KisNode()
        , m_d(new Private())
{
    setName(name);
    m_d->selection = new KisSelection();
}

KisMask::KisMask(const KisMask& rhs)
        : KisNode(rhs)
        , m_d(new Private())
{
    setName(rhs.name());
    m_d->selection = new KisSelection(*rhs.m_d->selection.data());
}

KisMask::~KisMask()
{
    delete m_d;
}

KisSelectionSP KisMask::selection() const
{
    return m_d->selection;
}

KisPaintDeviceSP KisMask::paintDevice() const
{
    return m_d->selection->getOrCreatePixelSelection();
}

void KisMask::setSelection(KisSelectionSP selection)
{
    m_d->selection = selection;
}

void KisMask::select(const QRect & rc, quint8 selectedness)
{
    Q_ASSERT(m_d->selection);
    KisPixelSelectionSP psel = m_d->selection->getOrCreatePixelSelection();
    psel->select(rc, selectedness);
    m_d->selection->updateProjection(rc);
}

QRect KisMask::extent() const
{
    Q_ASSERT(m_d->selection);
    return m_d->selection->selectedRect();
}

QRect KisMask::exactBounds() const
{
    Q_ASSERT(m_d->selection);
    return m_d->selection->selectedExactRect();
}

qint32 KisMask::x() const
{
    Q_ASSERT(m_d->selection);
    return m_d->selection->x();
}

void KisMask::setX(qint32 x)
{
    Q_ASSERT(m_d->selection);
    m_d->selection->setX(x);
}

qint32 KisMask::y() const
{
    Q_ASSERT(m_d->selection);
    return m_d->selection->y();
}

void KisMask::setY(qint32 y)
{
    Q_ASSERT(m_d->selection);
    m_d->selection->setY(y);
}

QRect KisMask::adjustedDirtyRect( const QRect& _rect ) const
{
    return _rect;
}

QRect KisMask::neededRect( const QRect& _rect ) const
{
    return _rect;
}

#include "kis_mask.moc"
