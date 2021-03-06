/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPROPERTY_FACTORY_H
#define KPROPERTY_FACTORY_H

#include "koproperty_global.h"
#include <QObject>

namespace KoProperty
{

class Widget;
class CustomProperty;
class Property;
class FactoryManagerInternal;

///*! A pointer to factory function which creates and returns widget for a given property type.*/
//typedef Widget *(*createWidget) (Property*);
//typedef CustomProperty *(*createCustomProperty) (Property*);

//! \brief A prototype for custom property factory
class KOPROPERTY_EXPORT CustomPropertyFactory : public QObject
{
public:
    explicit CustomPropertyFactory(QObject *parent);
    virtual ~CustomPropertyFactory();

    /*! \return a new instance of custom property for \a parent.
     Implement this for property types you want to support.
     Use parent->type() to get type of the property. */
    virtual CustomProperty* createCustomProperty(Property *parent) = 0;

    /*! \return a new instance of custom property for \a property.
     Implement this for property editor types you want to support.
     Use parent->type() to get type of the property. */
    virtual Widget* createCustomWidget(Property *property) = 0;
};

//! \brief Manages factories providing custom editors and properties.
/*! This class is static, you don't need to create an instance of it. It's used to enable the
     custom property/editors system.
     You may want to create your own property types and/or editors to:

     - Create your own editors for some special kind of properties, not included in
       KProperty basic editors;

     - Create composed properties, which contain more than one value. Child
       items will then be created in the Editor (that's how rect, size properties are created).

     \section custom_prop Using Custom Properties
     To create a custom property, create a subclass of \ref CustomProperty class. You need to implement
     some virtual functions, to customize the behaviour of your property
     (see \ref CustomProperty api doc).\n
     Then, you need to register the new created type, using \ref registerFactoryForProperty().
     The second parameter is an instance of CustomPropertyFactory-derived class
     implementing CustomPropertyFactory::createCustomProperty() method.\n
     To create a property of this type, just use the normal constructor, overriding
     the type parameter with the type you registered.

     \section custom_prop_composed Using Custom Properties to create composed properties
     Use a composed property when you need more than one editor for a property. Examples
     are rect, size or point properties.
     If you create a composed property, both parent and children properties must have custom
     (different) types.
     Child properties are created in CustomProperty constructor of the <b>parent</b> type,
     by adding CustomProperty::property() as parent in Property constructor.\n
     Child properties should return handleValue() == true and in CustomProperty::setValue(),
     parent's Property::setValue() should be called, making sure that useCustomProperty argument is set
     to false.\n
     Parent's handleValue() should be set to false, unless you cannot store the property in a QVariant.
     You just need to update children's value, making sure that useCustomProperty argument is set
     to false.

     \section custom_editor Using Custom Editors
     First, create a subclass of Widget, and implement all the virtuals you need to tweak
     the property editor. You can find examples of editors in the src/editors/ directory.\n
     Then, register it using \ref registerFactoryForEditor(), as for properties (see test/ dir
     for an example of custom editor). You can also override the editor provided by KoProperty,
     if it doesn't fit your needs (if you have created a better editor,
     send us the code, and it may get included in KProperty library).\n
     To use your new editor, just create properties with the type number you registered using
     \ref registerFactoryForEditor() . Your editor will automatically appear in the Editor.

     \section custom_prop_composed Using Custom Properties with value that cannot be stored in a QVariant
     You then need to set handleValue() to true. The Widget you create also have
     to call directly CustomProperty member to store the value. just make sure you call emitPropertyChanged()
     when the proerty value changes. Also make sure to avoid infinite recursion if you use children properties.

   \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
 */
class KOPROPERTY_EXPORT FactoryManager : public QObject
{
public:
    /*! Registers a custom factory \a factory for handling property editor for \a editorType.
    This custom factory will be used before defaults when widgetForProperty() is called.
    \a creator is not owned by this Factory object, but it's good idea
    to instantiate CustomPropertyFactory object itself as a child of Factory parent. For example:
    \code
      MyCustomPropertyFactory *f = new MyCustomPropertyFactory(KoProperty::Factory::self());
      KoProperty::Factory::self()->registerEditor( MyCustomType, f );
    \endcode */
    void registerFactoryForEditor(int editorType, CustomPropertyFactory *factory);

    /*! Registers custom factory \a factory for handling property editors for \a editorTypes.
     @see registerFactoryForEditor(). */
    void registerFactoryForEditors(const QList<int> &editorTypes, CustomPropertyFactory *factory);

    /*! \return custom factory for type \a type or NULL if there
     is no such property type registered.
     To create a custom widget createWidgetForProperty() should be rather used. */
    CustomPropertyFactory *factoryForEditorType(int type);

    /*! Creates and returns the editor for given property type.
    Warning: editor and viewer widgets won't have parent widget. Property editor
    cares about reparenting and deletion of returned widgets in machines.
    If \a createWidget is false, just create child properties, not widget.*/
    Widget* createWidgetForProperty(Property *property);

    /*! Registers a custom factory that handles a CustomProperty of a type \a type.
     This function will be called every time a property of \a type is created. */
    void registerFactoryForProperty(int propertyType, CustomPropertyFactory *factory);

    /*! Registers a custom property factory that handles a CustomProperty for \a types.
     @see registerFactoryForProperty() */
    void registerFactoryForProperties(const QList<int> &propertyTypes,
                                      CustomPropertyFactory *factory);

    /*! This function is called in Property::Property() to create (optional)
      custom property. It creates the custom property for built-in types, or
      calls one of createCustomProperty function previously registered for other types. */
    CustomProperty* createCustomProperty(Property *parent);

    /*! \return a pointer to a property factory instance.*/
    static FactoryManager* self();

protected:
    FactoryManager();
    ~FactoryManager();

    class Private;
    Private * const d;
    friend class FactoryManagerInternal;
};

}

#endif
