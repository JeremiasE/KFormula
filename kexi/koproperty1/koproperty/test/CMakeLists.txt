if(KDE4_BUILD_TESTS)

include_directories( ${CMAKE_SOURCE_DIR}/kexi/koproperty1 ${KOMAIN_INCLUDES} )

########### next target ###############

set(propertytest_SRCS main.cpp test.cpp )

kde4_add_executable(propertytest ${propertytest_SRCS})

target_link_libraries(propertytest  ${KDE4_KDECORE_LIBS} koproperty )
endif(KDE4_BUILD_TESTS)

########### install files ###############


