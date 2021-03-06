include_directories( ${CMAKE_SOURCE_DIR}/kchart
                     ${CMAKE_SOURCE_DIR}/kchart/kdchart/include
                     ${CMAKE_SOURCE_DIR}/interfaces
                     ${CMAKE_SOURCE_DIR}/kchart/shape/dialogs
                     ${KOGUIUTILS_INCLUDES}
                     ${KOTEXT_INCLUDES} )

########### Flake Plugin library ###############

SET (chartshape_LIB_SRCS
    KDChartConvertions.cpp
    Axis.cpp
    DataSet.cpp
    CellRegion.cpp
    CellRegionStringValidator.cpp
    TableModel.cpp
    Legend.cpp
    TextLabelDummy.cpp
    PlotArea.cpp
    ProxyModel.cpp
    KDChartModel.cpp
    Surface.cpp
    ThreeDScene.cpp
    ThreeDTransformation.cpp
    ChartDocument.cpp
    ChartShape.cpp
    ChartTool.cpp
    ChartToolFactory.cpp
    ChartConfigWidget.cpp
    ChartTableView.cpp
    dialogs/TableEditorDialog.cpp
    dialogs/NewAxisDialog.cpp
    dialogs/AxisScalingDialog.cpp
    dialogs/CellRegionDialog.cpp
    ../commands/ChartTypeCommand.cpp
)

kde4_add_ui_files(chartshape_LIB_SRCS
    ChartConfigWidget.ui
    dialogs/ChartTableEditor.ui
    dialogs/NewAxisDialog.ui
    dialogs/AxisScalingDialog.ui
    dialogs/CellRegionDialog.ui
)

kde4_add_library(chartshapelib SHARED ${chartshape_LIB_SRCS})
target_link_libraries(
    chartshapelib
    kdchart
    flake
    kotext
    kostore
    komain
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )
set_target_properties(chartshapelib PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS chartshapelib ${INSTALL_TARGETS_DEFAULT_ARGS})

kde4_add_plugin(chartshape ChartShapeFactory.cpp )
target_link_libraries(
    chartshape
    chartshapelib
    kdchart
    flake
    kostore
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )
install(TARGETS chartshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES chartshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
