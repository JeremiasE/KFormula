project( textPlugin)

add_subdirectory( pics )
add_subdirectory( tests )

include_directories( ${KOGUIUTILS_INCLUDES} ${KOTEXT_INCLUDES} ${KDE4_INCLUDE_DIR}/threadweaver/)

########### Flake Plugin library ###############

SET ( textshape_SRCS
    TextPlugin.cpp
    TextShape.cpp
    TextShapeFactory.cpp
    TextTool.cpp
    TextToolFactory.cpp
    Layout.cpp
    ListItemsHelper.cpp

    ChangeTracker.cpp
    TextChanges.cpp
    TextChange.cpp

    dialogs/StylesWidget.cpp
    dialogs/SimpleStyleWidget.cpp
    dialogs/ParagraphLayout.cpp
    dialogs/ParagraphIndentSpacing.cpp
    dialogs/ParagraphDecorations.cpp
    dialogs/ParagraphBulletsNumbers.cpp
    dialogs/ParagraphSettingsDialog.cpp
    dialogs/ListsSpinBox.cpp
    dialogs/StylesModel.cpp
    dialogs/StyleManager.cpp
    dialogs/StyleManagerDialog.cpp
    dialogs/ParagraphGeneral.cpp
    dialogs/NewStyleWidget.cpp
    dialogs/CharacterGeneral.cpp
    dialogs/CharacterHighlighting.cpp
    dialogs/InsertCharacter.cpp
    dialogs/FontDia.cpp
    dialogs/FontLayoutTab.cpp
    dialogs/FontDecorations.cpp
    dialogs/FontTab.cpp
    dialogs/LanguageTab.cpp
    dialogs/FormattingPreview.cpp
    dialogs/StyleManagerWelcome.cpp

    commands/ChangeListCommand.cpp
    commands/ChangeListLevelCommand.cpp
    commands/ListItemNumberingCommand.cpp
    commands/TextCommandBase.cpp
)

kde4_add_ui_files(textshape_SRCS
    dialogs/SimpleStyleWidget.ui
    dialogs/StylesWidget.ui
    dialogs/ParagraphLayout.ui
    dialogs/ParagraphIndentSpacing.ui
    dialogs/ParagraphDecorations.ui
    dialogs/ParagraphBulletsNumbers.ui
    dialogs/StyleManager.ui
    dialogs/ParagraphGeneral.ui
    dialogs/NewStyleWidget.ui
    dialogs/CharacterGeneral.ui
    dialogs/CharacterHighlighting.ui
    dialogs/StyleManagerWelcome.ui

    dialogs/FontDecorations.ui
    dialogs/FontLayoutTab.ui
    dialogs/LanguageTab.ui
)

kde4_add_plugin(textshape ${textshape_SRCS})

target_link_libraries(textshape
    koguiutils
    kotext
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
)
install(TARGETS textshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES textshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
#kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
