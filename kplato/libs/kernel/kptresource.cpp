/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>
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

#include "kptresource.h"
#include "kptappointment.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kptcalendar.h"
#include "kpteffortcostmap.h"
#include "kptschedule.h"
#include "kptxmlloaderobject.h"

#include <KoXmlReader.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <ksystemtimezone.h>
#include <ktimezone.h>

//Added by qt3to4:
#include <QList>

namespace KPlato
{

ResourceGroup::ResourceGroup()
    : QObject( 0 ) 
{
    m_project = 0;
    m_type = Type_Work;
    //kDebug()<<"("<<this<<")";
}

ResourceGroup::ResourceGroup( const ResourceGroup *group )
    : QObject( 0 ) 
{
    m_project = 0;
    copy( group );
}

ResourceGroup::~ResourceGroup() {
    //kDebug()<<"("<<this<<")";
    if (findId() == this) {
        removeId(); // only remove myself (I may be just a working copy)
    }
    foreach ( ResourceGroupRequest* r, m_requests ) {
        r->unregister( this );
    }
    while (!m_resources.isEmpty()) {
        delete m_resources.takeFirst();
    }
    //kDebug()<<"("<<this<<")";
}

void ResourceGroup::copy( const ResourceGroup *group )
{
    //m_project = group->m_project; //Don't copy
    m_id = group->m_id;
    m_type = group->m_type;
    m_name = group->m_name;
}

void ResourceGroup::changed() {
    if ( m_project ) {
        m_project->changed( this );
    }
}

void ResourceGroup::setId(const QString& id) {
    //kDebug()<<id;
    m_id = id;
}

void ResourceGroup::setName( const QString& n )
{
    m_name = n.trimmed();
    changed();
}

void ResourceGroup::setType( Type type )
{
     m_type = type;
     changed();
}

QString ResourceGroup::typeToString( bool trans ) const {
    return typeToStringList( trans ).at( m_type );
}

QStringList ResourceGroup::typeToStringList( bool trans ) {
    // keep theese in the same order as the enum!
    return QStringList() 
            << (trans ? i18n("Work") : QString("Work"))
            << (trans ? i18n("Material") : QString("Material"));
}

void ResourceGroup::setProject( Project *project )
{
    if ( project != m_project ) {
        if ( m_project  ) {
            removeId();
        }
    }
    m_project = project;
    foreach ( Resource *r, m_resources ) {
        r->setProject( project );
    }
}

bool ResourceGroup::isScheduled() const
{
    foreach ( Resource *r, m_resources ) {
        if ( r->isScheduled() ) {
            return true;
        }
    }
    return false;
}

void ResourceGroup::addResource(int index, Resource* resource, Risk*) {
    int i = index == -1 ? m_resources.count() : index;
    resource->setParentGroup( this );
    resource->setProject( m_project );
    m_resources.insert(i, resource );
}

Resource *ResourceGroup::takeResource(Resource *resource) {
    Resource *r = 0;
    int i = m_resources.indexOf(resource);
    if (i != -1) {
        r = m_resources.takeAt(i);
        r->setParentGroup( 0 );
        r->setProject( 0 );
    }
    return r;
}

int ResourceGroup::indexOf( const Resource *resource ) const
{
    return m_resources.indexOf( const_cast<Resource*>( resource ) ); //???
}

Risk* ResourceGroup::getRisk(int) {
    return 0L;
}

void ResourceGroup::addRequiredResource(ResourceGroup*) {
}

ResourceGroup* ResourceGroup::getRequiredResource(int) {
    return 0L;
}

void ResourceGroup::deleteRequiredResource(int) {
}

bool ResourceGroup::load(KoXmlElement &element, XMLLoaderObject &status ) {
    //kDebug();
    setId(element.attribute("id"));
    m_name = element.attribute("name");

    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "resource") {
            // Load the resource
            Resource *child = new Resource();
            if (child->load(e, status)) {
                status.project().addResource( this, child );
            } else {
                // TODO: Complain about this
                delete child;
            }
        }
    }
    return true;
}

void ResourceGroup::save(QDomElement &element)  const {
    //kDebug();

    QDomElement me = element.ownerDocument().createElement("resource-group");
    element.appendChild(me);

    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);

    foreach (Resource *r, m_resources) {
        r->save(me);
    }
}

void ResourceGroup::saveWorkPackageXML( QDomElement &element, const QList<Resource*> lst ) const
{
    QDomElement me = element.ownerDocument().createElement( "resource-group" );
    element.appendChild( me );

    me.setAttribute( "id", m_id );
    me.setAttribute( "name", m_name );

    foreach ( Resource *r, m_resources ) {
        if ( lst.contains( r ) ) {
            r->save( me );
        }
    }
}

void ResourceGroup::initiateCalculation(Schedule &sch) {
    foreach (Resource *r, m_resources) {
        r->initiateCalculation(sch);
    }
    clearNodes();
}

int ResourceGroup::units() {
    int u = 0;
    foreach (Resource *r, m_resources) {
        u += r->units();
    }
    return u;
}

ResourceGroup *ResourceGroup::findId(const QString &id) const {
    return m_project ? m_project->findResourceGroup(id) : 0;
}

bool ResourceGroup::removeId(const QString &id) { 
    return m_project ? m_project->removeResourceGroupId(id): false;
}

void ResourceGroup::insertId(const QString &id) { 
    //kDebug();
    if (m_project)
        m_project->insertResourceGroupId(id, this);
}

Appointment ResourceGroup::appointmentIntervals() const {
    Appointment a;
    foreach (Resource *r, m_resources) {
        a += r->appointmentIntervals();
    }
    return a;
}

Resource::Resource()
    : QObject( 0 ), // atm QObject is only for casting
    m_project(0),
    m_parent( 0 ),
    m_schedules(),
    m_currentSchedule( 0 )
{
    m_type = Type_Work;
    m_units = 100; // %

    m_availableFrom = DateTime( QDate::currentDate(), QTime( 0, 0, 0 ) );
    m_availableUntil = m_availableFrom.addYears(2);

    cost.normalRate = 100;
    cost.overtimeRate = 200;
    cost.fixed = 0;
    m_calendar = 0;
    m_currentSchedule = 0;
    //kDebug()<<"("<<this<<")";
}

Resource::Resource(Resource *resource)
    : QObject( 0 ), // atm QObject is only for casting
    m_project( 0 ),
    m_parent( 0 ),
    m_currentSchedule( 0 )
{
    //kDebug()<<"("<<this<<") from ("<<resource<<")";
    copy(resource); 
}

