

########### next target ###############

set(kritaycbcrplugin_PART_SRCS ycbcr_plugin.cc kis_ycbcr_u16_colorspace.cc kis_ycbcr_u8_colorspace.cc )

kde4_add_plugin(kritaycbcrplugin ${kritaycbcrplugin_PART_SRCS})



target_link_libraries(kritaycbcrplugin pigmentcms)

install(TARGETS kritaycbcrplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kritaycbcrplugin.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

install(FILES
    ycbcr_16.ctlp
    ycbcr_8.ctlp
    DESTINATION ${DATA_INSTALL_DIR}/pigmentcms/ctlprofiles)
