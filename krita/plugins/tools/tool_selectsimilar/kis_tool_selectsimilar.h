/*
 *  Copyright (c) 2004 Boudewijn Rempt (boud@valdyas.org)
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
#ifndef KIS_TOOL_SELECT_SIMILAR_H_
#define KIS_TOOL_SELECT_SIMILAR_H_

#include <QtGui/QPainter>
#include <kis_tool.h>
#include <KoToolFactory.h>
#include <kis_selection.h>
#include "flake/kis_node_shape.h"

class QWidget;
class KoCanvasBase;

/**
 * Tool to select colors by pointing at a color on the image.
 * TODO:
 *       Implement shift/shift-ctrl keyboard shortcuts for
 *       temporary add/subtract selection mode.
 */

class KisSelectionOptions;

class KisToolSelectSimilar : public KisTool
{

    Q_OBJECT

public:
    KisToolSelectSimilar(KoCanvasBase * canvas);
    virtual ~KisToolSelectSimilar();

    virtual void paint(QPainter& gc, const KoViewConverter &converter) {
        Q_UNUSED(gc);
        Q_UNUSED(converter);
    }
    virtual void mousePressEvent(KoPointerEvent *e);

public slots:

    void activate(bool);
    void deactivate();

    virtual void slotSetFuzziness(int);
    virtual void slotSetAction(int);

private:
    virtual QWidget* createOptionWidget();
    virtual QWidget* optionWidget();

    void setPickerCursor(selectionAction);

    QWidget *m_optWidget;
    KisSelectionOptions *m_selectionOptionsWidget;

    int m_fuzziness;
    selectionAction m_defaultSelectAction;
    selectionAction m_currentSelectAction;
    QTimer *m_timer;
    QCursor m_addCursor;
    QCursor m_subtractCursor;

private slots:
    void slotTimer();
};

class KisToolSelectSimilarFactory : public KoToolFactory
{

public:
    KisToolSelectSimilarFactory(QObject *parent, const QStringList&)
            : KoToolFactory(parent, "KisToolSelectSimilar", i18n("Select similar colors")) {
        setToolTip(i18n("Select similar colors"));
        setToolType(TOOL_TYPE_SELECTED);
        //setActivationShapeId( KIS_NODE_SHAPE_ID );
        setIcon("tool_similar_selection");
        setShortcut(KShortcut(Qt::CTRL + Qt::Key_E));
        setPriority(57);
    }

    virtual ~KisToolSelectSimilarFactory() {}

    virtual KoTool * createTool(KoCanvasBase *canvas) {
        return  new KisToolSelectSimilar(canvas);
    }

};


#endif // KIS_TOOL_SELECT_PICKER_H_