Resource::~Resource() {
    //kDebug()<<"("<<this<<")";
    if (findId() == this) {
        removeId(); // only remove myself (I may be just a working copy)
    }
    removeRequests();
    foreach (long key, m_schedules.keys()) {
        delete m_schedules.take(key);
    }
    clearExternalAppointments();
}

void Resource::removeRequests() {
    foreach (ResourceRequest *r, m_requests) {
        r->setResource(0); // avoid the request to mess with my list
        r->parent()->deleteResourceRequest(r);
    }
}

void Resource::setId(const QString& id) {
    //kDebug()<<id;
    m_id = id;
}

void Resource::copy(Resource *resource) {
    m_project = 0; // NOTE: Don't copy, will be set when added to a project
    //m_appointments = resource->appointments(); // Note
    m_id = resource->id();
    m_name = resource->name();
    m_initials = resource->initials();
    m_email = resource->email();
    m_availableFrom = resource->availableFrom();
    m_availableUntil = resource->availableUntil();
    
    m_units = resource->units(); // available units in percent

    m_type = resource->type();

    cost.normalRate = resource->normalRate();
    cost.overtimeRate = resource->overtimeRate();
    
    m_calendar = resource->m_calendar;

    // hmmmm
    //m_externalAppointments = resource->m_externalAppointments;
    //m_externalNames = resource->m_externalNames;
}

void Resource::changed()
{
    if ( m_project ) {
        m_project->changed( this );
    }
}

void Resource::setType( Type type )
{
    m_type = type;
    changed();
}

void Resource::setType(const QString &type)
{
    if (type == "Work")
        setType( Type_Work );
    else if (type == "Material")
        setType( Type_Material );
    else
        setType( Type_Work );
}

QString Resource::typeToString( bool trans ) const {
    return typeToStringList( trans ).at( m_type );
}

QStringList Resource::typeToStringList( bool trans ) {
    // keep theese in the same order as the enum!
    return QStringList() 
            << (trans ? i18n("Work") : QString("Work"))
            << (trans ? i18n("Material") : QString("Material"));
}

void Resource::setName( const QString n )
{
    m_name = n.trimmed();
    changed();
}

void Resource::setInitials( const QString initials )
{
    m_initials = initials.trimmed();
    changed();
}

void Resource::setEmail( const QString email )
{
    m_email = email;
    changed();
}

Calendar *Resource::calendar(bool local) const {
    if (!local && project() != 0 && m_calendar == 0 ) {
        return project()->defaultCalendar();
    }
    return m_calendar;
}

DateTime Resource::firstAvailableAfter(const DateTime &, const DateTime & ) const {
    return DateTime();
}

DateTime Resource::getBestAvailableTime(Duration /*duration*/) {
    return DateTime();
}

DateTime Resource::getBestAvailableTime(const DateTime /*after*/, const Duration /*duration*/) {
    return DateTime();
}

bool Resource::load(KoXmlElement &element, XMLLoaderObject &status) {
    //kDebug();
    QString s;
    setId(element.attribute("id"));
    m_name = element.attribute("name");
    m_initials = element.attribute("initials");
    m_email = element.attribute("email");
    setType(element.attribute("type"));
    m_calendar = status.project().findCalendar(element.attribute("calendar-id"));
    m_units = element.attribute("units", "100").toInt();
    s = element.attribute("available-from");
    if (!s.isEmpty())
        m_availableFrom = DateTime::fromString(s, status.projectSpec());
    s = element.attribute("available-until");
    if (!s.isEmpty())
        m_availableUntil = DateTime::fromString(s, status.projectSpec());
        
    cost.normalRate = KGlobal::locale()->readMoney(element.attribute("normal-rate"));
    cost.overtimeRate = KGlobal::locale()->readMoney(element.attribute("overtime-rate"));
    
    KoXmlElement parent = element.namedItem( "external-appointments" ).toElement();
    KoXmlElement e;
    forEachElement( e, parent ) {
        if ( e.nodeName() == "project" ) {
            QString id = e.attribute( "id" );
            if ( id.isEmpty() ) {
		kError()<<"Missing project id";
                continue;
            }
            clearExternalAppointments( id ); // in case...
            AppointmentIntervalList lst;
            lst.loadXML( e, status );
            Appointment *a = new Appointment();
            a->setIntervals( lst );
            a->setAuxcilliaryInfo( e.attribute( "name", "Unknown" ) );
            m_externalAppointments[ id ] = a;
        }
    }
    return true;
}

void Resource::save(QDomElement &element) const {
    //kDebug();
    QDomElement me = element.ownerDocument().createElement("resource");
    element.appendChild(me);

    if (calendar(true))
        me.setAttribute("calendar-id", m_calendar->id());
    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);
    me.setAttribute("initials", m_initials);
    me.setAttribute("email", m_email);
    me.setAttribute("type", typeToString());
    me.setAttribute("units", m_units);
    me.setAttribute("available-from", m_availableFrom.toString( KDateTime::ISODate ));
    me.setAttribute("available-until", m_availableUntil.toString( KDateTime::ISODate ));
    me.setAttribute("normal-rate", KGlobal::locale()->formatMoney(cost.normalRate));
    me.setAttribute("overtime-rate", KGlobal::locale()->formatMoney(cost.overtimeRate));
    
    if ( ! m_externalAppointments.isEmpty() ) {
        QDomElement e = me.ownerDocument().createElement("external-appointments");
        me.appendChild(e);
        foreach ( QString id, m_externalAppointments.uniqueKeys() ) {
            QDomElement el = e.ownerDocument().createElement("project");
            e.appendChild( el );
            el.setAttribute( "id", id );
            el.setAttribute( "name", m_externalAppointments[ id ]->auxcilliaryInfo() );
            m_externalAppointments[ id ]->intervals().saveXML( el );
        }
    }
}

bool Resource::isAvailable(Task * /*task*/) {
    bool busy = false;
/*
    foreach (Appointment *a, m_appointments) {
        if (a->isBusy(task->startTime(), task->endTime())) {
            busy = true;
            break;
        }
    }*/
    return !busy;
}

QList<Appointment*> Resource::appointments( long id ) const {
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return QList<Appointment*>();
    }
    return s->appointments();
}

Appointment *Resource::findAppointment(Node * /*node*/) {
/*
    foreach (Appointment *a, m_appointments) {
        if (a->node() == node)
            return a;
    }*/
    return 0;
}

bool Resource::addAppointment(Appointment *appointment) {
    if (m_currentSchedule)
        return m_currentSchedule->add(appointment);
    return false;
}

