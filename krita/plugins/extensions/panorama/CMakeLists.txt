
set(kritapanorama_PART_SRCS
    panorama.cc
    imageviewer.cc
    kis_images_blender.cpp
# image alignement framework
    imagealignment/harris_detector.cpp
    imagealignment/matching.cpp
    imagealignment/kis_control_points.cpp
    imagealignment/kis_image_alignment.cpp
    imagealignment/kis_image_alignment_model.cpp
    imagealignment/models/kis_image_alignment_projective_model.cpp
    imagealignment/kis_interest_points_detector.cpp
    )


include_directories( ${EIGEN2_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imagealignment)
kde4_add_ui_files(kritapanorama_PART_SRCS wdgpanoramacreation.ui )

kde4_add_plugin(kritapanorama ${kritapanorama_PART_SRCS})

target_link_libraries(kritapanorama kritaui)

install(TARGETS kritapanorama  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  panorama.rc  DESTINATION ${DATA_INSTALL_DIR}/kritaplugins)
install( FILES  kritapanorama.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

