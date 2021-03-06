/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2008
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

#include "widgets/kis_preset_widget.h"

#include <QPainter>
#include <QIcon>
#include <QWidget>

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include <kis_paintop_preset.h>

KisPresetWidget::KisPresetWidget(QWidget *parent, const char *name)
        : KisPopupButton(parent)
{
    setObjectName(name);
    m_preset = 0;
}

void KisPresetWidget::setPreset(KisPaintOpPresetSP preset)
{
    Q_ASSERT(preset);
    m_preset = preset.data();
    m_preset->updateImg();
    update();
}

void KisPresetWidget::updatePreview()
{
    if (m_preset) {
        m_preset->updateImg();
        update();
    }
}

void KisPresetWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    qint32 cw = width() - 1;
    qint32 ch = height() - 1;
    p.fillRect(0, 0, cw, ch, Qt::white);  // XXX: use a palette for this instead of white?

    if (m_preset) {
        p.drawImage(1, 1, m_preset->img().scaled(QSize(cw, ch), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    p.setPen(Qt::gray);
    p.drawRect(0, 0, cw + 1, ch + 1);
}


#include "kis_preset_widget.moc"
