set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories(  ${KOMAIN_INCLUDES} ${KOGUIUTILS_INCLUDES} )

########### next target ###############

set(zoomhandler_test_SRCS zoomhandler_test.cpp )

kde4_add_unit_test(zoomhandler_test TESTNAME libs-koguiutils-zoomhandler_test  ${zoomhandler_test_SRCS})

target_link_libraries(zoomhandler_test  ${KDE4_KDEUI_LIBS} koguiutils komain ${QT_QTTEST_LIBRARY})

########### next target ###############

set(zoomcontroller_test_SRCS zoomcontroller_test.cpp )


kde4_add_unit_test(zoomcontroller_test TESTNAME libs-koguiutils-zoomcontroller_test  ${zoomcontroller_test_SRCS})

target_link_libraries(zoomcontroller_test  ${KDE4_KDEUI_LIBS} koguiutils komain ${QT_QTTEST_LIBRARY})

########### end ###############

########### next target ###############

set(KoProgressUpdater_test_SRCS KoProgressUpdater_test.cpp )
kde4_add_unit_test(KoProgressUpdaterTest TESTNAME libs-koguiutils-KoProgressUpdaterTest ${KoProgressUpdater_test_SRCS})
target_link_libraries(KoProgressUpdaterTest  ${KDE4_KDEUI_LIBS} ${KDE4_THREADWEAVER_LIBRARY} koguiutils
                                             komain ${QT_QTTEST_LIBRARY})
