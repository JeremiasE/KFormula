include_directories(${CMAKE_SOURCE_DIR}/kspread ${KOGUIUTILS_INCLUDES})

set(kspread_plugin_tool_calendar_SRCS CalendarTool.cpp CalendarToolFactory.cpp CalendarToolWidget.cpp)
kde4_add_ui_files(kspread_plugin_tool_calendar_SRCS CalendarToolWidget.ui)
kde4_add_plugin(kspread_plugin_tool_calendar ${kspread_plugin_tool_calendar_SRCS})
target_link_libraries(kspread_plugin_tool_calendar kspreadcommon ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KDEUI_LIBS})
install(TARGETS kspread_plugin_tool_calendar DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspread_plugin_tool_calendar.desktop DESTINATION ${SERVICES_INSTALL_DIR})
