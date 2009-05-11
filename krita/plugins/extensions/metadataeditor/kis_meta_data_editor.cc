/*
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
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

#include "kis_meta_data_editor.h"

#include <QDomDocument>
#include <QFile>
#include <QUiLoader>

#include <klocale.h>
#include <kstandarddirs.h>
#include <kis_debug.h>

#include <kis_meta_data_store.h>
#include <kis_meta_data_entry.h>
#include <kis_meta_data_value.h>
#include <kis_meta_data_schema.h>
#include <kis_meta_data_schema_registry.h>

#include "kis_entry_editor.h"

struct KisMetaDataEditor::Private {
    KisMetaData::Store* originalStore;
    KisMetaData::Store* store;
    QMultiHash<KisMetaData::Value*, KisEntryEditor*> entryEditors;
};


KisMetaDataEditor::KisMetaDataEditor(QWidget* parent, KisMetaData::Store* originalStore) :
        KPageDialog(parent), d(new Private)
{
    d->originalStore = originalStore;
    d->store = new KisMetaData::Store(*originalStore);

    QStringList files = KGlobal::dirs()->findAllResources("data", "kritaplugins/metadataeditor/*.rc");

    foreach(const QString & file, files) {

        QFile xmlFile(file);
        xmlFile.open(QFile::ReadOnly);
        QString errMsg;
        int errLine, errCol;

        QDomDocument document;
        if (!document.setContent(&xmlFile, false, &errMsg, &errLine, &errCol)) {
            dbgPlugins << "Error reading XML at line" << errLine << " column" << errCol << " :" << errMsg;
        }
        QDomElement rootElement = document.documentElement();
        if (rootElement.tagName() != "MetaDataEditor") {
            dbgPlugins << "Invalid XML file";
        }

        const QString uiFileName = rootElement.attribute("uiFile");
        const QString pageName = rootElement.attribute("name");
        const QString iconName = rootElement.attribute("icon");
        if (uiFileName == "") continue;

        // Read the ui file
        QUiLoader loader;
        QFile uiFile(KStandardDirs::locate("data", "kritaplugins/metadataeditor/" + uiFileName));
        uiFile.open(QFile::ReadOnly);
        QWidget *widget = dynamic_cast<QWidget*>(loader.load(&uiFile, this));
        if (widget == 0) {
            dbgPlugins << "Failed to load ui file" << uiFileName;
            continue;
        }
        uiFile.close();

        QDomNodeList list = rootElement.childNodes();
        const int size = list.size();
        for (int i = 0; i < size; ++i) {
            QDomElement elem = list.item(i).toElement();
            if (elem.isNull() || elem.tagName() != "EntryEditor") continue;
            const QString editorName = elem.attribute("editorName");
            const QString schemaUri = elem.attribute("schemaUri");
            const QString entryName = elem.attribute("entryName");
            const QString editorSignal = '2' + elem.attribute("editorSignal");
            const QString propertyName = elem.attribute("propertyName");
            const QString structureField = elem.attribute("structureField");

            QWidget* obj = widget->findChild<QWidget*>(editorName);
            if (obj) {
                const KisMetaData::Schema* schema = KisMetaData::SchemaRegistry::instance()->schemaFromUri(schemaUri);
                if (schema) {
                    if (!d->store->containsEntry(schema, entryName)) {
                        dbgPlugins << " Store does not have yet entry :" << entryName << " in" << schemaUri  << " ==" << schema->generateQualifiedName(entryName);
                    }
                    KisMetaData::Value& value = d->store->getEntry(schema, entryName).value();
                    KisEntryEditor* ee = 0;
                    if (value.type() == KisMetaData::Value::Structure && !structureField.isEmpty()) {
                        QMap<QString, KisMetaData::Value>* structure = value.asStructure();
                        ee = new KisEntryEditor(obj, &(*structure)[ structureField ], propertyName);
                    } else {
                        ee = new KisEntryEditor(obj, &value, propertyName);
                    }
                    connect(obj, editorSignal.toAscii(), ee, SLOT(valueEdited()));
                    QList<KisEntryEditor*> otherEditors = d->entryEditors.values(&value);
                    foreach(KisEntryEditor* oe, otherEditors) {
                        connect(ee, SIGNAL(valueHasBeenEdited()), oe, SLOT(valueChanged()));
                        connect(oe, SIGNAL(valueHasBeenEdited()), ee, SLOT(valueChanged()));
                    }
                    d->entryEditors.insert(&value, ee);
                } else {
                    dbgPlugins << "Unknown schema :" << schemaUri;
                }
            } else {
                dbgPlugins << "Unknown object :" << editorName;
            }
        }
        xmlFile.close();

        KPageWidgetItem *page = new KPageWidgetItem(widget, pageName);
        if (!iconName.isEmpty()) {
            page->setIcon(KIcon(iconName));
        }
        addPage(page);
    }
}

KisMetaDataEditor::~KisMetaDataEditor()
{
    foreach(KisEntryEditor* e, d->entryEditors) {
        delete e;
    }
    delete d->store;
    delete d;
}

void KisMetaDataEditor::accept()
{
    KPageDialog::accept();
    d->originalStore->copyFrom(d->store);
}

