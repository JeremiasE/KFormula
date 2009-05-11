/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2006
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

#include "kis_mask_manager.h"


#include <klocale.h>
#include <kstandardaction.h>
#include <kaction.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>

#include <KoID.h>

#include <kis_transaction.h>
#include <filter/kis_filter_configuration.h>
#include <commands/kis_node_commands.h>
#include "dialogs/kis_dlg_transformation_effect.h"
#include <kis_undo_adapter.h>
#include <kis_paint_layer.h>
#include "kis_doc2.h"
#include "kis_view2.h"
#include <kis_layer.h>
#include <kis_group_layer.h>
#include <kis_transformation_mask.h>
#include <kis_filter_mask.h>
#include <kis_transparency_mask.h>
#include <kis_mask.h>
#include <kis_effect_mask.h>
#include "dialogs/kis_dlg_adjustment_layer.h"
#include "widgets/kis_mask_widgets.h"
#include <kis_selection.h>
#include <kis_pixel_selection.h>
#include "dialogs/kis_dlg_adj_layer_props.h"
#include <kis_selection_mask.h>
#include <kis_paint_device.h>
#include <kis_image.h>
#include <kis_transform_worker.h>
#include <KoCompositeOp.h>
#include <KoColorSpace.h>
#include <KoColor.h>
#include "kis_node_commands_adapter.h"

KisMaskManager::KisMaskManager(KisView2 * view)
        : m_view(view)
        , m_activeMask(0)
        , m_createTransparencyMask(0)
        , m_createFilterMask(0)
        , m_createTransformationMask(0)
        , m_createSelectionMask(0)
        , m_maskToSelection(0)
        , m_maskToLayer(0)
        , m_duplicateMask(0)
        , m_showMask(0)
        , m_removeMask(0)
        , m_raiseMask(0)
        , m_lowerMask(0)
        , m_maskToTop(0)
        , m_maskToBottom(0)
        , m_mirrorMaskX(0)
        , m_mirrorMaskY(0)
        , m_maskProperties(0)
        , m_commandsAdapter( new KisNodeCommandsAdapter( m_view ) )
{
}

