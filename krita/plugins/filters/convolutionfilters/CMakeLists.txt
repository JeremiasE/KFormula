
set(kritaconvolutionfilters_PART_SRCS
   convolutionfilters.cc
   kis_convolution_filter.cc )


kde4_add_plugin(kritaconvolutionfilters ${kritaconvolutionfilters_PART_SRCS})



target_link_libraries(kritaconvolutionfilters kritaui)

install(TARGETS kritaconvolutionfilters  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kritaconvolutionfilters.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

