/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2005 Casper Boemann <cbr@boemann.dk>
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
#ifndef KIS_LAYER_OPACITY_COMMAND_H
#define KIS_LAYER_OPACITY_COMMAND_H

#include <krita_export.h>
#include <QUndoCommand>
#include <QRect>
#include <QPoint>

#include "kis_types.h"
#include "kis_layer_command.h"

class KoCompositeOp;
class KisLayer;

/// The command for setting the layer opacity
class KRITAIMAGE_EXPORT KisLayerOpacityCommand : public KisLayerCommand
{

public:
    /**
     * Constructor
     * @param layer The layer the command will be working on.
     * @param oldOpacity the old layer opacity
     * @param newOpacity the new layer opacity
     */
    KisLayerOpacityCommand(KisLayerSP layer, quint8 oldOpacity, quint8 newOpacity);

    virtual void redo();
    virtual void undo();

private:
    quint8 m_oldOpacity;
    quint8 m_newOpacity;
};

#endif