void KisMaskManager::setup(KActionCollection * actionCollection)
{
    // XXX: Set shortcuts, tooltips, what's this text and statustips!

    m_createTransparencyMask  = new KAction(i18n("Transparency Mask"), this);
    actionCollection->addAction("create_transparency_mask", m_createTransparencyMask);
    connect(m_createTransparencyMask, SIGNAL(triggered()), this, SLOT(createTransparencyMask()));

    m_createFilterMask  = new KAction(i18n("Filter Mask..."), this);
    actionCollection->addAction("create_filter_mask", m_createFilterMask);
    connect(m_createFilterMask, SIGNAL(triggered()), this, SLOT(createFilterMask()));
#if 0 // XXX_2.0
    m_createTransformationMask  = new KAction(i18n("Transformation Mask..."), this);
    actionCollection->addAction("create_transformation_mask", m_createTransformationMask);
    connect(m_createTransformationMask, SIGNAL(triggered()), this, SLOT(createTransformationMask()));
#endif
    m_createSelectionMask = new KAction(i18n("Selection Mask..."), this);
    actionCollection->addAction("create_selection_mask", m_createSelectionMask);
    connect(m_createSelectionMask, SIGNAL(triggered()), this, SLOT(createSelectionmask()));

    m_maskToSelection  = new KAction(i18n("Mask To Selection"), this);
    actionCollection->addAction("create_selection_from_mask", m_maskToSelection);
    connect(m_maskToSelection, SIGNAL(triggered()), this, SLOT(maskToSelection()));

    m_maskToLayer = new KAction(i18n("Create Layer from Mask..."), this);
    actionCollection->addAction("create_layer_from_mask", m_maskToLayer);
    connect(m_maskToLayer, SIGNAL(triggered()), this, SLOT(maskToLayer()));

    m_duplicateMask = new KAction(i18n("Duplicate Mask"), this);
    actionCollection->addAction("duplicate_mask", m_duplicateMask);
    connect(m_duplicateMask, SIGNAL(triggered()), this, SLOT(duplicateMask()));

    m_removeMask  = new KAction(i18n("Remove Mask"), this);
    actionCollection->addAction("remove_mask", m_removeMask);
    connect(m_removeMask, SIGNAL(triggered()), this, SLOT(removeMask()));

    m_showMask  = new KToggleAction(i18n("Show Mask"), this);
    actionCollection->addAction("show_mask", m_showMask);
    connect(m_showMask, SIGNAL(triggered()), this, SLOT(showMask()));

    m_raiseMask = new KAction(i18n("Raise Mask"), this);
    actionCollection->addAction("raise_mask", m_raiseMask);
    connect(m_raiseMask, SIGNAL(triggered()), this, SLOT(raiseMask()));

    m_lowerMask  = new KAction(i18n("Lower Mask"), this);
    actionCollection->addAction("lower_mask", m_lowerMask);
    connect(m_lowerMask, SIGNAL(triggered()), this, SLOT(lowerMask()));

    m_maskToTop  = new KAction(i18n("Move Mask to Top"), this);
    actionCollection->addAction("mask_to_top", m_maskToTop);
    connect(m_maskToTop, SIGNAL(triggered()), this, SLOT(maskToTop()));

    m_maskToBottom = new KAction(i18n("Move Mask to Bottom"), this);
    actionCollection->addAction("mask_to_bottom", m_maskToBottom);
    connect(m_maskToBottom, SIGNAL(triggered()), this, SLOT(maskToBottom()));

    m_mirrorMaskX = new KAction(i18n("Mirror Mask Horizontally"), this);
    actionCollection->addAction("mirror_mask_x", m_mirrorMaskX);
    connect(m_mirrorMaskX, SIGNAL(triggered()), this, SLOT(mirrorMaskX()));

    m_mirrorMaskY  = new KAction(i18n("Mirror Mask Vertically"), this);
    actionCollection->addAction("mirror_mask_y", m_mirrorMaskY);
    connect(m_mirrorMaskY, SIGNAL(triggered()), this, SLOT(mirrorMaskY()));

    m_maskProperties  = new KAction(i18n("Mask Properties"), this);
    actionCollection->addAction("mask_properties", m_maskProperties);
    connect(m_maskProperties, SIGNAL(triggered()), this, SLOT(maskProperties()));

}

void KisMaskManager::updateGUI()
{
    // XXX: enable/disable menu items according to whether there's a mask selected currently
    // XXX: disable the selection mask item if there's already a selection mask
}

KisMaskSP KisMaskManager::activeMask()
{
    return m_activeMask;
}

KisPaintDeviceSP KisMaskManager::activeDevice()
{
    // XXX: we may need to also have a possibility of getting the vector part of the
    // selection here
    if (m_activeMask) {
        KisSelectionSP selection = m_activeMask->selection();
        if (selection)
            return m_activeMask->selection()->getOrCreatePixelSelection();
    }
    return 0;
}

void KisMaskManager::activateMask(KisMaskSP mask)
{
    m_activeMask = mask;
    emit sigMaskActivated(mask);
}

void KisMaskManager::createTransparencyMask()
{
    KisLayerSP parent = m_view->activeLayer();
    if (parent) {
        if (parent == 0)
            createTransparencyMask(parent, parent->firstChild());
        else
            createTransparencyMask(parent, m_activeMask);
    }
}

void KisMaskManager::createTransparencyMask(KisNodeSP parent, KisNodeSP above)
{
    KisMaskSP mask = new KisTransparencyMask();
    KisSelectionSP selection = m_view->selection();
    if (selection) {
        mask->setSelection(selection);
    }
    mask->setName(i18n("Transparency Mask"));     // XXX:Auto-increment a number here, like with layers
    m_commandsAdapter->addNode(mask, parent, above);
    mask->setDirty();
    activateMask(mask);
}


void KisMaskManager::createFilterMask()
{
    KisLayerSP parent = m_view->activeLayer();

    if (parent) {
        if (m_activeMask == 0)
            createFilterMask(parent, parent->firstChild());
        else
            createFilterMask(parent, m_activeMask);

    }
}

