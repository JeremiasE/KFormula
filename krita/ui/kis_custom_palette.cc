/*
 *  Copyright (c) 2005 Bart Coppens <kde@bartcoppens.be>
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

#include "kis_custom_palette.h"

#include <KoImageResource.h>
#include <QLineEdit>
#include <QImage>
#include <QPushButton>
#include <QRegExp>
#include <QValidator>
#include <QMap>
#include <QListWidgetItem>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>
#include <kcolordialog.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kis_debug.h>

#include "kis_view2.h"
#include "KoColorSet.h"
#include "KoResourceServerProvider.h"


KisCustomPalette::KisCustomPalette(QList<KoColorSet*> &palettes, QWidget *parent, const char* name, const QString& caption, KisView2* view)
        : KisWdgCustomPalette(parent, name), m_view(view)
{
    Q_ASSERT(m_view);
    m_palette = 0;
    m_editMode = false;
    setWindowTitle(caption);

    foreach(KoColorSet *palette, palettes) {
        QListWidgetItem* item = new QListWidgetItem(palette->name());
        paletteList->addItem(item);
        m_palettes[item] = palette;
    }
    if (paletteList->count() > 0) {
        paletteList->setCurrentRow(0);
        setPalette(m_palettes.value(paletteList->currentItem()));
    }

    KoResourceServer<KoColorSet>* rServer = KoResourceServerProvider::instance()->paletteServer();
    m_rServerAdapter = new KoResourceServerAdapter<KoColorSet>(rServer);

    connect(paletteList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(slotPaletteChanged(QListWidgetItem*, QListWidgetItem*)));
    connect(addColor, SIGNAL(pressed()), this, SLOT(slotAddNew()));
    connect(removeColor, SIGNAL(pressed()), this, SLOT(slotRemoveCurrent()));
    //connect(addPalette, SIGNAL(pressed()), this, SLOT(slotAddPredefined()));
}

KisCustomPalette::~KisCustomPalette()
{
    delete m_rServerAdapter;
}

void KisCustomPalette::setPalette(KoColorSet* p)
{
    m_palette = p;
    //view->setPalette(m_palette);
    palettename->setText(p->name());
}

void KisCustomPalette::slotAddNew()
{
    // Let the user select a new color
    // FIXME also let him add the current paint color to the palette
    // or even better, let the color picker have an option 'Add to
    // palette'!
#ifdef __GNUC__
#warning "KisCustomPalette::slotAddNew: re-implement adding of new colors to palettes"
#endif

    /*
        KoColorSetEntry entry;

        m_palette->add(entry);

        // Just reload the palette completely for the view updating
        view->setPalette(m_palette);

        if (!m_palette->save())
            KMessageBox::error(0, i18n("Cannot write to palette file %1. Maybe it is read-only.",m_palette->filename()), i18n("Palette"));
    */
}

void KisCustomPalette::slotRemoveCurrent()
{
    if (m_palette) {
        // Just reload the palette completely for the view updating
//        view->setPalette(m_palette);

        if (!m_palette->save())
            KMessageBox::error(0, i18n("Cannot write to palette file %1. Maybe it is read-only.", m_palette->filename()), i18n("Palette"));
    }
}

void KisCustomPalette::slotAddPalette()
{

    bool ok;
    QRegExpValidator validator(QRegExp("\\S+"), this);
    QString name = KInputDialog::getText(i18n("Add New Palette"),
                                         i18n("Palette name:"),
                                         i18n("Unnamed"), &ok,
                                         0, &validator);

    if (!ok)
        return;

    KoColorSet* palette = new KoColorSet();
    palette->setName(name);

    // Save in the directory that is likely to be: ~/.kde/share/apps/krita/palettes
    // a unique file with this palettename
    QString dir = KGlobal::dirs()->saveLocation("data", "krita/palettes");
    QString extension;

    extension = ".gpl";
    QString tempFileName;
    {
        KTemporaryFile file;
        file.setPrefix(dir);
        file.setSuffix(extension);
        file.setAutoRemove(false);
        file.open();
        tempFileName = file.fileName();
    }
    // Save it to that file
    palette->setFilename(tempFileName);

    if (!palette->save()) {
        KMessageBox::error(0, i18n("Cannot write to palette file %1. Maybe it is read-only.", m_palette->filename()), i18n("Palette"));
        return;
    }

    // Add it to the palette server, so that it automatically gets to the mediators, and
    // so to the other choosers can pick it up, if they want to
    // This probably leaks!
    if (m_rServerAdapter)
        m_rServerAdapter->addResource(palette);

    QListWidgetItem* item = new QListWidgetItem(palette->name());
    paletteList->addItem(item);
    m_palettes[item] = palette;
    paletteList->setCurrentItem(item);
}

void KisCustomPalette::slotPaletteChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    Q_UNUSED(previous);
    setPalette(m_palettes.value(current));
}


#include "kis_custom_palette.moc"
