


add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


########### next target ###############

set(asciiimport_PART_SRCS asciiimport.cc ImportDialog.cc )


kde4_add_ui3_files(asciiimport_PART_SRCS ImportDialogUI.ui )

kde4_add_plugin(asciiimport WITH_PREFIX ${asciiimport_PART_SRCS})

target_link_libraries(asciiimport ${QT_QT3SUPPORT_LIBRARY}  ${KDE4_KDEUI_LIBS} komain kwordexportfilters)

install(TARGETS asciiimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(asciiexport_PART_SRCS asciiexport.cc ExportDialog.cc )

kde4_add_ui3_files(asciiexport_PART_SRCS ExportDialogUI.ui )

kde4_add_plugin(asciiexport WITH_PREFIX ${asciiexport_PART_SRCS})



target_link_libraries(asciiexport  ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KDEUI_LIBS} komain kwordexportfilters)

install(TARGETS asciiexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_ascii_import.desktop kword_ascii_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