void KisMaskManager::addEffectMask( KisNodeSP parent, KisEffectMaskSP mask )
{
    m_commandsAdapter->addNode( mask, parent, 02110 );
    activateMask( mask );
}

void KisMaskManager::createFilterMask(KisNodeSP parent, KisNodeSP above)
{
    KisLayerSP layer = dynamic_cast<KisLayer*>(parent.data());
    if (! layer)
        return;

    KisPaintDeviceSP dev = layer->projection();
    KisFilterMask * mask = new KisFilterMask();

    if (layer->selection()) {
        mask->setSelection(layer->selection());
    }
    else if(m_view->image()->globalSelection()) {
        mask->setSelection(m_view->image()->globalSelection());
    } else {
        // XXX not sure why it is needed, but without this the mask is unselected from the beginning
        mask->selection()->getOrCreatePixelSelection()->select(m_view->image()->bounds());
        mask->selection()->setDeselected(true);
    }

    mask->setName(i18n("New filter mask"));
    m_commandsAdapter->addNode(mask, parent, above);

    KisDlgAdjustmentLayer dlg(mask, mask, dev, m_view->image(), mask->name(), i18n("New Filter Mask"), m_view, "dlgfiltermask");

    if (dlg.exec() == QDialog::Accepted) {
        KisFilterConfiguration * filter = dlg.filterConfiguration();
        QString name = dlg.layerName();
        mask->setFilter(filter);
        activateMask(mask);
    } else {
        m_view->image()->removeNode(mask);
    }
}

void KisMaskManager::createTransformationMask()
{
    KisLayerSP activeLayer = m_view->activeLayer();

    if (activeLayer) {
        if (m_activeMask == 0)
            createTransformationMask(activeLayer, activeLayer->firstChild());
        else
            createTransformationMask(activeLayer, m_activeMask);
    }
}

void KisMaskManager::createTransformationMask(KisNodeSP parent, KisNodeSP above)
{
    KisDlgTransformationEffect dlg(QString(), 1.0, 1.0, 0.0, 0.0, 0.0, 0, 0, KoID("Mitchell"), m_view);
    if (dlg.exec() == QDialog::Accepted) {
        KisTransformationMask * mask = new KisTransformationMask();

        KisSelectionSP selection = m_view->selection();
        if (selection) {
            mask->setSelection(selection);
        }
        mask->setName(dlg.transformationEffect()->maskName());
        mask->setXScale(dlg.transformationEffect()->xScale());
        mask->setYScale(dlg.transformationEffect()->yScale());
        mask->setXShear(dlg.transformationEffect()->xShear());
        mask->setYShear(dlg.transformationEffect()->yShear());
        mask->setRotation(dlg.transformationEffect()->rotation());
        mask->setXTranslation(dlg.transformationEffect()->moveX());
        mask->setYTranslation(dlg.transformationEffect()->moveY());
        mask->setFilterStrategy(dlg.transformationEffect()->filterStrategy());
        m_commandsAdapter->addNode(mask, parent, above);
        activateMask(mask);
        mask->setDirty(selection->selectedExactRect());
    }
}

void KisMaskManager::createSelectionmask()
{
    KisLayerSP activeLayer = m_view->activeLayer();

    if (activeLayer && activeLayer->selectionMask() == 0) {
        if (m_activeMask == 0)
            createSelectionMask(activeLayer, activeLayer->firstChild());
        else
            createSelectionMask(activeLayer, m_activeMask);
    }
}

void KisMaskManager::createSelectionMask(KisNodeSP parent, KisNodeSP above)
{
    if (parent->inherits("KisLayer")) {
        KisLayer * layer = dynamic_cast<KisLayer*>(parent.data());
        if (layer && layer->selectionMask()) {
            return;
        }
    }
    KisMaskSP mask = new KisSelectionMask(m_view->image());
    KisSelectionSP selection = m_view->selection();
    if (selection) {
        mask->setSelection(selection);
    }
    mask->setName(i18n("Selection"));     // XXX:Auto-increment a number here, like with layers
    m_commandsAdapter->addNode(mask, parent, above);
}


