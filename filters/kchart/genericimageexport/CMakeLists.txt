

include_directories(
	${CMAKE_SOURCE_DIR}/libs/flake
	${CMAKE_SOURCE_DIR}/kchart 
	${CMAKE_SOURCE_DIR}/kchart/shape
	${CMAKE_SOURCE_DIR}/kchart/kdchart/include
	${CMAKE_SOURCE_DIR}/interfaces 
	${CMAKE_SOURCE_DIR}/filters/libdialogfilter 
	${KDE4_INCLUDE_DIR} 
	${QT_INCLUDES}  )


########### next target ###############

set(kchartgenericimageexport_PART_SRCS genericimageexport.cpp ${libdialogfilter_SRCS})


kde4_add_plugin(kchartgenericimageexport WITH_PREFIX ${kchartgenericimageexport_PART_SRCS})

target_link_libraries(
    kchartgenericimageexport 
    kchartcommon
    kostore
    komain
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kchartgenericimageexport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kchart_bmp_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kchart_jpeg_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kchart_mng_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kchart_png_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kchart_xbm_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kchart_xpm_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})