bool Resource::addAppointment(Appointment *appointment, Schedule &main) {
    Schedule *s = findSchedule(main.id());
    if (s == 0) {
        s = createSchedule(&main);
    }
    appointment->setResource(s);
    return s->add(appointment);
}

// called from makeAppointment
void Resource::addAppointment(Schedule *node, DateTime &start, DateTime &end, double load) {
    Q_ASSERT( start < end );
    Schedule *s = findSchedule(node->id());
    if (s == 0) {
        s = createSchedule(node->parent());
    }
    s->setCalculationMode( node->calculationMode() );
    //kDebug()<<"id="<<node->id()<<" Mode="<<node->calculationMode()<<""<<start<<end;
    s->addAppointment(node, start, end, load);
}

void Resource::initiateCalculation(Schedule &sch) {
    m_currentSchedule = createSchedule(&sch);
}

Schedule *Resource::schedule( long id ) const
{
    return id == -1 ? m_currentSchedule : findSchedule( id );
}

Schedule *Resource::findSchedule( long id ) const
{
    if ( m_schedules.contains( id ) )
        return m_schedules[ id ];
    return 0;
}

bool Resource::isScheduled() const
{
    foreach ( Schedule *s, m_schedules ) {
        if ( s->isScheduled() ) {
            return true;
        }
    }
    return false;
}

void Resource::deleteSchedule(Schedule *schedule) {
    takeSchedule(schedule);
    delete schedule;
}

void Resource::takeSchedule(const Schedule *schedule) {
    if (schedule == 0)
        return;
    if (m_currentSchedule == schedule)
        m_currentSchedule = 0;
    m_schedules.take(schedule->id());
}

void Resource::addSchedule(Schedule *schedule) {
    if (schedule == 0)
        return;
    m_schedules.remove(schedule->id());
    m_schedules.insert(schedule->id(), schedule);
}

ResourceSchedule *Resource::createSchedule(const QString& name, int type, long id) {
    ResourceSchedule *sch = new ResourceSchedule(this, name, (Schedule::Type)type, id);
    addSchedule(sch);
    return sch;
}

ResourceSchedule *Resource::createSchedule(Schedule *parent) {
    ResourceSchedule *sch = new ResourceSchedule(parent, this);
    //kDebug()<<"id="<<sch->id();
    addSchedule(sch);
    return sch;
}

KDateTime::Spec Resource::timeSpec() const
{
    Calendar *cal = calendar();
    if ( cal ) {
        return cal->timeSpec();
    }
    if ( m_project ) {
        return m_project->timeSpec();
    }
    return KDateTime::Spec::LocalZone();
}

void Resource::makeAppointment(Schedule *node, const DateTime &from, const DateTime &end) {
    KLocale *locale = KGlobal::locale();
    //kDebug()<<"node id="<<node->id()<<" mode="<<node->calculationMode()<<""<<from<<" -"<<end;
    if (!from.isValid() || !end.isValid()) {
        m_currentSchedule->logWarning( i18n( "Make appointments: Invalid time" ) );
        return;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        m_currentSchedule->logWarning( i18n( "Resource %1 has no calendar defined", m_name ) );
        return;
    }
    DateTime time = from;
    while (time < end) {
        //kDebug()<<time<<" to"<<end;
        if (!time.isValid() || !end.isValid()) {
            m_currentSchedule->logWarning( i18n( "Make appointments: Invalid time" ) );
            return;
        }
        if (!cal->hasInterval(time, end, m_currentSchedule)) {
            //kDebug()<<time<<" to"<<end<<": No (more) interval(s)";
            m_currentSchedule->logWarning( i18n ( "Resource %1 only partially available", m_name ) );
            //node->resourceNotAvailable = true;
            return; // nothing more to do
        }
        DateTimeInterval i = cal->firstInterval(time, end, m_currentSchedule);
        if (!i.second.isValid()) {
            m_currentSchedule->logWarning( i18n( "Make appointments: Invalid interval %1 to %2", locale->formatDateTime( time ), locale->formatDateTime( end ) ) );
            return;
        }
        if (time == i.second)
            return; // hmmm, didn't get a new interval, avoid loop
        //kDebug()<<m_name<<"-->"<<node->node()->name()<<" add :"<<i.first.toString()<<" to"<<i.second.toString();
        addAppointment(node, i.first, i.second, m_units);
        if (!(node->workStartTime.isValid()) || i.first < node->workStartTime)
            node->workStartTime = i.first;
        if (!(node->workEndTime.isValid()) || i.second > node->workEndTime)
            node->workEndTime = i.second;
        time = i.second;
    }
    return;
}
void Resource::makeAppointment(Schedule *node) {
    //kDebug()<<m_name<<": id="<<m_currentSchedule->id()<<" mode="<<m_currentSchedule->calculationMode()<<node->node()->name()<<": id="<<node->id()<<" mode="<<node->calculationMode()<<""<<node->startTime;
    KLocale *locale = KGlobal::locale();
    if (!node->startTime.isValid()) {
        m_currentSchedule->logWarning( i18n( "Make appointments: Node start time is not valid" ) );
        return;
    }
    if (!node->endTime.isValid()) {
        m_currentSchedule->logWarning( i18n( "Make appointments: Node end time is not valid" ) );
        return;
    }
    node->resourceNotAvailable = false;
    node->workStartTime = DateTime();
    node->workEndTime = DateTime();
    Calendar *cal = calendar();
    if (m_type == Type_Material) {
        DateTime from = availableAfter(node->startTime, node->endTime);
        DateTime end = availableBefore(node->endTime, node->startTime);
        if (!from.isValid() || !end.isValid()) {
            return;
        }
        if (cal == 0) {
            // Allocate the whole period
            addAppointment(node, from, end, m_units);
            return;
        }
        makeAppointment(node, from, end);
    }
    if (!cal) {
        m_currentSchedule->logWarning( i18n( "Resource %1 has no calendar defined", m_name ) );
        return; 
    }
    //TODO: units and standard non-working days
    DateTime time = node->startTime;
    DateTime end = node->endTime;
    time = availableAfter(time, end);
    if (!time.isValid()) {
        m_currentSchedule->logWarning( i18n( "Resource %1 not available in interval: %2 to %3", m_name, locale->formatDateTime( node->startTime ), locale->formatDateTime( end ) ) );
        node->resourceNotAvailable = true;
        return;
    }
    end = availableBefore(end, time);
    if (!end.isValid()) {
        m_currentSchedule->logWarning( i18n( "Resource %1 not available in interval: %2 to %3", m_name, locale->formatDateTime( time ), locale->formatDateTime( node->endTime ) ) );
        node->resourceNotAvailable = true;
        return;
    }
    //kDebug()<<time.toString()<<" to"<<end.toString();
    makeAppointment(node, time, end);
}

