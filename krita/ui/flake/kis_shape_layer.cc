/*
 *  Copyright (c) 2006-2008 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2007 Thomas Zander <zander@kde.org>
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

#include "kis_shape_layer.h"


#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QDomElement>
#include <QDomDocument>
#include <QIcon>
#include <QString>
#include <QList>
#include <QMap>
#include <QDebug>

#include <ktemporaryfile.h>
#include <kicon.h>

#include <KoColorSpace.h>
#include <KoCompositeOp.h>
#include <KoDataCenter.h>
#include <KoDocument.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoGenStyle.h>
#include <KoImageCollection.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoOdfStylesReader.h>
#include <KoOdfWriteStore.h>
#include <KoPageLayout.h>
#include <KoProperties.h>
#include <KoShapeContainer.h>
#include <KoShapeLayer.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeManager.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoStore.h>
#include <KoShapeControllerBase.h>
#include <KoStoreDevice.h>
#include <KoViewConverter.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoSelection.h>

#include <kis_types.h>
#include <kis_image.h>
#include <kis_paint_device.h>
#include "kis_shape_layer_canvas.h"
#include "kis_image_view_converter.h"
#include <kis_painter.h>
#include "kis_node_visitor.h"
#include "kis_effect_mask.h"

class KisShapeLayer::Private
{
public:
    KoViewConverter * converter;
    qint32 x;
    qint32 y;
    KisPaintDeviceSP projection;
    KisPaintDeviceSP filteredProjection;
    KisShapeLayerCanvas * canvas;
    KoShapeControllerBase* controller;
};

KisShapeLayer::KisShapeLayer(KoShapeContainer * parent,
                             KoShapeControllerBase* controller,
                             KisImageSP img,
                             const QString &name,
                             quint8 opacity)
        : KisExternalLayer(img, name, opacity)
        , m_d(new Private())
{
    KoShapeContainer::setParent(parent);
    setShapeId(KIS_SHAPE_LAYER_ID);

    m_d->converter = new KisImageViewConverter(image());
    m_d->x = 0;
    m_d->y = 0;
    m_d->projection = new KisPaintDevice(img->colorSpace());
    m_d->canvas = new KisShapeLayerCanvas(this, m_d->converter);
    m_d->canvas->setProjection(m_d->projection);
    m_d->controller = controller;

    connect(m_d->canvas->shapeManager(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

KisShapeLayer::~KisShapeLayer()
{
    delete m_d->converter;
    delete m_d->canvas;
    delete m_d;
}

bool KisShapeLayer::allowAsChild(KisNodeSP node) const
{
    if (node->inherits("KisMask"))
        return true;
    else
        return false;
}

void KisShapeLayer::addChild(KoShape *object)
{
    KoShapeLayer::addChild(object);
    m_d->canvas->shapeManager()->add(object);

    setDirty(m_d->converter->documentToView(object->boundingRect()).toRect());
}

void KisShapeLayer::removeChild(KoShape *object)
{
    m_d->canvas->shapeManager()->remove(object);
    KoShapeLayer::removeChild(object);
}

QIcon KisShapeLayer::icon() const
{
    return KIcon("bookmark-new");
}

void KisShapeLayer::updateProjection(const QRect& rc)
{
    dbgImage << "KisShapeLayer::updateProjection()" << rc;

    KoProperties props;
    props.setProperty("visible", true);
    QList<KisNodeSP> masks = childNodes(QStringList("KisEffectMask"), props);

    if(masks.empty()) {
        m_d->filteredProjection = 0; // Don't use the filtered projection anymore
    } else {
        if( !m_d->filteredProjection || !(*m_d->filteredProjection->colorSpace() == *m_d->projection->colorSpace() ) ) {
            m_d->filteredProjection = new KisPaintDevice(m_d->projection->colorSpace());
        } else {
            m_d->filteredProjection->clear(rc);
        }
        applyEffectMasks(m_d->projection, m_d->filteredProjection, rc);
    }
}

KisPaintDeviceSP KisShapeLayer::projection() const
{
    if(m_d->filteredProjection)
    {
        return m_d->filteredProjection;
    } else {
        return m_d->projection;
    }
}

qint32 KisShapeLayer::x() const
{
    return m_d->x;
}

void KisShapeLayer::setX(qint32 x)
{
    if (x == m_d->x) return;
    m_d->x = x;
    setDirty();
}

qint32 KisShapeLayer::y() const
{
    return m_d->y;
}

void KisShapeLayer::setY(qint32 y)
{
    if (y == m_d->y) return;
    m_d->y = y;
    setDirty();
}

QRect KisShapeLayer::extent() const
{
    QRect rc = boundingRect().toRect();
    return QRectF(rc.x() * image()->xRes(), rc.y() * image()->yRes(), rc.width() * image()->xRes(), rc.height() * image()->yRes()).toRect();
}

QRect KisShapeLayer::exactBounds() const
{
    QRect rc = boundingRect().toRect();
    return QRectF(rc.x() * image()->xRes(), rc.y() * image()->yRes(), rc.width() * image()->xRes(), rc.height() * image()->yRes()).toRect();
}

bool KisShapeLayer::accept(KisNodeVisitor& visitor)
{
    return visitor.visit(this);
}

KoShapeManager *KisShapeLayer::shapeManager() const
{
    return m_d->canvas->shapeManager();
}

bool KisShapeLayer::saveLayer(KoStore * store) const
{
    store->disallowNameExpansion();
    KoOdfWriteStore odfStore(store);
    KoXmlWriter* manifestWriter = odfStore.manifestWriter("application/vnd.oasis.opendocument.graphics");
    KoEmbeddedDocumentSaver embeddedSaver;
    KoDocument::SavingContext documentContext(odfStore, embeddedSaver);

    if (!store->open("content.xml"))
        return false;

    KoStoreDevice storeDev(store);
    KoXmlWriter * docWriter = KoOdfWriteStore::createOasisXmlWriter(&storeDev, "office:document-content");

    // for office:master-styles
    KTemporaryFile masterStyles;
    masterStyles.open();
    KoXmlWriter masterStylesTmpWriter(&masterStyles, 1);

    KoPageLayout page;
    page.format = KoPageFormat::defaultFormat();
    QRectF rc = boundingRect();
    page.width = rc.width();
    page.height = rc.height();
    if ( page.width > page.height ) {
        page.orientation = KoPageFormat::Landscape;
    }
    else {
         page.orientation = KoPageFormat::Portrait;
    }

    KoGenStyles mainStyles;
    KoGenStyle pageLayout = page.saveOdf();
    QString layoutName = mainStyles.lookup(pageLayout, "PL");
    KoGenStyle masterPage(KoGenStyle::StyleMaster);
    masterPage.addAttribute("style:page-layout-name", layoutName);
    mainStyles.lookup(masterPage, "Default", KoGenStyles::DontForceNumbering);

    KTemporaryFile contentTmpFile;
    contentTmpFile.open();
    KoXmlWriter contentTmpWriter(&contentTmpFile, 1);

    contentTmpWriter.startElement("office:body");
    contentTmpWriter.startElement("office:drawing");

    KoShapeSavingContext shapeContext(contentTmpWriter, mainStyles, documentContext.embeddedSaver);

    shapeContext.xmlWriter().startElement("draw:page");
    shapeContext.xmlWriter().addAttribute("draw:name", "");
    shapeContext.xmlWriter().addAttribute("draw:id", "page1");
    shapeContext.xmlWriter().addAttribute("draw:master-page-name", "Default");

    saveOdf(shapeContext);

    shapeContext.xmlWriter().endElement(); // draw:page

    contentTmpWriter.endElement(); // office:drawing
    contentTmpWriter.endElement(); // office:body

    mainStyles.saveOdfAutomaticStyles(docWriter, false);

    // And now we can copy over the contents from the tempfile to the real one
    contentTmpFile.seek(0);
    docWriter->addCompleteElement(&contentTmpFile);

    docWriter->endElement(); // Root element
    docWriter->endDocument();
    delete docWriter;

    if (!store->close())
        return false;

    embeddedSaver.saveEmbeddedDocuments(documentContext);

    manifestWriter->addManifestEntry("content.xml", "text/xml");

    if (! mainStyles.saveOdfStylesDotXml(store, manifestWriter)) {
        return false;
    }

    manifestWriter->addManifestEntry("settings.xml", "text/xml");

    if (! shapeContext.saveDataCenter( documentContext.odfStore.store(),
                                       documentContext.odfStore.manifestWriter() ))
        return false;

    // Write out manifest file
    if (!odfStore.closeManifestWriter()) {
        dbgImage << "closing manifestWriter failed";
        return false;
    }

    return true;
}

bool KisShapeLayer::loadLayer( KoStore* store )
{
    KoOdfReadStore odfStore( store );
    QString errorMessage;

    odfStore.loadAndParse( errorMessage );

    if ( !errorMessage.isEmpty() ) {
        qDebug() << errorMessage;
        return false;
    }

    KoXmlElement contents = odfStore.contentDoc().documentElement();

//    qDebug() <<"Start loading OASIS document..." << contents.text();
//    qDebug() <<"Start loading OASIS contents..." << contents.lastChild().localName();
//    qDebug() <<"Start loading OASIS contents..." << contents.lastChild().namespaceURI();
//    qDebug() <<"Start loading OASIS contents..." << contents.lastChild().isElement();

    KoXmlElement body( KoXml::namedItemNS( contents, KoXmlNS::office, "body" ) );

    if( body.isNull() )
    {
        qDebug() <<"No office:body found!";
        //setErrorMessage( i18n( "Invalid OASIS document. No office:body tag found." ) );
        return false;
    }

    body = KoXml::namedItemNS( body, KoXmlNS::office, "drawing");
    if(body.isNull())
    {
        qDebug() <<"No office:drawing found!";
        //setErrorMessage( i18n( "Invalid OASIS document. No office:drawing tag found." ) );
        return false;
    }

    KoXmlElement page( KoXml::namedItemNS( body, KoXmlNS::draw, "page" ) );
    if(page.isNull())
    {
        qDebug() <<"No office:drawing found!";
        //setErrorMessage( i18n( "Invalid OASIS document. No draw:page tag found." ) );
        return false;
    }

    KoXmlElement * master = 0;
    if( odfStore.styles().masterPages().contains( "Standard" ) )
        master = odfStore.styles().masterPages().value( "Standard" );
    else if( odfStore.styles().masterPages().contains( "Default" ) )
        master = odfStore.styles().masterPages().value( "Default" );
    else if( ! odfStore.styles().masterPages().empty() )
        master = odfStore.styles().masterPages().begin().value();

    if( master )
    {
        const KoXmlElement *style = odfStore.styles().findStyle(
            master->attributeNS( KoXmlNS::style, "page-layout-name", QString() ) );
        KoPageLayout pageLayout;
        pageLayout.loadOdf( *style );
        setSize( QSizeF( pageLayout.width, pageLayout.height ) );
    }
    else
    {
        kWarning() << "No master page found!";
        return false;
    }

    KoOdfLoadingContext context( odfStore.styles(), odfStore.store() );
    context.setManifestFile(QString("tar:/") + odfStore.store()->currentPath() + "META-INF/manifest.xml");
    KoShapeLoadingContext shapeContext( context, m_d->controller->dataCenterMap() );


    KoXmlElement layerElement;
    forEachElement( layerElement, context.stylesReader().layerSet() )
    {
        KoShapeLayer * l = new KoShapeLayer();
        if( !loadOdf( layerElement, shapeContext ) ) {
            kWarning() << "Could not load shape layer!";
            return false;
        }
    }

    KoXmlElement child;
    forEachElement( child, page )
    {
        KoShape * shape = KoShapeRegistry::instance()->createShapeFromOdf( child, shapeContext );
        if( shape ) {
            addChild( shape );
        }
    }

    return true;

}


KisPaintDeviceSP KisShapeLayer::paintDevice() const
{
    return 0;
}

void KisShapeLayer::selectionChanged()
{
    emit selectionChanged( m_d->canvas->shapeManager()->selection()->selectedShapes());
}

#include "kis_shape_layer.moc"
