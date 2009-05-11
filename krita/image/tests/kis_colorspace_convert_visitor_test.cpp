/*
 *  Copyright (c) 2007 Boudewijn Rempt boud@valdyas.org
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

#include "kis_colorspace_convert_visitor_test.h"

#include <qtest_kde.h>
#include <KoColorTransformation.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include "kis_colorspace_convert_visitor.h"

#include "kis_image.h"

void KisColorSpaceConvertVisitorTest::testCreation()
{
    const KoColorSpace * colorSpace = KoColorSpaceRegistry::instance()->colorSpace("CMYK", 0);
    QVERIFY(colorSpace);
    KisColorSpaceConvertVisitor test(colorSpace, KoColorConversionTransformation::IntentPerceptual);
}


QTEST_KDEMAIN(KisColorSpaceConvertVisitorTest, GUI)
#include "kis_colorspace_convert_visitor_test.moc"
