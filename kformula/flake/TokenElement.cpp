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

const QList<BasicElement*> TokenElement::childElements() const
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

void TokenElement::layout( const AttributeManager* am )
{
    kDebug()<<"Layouting";
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

bool TokenElement::insertChild( int position, BasicElement* child )
{
    if( child && child->elementType() == Glyph ) {
    //    m_rawString.insert( QChar( QChar::ObjectReplacementCharacter ) );
    //    m_glyphs.insert();
        return false;
    } else {
        return false;
    }
}


bool TokenElement::insertText ( int position, const QString& text )
{
    m_rawString.insert (position,text);
    return true;
}


QList<GlyphElement*> TokenElement::removeText ( int position, int length )
{
    QList<GlyphElement*> tmp;
    //find out, how many glyphs we have
    int counter=0;
    for (int i=position; i<position+length; ++i) {
        if (m_rawString[ position ] == QChar::ObjectReplacementCharacter) {
            counter++;
        }
    }
    
    int start=0;
    //find out where we should start removing glyphs
    if (counter>0) {
        for (int i=0; i<position; ++i) {
            if (m_rawString[position] == QChar::ObjectReplacementCharacter) {
                start++;
            }
        }
    } 
    for (int i=start; i<start+counter; ++i) {
        tmp.append(m_glyphs.takeAt(i));
    }
    m_rawString.remove(position,length);
    return tmp;
    
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


QLineF TokenElement::cursorLine(int position) const
{
    // inside tokens let the token calculate the cursor x offset
    double tmp = cursorOffset( position );
    QPointF top = absoluteBoundingRect().topLeft() + QPointF( tmp, 0 );
    QPointF bottom = top + QPointF( 0.0,height() );
    return QLineF(top,bottom);
}

bool TokenElement::acceptCursor( const FormulaCursor* cursor )
{
    Q_UNUSED( cursor )
    return true;
}

bool TokenElement::moveCursor(FormulaCursor* newcursor, FormulaCursor* oldcursor) {
    if ( (newcursor->direction()==MoveUp) ||
	 (newcursor->direction()==MoveDown) ||
	 (newcursor->isHome() && newcursor->direction()==MoveLeft) ||
	 (newcursor->isEnd() && newcursor->direction()==MoveRight) ) {
	return false;
    }
    switch( newcursor->direction() ) {
	case MoveLeft:
	    newcursor->setPosition(newcursor->position()-1);
	    break;
	case MoveRight:
	    newcursor->setPosition(newcursor->position()+1);
	    break;
    }
    return true;
}


double TokenElement::cursorOffset( const int position) const
{
    int counter = 0;
    QPainterPath tmppath;
    for( int i = 0; i < position; i++ ) {
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


void TokenElement::setText ( const QString& text )
{
    //TODO: check for ObjectReplacement characters
    m_rawString=text;
    m_glyphs.empty();
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
