
set(kritaoilpaintfilter_PART_SRCS kis_oilpaint_filter_plugin.cc kis_oilpaint_filter.cc )


kde4_add_plugin(kritaoilpaintfilter ${kritaoilpaintfilter_PART_SRCS})



target_link_libraries(kritaoilpaintfilter kritaui)

install(TARGETS kritaoilpaintfilter  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kritaoilpaintfilter.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
