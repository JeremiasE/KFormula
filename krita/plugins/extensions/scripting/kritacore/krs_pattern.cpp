/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "krs_pattern.h"

#include "krs_module.h"

#include <kis_pattern.h>

using namespace Scripting;

Pattern::Pattern(Module* module, KisPattern* pattern, bool sharedPattern)
        : QObject(module)
        , m_pattern(pattern)
        , m_sharedPattern(sharedPattern)
{
    setObjectName("KritaPattern");
}

Pattern::~Pattern()
{
    if (!m_sharedPattern)
        delete m_pattern;
}
