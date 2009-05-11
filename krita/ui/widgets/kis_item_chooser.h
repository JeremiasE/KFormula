/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
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
#ifndef KIS_ITEM_CHOOSER_H_
#define KIS_ITEM_CHOOSER_H_

#include <QList>
#include <QWidget>
#include <kvbox.h>
#include <krita_export.h>

class KHBox;

class KoResourceItemChooser;
class KoResourceItem;
class QTableWidgetItem;

/**
 * A special type of item chooser that can contain extra widgets that show
 * more information about the currently selected item. Reimplement update()
 * to extract that information and fill the appropriate widgets.
 */
class KRITAUI_EXPORT KisItemChooser : public QWidget
{

    Q_OBJECT

public:

    KisItemChooser(QWidget *parent = 0, const char *name = 0);
    virtual ~KisItemChooser();

    QTableWidgetItem *currentItem();
    void setCurrent(QTableWidgetItem *item);
    void setCurrent(int index);

    void removeItem( KoResourceItem *item );
    void selectItem( QTableWidgetItem* item );

public slots:

    void addItem(KoResourceItem *item);
    void addItems(const QList<KoResourceItem *>& items);

signals:

    void selected(QTableWidgetItem *item);
    void importClicked();
    void deleteClicked();
    void update( QTableWidgetItem* );

protected:

    QWidget *chooserWidget() const;

private slots:
    void slotItemSelected(QTableWidgetItem *item);

private:
    KHBox *m_frame;
    KoResourceItemChooser *m_chooser;
};

#endif // KIS_ITEM_CHOOSER_H_

