
set(kritaembossfilter_PART_SRCS kis_emboss_filter_plugin.cc kis_emboss_filter.cc )


kde4_add_plugin(kritaembossfilter ${kritaembossfilter_PART_SRCS})



target_link_libraries(kritaembossfilter kritaui)

install(TARGETS kritaembossfilter  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kritaembossfilter.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