Duration Resource::effort( const DateTime &start, const Duration &duration, bool backward, bool *ok) const
{
    return effort( m_currentSchedule, start, duration, backward, ok );
}

// the amount of effort we can do within the duration
Duration Resource::effort(Schedule *sch, const DateTime &start, const Duration &duration, bool backward, bool *ok) const {
    //kDebug()<<m_name<<":"<<start<<" for duration"<<duration.toString(Duration::Format_Day);
    bool sts=false;
    Duration e;
    if (duration == 0) {
        kWarning()<<"zero duration";
        return e;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        if ( sch ) sch->logWarning( i18n( "Resource %1 has no calendar defined", m_name ) );
        return e;
    }
    if (backward) {
        DateTime limit = start - duration;
        if ( limit < m_availableFrom ) {
            limit = m_availableFrom;
        }
        DateTime t = availableBefore(start, limit);
        if (t.isValid()) {
            sts = true;
            if ( t < limit && sch ) sch->logDebug( " t < limit: t=" + t.toString() + " limit=" + limit.toString() );
            e = (cal->effort(limit, t, sch) * m_units)/100;
        } else {
            //sch->logDebug( "Resource not available in interval:" + start.toString() + "," + limit.toString() );
        }
    } else {
        DateTime limit = start + duration;
        if ( limit > m_availableUntil ) {
            limit = m_availableUntil;
        }
        DateTime t = availableAfter(start, limit);
        if (t.isValid()) {
            sts = true;
            if ( t > limit && sch ) sch->logDebug( "t > limit: t=" + t.toString() + " limit=" + limit.toString() );
            e = (cal->effort(t, limit, sch) * m_units)/100;
        } else {
            //sch->logDebug( "Resource not available in interval:" + start.toString() + "," + limit.toString() );
        }
    }
    //kDebug()<<start<<" e="<<e.toString(Duration::Format_Day)<<" ("<<m_units<<")";
    if (ok) *ok = sts;
    return e;
}

DateTime Resource::availableAfter(const DateTime &time, const DateTime limit ) const {
    return availableAfter( time, limit, m_currentSchedule );
}

DateTime Resource::availableBefore(const DateTime &time, const DateTime limit) const {
    return availableBefore( time, limit, m_currentSchedule );
}

DateTime Resource::availableAfter(const DateTime &time, const DateTime limit, Schedule *sch) const {
    DateTime t;
    if (m_units == 0) {
        return t;
    }
    DateTime lmt = m_availableUntil;
    if (limit.isValid() && limit < lmt) {
        lmt = limit;
    }
    if (time >= lmt) {
        return t;
    }
    if (type() == Type_Material) {
        t = time > m_availableFrom ? time : m_availableFrom;
        //kDebug()<<time.toString()<<"="<<t.toString()<<""<<m_name;
        return t;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        if ( sch ) sch->logWarning( i18n( "Resource %1 has no calendar defined", m_name ) );
        return t;
    }
    t = m_availableFrom > time ? m_availableFrom : time;
    if ( t >= lmt ) {
        //kDebug()<<t<<lmt;
        return DateTime();
    }
    t = cal->firstAvailableAfter(t, lmt, sch);
    //kDebug()<<m_currentSchedule<<""<<m_name<<" id="<<m_currentSchedule->id()<<" mode="<<m_currentSchedule->calculationMode()<<" returns:"<<time.toString()<<"="<<t.toString()<<""<<lmt.toString();
    return t;
}

DateTime Resource::availableBefore(const DateTime &time, const DateTime limit, Schedule *sch) const {
    DateTime t;
    if (m_units == 0) {
        return t;
    }
    DateTime lmt = m_availableFrom;
    if (limit.isValid() && limit > lmt) {
        lmt = limit;
    }
    if (time <= lmt) {
        return t;
    }
    if (type() == Type_Material) {
        t = time < m_availableUntil ? time : m_availableUntil;
        //kDebug()<<time.toString()<<"="<<t.toString()<<""<<m_name;
        return t;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        return t;
    }
    if (!m_availableUntil.isValid()) {
        sch->logDebug( "availabelUntil is invalid" );
        t = time;
    } else {
        t = m_availableUntil < time ? m_availableUntil : time;
    }
    if ( t < lmt ) sch->logDebug( "t < lmt: " + t.toString() + " < " + lmt.toString() );
    t = cal->firstAvailableBefore(t, lmt, sch );
    if ( t.isValid() && t < lmt ) sch->logDebug( " t < lmt: t=" + t.toString() + " lmt=" + lmt.toString() );
    return t;
}

Resource *Resource::findId(const QString &id) const { 
    return m_project ? m_project->findResource(id) : 0; 
}

bool Resource::removeId(const QString &id) { 
    return m_project ? m_project->removeResourceId(id) : false; 
}

void Resource::insertId(const QString &id) { 
    //kDebug();
    if (m_project)
        m_project->insertResourceId(id, this); 
}

Calendar *Resource::findCalendar(const QString &id) const { 
    return (m_project ? m_project->findCalendar(id) : 0); 
}

bool Resource::isOverbooked() const {
    return isOverbooked(KDateTime(), KDateTime());
}

bool Resource::isOverbooked(const QDate &date) const {
    return isOverbooked(KDateTime(date), KDateTime(date.addDays(1)));
}

bool Resource::isOverbooked(const KDateTime &start, const KDateTime &end) const {
    //kDebug()<<m_name<<":"<<start.toString()<<" -"<<end.toString()<<" cs=("<<m_currentSchedule<<")";
    return m_currentSchedule ? m_currentSchedule->isOverbooked(start, end) : false;
}

Appointment Resource::appointmentIntervals( long id ) const {
    Appointment a;
    Schedule *s = findSchedule( id );
    if ( s == 0 ) {
        return a;
    }
    foreach (Appointment *app, static_cast<ResourceSchedule*>( s )->appointments()) {
        a += *app;
    }
    return a;
}

Appointment Resource::appointmentIntervals() const {
    Appointment a;
    if (m_currentSchedule == 0)
        return a;
    foreach (Appointment *app, m_currentSchedule->appointments()) {
        a += *app;
    }
    return a;
}

Duration Resource::plannedEffort(const QDate &date) const {
    return m_currentSchedule ? m_currentSchedule->plannedEffort(date) : Duration::zeroDuration;
}

void Resource::setProject( Project *project )
{
    if ( project != m_project ) {
        if ( m_project  ) {
            removeId();
        }
    }
    m_project = project;
}

