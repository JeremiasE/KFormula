/* This file is part of the KDE project

   Copyright (C) 2007 Emanuele Tamponi <emanuele@valinor.it>

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

#ifndef KIS_PAINTERLY_MIXER_DOCKER_H_
#define KIS_PAINTERLY_MIXER_DOCKER_H_

#include <QDockWidget>
#include <KoDockFactory.h>

#include "kis_view2.h"

class KisPainterlyMixer;

class KisPainterlyMixerDocker : public QDockWidget
{
    Q_OBJECT

public:
    KisPainterlyMixerDocker(KisView2 *view);
    virtual ~KisPainterlyMixerDocker();

private:
    KisPainterlyMixer *m_painterlyMixer;
};


class KisPainterlyMixerDockerFactory : public KoDockFactory
{
public:
    KisPainterlyMixerDockerFactory(KisView2 *view) : m_view(view) {}
    ~KisPainterlyMixerDockerFactory() {}

    QString id() const;
    QDockWidget *createDockWidget();
    DockPosition defaultDockPosition() const {
        return DockMinimized;
    }

private:
    KisView2 *m_view;
};

#endif // KIS_PAINTERLY_MIXER_DOCKER_H_
