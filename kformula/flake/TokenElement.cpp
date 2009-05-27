/* This file is part of the KDE project
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

#include "TokenElement.h"
#include "AttributeManager.h"
#include "FormulaCursor.h"
#include "GlyphElement.h"
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <QPainter>
#include <kdebug.h>

TokenElement::TokenElement( BasicElement* parent ) : BasicElement( parent )
{}

const QList<BasicElement*> TokenElement::childElements()
{
    // only return the mglyph elements
    QList<BasicElement*> tmpList;
    foreach( GlyphElement* tmp, m_glyphs )
        tmpList << tmp;

    return tmpList;
}

void TokenElement::paint( QPainter& painter, AttributeManager* am )
{
    // set the painter to background color and paint it
     painter.setPen( am->colorOf( "mathbackground", this ) );
    painter.setBrush( QBrush( painter.pen().color() ) );
    painter.drawRect( QRectF( 0.0, 0.0, width(), height() ) );

    // set the painter to foreground color and paint the text in the content path
    painter.setPen( am->colorOf( "mathcolor", this ) );
    painter.setBrush( QBrush( painter.pen().color() ) );
    painter.translate( 0, baseLine() );
    painter.drawPath( m_contentPath );
}

int TokenElement::length() const
{
    return m_rawString.length();
}

bool TokenElement::isToken() const {
    return true;
}

void TokenElement::layout( const AttributeManager* am )
{
    // Query the font to use
    m_font = am->font( this );

    // save the token in an empty path
    m_contentPath = QPainterPath();

    // replace all the object replacement characters with glyphs
    int counter = 0;
    for( int i = 0; i < m_rawString.length(); i++ ) {
        if( m_rawString[ i ] != QChar::ObjectReplacementCharacter )
            //chunk.append( m_rawString[ i ] );
	    renderToPath(QString(m_rawString[i]),m_contentPath);
        else {
            m_glyphs[ counter ]->renderToPath( QString(), m_contentPath );
            counter++;
        }
    }
    // As the text is added to ( 0 / 0 ) the baseline equals the top edge of the
    // elements bounding rect, while translating it down the text's baseline moves too
    setBaseLine( -m_contentPath.boundingRect().y() ); // set baseline accordingly
    //TODO: Hae?
    setWidth( m_contentPath.boundingRect().right() );
    setHeight( m_contentPath.boundingRect().height() );
}

void TokenElement::insertChild( FormulaCursor* cursor, BasicElement* child )
{
/*    if( child && child->elementType() == Glyph ) {
        m_rawString.insert( QChar( QChar::ObjectReplacementCharacter ) );
        m_glyphs.insert();
    }
    else*/ if( !child )
	//TODO: get the text from child
        m_rawString.insert( cursor->position(), cursor->inputBuffer() );
	kDebug() << "-"<<m_rawString << "InputBuffer:"<<cursor->inputBuffer();
}

bool TokenElement::setCursorTo(FormulaCursor* cursor, QPointF point) {
    int counter = 0;
    int i = 0;
    QPainterPath tmppath;
    kDebug()<<"point: "<<point<<"-"<<boundingRect().width();
    cursor->setCurrentElement(this);
    if (boundingRect().width()<point.x()) {
	    cursor->setPosition(length());
	    return true;
    }
    double oldx=0;
    //Find the letter we clicked on
    for( i = 0; i < m_rawString.length(); i++ ) {
        //add a character to the string
	if( m_rawString[ i ] != QChar::ObjectReplacementCharacter ) {
	    renderToPath( QString(m_rawString[i]), tmppath );
	}
        else {
	    m_glyphs[ counter ]->renderToPath( QString(), tmppath );
	    counter++;
        }
 	//check if we found the character, on which the point is
	if (tmppath.boundingRect().right()>=point.x()) {
	    break;
	}
	//save the old width of the path
	oldx=tmppath.boundingRect().right();
    }
    //Find out, if we should place the cursor before or after the character
    if ((point.x()-oldx)>(tmppath.boundingRect().right()-point.x())) {	
	i++;
    }
    cursor->setPosition(i);
    return true;
}


QLineF TokenElement::cursorLine(const FormulaCursor* cursor)
{
    // inside tokens let the token calculate the cursor x offset
    double tmp = cursorOffset( cursor );
    QPointF top = absoluteBoundingRect().topLeft() + QPointF( tmp, 0 );
    QPointF bottom = top + QPointF( 0.0,height() );
    return QLineF(top,bottom);
}


void TokenElement::removeChild( FormulaCursor* cursor, BasicElement* child )
{
    m_rawString.remove( cursor->position(), 1 );
}

bool TokenElement::acceptCursor( const FormulaCursor* cursor )
{
    Q_UNUSED( cursor )
    return true;
}

bool TokenElement::moveCursor(FormulaCursor* cursor) {
    if ( (cursor->direction()==MoveUp) ||
	 (cursor->direction()==MoveDown)) {
	//we can't move the cursor vertically
	return false;
    }
    if ( (cursor->isHome() && cursor->direction()==MoveLeft) ||
	 (cursor->isEnd() && cursor->direction()==MoveRight) ) {
	return BasicElement::moveCursor(cursor);
    }
    switch( cursor->direction() ) {
	case MoveLeft:
	    cursor->setPosition(cursor->position()-1);
	    break;
	case MoveRight:
	    cursor->setPosition(cursor->position()+1);
	    break;
    }
    return true;
}


double TokenElement::cursorOffset( const FormulaCursor* cursor ) 
{
    int counter = 0;
    QPainterPath tmppath;
    for( int i = 0; i < cursor->position(); i++ ) {
        if( m_rawString[ i ] != QChar::ObjectReplacementCharacter ) {
	    renderToPath( QString(m_rawString[i]), tmppath );
	}
        else {
	    m_glyphs[ counter ]->renderToPath( QString(), tmppath );
	    counter++;
        }
    }
    return tmppath.boundingRect().right();
}

QFont TokenElement::font() const
{
    return m_font;
}

bool TokenElement::readMathMLContent( const KoXmlElement& element )
{
    // iterate over all child elements ( possible embedded glyphs ) and put the text
    // content in the m_rawString and mark glyph positions with
    // QChar::ObjectReplacementCharacter
    GlyphElement* tmpGlyph;
    KoXmlNode node = element.firstChild();
    while( !node.isNull() ) {
        if( node.isElement() && node.toElement().tagName() == "mglyph" ) {
            tmpGlyph = new GlyphElement( this );
            m_rawString.append( QChar( QChar::ObjectReplacementCharacter ) );
            tmpGlyph->readMathML( node.toElement() );
            m_glyphs.append(tmpGlyph);
        }
        else if( node.isElement() )
            return false;
        else
            m_rawString.append( node.toText().data().trimmed() );

        node = node.nextSibling();
    }
    return true;
}

void TokenElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    // split the m_rawString into text content chunks that are divided by glyphs 
    // which are represented as ObjectReplacementCharacter and write each chunk
    QStringList tmp = m_rawString.split( QChar( QChar::ObjectReplacementCharacter ) );
    for ( int i = 0; i < tmp.count(); i++ ) {
        if( m_rawString.startsWith( QChar( QChar::ObjectReplacementCharacter ) ) ) {
            m_glyphs[ i ]->writeMathML( writer );
            if (i + 1 < tmp.count()) {
                writer->addTextNode( tmp[ i ] );
            }
        }
        else {
            writer->addTextNode( tmp[ i ] );
            if (i + 1 < tmp.count()) {
                m_glyphs[ i ]->writeMathML( writer );
            }
        }
    }
}

