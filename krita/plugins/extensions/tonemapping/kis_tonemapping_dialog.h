/*
 *  Copyright (c) 2007 Cyrille Berger (cberger@cberger.net)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_TONEMAPPING_DIALOG_H_
#define _KIS_TONEMAPPING_DIALOG_H_

#include <QDialog>
#include <kis_types.h>

class Ui_WdgToneMappingDialog;

class KisToneMappingDialog : public QDialog
{
    Q_OBJECT
public:
    KisToneMappingDialog(QWidget* parent, KisLayerSP layer);
protected slots:
    void slotBookmarkedToneMappingConfigurationSelected(int);
    void slotOperatorSelected(int);
    void apply();
    void editConfigurations();
private:
    struct Private;
    Private* const d;
};

#endif
