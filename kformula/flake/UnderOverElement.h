/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef UNDEROVERELEMENT_H
#define UNDEROVERELEMENT_H

#include "BasicElement.h"
#include "kformula_export.h"

/**
 * @short Implementation of the MathML mover, munder and moverunder elements
 *
 */
class KOFORMULA_EXPORT UnderOverElement : public BasicElement {
public:
    /// The standard constructor
    UnderOverElement( BasicElement* parent = 0 );

    /// The standard destructor
    ~UnderOverElement();

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement*> childElements();

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am AttributeManager containing style info
     */
    void paint( QPainter& painter, AttributeManager* am );

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    void layout( const AttributeManager* am );

    ///inherited from BasicElement
    bool acceptCursor( const FormulaCursor* cursor );

    /// @return The default value of the attribute for this element
    QString attributesDefaultValue( const QString& attribute ) const; 

    /// @return The element's ElementType
    ElementType elementType() const;
   
protected:
    /// Read all content from the node - reimplemented by child elements
    bool readMathMLContent( const KoXmlElement& element );

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    void writeMathMLContent( KoXmlWriter* writer ) const;   
 
private:
    /// The element used as basis for the under and the over element
    BasicElement* m_baseElement;

    /// The element that is layouted under the base element
    BasicElement* m_underElement;

    /// The element that is layouted over the base element
    BasicElement* m_overElement;
};

#endif // UNDEROVERELEMENT_H
