include_directories( ${KOMAIN_INCLUDES})

########### next target ###############

set(clipartthumbnail_SRCS clipartcreator.cpp )

kde4_add_plugin(clipartthumbnail ${clipartthumbnail_SRCS})



target_link_libraries(clipartthumbnail ${KDE4_KIO_LIBS} komain )

install(TARGETS clipartthumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kofficethumbnail_SRCS kofficecreator.cpp )

kde4_add_plugin(kofficethumbnail ${kofficethumbnail_SRCS})



target_link_libraries(kofficethumbnail ${KDE4_KPARTS_LIBS} ${QT_QT3SUPPORT_LIBRARY} komain )

install(TARGETS kofficethumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  clipartthumbnail.desktop kofficethumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


