/* This file is part of the KDE project
   Copyright (C) 2003-2007 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kptdatetime.h"

#include <kdebug.h>

namespace KPlato
{

DateTime::DateTime() : KDateTime() {
}

DateTime::DateTime(const QDateTime &dt, const KDateTime::Spec &spec) : KDateTime(dt, spec) {
}

DateTime::DateTime(const KDateTime &dt) : KDateTime( dt ) {
}

DateTime::DateTime(const QDate &date, const KDateTime::Spec &spec) : KDateTime(date, spec) {
}

DateTime::DateTime(const QDate &date, const QTime &time, const KDateTime::Spec &spec) : KDateTime(date, time, spec ) {
}

void DateTime::add(const Duration &duration) {
    if (isValid()) {
        KDateTime x = addMSecs(duration.milliseconds());
        setDate( x.date() );
        setTime( x.time() );
        //kDebug()<<toString();
    }
}

void DateTime::subtract(const Duration &duration) {
    if (isValid()) {
        KDateTime x = addMSecs(-duration.milliseconds());
        setDate( x.date() );
        setTime( x.time() );
        //kDebug()<<toString();
    }
}

Duration DateTime::duration(const DateTime &dt) const {
    Duration dur;
    if (isValid() && dt.isValid()) {
        qint64 s = secsTo_long( dt );
        qint64 ms = dt.time().msec() - time().msec();
        //kDebug()<<s<<","<<ms;
        if ( ms < 0  && s > 0 ) {
            s += 1;
        } else if ( ms > 0 && s < 0 ) {
            s -= 1;
        }
        dur = Duration( qAbs( (s * 1000 ) + ms ) );
    }
    //kDebug()<<dur.milliseconds();
    return dur;
}

DateTime DateTime::operator+(const Duration &duration) const {
    DateTime d(*this);
    d.add(duration);
    return d;
}

DateTime& DateTime::operator+=(const Duration &duration) {
    add(duration);
    return *this;
}

DateTime DateTime::operator-(const Duration &duration) const {
    DateTime d(*this);
    d.subtract(duration);
    return d;
}

DateTime& DateTime::operator-=(const Duration &duration) {
    subtract(duration);
    return *this;
}

DateTime DateTime::fromString( const QString dts, const KDateTime::Spec &spec )
{
    if (dts.isEmpty()) {
        return DateTime();
    }
    KDateTime dt = KDateTime::fromString(dts);
    if ( ! dt.isValid() ) {
        // try to parse in qt default format (used in early version)
        return DateTime( QDateTime::fromString(dts), spec );
    }
    if ( dt.isClockTime() ) {
        // timezone offset missing, set to spec
        return DateTime( dt.dateTime(), spec );
    }
    DateTime t = DateTime( dt.toTimeSpec( spec ) );
    //kDebug()<<dt<<" ->"<<t.toString();
    return t;
}

}  //KPlato namespace
