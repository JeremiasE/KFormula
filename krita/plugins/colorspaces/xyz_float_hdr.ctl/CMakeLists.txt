set(kritaxyzfloathdrplugin_PART_SRCS xyz_float_hdr.cc kis_xyz_f32_hdr_colorspace.cc )

kde4_add_plugin(kritaxyzfloathdrplugin ${kritaxyzfloathdrplugin_PART_SRCS})

target_link_libraries(kritaxyzfloathdrplugin pigmentcms)

install(TARGETS kritaxyzfloathdrplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


install( FILES  kritaxyzfloathdrplugin.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

install(FILES
    xyzaf32.ctlp
    DESTINATION ${DATA_INSTALL_DIR}/pigmentcms/ctlprofiles)
