


add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


########### next target ###############

set(wmlimport_PART_SRCS wmlimport.cc wmlparser.cpp )


kde4_add_plugin(wmlimport WITH_PREFIX ${wmlimport_PART_SRCS})



target_link_libraries(
    wmlimport
    komain
    kwordexportfilters
    ${KDE4_KDECORE_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS wmlimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(wmlexport_PART_SRCS wmlexport.cc )

kde4_add_plugin(wmlexport WITH_PREFIX ${wmlexport_PART_SRCS})



target_link_libraries(
    wmlexport
    kwordexportfilters
    komain
    ${KDE4_KDECORE_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS wmlexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_wml_import.desktop kword_wml_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


