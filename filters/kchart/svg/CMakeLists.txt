

include_directories(
    ${CMAKE_SOURCE_DIR}/libs/flake
    ${CMAKE_SOURCE_DIR}/kchart
    ${CMAKE_SOURCE_DIR}/kchart/shape
    ${CMAKE_SOURCE_DIR}/kchart/kdchart/include
    ${CMAKE_SOURCE_DIR}/interfaces
    ${KDE4_INCLUDES}
)


########### next target ###############

set(kchartsvgexport_PART_SRCS svgexport.cc )


kde4_add_plugin(kchartsvgexport WITH_PREFIX ${kchartsvgexport_PART_SRCS})



target_link_libraries(
    kchartsvgexport
    kchartcommon
    kostore
    komain
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kchartsvgexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kchart_svg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