void KisMaskManager::maskToSelection()
{
    // XXX: should we remove the mask when setting the mask as selection?
    // XXX: should the selection be layer-local, or global?
    if (!m_activeMask) return;
    KisImageSP image = m_view->image();
    if (!image) return;
    // TODO require a macro
    image->setGlobalSelection(m_activeMask->selection()); // TODO that definitively require a command !
    m_commandsAdapter->removeNode(m_activeMask);
    m_activeMask = 0;
}

void KisMaskManager::maskToLayer()
{
    // XXX: Should we also be able to create other layertypes than paint layer from masks?
    // XXX: Right now, I create black pixels with the alpha channel set to the selection,
    //      should we create grayscale pixels?
    if (!m_activeMask) return;
    KisImageSP image = m_view->image();
    if (!image) return;
    KisLayerSP activeLayer = m_view->activeLayer();
    if (!activeLayer) return;

    KisSelectionSP selection = m_activeMask->selection();
    selection->updateProjection();
    KisPaintLayerSP layer =
        new KisPaintLayer(image, m_activeMask->name(), OPACITY_OPAQUE);

    const KoColorSpace * cs = layer->colorSpace();
    QRect rc = selection->selectedExactRect();
    KoColor color(Qt::black, cs);
    int pixelsize = cs->pixelSize();
    KisHLineIteratorPixel dstiter = layer->paintDevice()->createHLineIterator(rc.x(), rc.y(), rc.width(), selection);
    for (int row = 0; row < rc.height(); ++row) {
        while (!dstiter.isDone()) {
            cs->setAlpha(color.data(), dstiter.selectedness(), 1);
            memcpy(dstiter.rawData(), color.data(), pixelsize);
        }
        dstiter.nextRow();
    }

    // TODO make a macro (require a string)
    m_commandsAdapter->removeNode(m_activeMask);
    m_activeMask = 0;
    m_commandsAdapter->addNode(layer, activeLayer->parent(), activeLayer);
}

void KisMaskManager::duplicateMask()
{
    if (!m_activeMask) return;
    if (!!m_view->image()) return;
    if (m_activeMask->inherits("KisSelectionMask")) return; // Cannot duplicate selection masks
    KisNodeSP dup = m_activeMask->clone();
    m_commandsAdapter->addNode(dup, m_activeMask->parent(), m_activeMask);

}

void KisMaskManager::removeMask()
{
    if (!m_activeMask) return;
    if (!m_view->image()) return;
    m_commandsAdapter->removeNode(m_activeMask);
}

void KisMaskManager::mirrorMaskX()
{
    // XXX_NODE: This is a load of copy-past from KisLayerManager -- how can I fix that?
    // XXX_NODE: we should also mirror the shape-based part of the selection!
    if (!m_activeMask) return;

    KisPaintDeviceSP dev = m_activeMask->selection()->getOrCreatePixelSelection();
    if (!dev) return;

    KisTransaction * t = 0;
    if (m_view->undoAdapter() && m_view->undoAdapter()->undo()) {
        t = new KisTransaction(i18n("Mirror Mask X"), dev);
        Q_CHECK_PTR(t);
    }

    QRect dirty = KisTransformWorker::mirrorX(dev, m_view->selection() );
    m_activeMask->setDirty(dirty);

    if (t) m_view->undoAdapter()->addCommand(t);

    m_view->document()->setModified(true);
    m_activeMask->selection()->updateProjection();
    masksUpdated();
    m_view->canvas()->update();
}

void KisMaskManager::mirrorMaskY()
{
    // XXX_NODE: This is a load of copy-past from KisLayerManager -- how can I fix that?
    // XXX_NODE: we should also mirror the shape-based part of the selection!
    if (!m_activeMask) return;

    KisPaintDeviceSP dev = m_activeMask->selection()->getOrCreatePixelSelection();
    if (!dev) return;

    KisTransaction * t = 0;
    if (m_view->undoAdapter() && m_view->undoAdapter()->undo()) {
        t = new KisTransaction(i18n("Mirror Layer Y"), dev);
        Q_CHECK_PTR(t);
    }

    QRect dirty = KisTransformWorker::mirrorY(dev, m_view->selection() );
    m_activeMask->setDirty(dirty);


    if (t) m_view->undoAdapter()->addCommand(t);

    m_view->document()->setModified(true);
    m_activeMask->selection()->updateProjection();
    masksUpdated();
    m_view->canvas()->update();
}

