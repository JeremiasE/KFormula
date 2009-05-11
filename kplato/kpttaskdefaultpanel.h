/* This file is part of the KDE project
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

#ifndef KPTTASKDEFAULTPANEL_H
#define KPTTASKDEFAULTPANEL_H

#include "kplato_export.h"

#include "ui_kptconfigtaskpanelbase.h"
#include "kptduration.h"


namespace KPlato
{

class DateTime;
class Task;
class StandardWorktime;
class MacroCommand;

class ConfigTaskPanelImpl : public QWidget, public Ui_ConfigTaskPanelBase
{
    Q_OBJECT
public:
    ConfigTaskPanelImpl(QWidget *parent, const char *name);
    
    virtual int schedulingType() const;
    virtual int estimationType() const;
    virtual int optimistic() const;
    virtual int pessimistic();
    virtual double estimationValue();
    virtual QDateTime startDateTime();
    virtual QDateTime endDateTime();
    virtual QTime startTime() const;
    virtual QTime endTime();
    virtual QDate startDate();
    virtual QDate endDate();

public slots:
    virtual void setSchedulingType( int type );
    virtual void changeLeader();
    virtual void setEstimationType( int type );
    virtual void setOptimistic( int value );
    virtual void setPessimistic( int value );
    virtual void enableDateTime( int scheduleType );
    virtual void estimationTypeChanged( int type );
    virtual void setEstimate( double duration );
    virtual void setEstimateType( int type );
    virtual void checkAllFieldsFilled();
//    virtual void setEstimateScales( double day );
    virtual void startDateChanged();
    virtual void startTimeChanged( const QTime & time );
    virtual void endDateChanged();
    virtual void endTimeChanged( const QTime & time );
    virtual void scheduleTypeChanged( int value );
    virtual void setStartTime( const QTime & time );
    virtual void setEndTime( const QTime & time );
    virtual void setStartDateTime( const QDateTime & dt );
    virtual void setEndDateTime( const QDateTime & dt );
    virtual void setStartDate( const QDate & date );
    virtual void setEndDate( const QDate & date );

signals:
    void obligatedFieldsFilled( bool );
    void schedulingTypeChanged( int );
    void changed();

protected:
    bool useTime;
};

class KPLATO_EXPORT TaskDefaultPanel : public ConfigTaskPanelImpl {
    Q_OBJECT
public:
    explicit TaskDefaultPanel(Task &task, QWidget *parent=0, const char *name=0);

    MacroCommand *buildCommand();

    bool ok();

    void setStartValues(Task &task);

public slots:
    virtual void estimationTypeChanged(int type);
    virtual void scheduleTypeChanged(int value);
    
private:
    Task &m_task;
    
    Duration m_estimate;
    Duration m_duration;
};

} //KPlato namespace

#endif // TASKDEFAULTPANEL_H
