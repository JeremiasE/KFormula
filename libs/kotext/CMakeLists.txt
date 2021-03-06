include_directories( ${KOGUIUTILS_INCLUDES} ${KOMAIN_INCLUDES} )

add_subdirectory( tests )
add_subdirectory( styles/tests )
add_subdirectory( opendocument/tests )

########### next target ###############
set(kotext_LIB_SRCS
    KoTextShapeData.cpp
    KoText.cpp
    KoTextBlockData.cpp
    KoTextSelectionHandler.cpp
    KoTextBlockBorderData.cpp
    KoTextDocumentLayout.cpp
    KoTextOdfSaveHelper.cpp
    KoTextPaste.cpp
    KoTextDocument.cpp
    KoList.cpp

    KoTextEditingRegistry.cpp
    KoTextEditingFactory.cpp
    KoTextEditingPlugin.cpp

    KoInlineTextObjectManager.cpp
#    KoInlineObjectFactory.cpp
#    KoInlineObjectRegistry.cpp
    InsertVariableActionBase.cpp
    InsertVariableAction.cpp
    InsertNamedVariableAction.cpp
    InsertTextReferenceAction.cpp
    KoInlineObject.cpp
    KoVariable.cpp
    KoVariableManager.cpp
    KoNamedVariable.cpp
    KoVariableFactory.cpp
    KoVariableRegistry.cpp
    KoTextLocator.cpp
    KoTextReference.cpp
    KoTextAnchor.cpp
    KoTextShapeContainerModel.cpp
    KoBookmark.cpp
    KoBookmarkManager.cpp
    KoInlineNote.cpp
    KoRulerController.cpp
    FindDirection_p.cpp
    KoFindStrategy.cpp
    KoReplaceStrategy.cpp
    KoFind_p.cpp
    KoFind.cpp
    KoTextDebug.cpp
    KoTextPage.cpp

    styles/Styles_p.cpp
    styles/KoCharacterStyle.cpp
    styles/KoParagraphStyle.cpp
    styles/KoStyleManager.cpp
    styles/KoListStyle.cpp
    styles/KoListLevelProperties.cpp
    styles/ChangeFollower.cpp
    opendocument/KoTextSharedLoadingData.cpp
    opendocument/KoTextSharedSavingData.cpp
    opendocument/KoTextLoader.cpp
    opendocument/KoTextWriter.cpp

    changetracker/KoChangeTracker.cpp
    changetracker/KoChangeTrackerElement.cpp
    changetracker/KoDeleteChangeMarker.cpp
    KoTextDrag.cpp
)

kde4_add_ui_files(kotext_LIB_SRCS
   timedateformatwidget.ui
    KoParagDecorationTab.ui
    KoCompletionBase.ui
)

kde4_add_library(kotext SHARED ${kotext_LIB_SRCS})

target_link_libraries(kotext flake ${KDE4_KDEUI_LIBS} ${KDE4_KDE3SUPPORT_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KUTILS_LIBS} koguiutils )
target_link_libraries(kotext LINK_INTERFACE_LIBRARIES flake)

set_target_properties(kotext PROPERTIES
   VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION}
)
install(TARGETS kotext  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES texteditingplugin.desktop inlinetextobject.desktop textvariableplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