void KisMaskManager::maskProperties()
{
    if (!m_activeMask) return;

    if (m_activeMask->inherits("KisTransformationMask")) {
        KisTransformationMask * mask = static_cast<KisTransformationMask*>(m_activeMask.data());

        KisDlgTransformationEffect dlg(mask->name(),
                                       mask->xScale(),
                                       mask->yScale(),
                                       mask->xShear(),
                                       mask->yShear(),
                                       mask->rotation(),
                                       mask->xTranslate(),
                                       mask->yTranslate(),
                                       KoID(mask->filterStrategy()->id()),
                                       m_view);
        if (dlg.exec() == QDialog::Accepted) {
            // XXX_NODE: make undoable
            KisTransformationSettingsCommand * cmd = new KisTransformationSettingsCommand
            (mask,
             mask->name(),
             mask->xScale(),
             mask->yScale(),
             mask->xShear(),
             mask->yShear(),
             mask->rotation(),
             mask->xTranslate(),
             mask->yTranslate(),
             mask->filterStrategy(),
             dlg.transformationEffect()->maskName(),
             dlg.transformationEffect()->xScale(),
             dlg.transformationEffect()->yScale(),
             dlg.transformationEffect()->xShear(),
             dlg.transformationEffect()->yShear(),
             dlg.transformationEffect()->rotation(),
             dlg.transformationEffect()->moveX(),
             dlg.transformationEffect()->moveY(),
             dlg.transformationEffect()->filterStrategy());
            cmd->redo();
            m_view->undoAdapter()->addCommand(cmd);
            m_view->document()->setModified(true);
            mask->setDirty(mask->extent());
        }
    } else if (m_activeMask->inherits("KisFilterMask")) {
        KisFilterMask * mask = static_cast<KisFilterMask*>(m_activeMask.data());

        KisLayerSP layer = dynamic_cast<KisLayer*>(mask->parent().data());
        if (! layer)
            return;

        KisPaintDeviceSP dev = layer->projection();
        KisDlgAdjLayerProps dlg(layer->projection(), layer->image(), mask->filter(), mask->name(), i18n("Effect Mask Properties"), m_view, "dlgeffectmaskprops");
        QString before;
        if (dlg.filterConfiguration())
            before = dlg.filterConfiguration()->toLegacyXML();
        if (dlg.exec() == QDialog::Accepted) {
            QString after;
            if (dlg.filterConfiguration())
                after = dlg.filterConfiguration()->toLegacyXML();
            KisChangeFilterCmd<KisFilterMaskSP> * cmd = new KisChangeFilterCmd<KisFilterMaskSP>(mask,
                    dlg.filterConfiguration(),
                    before,
                    after);
            cmd->redo();
            m_view->undoAdapter()->addCommand(cmd);
            m_view->document()->setModified(true);
            mask->setDirty();
        }
    } else {
        // Not much to show for transparency or selection masks?
    }
}

void KisMaskManager::masksUpdated()
{
    m_view->updateGUI();
}

void KisMaskManager::showMask()
{
    // XXX: make sure the canvas knows it should paint the active mask as a mask
}

void KisMaskManager::raiseMask()
{
    if (!m_activeMask) return;
    if (!m_view->image()) return;
    m_commandsAdapter->raise(m_activeMask);
}

void KisMaskManager::lowerMask()
{
    if (!m_activeMask) return;
    if (!m_view->image()) return;
    m_commandsAdapter->lower(m_activeMask);
}

void KisMaskManager::maskToTop()
{
    if (!m_activeMask) return;
    if (!m_view->image()) return;
    m_commandsAdapter->toTop(m_activeMask);
}

void KisMaskManager::maskToBottom()
{
    if (!m_activeMask) return;
    if (!m_view->image()) return;
    m_commandsAdapter->toBottom(m_activeMask);
}


#include "kis_mask_manager.moc"
