

add_subdirectory( templates )




########### next target ###############

set(kritacmykplugin_PART_SRCS cmyk_plugin.cc kis_cmyk_colorspace.cc )


kde4_add_plugin(kritacmykplugin ${kritacmykplugin_PART_SRCS})



target_link_libraries(
    kritacmykplugin
    pigmentcms
    ${LCMS_LIBRARIES}
    ${KDE4_KDECORE_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    )

install(TARGETS kritacmykplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kritacmykplugin.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

if(MSVC)
  # avoid "cannot open file 'LIBC.lib'" error
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
endif(MSVC)

