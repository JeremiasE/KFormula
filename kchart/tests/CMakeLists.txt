set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_DIRECTORY} )

include_directories( ${CMAKE_SOURCE_DIR}/kchart
                     ${CMAKE_SOURCE_DIR}/kchart/shape
                     ${CMAKE_SOURCE_DIR}/kchart/kdchart/include
                     ${CMAKE_SOURCE_DIR}/interfaces
                     ${CMAKE_SOURCE_DIR}/kchart/shape/dialogs
                     ${KOGUIUTILS_INCLUDES}
                     ${KOTEXT_INCLUDES} )

########### next target ###############
set( TestProxyModel_test_SRCS TestProxyModel.cpp )
kde4_add_unit_test( TestProxyModel TESTNAME kchart-TestProxyModel ${TestProxyModel_test_SRCS} )
target_link_libraries( TestProxyModel ${KDE4_KDECORE_LIBS} flake ${QT_QTTEST_LIBRARY} chartshapelib )

