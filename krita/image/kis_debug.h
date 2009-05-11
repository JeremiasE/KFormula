/*
 *  Copyright (c) 2005 Boudewijn Rempt <boud@valdyas.org>
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
#ifndef KIS_DEBUG_AREAS_H_
#define KIS_DEBUG_AREAS_H_

#include <kdebug.h>

//#define KRITA_RELEASE_MODE

/**
 * In krita code, include kis_debug.h instead of kdebug.h. Then use one of the areas #defined here
 * instead of the raw numbers. Also, try not to check in code outside of tests that uses kDebug
 * without an area.
 */
#ifdef KRITA_RELEASE_MODE
#define dbgKrita if (false) kDebug(41000) // For temporary debug lines, where you'd have used kDebug() before.
#define dbgImage if (false) kDebug(41001)
#define dbgRegistry if (false) kDebug(41002)
#define dbgTools if (false) kDebug(41003)
#define dbgTiles if (false) kDebug(41004)
#define dbgFilters if (false) kDebug(41005)
#define dbgPlugins if (false) kDebug(41006)
#define dbgUI if (false) kDebug(41007)
#define dbgFile if (false) kDebug(41008)
#define dbgMath if (false) kDebug(41009)
#define dbgRender if (false) kDebug(41010)
#define dbgScript if (false) kDebug(41011)

#define warnKrita if (false) kWarning(41000) // For temporary debug lines, where you'd have used kWarning() before.
#define warnImage if (false) kWarning(41001)
#define warnRegistry if (false) kWarning(41002)
#define warnTools if (false) kWarning(41003)
#define warnTiles if (false) kWarning(41004)
#define warnFilters if (false) kWarning(41005)
#define warnPlugins if (false) kWarning(41006)
#define warnUI if (false) kWarning(41007)
#define warnFile if (false) kWarning(41008)
#define warnMath if (false) kWarning(41009)
#define warnRender if (false) kWarning(41010)
#define warnScript if (false) kWarning(41011)

#define errKrita if (false) kError(41000) // For temporary debug lines, where you'd have used kError() before.
#define errImage if (false) kError(41001)
#define errRegistry if (false) kError(41002)
#define errTools if (false) kError(41003)
#define errTiles if (false) kError(41004)
#define errFilters if (false) kError(41005)
#define errPlugins if (false) kError(41006)
#define errUI if (false) kError(41007)
#define errFile if (false) kError(41008)
#define errMath if (false) kError(41009)
#define errRender if (false) kError(41010)
#define errScript if (false) kError(41011)

#define fatalKrita if (false) kFatal(41000) // For temporary debug lines, where you'd have used kFatal() before.
#define fatalImage if (false) kFatal(41001)
#define fatalRegistry if (false) kFatal(41002)
#define fatalTools if (false) kFatal(41003)
#define fatalTiles if (false) kFatal(41004)
#define fatalFilters if (false) kFatal(41005)
#define fatalPlugins if (false) kFatal(41006)
#define fatalUI if (false) kFatal(41007)
#define fatalFile if (false) kFatal(41008)
#define fatalMath if (false) kFatal(41009)
#define fatalRender if (false) kFatal(41010)
#define fatalScript if (false) kFatal(41011)

#else

#define dbgKrita kDebug(41000) // For temporary debug lines, where you'd have used kDebug() before.
#define dbgImage kDebug(41001)
#define dbgRegistry kDebug(41002)
#define dbgTools kDebug(41003)
#define dbgTiles kDebug(41004)
#define dbgFilters kDebug(41005)
#define dbgPlugins kDebug(41006)
#define dbgUI kDebug(41007)
#define dbgFile kDebug(41008)
#define dbgMath kDebug(41009)
#define dbgRender kDebug(41010)
#define dbgScript kDebug(41011)

#define warnKrita kWarning(41000) // For temporary debug lines, where you'd have used kWarning() before.
#define warnImage kWarning(41001)
#define warnRegistry kWarning(41002)
#define warnTools kWarning(41003)
#define warnTiles kWarning(41004)
#define warnFilters kWarning(41005)
#define warnPlugins kWarning(41006)
#define warnUI kWarning(41007)
#define warnFile kWarning(41008)
#define warnMath kWarning(41009)
#define warnRender kWarning(41010)
#define warnScript kWarning(41011)

#define errKrita kError(41000) // For temporary debug lines, where you'd have used kError() before.
#define errImage kError(41001)
#define errRegistry kError(41002)
#define errTools kError(41003)
#define errTiles kError(41004)
#define errFilters kError(41005)
#define errPlugins kError(41006)
#define errUI kError(41007)
#define errFile kError(41008)
#define errMath kError(41009)
#define errRender kError(41010)
#define errScript kError(41011)

#define fatalKrita kFatal(41000) // For temporary debug lines, where you'd have used kFatal() before.
#define fatalImage kFatal(41001)
#define fatalRegistry kFatal(41002)
#define fatalTools kFatal(41003)
#define fatalTiles kFatal(41004)
#define fatalFilters kFatal(41005)
#define fatalPlugins kFatal(41006)
#define fatalUI kFatal(41007)
#define fatalFile kFatal(41008)
#define fatalMath kFatal(41009)
#define fatalRender kFatal(41010)
#define fatalScript kFatal(41011)
#endif

/**
 * Use this macro to display in the output stream the name of a variable followed by its value.
 */
#define ppVar( var ) #var << "=" << var

#endif
