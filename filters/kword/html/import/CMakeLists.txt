include_directories(${KOGUIUTILS_INCLUDES} ${KOTEXT_INCLUDES})

########### next target ###############

set(htmlimport_PART_SRCS htmlimport.cpp khtmlreader.cpp kwdwriter.cpp )

kde4_add_plugin(htmlimport WITH_PREFIX ${htmlimport_PART_SRCS})

target_link_libraries(htmlimport  kotext ${KDE4_KHTML_LIBS} ${KDE4_KDEUI_LIBS}  ${QT_QT3SUPPORT_LIBRARY} kostore komain)

install(TARGETS htmlimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_html_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


