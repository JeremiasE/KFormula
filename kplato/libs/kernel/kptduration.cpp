/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004-2007 Dag Andersen <danders@get2net.dk>

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

#include "kptduration.h"
#include "kptdatetime.h"

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

#include <QRegExp>
#include <QStringList>

namespace KPlato
{

// Set the value of Duration::zeroDuration to zero.
const Duration Duration::zeroDuration( 0, 0, 0 );

Duration::Duration() {
    m_ms = 0;
}

Duration::Duration( double value, Duration::Unit unit ) {
    if (unit == Unit_ms) m_ms = (qint64)value;
    else if (unit == Unit_s) m_ms = (qint64)(value * 1000);
    else if (unit == Unit_m) m_ms = (qint64)(value * ( 1000 * 60 ));
    else if (unit == Unit_h) m_ms = (qint64)(value * ( 1000 * 60 * 60 ));
    else if (unit == Unit_d) m_ms = (qint64)(value * ( 1000 * 60 * 60 * 24 ));
}

Duration::Duration(const Duration &d) {
    copy( d );
}

Duration::Duration(unsigned d, unsigned h, unsigned m, unsigned s, unsigned ms) {
    m_ms = ms;
    m_ms += static_cast<qint64>(s) * 1000; // cast to avoid potential overflow problem
    m_ms += static_cast<qint64>(m) * 60 * 1000;
    m_ms += static_cast<qint64>(h) * 60 * 60 * 1000;
    m_ms += static_cast<qint64>(d) * 24 * 60 * 60 * 1000;
}

Duration::Duration(const qint64 value, Duration::Unit unit) {
    if (unit == Unit_ms) m_ms = value;
    else if (unit == Unit_s) m_ms = (qint64)(value * 1000);
    else if (unit == Unit_m) m_ms = (qint64)(value * ( 1000 * 60 ));
    else if (unit == Unit_h) m_ms = (qint64)(value * ( 1000 * 60 * 60 ));
    else if (unit == Unit_d) m_ms = (qint64)(value * ( 1000 * 60 * 60 * 24 ));
    else kError()<<"Unknown unit: "<<unit;
}

Duration::~Duration() {
}

void Duration::add(const Duration &delta) {
    m_ms += delta.m_ms;
}

void Duration::add(qint64 delta) {
    qint64 tmp = m_ms + delta;
    if (tmp < 0) {
        kDebug()<<"Underflow"<<(long int)delta<<" from"<<this->toString();
        m_ms = 0;
        return;
    }
    m_ms = tmp;
}

void Duration::subtract(const Duration &delta) {
    if (m_ms < delta.m_ms) {
        kDebug()<<"Underflow"<<delta.toString()<<" from"<<this->toString();
        m_ms = 0;
        return;
    }
    m_ms -= delta.m_ms;
}

Duration Duration::operator*(int value) const {
    Duration dur(*this);
    if (value < 0) {
        kDebug()<<"Underflow"<<value<<" from"<<this->toString();
    }
    else {
        dur.m_ms = m_ms * value; //FIXME
    }
    return dur;
}

Duration Duration::operator/(int value) const {
    Duration dur(*this);
    if (value <= 0) {
        kDebug()<<"Underflow"<<value<<" from"<<this->toString();
    }
    else {
        dur.m_ms = m_ms / value; //FIXME
    }
    return dur;
}

Duration Duration::operator*(const double value) const {
    Duration dur(*this);
    dur.m_ms = qAbs(m_ms * (qint64)value);
    return dur;
}

Duration Duration::operator*(const Duration value) const {
    Duration dur(*this);
    dur.m_ms = m_ms * value.m_ms;
    return dur;
}

double Duration::operator/(const Duration &d) const {
    if (d == zeroDuration) {
        kDebug()<<"Divide by zero:"<<this->toString();
        return 0.0;
    }
    return (double)(m_ms) / (double)(d.m_ms);
}

QString Duration::format(Unit unit, int pres, const KLocale *locale) const
{
    if ( locale == 0 ) {
        locale = KGlobal::locale();
    }
    /* FIXME if necessary
    return i18nc( "<duration><unit>", "%1%2", locale->formatNumber(toDouble(unit), pres), unitToString(unit) );*/
    return locale->formatNumber( toDouble( unit ), pres ) + unitToString( unit );
}

QString Duration::toString(Format format) const {
    qint64 ms;
    double days;
    unsigned hours;
    unsigned minutes;
    unsigned seconds;
    QString result;

    switch (format) {
        case Format_Hour:
            ms = m_ms;
            hours = ms / (1000 * 60 * 60);
            ms -= (qint64)hours * (1000 * 60 * 60);
            minutes = ms / (1000 * 60);
            result = QString("%1h%2m").arg(hours).arg(minutes);
            break;
        case Format_Day:
            days = m_ms / (1000 * 60 * 60 * 24.0);
            result = QString("%1d").arg(QString::number(days, 'f', 4));
            break;
        case Format_DayTime:
            ms = m_ms;
            days = m_ms / (1000 * 60 * 60 * 24);
            ms -= (qint64)days * (1000 * 60 * 60 * 24);
            hours = ms / (1000 * 60 * 60);
            ms -= (qint64)hours * (1000 * 60 * 60);
            minutes = ms / (1000 * 60);
            ms -= minutes * (1000 * 60);
            seconds = ms / (1000);
            ms -= seconds * (1000);
            result.sprintf("%u %02u:%02u:%02u.%u", (unsigned)days, hours, minutes, seconds, (unsigned)ms);
            break;
        case Format_HourFraction:
            result = KGlobal::locale()->formatNumber(toDouble(Unit_h), 2);
            break;
        // i18n
        case Format_i18nHour:
            ms = m_ms;
            hours = ms / (1000 * 60 * 60);
            ms -= (qint64)hours * (1000 * 60 * 60);
            minutes = ms / (1000 * 60);
            result = i18nc("<hours>h:<minutes>m", "%1h:%2m", hours, minutes);
            break;
        case Format_i18nDay:
            result = KGlobal::locale()->formatNumber(toDouble(Unit_d), 2);
            break;
        case Format_i18nDayTime:
            ms = m_ms;
            days = m_ms / (1000 * 60 * 60 * 24);
            ms -= (qint64)days * (1000 * 60 * 60 * 24);
            hours = ms / (1000 * 60 * 60);
            ms -= (qint64)hours * (1000 * 60 * 60);
            minutes = ms / (1000 * 60);
            ms -= minutes * (1000 * 60);
            seconds = ms / (1000);
            ms -= seconds * (1000);
            if (days == 0) {
                result = toString(Format_i18nHour);
            } else {
                result = i18nc("<days>d <hours>h:<minutes>m", "%1d %2h:%3m", days, hours, minutes);
            }
            break;
        case Format_i18nHourFraction:
            result = KGlobal::locale()->formatNumber(toDouble(Unit_h), 2);
            break;
        default:
            kFatal()<<"Unknown format"<<endl;
            break;
    }
    return result;
}

Duration Duration::fromString(const QString &s, Format format, bool *ok) {
    if (ok) *ok = false;
    QRegExp matcher;
    Duration tmp;
    switch (format) {
        case Format_Hour: {
            matcher.setPattern("^(\\d*)h(\\d*)m$" );
            int pos = matcher.indexIn(s);
            if (pos > -1) {
                tmp.addHours(matcher.cap(1).toUInt());
                tmp.addMinutes(matcher.cap(2).toUInt());
                if (ok) *ok = true;
            }
            break;
        }
        case Format_DayTime: {
            matcher.setPattern("^(\\d*) (\\d*):(\\d*):(\\d*)\\.(\\d*)$" );
            int pos = matcher.indexIn(s);
            if (pos > -1) {
                tmp.addDays(matcher.cap(1).toUInt());
                tmp.addHours(matcher.cap(2).toUInt());
                tmp.addMinutes(matcher.cap(3).toUInt());
                tmp.addSeconds(matcher.cap(4).toUInt());
                tmp.addMilliseconds(matcher.cap(5).toUInt());
                if (ok) *ok = true;
            }
            break;
        }
        case Format_HourFraction: {
            // should be in double format
            bool res;
            double f = KGlobal::locale()->readNumber(s, &res);
            if (ok) *ok = res;
            if (res) {
                return Duration((qint64)(f)*3600*1000);
            }
            break;
        }
        default:
            kFatal()<<"Unknown format"<<endl;
            break;
    }
    return tmp;
}

void Duration::get(unsigned *days, unsigned *hours, unsigned *minutes, unsigned *seconds, unsigned *milliseconds) const {
    qint64 ms;
    qint64 tmp;

    ms = m_ms;
    tmp = ms / (1000 * 60 * 60 * 24);
    *days = tmp;
    ms -= tmp * (1000 * 60 * 60 * 24);
    tmp = ms / (1000 * 60 * 60);
    *hours = tmp;
    ms -= tmp * (1000 * 60 * 60);
    tmp = ms / (1000 * 60);
    *minutes = tmp;
    ms -= tmp * (1000 * 60);
    tmp = ms / (1000);
    if (seconds)
        *seconds = tmp;
    ms -= tmp * (1000);
    if (milliseconds)
        *milliseconds = ms;
}

QStringList Duration::unitList( bool trans )
{
    QStringList lst;
    lst << ( trans ? i18n( "d" ) : "d" )
        << ( trans ? i18n( "h" ) : "h" )
        << ( trans ? i18n( "m" ) : "m" )
        << ( trans ? i18n( "s" ) : "s" )
        << ( trans ? i18n( "ms" ) : "ms" );
    return lst;
}

QString Duration::unitToString( Duration::Unit unit, bool trans )
{
    switch ( unit ) {
        case Unit_ms: return trans ? i18n( "ms" ) : "ms";
        case Unit_s: return trans ? i18n( "s" ) : "s";
        case Unit_m: return trans ? i18n( "m" ) : "m";
        case Unit_h: return trans ? i18n( "h" ) : "h";
        case Unit_d: return trans ? i18n( "d" ) : "d";
    }
    return QString();
}

Duration::Unit Duration::unitFromString( const QString &u )
{
    if (u == "ms") return Unit_ms;
    else if (u == "s") return Unit_s;
    else if (u == "m") return Unit_m;
    else if (u == "h") return Unit_h;
    else if (u == "d") return Unit_d;
    else kError()<<"Illegal unit: "<<u<<endl;
    return Unit_ms; 
}

bool Duration::valueFromString( const QString &value, double &rv, Unit &unit ) {
    QStringList lst = Duration::unitList();
    foreach ( QString s, lst ) {
        int pos = value.lastIndexOf( s );
        if ( pos != -1 ) {
            unit = Duration::unitFromString( s );
            QString v = value;
            v.remove( s );
            bool ok;
            rv = v.toDouble( &ok );
            kError()<<value<<" -> "<<v<<", "<<s<<" = "<<ok<<endl;
            return ok;
        }
    }
    kError()<<"Illegal format, no unit: "<<value<<endl;
    return false;
}

void Duration::copy( const Duration &d )
{
    m_ms = d.m_ms;
}

}  //KPlato namespace