void Resource::addExternalAppointment( const QString &id, const QString &name, const DateTime &from, const DateTime &end, double load )
{
    Appointment *a = m_externalAppointments.value( id );
    if ( a == 0 ) {
        a = new Appointment();
        a->setAuxcilliaryInfo( name );
        a->addInterval( from, end, load );
        //kDebug()<<m_name<<name<<"new appointment:"<<a<<from<<end<<load;
        m_externalAppointments[ id ] = a;
        int row = m_externalAppointments.keys().indexOf( id );
        m_externalAppointments.remove( id );
        emit externalAppointmentToBeAdded( this, row );
        m_externalAppointments[ id ] = a;
        emit externalAppointmentAdded( this, a );
    } else {
        //kDebug()<<m_name<<name<<"new interval:"<<a<<from<<end<<load;
        a->addInterval( from, end, load );
        emit externalAppointmentChanged( this, a );
    }
}

void Resource::clearExternalAppointments()
{
    foreach ( QString id, m_externalAppointments.keys() ) {
        clearExternalAppointments( id );
    }
}

void Resource::clearExternalAppointments( const QString projectId )
{
    while ( m_externalAppointments.contains( projectId ) ) {
        int row = m_externalAppointments.keys().indexOf( projectId );
        emit externalAppointmentToBeRemoved( this, row );
        Appointment *a  = m_externalAppointments.take( projectId );
        emit externalAppointmentRemoved();
        delete a;
    }
}

AppointmentIntervalList Resource::externalAppointments( const QString &id )
{
    if ( ! m_externalAppointments.contains( id ) ) {
        return AppointmentIntervalList();
    }
    return m_externalAppointments[ id ]->intervals();
}

AppointmentIntervalList Resource::externalAppointments() const
{
    //kDebug()<<m_externalAppointments;
    Appointment app;
    foreach ( Appointment *a, m_externalAppointments ) {
        app += *a;
    }
    return app.intervals();
}


/////////   Risk   /////////
Risk::Risk(Node *n, Resource *r, RiskType rt) {
    m_node=n;
    m_resource=r;
    m_riskType=rt;
}

Risk::~Risk() {
}

ResourceRequest::ResourceRequest(Resource *resource, int units)
    : m_resource(resource),
      m_units(units),
      m_parent(0) {
    //kDebug()<<"("<<this<<") Request to:"<<(resource ? resource->name() : QString("None"));
}

ResourceRequest::~ResourceRequest() {
    //kDebug()<<"("<<this<<") resource:"<<(m_resource ? m_resource->name() : QString("None"));
    if (m_resource)
        m_resource->unregisterRequest(this);
    m_resource = 0;
}

bool ResourceRequest::load(KoXmlElement &element, Project &project) {
    //kDebug();
    m_resource = project.resource(element.attribute("resource-id"));
    if (m_resource == 0) {
        kWarning()<<"The referenced resource does not exist: resource id="<<element.attribute("resource-id")<<endl;
        return false;
    }
    m_units  = element.attribute("units").toInt();
    return true;
}

void ResourceRequest::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("resource-request");
    element.appendChild(me);
    me.setAttribute("resource-id", m_resource->id());
    me.setAttribute("units", m_units);
}

int ResourceRequest::units() const {
    //kDebug()<<m_resource->name()<<": units="<<m_units;
    return m_units;
}

int ResourceRequest::workUnits() const {
    if (m_resource->type() == Resource::Type_Work)
        return units();
        
    //kDebug()<<"units=0";
    return 0;
}

Task *ResourceRequest::task() const {
    return m_parent ? m_parent->task() : 0;
}

Schedule *ResourceRequest::resourceSchedule( Schedule *ns )
{
    Resource *r = resource();
    Schedule *s = r->findSchedule(ns->id());
    if (s == 0) {
        s = r->createSchedule(ns->parent());
    }
    s->setCalculationMode( ns->calculationMode() );
    static_cast<ResourceSchedule*>( s )->setNodeSchedule( ns );
    //kDebug()<<s->name()<<": id="<<s->id()<<" mode="<<s->calculationMode();
    return s;
}

DateTime ResourceRequest::workTimeAfter(const DateTime &dt) {
    return m_resource->availableAfter( dt, DateTime(), 0 );
}

DateTime ResourceRequest::workTimeBefore(const DateTime &dt) {
    return m_resource->availableBefore( dt, DateTime(), 0 );
}

DateTime ResourceRequest::availableAfter(const DateTime &time, Schedule *ns) {
    resource()->setCurrentSchedulePtr( resourceSchedule( ns ) );
    return resource()->availableAfter(time);
}

DateTime ResourceRequest::availableBefore(const DateTime &time, Schedule *ns) {
    resource()->setCurrentSchedulePtr( resourceSchedule( ns ) );
    return resource()->availableBefore(time);
}

Duration ResourceRequest::effort( const DateTime &time, const Duration &duration, Schedule *ns, bool backward, bool *ok ) {
    resource()->setCurrentSchedulePtr( resourceSchedule( ns ) );
    return resource()->effort(time, duration, backward, ok);
}

void ResourceRequest::makeAppointment( Schedule *ns )
{
    if ( m_resource ) {
        m_resource->setCurrentSchedulePtr( resourceSchedule( ns ) );
        m_resource->makeAppointment( ns );
    }
}

/////////
ResourceGroupRequest::ResourceGroupRequest(ResourceGroup *group, int units)
    : m_group(group), m_units(units), m_parent(0) {

    //kDebug()<<"Request to:"<<(group ? group->name() : QString("None"));
    if (group)
        group->registerRequest(this);
}

ResourceGroupRequest::~ResourceGroupRequest() {
    //kDebug();
    if (m_group)
        m_group->unregisterRequest(this);

    while (!m_resourceRequests.isEmpty()) {
        delete m_resourceRequests.takeFirst();
    }
}

void ResourceGroupRequest::addResourceRequest(ResourceRequest *request) {
    //kDebug()<<"("<<request<<") to Group:"<<(void*)m_group;
    request->setParent(this);
    m_resourceRequests.append(request);
    request->registerRequest();
    changed();
}

ResourceRequest *ResourceGroupRequest::takeResourceRequest(ResourceRequest *request) {
    if (request)
        request->unregisterRequest();
    ResourceRequest *r = 0;
    int i = m_resourceRequests.indexOf(request);
    if (i != -1) {
        r = m_resourceRequests.takeAt(i);
    }
    changed();
    return r;
}

ResourceRequest *ResourceGroupRequest::find(Resource *resource) {
    foreach (ResourceRequest *gr, m_resourceRequests) {
        if (gr->resource() == resource) {
            return gr;
        }
    }
    return 0;
}

