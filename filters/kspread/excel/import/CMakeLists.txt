

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder ${CMAKE_BINARY_DIR}/filters/)


########### next target ###############

set(sidewinder_SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/cell.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/excel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/format.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/pole.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/sheet.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/ustring.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/value.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../sidewinder/workbook.cpp
)

set(excelimport_PART_SRCS excelimport.cc ${sidewinder_SRCS})

kde4_add_plugin(excelimport WITH_PREFIX ${excelimport_PART_SRCS})



target_link_libraries(excelimport koodf ${QT_QT3SUPPORT_LIBRARY} ${QT_QTCORE_LIBRARY} kostore komain)

install(TARGETS excelimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_excel_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


