/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/
#ifndef __KDCHARTABSTRACTSERIALIZER_H__
#define __KDCHARTABSTRACTSERIALIZER_H__

/** \file KDChartAbstractSerializer.h
  */

#include <KDChartGlobal>

#include "kdchartserializer_export.h"

class QDomDocument;
class QDomElement;
class QDomNode;
class QObject;

namespace KDChart {

    class KDCHARTSERIALIZER_EXPORT AbstractSerializer
    {
    public:
        virtual ~AbstractSerializer() {}

        virtual void saveElement( QDomDocument& doc, QDomElement& e, const QObject* obj ) const = 0;
        virtual bool parseElement( const QDomElement& container, QObject* ptr ) const = 0;
    };

} // end of namespace

#endif