ResourceRequest *ResourceGroupRequest::resourceRequest( const QString &name ) {
    foreach (ResourceRequest *r, m_resourceRequests) {
        if (r->resource()->name() == name )
            return r;
    }
    return 0;
}

QStringList ResourceGroupRequest::requestNameList() const {
    QStringList lst;
    if ( m_units > 0 && m_group ) {
        lst << m_group->name();
    }
    foreach ( ResourceRequest *r, m_resourceRequests ) {
        if ( r->resource() ) {
            lst << r->resource()->name();
        }
    }
    return lst;
}

bool ResourceGroupRequest::load(KoXmlElement &element, Project &project) {
    //kDebug();
    m_group = project.findResourceGroup(element.attribute("group-id"));
    if (m_group == 0) {
        kError()<<"The referenced resource group does not exist: group id="<<element.attribute("group-id");
        return false;
    }
    m_group->registerRequest(this);
    
    m_units  = element.attribute("units").toInt();

    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "resource-request") {
            ResourceRequest *r = new ResourceRequest();
            if (r->load(e, project))
                addResourceRequest(r);
            else {
                kError()<<"Failed to load resource request"<<endl;
                delete r;
            }
        }
    }
    return true;
}

void ResourceGroupRequest::save(QDomElement &element) const {
    if (units() == 0)
        return;
    QDomElement me = element.ownerDocument().createElement("resourcegroup-request");
    element.appendChild(me);
    me.setAttribute("group-id", m_group->id());
    me.setAttribute("units", m_units);
    foreach (ResourceRequest *r, m_resourceRequests)
        r->save(me);
}

int ResourceGroupRequest::units() const {
    int units = m_units;
    foreach (ResourceRequest *r, m_resourceRequests) {
        units += r->units();
    }
    //kDebug()<<"units="<<units;
    return units;
}

int ResourceGroupRequest::workUnits() const {
    int units = 0;
    if (m_group->type() == ResourceGroup::Type_Work)
        units = m_units;
    foreach (ResourceRequest *r, m_resourceRequests) {
        units += r->workUnits();
    }
    //kDebug()<<"units="<<units;
    return units;
}

//TODO: handle nonspecific resources
Duration ResourceGroupRequest::effort(const DateTime &time, const Duration &duration, Schedule *ns, bool backward, bool *ok) const {
    Duration e;
    bool sts=false;
    if (ok) *ok = sts;
    foreach (ResourceRequest *r, m_resourceRequests) {
        e += r->effort(time, duration, ns, backward, &sts);
        if (sts && ok) *ok = sts;
        //kDebug()<<(backward?"(B)":"(F)" )<<it.current()->resource()->name()<<": time="<<time<<" dur="<<duration.toString()<<"gave e="<<e.toString();
    }
    //kDebug()<<time.toString()<<"d="<<duration.toString()<<": e="<<e.toString();
    return e;
}

int ResourceGroupRequest::numDays(const DateTime &time, bool backward) const {
    DateTime t1, t2 = time;
    if (backward) {
        foreach (ResourceRequest *r, m_resourceRequests) {
            t1 = r->resource()->availableFrom();
            if (!t2.isValid() || t2 > t1)
                t2 = t1;
        }
        //kDebug()<<"bw"<<time.toString()<<":"<<t2.daysTo(time);
        return t2.daysTo(time);
    }
    foreach (ResourceRequest *r, m_resourceRequests) {
        t1 = r->resource()->availableUntil();
        if (!t2.isValid() || t2 < t1)
            t2 = t1;
    }
    //kDebug()<<"fw"<<time.toString()<<":"<<time.daysTo(t2);
    return time.daysTo(t2);
}

Duration ResourceGroupRequest::duration(const DateTime &time, const Duration &_effort, Schedule *ns, bool backward) {
    //kDebug()<<"--->"<<(backward?"(B)":"(F)")<<m_group->name()<<""<<time.toString()<<": effort:"<<_effort.toString(Duration::Format_Day)<<" ("<<_effort.milliseconds()<<")";
    KLocale *locale = KGlobal::locale();
    Duration e;
    if (_effort == Duration::zeroDuration) {
        return e;
    }
    DateTime logtime = time;
    bool sts=true;
    bool match = false;
    DateTime start = time;
    int inc = backward ? -1 : 1;
    DateTime end = start;
    Duration e1;
    Duration d(1, 0, 0); // 1 day
    int nDays = numDays(time, backward) + 1;
    for (int i=0; !match && i <= nDays; ++i) {
        // days
        end = end.addDays(inc);
        e1 = effort(start, d, ns, backward, &sts);
        //kDebug()<<"["<<i<<"of"<<nDays<<"]"<<(backward?"(B)":"(F):")<<"  start="<<start<<" e+e1="<<(e+e1).toString()<<" match"<<_effort.toString();
        if (e + e1 < _effort) {
            e += e1;
            start = end;
        } else if (e + e1 == _effort) {
            e += e1;
            match = true;
        } else {
            end = start;
            break;
        }
    }
    if ( ! match ) {
        ns->logDebug( "Days: duration " + logtime.toString() + " - " + end.toString() + " e=" + e.toString() + " (" + (_effort - e).toString() + ')' );
        
        logtime = start;
        d = Duration(0, 1, 0); // 1 hour
        for (int i=0; !match && i < 24; ++i) {
            // hours
            end = end.addSecs(inc*60*60);
            e1 = effort(start, d, ns, backward, &sts);
            if (e + e1 < _effort) {
                e += e1;
                start = end;
            } else if (e + e1 == _effort) {
                e += e1;
                match = true;
            } else {
                end = start;
                break;
            }
            //kDebug()<<"duration(h)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")";
        }
        //kDebug()<<"duration"<<(backward?"backward":"forward:")<<start.toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")  match="<<match<<" sts="<<sts;
    }
    if ( ! match ) {
        ns->logDebug( "Hours: duration " + logtime.toString() + " - " + end.toString() + " e=" + e.toString() + " (" + (_effort - e).toString() + ')' );
        
        logtime = start;
        d = Duration(0, 0, 1); // 1 minute
        for (int i=0; !match && i < 60; ++i) {
            //minutes
            end = end.addSecs(inc*60);
            e1 = effort(start, d, ns, backward, &sts);
            if (e + e1 < _effort) {
                e += e1;
                start = end;
            } else if (e + e1 == _effort) {
                e += e1;
                match = true;
            } else if (e + e1 > _effort) {
                end = start;
                break;
            }
            //kDebug()<<"duration(m)"<<(backward?"backward":"forward:")<<"  time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")";
        }
        //kDebug()<<"duration"<<(backward?"backward":"forward:")<<"  start="<<start.toString()<<" e="<<e.toString()<<" match="<<match<<" sts="<<sts;
    }
    if ( ! match ) {
        ns->logDebug( "Minutes: duration " + logtime.toString() + " - " + end.toString() + " e=" + e.toString() + " (" + (_effort - e).toString() + ')' );
        
        logtime = start;
        d = Duration(0, 0, 0, 1); // 1 second
        for (int i=0; !match && i < 60 && sts; ++i) {
            //seconds
            end = end.addSecs(inc);
            e1 = effort(start, d, ns, backward, &sts);
            if (e + e1 < _effort) {
                e += e1;
                start = end;
            } else if (e + e1 == _effort) {
                e += e1;
                match = true;
            } else if (e + e1 > _effort) {
                end = start;
                break;
            }
            //kDebug()<<"duration(s)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")";
        }
    }
    if ( ! match ) {
        ns->logDebug( "Seconds: duration " + logtime.toString() + " - " + end.toString() + " e=" + e.toString() + " (" + (_effort - e).toString() + ')' );
        
        d = Duration(0, 0, 0, 0, 1); // 1 millisecond
        for (int i=0; !match && i < 1000; ++i) {
            //milliseconds
            end.setTime(end.time().addMSecs(inc));
            e1 = effort(start, d, ns, backward, &sts);
            if (e + e1 < _effort) {
                e += e1;
                start = end;
            } else if (e + e1 == _effort) {
                e += e1;
                match = true;
            } else if (e + e1 > _effort) {
                break;
            }
            //kDebug()<<"duration(ms)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")";
        }
    }
    if (!match) {
        ns->logError( i18n( "Could not match effort. Want: %1 got: %2", _effort.toString( Duration::Format_Hour ), e.toString( Duration::Format_Hour ) ) );
        foreach (ResourceRequest *r, m_resourceRequests) {
            Resource *res = r->resource();
            ns->logInfo( i18n( "Resource %1 available from %2 to %3", res->name(), locale->formatDateTime( res->availableFrom() ), locale->formatDateTime( res->availableUntil() ) ) );
        }

    }
    DateTime t;
    if (e != Duration::zeroDuration) {
        t = backward ? availableAfter(end, ns) : availableBefore(end, ns);
    }
    end = t.isValid() ? t : time;
    //kDebug()<<"<---"<<(backward?"(B)":"(F)")<<m_group->name()<<":"<<end.toString()<<"-"<<time.toString()<<"="<<(end - time).toString()<<" effort:"<<_effort.toString(Duration::Format_Day);
    return (end>time?end-time:time-end);
}

