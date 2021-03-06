set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${KOTEXT_INCLUDES} )

if(MSVC)
  # avoid "cannot open file 'LIBC.lib'" error
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
endif(MSVC)

# all tests for obsolete classes
#   ########### kotextformattertest ###############
#
#   set(kotextformattertest_SRCS kotextformattertest.cpp )
#
#   kde4_add_executable(kotextformattertest RUN_UNINSTALLED ${kotextformattertest_SRCS})
#
#   target_link_libraries(kotextformattertest kotext)
#
#   ########### kobordertest ###############
#
#   set(kobordertest_SRCS kobordertest.cpp )
#
#   kde4_add_executable(kobordertest RUN_UNINSTALLED ${kobordertest_SRCS})
#
#   target_link_libraries(kobordertest kotext)
#
#   ########### kovariabletest ###############
#
#   set(kovariabletest_SRCS kovariabletest.cpp)
#
#   kde4_add_executable(kovariabletest RUN_UNINSTALLED ${kovariabletest_SRCS})
#
#   target_link_libraries(kovariabletest kotext ${QT_QTTEST_LIBRARY})
#
#   add_test(kotext-kovariabletest ${CMAKE_CURRENT_BINARY_DIR}/kovariabletest)
#
#   ########### KoUserStyleTester ###############
#
#   set(KoUserStyleTester_SRCS KoUserStyleTester.cpp)
#
#   kde4_add_executable(KoUserStyleTester RUN_UNINSTALLED ${KoUserStyleTester_SRCS})
#
#   target_link_libraries(KoUserStyleTester kotext ${QT_QTTEST_LIBRARY})
#
#   add_test(kotext-KoUserStyleTester ${CMAKE_CURRENT_BINARY_DIR}/KoUserStyleTester)
#
#   ########### end ###############

########### next target ###############

set(TestDocumentLayout_test_SRCS TestDocumentLayout.cpp)
kde4_add_unit_test(TestDocumentLayout1 TESTNAME libs-kotext-TestDocumentLayout  ${TestDocumentLayout_test_SRCS})
target_link_libraries(TestDocumentLayout1 kotext ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestBorder_test_SRCS TestBorder.cpp)
kde4_add_unit_test(TestBorder TESTNAME libs-kotext-TestBorder  ${TestBorder_test_SRCS})
target_link_libraries(TestBorder kotext ${QT_QTTEST_LIBRARY})
