/* This file is part of the KOffice project
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kptabstractchartmodel.h"


namespace KPlato
{
    
ChartDataIndex::ChartDataIndex()
    : userData( 0 ),
      m_number( -1 )
{
}

ChartDataIndex::ChartDataIndex( int number, int userdata )
    : userData( userdata ),
      m_number( number )
{
}

ChartDataIndex::~ChartDataIndex()
{
}

bool ChartDataIndex::isValid() const
{
    return m_number >= 0;
}

int ChartDataIndex::number() const
{
    return m_number;
}

QDebug ChartDataIndex::debug( QDebug dbg ) const
{
    dbg.nospace() << "( KPlato::ChartDataIndex[ number=" << m_number << " userData=" << userData << " )";
    return dbg.space();
}


//---------------------
ChartAxisIndex::ChartAxisIndex()
    : userData( 0 ),
      parentId( -1 ),
      m_number( -1 )
{
}

ChartAxisIndex::ChartAxisIndex( int number, int userData )
    : userData( userData ),
      parentId( -1 ),
      m_number( number )
{
}

ChartAxisIndex::~ChartAxisIndex()
{
}

bool ChartAxisIndex::isValid() const
{
    return m_number >= 0;
}

int ChartAxisIndex::number() const
{
    return m_number;
}

QDebug ChartAxisIndex::debug( QDebug dbg ) const
{
    dbg.nospace() << "( KPlato::ChartAxisIndex[ number=" << m_number << " userData=" << userData << " parentId=" << parentId<< " )";
    return dbg.space();
}

//-------------------------
AbstractChartModel::AbstractChartModel( QObject *parent )
    : QObject( parent )
{
}

AbstractChartModel::~AbstractChartModel()
{
}

bool AbstractChartModel::hasChildren( const ChartDataIndex &parent ) const
{
    return childCount( parent ) > 0;
}

ChartDataIndex AbstractChartModel::index( int, const ChartDataIndex & ) const
{
    return ChartDataIndex();
}

ChartAxisIndex AbstractChartModel::parent( const ChartAxisIndex & ) const
{
    return ChartAxisIndex();
}

int AbstractChartModel::childCount( const ChartDataIndex & ) const
{
    return 0;
}

bool AbstractChartModel::hasAxisChildren( const ChartAxisIndex &parent ) const
{
    return axisCount( parent ) > 0;
}

ChartDataIndex AbstractChartModel::createDataIndex( int number, const ChartAxisIndex &axisIndex, int userdata ) const
{
    ChartDataIndex idx( number, userdata );
    idx.m_axisIndex = axisIndex;
    //kDebug()<<"Created"<<idx;
    return idx;
}

ChartDataIndex AbstractChartModel::createDataIndex( int number, const ChartDataIndex &parent, int userdata ) const
{
    ChartDataIndex idx( number, userdata );
    idx.m_axisIndex = parent.axisIndex();
    //kDebug()<<"Created"<<idx;
    return idx;
}

ChartAxisIndex AbstractChartModel::createAxisIndex( int number, const ChartAxisIndex &parent, int userdata, int parentId ) const
{
    ChartAxisIndex idx( number, userdata );
    idx.parentId = parentId;
    //kDebug()<<"Created"<<idx;
    return idx;
}


} //namespace KPlato

QDebug operator<<( QDebug dbg, const KPlato::ChartAxisIndex& index )
{
    return index.debug( dbg );
}

QDebug operator<<( QDebug dbg, const KPlato::ChartDataIndex& index )
{
    return index.debug( dbg );
}

#include "kptabstractchartmodel.moc"
