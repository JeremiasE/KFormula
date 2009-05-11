/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_DYNAMIC_PROGRAMS_EDITOR_H_
#define _KIS_DYNAMIC_PROGRAMS_EDITOR_H_

#include <kdialog.h>

class QVBoxLayout;
class Ui_DynamicProgramsEditor;
class KisBookmarkedConfigurationManager;
class KisBookmarkedConfigurationsModel;
class KisDynamicProgram;
class KisDynamicProgramFactoryRegistry;

class KisDynamicProgramsEditor : public KDialog
{
    Q_OBJECT
public:
    KisDynamicProgramsEditor(QWidget* parent, KisBookmarkedConfigurationsModel* bookmarksManager, const KisDynamicProgramFactoryRegistry* factoryRegistry);
    ~KisDynamicProgramsEditor();
public slots:
    void setCurrentProgram(const QString& text);
    void addProgram();
    void deleteProgram();
    void saveCurrentProgram();
private:
    Ui_DynamicProgramsEditor* m_dynamicProgramsEditor;
    QWidget* m_currentEditor;
    QVBoxLayout *m_frameVBoxLayout;
    KisBookmarkedConfigurationManager* m_bookmarksManager;
    KisBookmarkedConfigurationsModel* m_bookmarksModel;
    KisDynamicProgram* m_currentProgram;
    const KisDynamicProgramFactoryRegistry* m_factoryRegistry;
};

#endif
