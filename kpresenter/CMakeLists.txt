project(kpresenter)

include(CheckPrototypeExists)

check_prototype_exists(DPMSCapable "X11/Xlib.h;X11/extensions/dpms.h" HAVE_DPMSCAPABLE_PROTO)
macro_bool_to_01(X11_dpms_FOUND HAVE_DPMS) # kpresenter

add_subdirectory( part )


#configure_file(config-kpresenter.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kpresenter.h )
#
#include_directories(
#		${KOTEXT_INCLUDES}
#		${CMAKE_SOURCE_DIR}/libs/kformula
#		${KOGUIUTILS_INCLUDES}
#		${KOPAINTER_INCLUDES})
#
#include_directories( ${X11_dpms_INCLUDE_PATH} )
#
#add_subdirectory( autoforms )
add_subdirectory( pics )
add_subdirectory( templates )
add_subdirectory( plugins )
add_subdirectory( data )
add_subdirectory( styles )
#add_subdirectory( dtd )
#add_subdirectory( slideshow )
#add_subdirectory( toolbar )
#
#
#
#
############ next target ###############
#set(libaf_SRCS
#		autoformEdit/ATFInterpreter.cpp
#		autoformEdit/AFChoose.cpp
#		)
#
#
#set(kpresenterprivate_LIB_SRCS ${libaf_SRCS}
#   KoPointArray.cpp
#   KPrCanvas.cpp
#   KPrPage.cpp
#   KPrFindReplace.cpp
#   KPrBackDia.cpp
#   KPrWebPresentation.cpp
#   KPrView.cpp
#   KPrMSPresentationSetup.cpp
#   KPrBrush.cpp
#   KPrDocument.cpp
#   KPrSoundPlayer.cpp
#   KPrPgConfDia.cpp
#   KPrEffectDia.cpp
#   KPrCommand.cpp
#   KPrUtils.cpp
#   KPrEffectHandler.cpp
#   KPrPageEffects.cpp
#   KPrTransEffectDia.cpp
#   KPrObject.cpp
#   KPrLineObject.cpp
#   KPrRectObject.cpp
#   KPrEllipseObject.cpp
#   KPrAutoformObject.cpp
#   KPrTextObject.cpp
#   KPrPixmapObject.cpp
#   KPrPieObject.cpp
#   KPrPartObject.cpp
#   KPrGroupObject.cpp
#   KPrBackground.cpp
#   KPrGradient.cpp
#   KPrGradientCollection.cpp
#   KPrGotoPage.cpp
#   KPrFactory.cpp
#   KPrSideBar.cpp
#   KPrConfig.cpp
#   KPrVariableCollection.cpp
#   KPrTextDocument.cpp
#   KPrPointObject.cpp
#   KPrFreehandObject.cpp
#   KPrPolylineObject.cpp
#   KPrBezierCurveObject.cpp
#   KPrPolygonObject.cpp
#   KPrNoteBar.cpp
#   KPrPresDurationDia.cpp
#   KPrSVGPathParser.cpp
#   KPrShadowDialogImpl.cpp
#   KPrTextPreview.cpp
#   KPrRotationDialogImpl.cpp
#   #KPrStyleManager.cpp
#   KPrBgSpellCheck.cpp
#   KPrMoveHelpLineDia.cpp
#   KPrDuplicateObjDia.cpp
#   KPrClosedLineObject.cpp
#   KPrImportStyleDia.cpp
#   KPrImageEffectDia.cpp
#   KPrCustomSlideShowDia.cpp
#   KPrPBPreview.cpp
#   KPrPenStyleWidget.cpp
#   KPrRectPreview.cpp
#   KPrRectProperty.cpp
#   KPrPiePreview.cpp
#   KPrPolygonPreview.cpp
#   KPrPicturePreview.cpp
#   KPrBrushProperty.cpp
#   KPrGeneralProperty.cpp
#   KPrPolygonProperty.cpp
#   KPrPieProperty.cpp
#   KPrPictureProperty.cpp
#   KPrMarginWidget.cpp
#   KPrTextProperty.cpp
#   KPrObjectProperties.cpp
#   KPrPropertyEditor.cpp
#   KPrSlideTransitionDia.cpp
#   KPrPrinterDlg.cpp
#   KPrAutoFormObjectAdaptor.cpp
#   KPrFreehandObjectAdaptor.cpp
#   KPrObjectAdaptor.cpp
#   KPrPixmapObjectAdaptor.cpp
#   KPrQuadricBezierCurveObjectAdaptor.cpp
#   KPrCubicBezierCurveObjectAdaptor.cpp
#   KPrLineObjectAdaptor.cpp
#   KPrPolygonObjectAdaptor.cpp
#   KPrRectObjectAdaptor.cpp
#   KPrViewAdaptor.cpp
#   KPrDocumentAdaptor.cpp
#   KPrObject2DAdaptor.cpp
#   KPrPieObjectAdaptor.cpp
#   KPrPolyLineObjectAdaptor.cpp
#   KPrTextObjectAdaptor.cpp
#)
#
#kde4_automoc(${kpresenterprivate_LIB_SRCS})
#
#kde4_add_ui3_files(kpresenterprivate_LIB_SRCS
#   insertpagedia.ui
#   shadowdialog.ui
#   )
#
#kde4_add_ui_files(kpresenterprivate_LIB_SRCS brushpropertyui.ui rectpropertyui.ui marginui.ui gradientpropertyui.ui imageEffectBase.ui slidetransitionwidget.ui piepropertyui.ui rotationpropertyui.ui penstyle.ui polygonpropertyui.ui picturepropertyui.ui)
#
#kde4_add_library(kpresenterprivate SHARED ${kpresenterprivate_LIB_SRCS})
#
#target_link_libraries(kpresenterprivate  ${KDE4_KDE3SUPPORT_LIBS} koguiutils kotext kopainter )
#
#set_target_properties(kpresenterprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
#install(TARGETS kpresenterprivate  ${INSTALL_TARGETS_DEFAULT_ARGS})
#
#
############ next target ###############
#
#set(kpresenterpart_PART_SRCS KPrFactoryInit.cpp )
#
##
#kde4_add_plugin(kpresenterpart WITH_PREFIX ${kpresenterpart_PART_SRCS})
#
#
#
#target_link_libraries(kpresenterpart  ${KDE4_KDE3SUPPORT_LIBS} kpresenterprivate komain koguiutils)
#
#install(TARGETS kpresenterpart  DESTINATION ${PLUGIN_INSTALL_DIR})
#
#
############ next target ###############
#
#set(kpresenter_KDEINIT_SRCS main.cpp )
#
##
#kde4_add_kdeinit_executable( kpresenter ${kpresenter_KDEINIT_SRCS})
#
#target_link_libraries(kdeinit_kpresenter  ${KDE4_KDE3SUPPORT_LIBS} komain koguiutils )
#
#install(TARGETS kdeinit_kpresenter  ${INSTALL_TARGETS_DEFAULT_ARGS})
#
#target_link_libraries( kpresenter kdeinit_kpresenter )
#install(TARGETS kpresenter  DESTINATION ${BIN_INSTALL_DIR})
#
############ install files ###############
#
#install( FILES  kpresenter.rc kpresenter_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kpresenter)
#install(PROGRAMS kprconverter.pl DESTINATION ${BIN_INSTALL_DIR} )