DateTime ResourceGroupRequest::workTimeAfter(const DateTime &time) {
    DateTime start;
    foreach (ResourceRequest *r, m_resourceRequests) {
        DateTime t = r->workTimeAfter( time );
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kDebug()<<time.toString()<<"="<<start.toString();
    return start;
}

DateTime ResourceGroupRequest::workTimeBefore(const DateTime &time) {
    DateTime end;
    foreach (ResourceRequest *r, m_resourceRequests) {
        DateTime t = r->workTimeBefore( time );
        if (t.isValid() && (!end.isValid() ||t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}

DateTime ResourceGroupRequest::availableAfter(const DateTime &time, Schedule *ns) {
    DateTime start;
    foreach (ResourceRequest *r, m_resourceRequests) {
        DateTime t = r->availableAfter(time, ns);
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kDebug()<<time.toString()<<"="<<start.toString()<<""<<m_group->name();
    return start;
}

DateTime ResourceGroupRequest::availableBefore(const DateTime &time, Schedule *ns) {
    DateTime end;
    foreach (ResourceRequest *r, m_resourceRequests) {
        DateTime t = r->availableBefore(time, ns);
        if (t.isValid() && (!end.isValid() || t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    //kDebug()<<time.toString()<<"="<<end.toString()<<""<<m_group->name();
    return end;
}

void ResourceGroupRequest::makeAppointments(Schedule *schedule) {
    //kDebug();
    foreach (ResourceRequest *r, m_resourceRequests) {
        r->makeAppointment(schedule);
    }
}

void ResourceGroupRequest::reserve(const DateTime &start, const Duration &duration) {
    m_start = start;
    m_duration = duration;
}

bool ResourceGroupRequest::isEmpty() const {
    return m_resourceRequests.isEmpty() && m_units == 0;
}

Task *ResourceGroupRequest::task() const {
    return m_parent ? &(m_parent->task()) : 0;
}

void ResourceGroupRequest::changed()
{
     if ( m_parent ) 
         m_parent->changed();
}

/////////
ResourceRequestCollection::ResourceRequestCollection(Task &task)
    : m_task(task) {
    //kDebug()<<this<<(void*)(&task);
}

ResourceRequestCollection::~ResourceRequestCollection() {
    //kDebug()<<this;
    while (!m_requests.empty()) {
        delete m_requests.takeFirst();
    }
}

void ResourceRequestCollection::addRequest( ResourceGroupRequest *request )
{
    foreach ( ResourceGroupRequest *r, m_requests ) {
        if ( r->group() == request->group() ) {
            kError()<<"Request to this group already exists";
            kError()<<"Task:"<<m_task.name()<<"Group:"<<request->group()->name();
            Q_ASSERT( r->group() != request->group() );
        }
    }
    m_requests.append( request );
    request->setParent( this );
    changed();
}

ResourceGroupRequest *ResourceRequestCollection::find(ResourceGroup *group) const {
    foreach (ResourceGroupRequest *r, m_requests) {
        if (r->group() == group)
            return r; // we assume only one request to the same group
    }
    return 0;
}


ResourceRequest *ResourceRequestCollection::find(Resource *resource) const {
    ResourceRequest *req = 0;
    QListIterator<ResourceGroupRequest*> it(m_requests);
    while (req == 0 && it.hasNext()) {
        req = it.next()->find(resource);
    }
    return req;
}

ResourceRequest *ResourceRequestCollection::resourceRequest( const QString &name ) const {
    ResourceRequest *req = 0;
    QListIterator<ResourceGroupRequest*> it(m_requests);
    while (req == 0 && it.hasNext()) {
        req = it.next()->resourceRequest( name );
    }
    return req;
}

QStringList ResourceRequestCollection::requestNameList() const {
    QStringList lst;
    foreach ( ResourceGroupRequest *r, m_requests ) {
        lst << r->requestNameList();
    }
    return lst;
}

QList<Resource*> ResourceRequestCollection::requestedResources() const {
    QList<Resource*> lst;
    foreach ( ResourceGroupRequest *g, m_requests ) {
        foreach ( ResourceRequest *r, g->resourceRequests() ) {
            lst << r->resource();
        }
    }
    return lst;
}


bool ResourceRequestCollection::contains( const QString &identity ) const {
    QStringList lst = requestNameList();
    return lst.indexOf( QRegExp( identity, Qt::CaseSensitive, QRegExp::FixedString ) ) != -1;
}

ResourceGroupRequest *ResourceRequestCollection::findGroupRequestById( const QString &id ) const
{
    foreach ( ResourceGroupRequest *r, m_requests ) {
        if ( r->group()->id() == id ) {
            return r;
        }
    }
    return 0;
}

// bool ResourceRequestCollection::load(KoXmlElement &element, Project &project) {
//     //kDebug();
//     return true;
// }

void ResourceRequestCollection::save(QDomElement &element) const {
    //kDebug();
    foreach (ResourceGroupRequest *r, m_requests) {
        r->save(element);
    }
}

int ResourceRequestCollection::units() const {
    //kDebug();
    int units = 0;
    foreach (ResourceGroupRequest *r, m_requests) {
        units += r->units();
        //kDebug()<<" Group:"<<r->group()->name()<<" now="<<units;
    }
    return units;
}

int ResourceRequestCollection::workUnits() const {
    //kDebug();
    int units = 0;
    foreach (ResourceGroupRequest *r, m_requests) {
        units += r->workUnits();
    }
    //kDebug()<<" units="<<units;
    return units;
}

// Returns the longest duration needed by any of the groups.
// The effort is distributed on "work type" resourcegroups in proportion to
// the amount of resources requested for each group.
// "Material type" of resourcegroups does not (atm) affect the duration.
Duration ResourceRequestCollection::duration(const DateTime &time, const Duration &effort, Schedule *ns, bool backward) {
    //kDebug()<<"time="<<time.toString()<<" effort="<<effort.toString(Duration::Format_Day)<<" backward="<<backward;
    if (isEmpty()) {
        return effort;
    }
    Duration dur;
    int units = workUnits();
    if (units == 0)
        units = 100; //hmmmm
    foreach (ResourceGroupRequest *r, m_requests) {
        if (r->isEmpty())
            continue;
        if (r->group()->type() == ResourceGroup::Type_Work) {
            Duration d = r->duration(time, (effort*r->workUnits())/units, ns, backward);
            if (d > dur)
                dur = d;
        } else if (r->group()->type() == ResourceGroup::Type_Material) {
            //TODO
            if (dur == Duration::zeroDuration)
                dur = effort;
        }
    }
    return dur;
}

DateTime ResourceRequestCollection::workTimeAfter(const DateTime &time) {
    DateTime start;
    foreach (ResourceGroupRequest *r, m_requests) {
        DateTime t = r->workTimeAfter( time );
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kDebug()<<time.toString()<<"="<<start.toString();
    return start;
}

DateTime ResourceRequestCollection::workTimeBefore(const DateTime &time) {
    DateTime end;
    foreach (ResourceGroupRequest *r, m_requests) {
        DateTime t = r->workTimeBefore( time );
        if (t.isValid() && (!end.isValid() ||t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}

DateTime ResourceRequestCollection::availableAfter(const DateTime &time, Schedule *ns) {
    DateTime start;
    foreach (ResourceGroupRequest *r, m_requests) {
        DateTime t = r->availableAfter(time, ns);
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kDebug()<<time.toString()<<"="<<start.toString();
    return start;
}

DateTime ResourceRequestCollection::availableBefore(const DateTime &time, Schedule *ns) {
    DateTime end;
    foreach (ResourceGroupRequest *r, m_requests) {
        DateTime t = r->availableBefore(time, ns);
        if (t.isValid() && (!end.isValid() ||t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}


void ResourceRequestCollection::makeAppointments(Schedule *schedule) {
    //kDebug();
    foreach (ResourceGroupRequest *r, m_requests) {
        r->makeAppointments(schedule);
    }
}

void ResourceRequestCollection::reserve(const DateTime &start, const Duration &duration) {
    //kDebug();
    foreach (ResourceGroupRequest *r, m_requests) {
        r->reserve(start, duration);
    }
}

bool ResourceRequestCollection::isEmpty() const {
    foreach (ResourceGroupRequest *r, m_requests) {
        if (!r->isEmpty())
            return false;
    }
    return true;
}

void ResourceRequestCollection::changed()
{
    //kDebug()<<(void*)(&m_task);
    m_task.changed();
}

#ifndef NDEBUG

void ResourceGroup::printDebug(const QString& _indent)
{
    QString indent = _indent;
    kDebug()<<indent<<"  + Resource group:"<<m_name<<" id="<<m_id;
    indent += "   !";
    foreach (Resource *r, m_resources)
        r->printDebug(indent);
}
void Resource::printDebug(const QString& _indent)
{
    QString indent = _indent;
    kDebug()<<indent<<"  + Resource:"<<m_name<<" id="<<m_id/*<<" Overbooked="<<isOverbooked()*/;
    indent += "      ";
    foreach (ResourceRequest *r, m_requests) {
        r->printDebug(indent);
    }
    foreach (Schedule *s, m_schedules) {
        s->printDebug(indent);
    }
    kDebug()<<indent<<"  + External appointments:"<<m_externalAppointments.count();
    foreach ( Appointment *a, m_externalAppointments ) {
        kDebug()<<indent<<"  ! Appointment to:"<<a->auxcilliaryInfo();
        a->printDebug(indent);
    }
    indent += "  !";
}

void ResourceGroupRequest::printDebug(const QString& _indent)
{
    QString indent = _indent;
    kDebug()<<indent<<"  + Request to group:"<<(m_group ? m_group->name() :"None")<<" units="<<m_units<<"%";
    indent += "  !";
    
    foreach (ResourceRequest *r, m_resourceRequests) {
        r->printDebug(indent);
    }
}

void ResourceRequest::printDebug(const QString& indent)
{
    kDebug()<<indent<<"  + Request to resource:"<<(m_resource ? m_resource->name() :"None")<<" units="<<m_units<<"%";
}

void ResourceRequestCollection::printDebug(const QString& indent)
{
    kDebug()<<indent<<"  + Resource requests:";
    foreach (ResourceGroupRequest *r, m_requests) {
        r->printDebug(indent+"  ");
    }
}

#endif

}  //KPlato namespace

#include "kptresource.moc"
