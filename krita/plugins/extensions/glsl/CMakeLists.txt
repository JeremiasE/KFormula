set(kritaglsl_PART_SRCS glsl.cc dlg_glsl.cc kis_glsl_widget.cc)

kde4_add_ui_files(kritaglsl_PART_SRCS wdg_glsl.ui )
kde4_add_plugin(kritaglsl ${kritaglsl_PART_SRCS})

target_link_libraries(kritaglsl kritaui ${QT_QTOPENGL_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})

install(TARGETS kritaglsl  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  kritaglsl.rc  DESTINATION ${DATA_INSTALL_DIR}/kritaplugins)
install( FILES  kritaglsl.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
