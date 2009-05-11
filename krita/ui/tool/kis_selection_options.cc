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

#include "kis_selection_options.h"

#include <QWidget>
#include <QRadioButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLayout>


#include "kis_types.h"
#include "kis_layer.h"
#include "kis_image.h"
#include "kis_selection.h"
#include "kis_paint_device.h"
#include "canvas/kis_canvas2.h"
#include "kis_view2.h"

KisSelectionOptions::KisSelectionOptions(KisCanvas2 * canvas)
        : m_canvas(canvas)
{
    m_page = new WdgSelectionOptions(this);
    Q_CHECK_PTR(m_page);

    QVBoxLayout * l = new QVBoxLayout(this);
    l->addWidget(m_page);

    connect(m_page->cmbMode, SIGNAL(activated(int)), this, SIGNAL(modeChanged(int)));
    connect(m_page->cmbAction, SIGNAL(activated(int)), this, SIGNAL(actionChanged(int)));
}

KisSelectionOptions::~KisSelectionOptions()
{
}

int KisSelectionOptions::action()
{
    return m_page->cmbAction->currentIndex();
}

void KisSelectionOptions::slotActivated()
{
    // XXX_SELECTION
}

bool KisSelectionOptions::antiAliasSelection()
{
    return m_page->chkAntiAliasing->isChecked();
}

void KisSelectionOptions::disableAntiAliasSelectionOption()
{
    m_page->chkAntiAliasing->setDisabled(true);
}

void KisSelectionOptions::disableSelectionModeOption()
{
    m_page->cmbMode->setDisabled(true);
}

#include "kis_selection_options.moc"
