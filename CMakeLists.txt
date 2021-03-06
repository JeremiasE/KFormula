project(koffice)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

if(KDE4_BUILD_TESTS)
    # only with this definition will the FOO_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

# define the generic version of the KOffice libraries here
# this makes it easy to advance it when the next KOffice release comes
set(GENERIC_KOFFICE_LIB_VERSION "5.0.0")
set(GENERIC_KOFFICE_LIB_SOVERSION "5")

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

# search packages used by KDE
set(KDE_MIN_VERSION "4.1.0")
find_package(KDE4 4.1.0 REQUIRED)
find_package(Qt4 4.5.0 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
find_package(Perl REQUIRED)
find_package(ZLIB REQUIRED)

find_package(LCMS REQUIRED)
set(REQUIRED_LCMS_VERSION 118)

if(LCMS_FOUND AND NOT LCMS_VERSION LESS ${REQUIRED_LCMS_VERSION})
    set(HAVE_REQUIRED_LCMS_VERSION TRUE)
else(LCMS_FOUND AND NOT LCMS_VERSION LESS ${REQUIRED_LCMS_VERSION})
    set(HAVE_REQUIRED_LCMS_VERSION FALSE)
endif(LCMS_FOUND AND NOT LCMS_VERSION LESS ${REQUIRED_LCMS_VERSION})

macro_log_feature(HAVE_REQUIRED_LCMS_VERSION "LittleCMS" "Color management engine" "http://www.littlecms.com" TRUE "1.18" "Required by KOffice")

find_package(PNG REQUIRED)

macro_optional_find_package(CreateResources)
macro_log_feature(CreateResources_FOUND "Create Resources" "Create Resources (brushes, palettes etc.)" "http://create.freedesktop.org" FALSE "" "Required by KOffice for optional extra resources.")

macro_optional_find_package(KdepimLibs)
macro_log_feature(KDEPIMLIBS_FOUND "KDE PIMLibs" "KDE Personal Information Management Libraries" "http://www.kde.org/" FALSE "" "Required by KPlato and the KDE address book integration (available as a module in KDE)")

macro_optional_find_package(Boost)
macro_log_feature(Boost_FOUND "Boost" "Boost C++ Libraries" "http://www.boost.org" FALSE "" "Required by KPresenter")

macro_optional_find_package(Eigen2)
macro_log_feature(EIGEN2_FOUND "Eigen" "C++ template library for linear algebra" "http://eigen.tuxfamily.org" FALSE "2.0" "Required by KSpread and Krita (available as a module in kdesupport)")

macro_optional_find_package(Blitz)
macro_log_feature(BLITZ_FOUND "QImageBlitz" "Image effect library" "http://www.kde.org" FALSE "" "Required by Krita (available in kdesupport)")

macro_optional_find_package(QCA2)
macro_log_feature(QCA2_FOUND "QCA" "Qt Cryptographic Architecture" "http://delta.affinix.com/qca" FALSE "2.0" "Required for encrypted OpenDocument files support (available as a module in kdesupport)")

set(EXIV2_MIN_VERSION "0.16")
macro_optional_find_package(Exiv2)
macro_log_feature(EXIV2_FOUND "Exiv2" "Image metadata library and tools" "http://www.exiv2.org" FALSE "0.16" "Required by Krita")


if (APPLE)
   find_package(Carbon REQUIRED)
endif (APPLE)


#Set the build of TextShape changetraker

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS})

if(WIN32)
  # detect oxygen icon dir at configure time based on KDEDIRS - there may be different package installation locations
  execute_process(COMMAND "${KDE4_KDECONFIG_EXECUTABLE}" --path icon OUTPUT_VARIABLE _dir ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  file(TO_CMAKE_PATH "${_dir}" __dir)
  find_path(KDE4_ICON_DIR oxygen PATHS
      ${__dir}
  )
  message(STATUS "using oxygen application icons from ${KDE4_ICON_DIR}")
else(WIN32)
    set (KDE4_ICON_DIR  ${CMAKE_INSTALL_PREFIX}/share/icons)
endif(WIN32)


include (MacroAdditionalCleanFiles)
include (MacroAddFileDependencies)

configure_file(config-prefix.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-prefix.h )

macro_optional_find_package(OpenEXR)
macro_bool_to_01(OPENEXR_FOUND HAVE_OPENEXR)
configure_file(config-openexr.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-openexr.h )

macro_optional_find_package(OpenGL)
set(HAVE_OPENGL 0)

if(OPENGL_FOUND)
    message(STATUS "Found OpenGL: ${OPENGL_LIBRARIES}")
    if(QT_QTOPENGL_FOUND)
        message(STATUS "Found Qt OpenGL support")
        set(HAVE_OPENGL 1)
    else(QT_QTOPENGL_FOUND)
        message(STATUS "Did NOT find Qt OpenGL support. Check your Qt configuration")
    endif(QT_QTOPENGL_FOUND)
else(OPENGL_FOUND)
    message(STATUS "Did NOT find OpenGL libraries")
endif(OPENGL_FOUND)

macro_log_feature(HAVE_OPENGL "OpenGL" "OpenGL support" "" FALSE "" "Required by parts of Krita")

configure_file(config-opengl.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-opengl.h )

# Look for OpenCTL
macro_optional_find_package(OpenCTL)

macro_log_feature(OPENCTL_FOUND "OpenCTL" "Free Color Transformation Language implementation" "http://www.openctl.org" FALSE "0.9.2" "Required for High Dynamic Range Color Spaces, YCbCr and LMS support")
configure_file(config-openctl.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-openctl.h )


include (TestBigEndian)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)
configure_file(config-endian.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-endian.h )

# for config.h and <toplevel/foo.h> includes (if any?)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

# kostore is at the bottom of stack, so it has the dependency on the kde4 includes.
set(KOSTORE_INCLUDES ${CMAKE_SOURCE_DIR}/libs/store ${CMAKE_BINARY_DIR}/libs/store ${KDE4_INCLUDES})

# koodf depends on kostore
set(KOODF_INCLUDES ${CMAKE_SOURCE_DIR}/libs/odf ${CMAKE_BINARY_DIR}/libs/odf ${KOSTORE_INCLUDES})

# komain depends on koodf
set(KOMAIN_INCLUDES ${CMAKE_SOURCE_DIR}/libs/main ${CMAKE_SOURCE_DIR}/libs/main/threadAction ${CMAKE_BINARY_DIR}/libs/main ${KOODF_INCLUDES})

# pigment depends on komain and lcms
set(PIGMENT_INCLUDES ${CMAKE_SOURCE_DIR}/libs/pigment ${CMAKE_SOURCE_DIR}/libs/pigment/colorspaces ${CMAKE_SOURCE_DIR}/libs/pigment/colorprofiles ${LCMS_INCLUDE_DIR} )

# flake depends on komain
set(FLAKE_INCLUDES ${CMAKE_SOURCE_DIR}/libs/flake ${CMAKE_SOURCE_DIR}/libs/flake/commands ${CMAKE_SOURCE_DIR}/libs/flake/tools ${CMAKE_BINARY_DIR}/libs/flake)

# koresources depends on pigment
set(KORESOURCES_INCLUDES ${CMAKE_SOURCE_DIR}/libs/resources ${CMAKE_BINARY_DIR}/libs/resources ${PIGMENT_INCLUDES})

# koguiutils depends on komain, flake and pigment
set(KOGUIUTILS_INCLUDES ${CMAKE_SOURCE_DIR}/libs/guiutils ${CMAKE_BINARY_DIR}/libs/guiutils ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${PIGMENT_INCLUDES} ${KORESOURCES_INCLUDES})

# The rest must be specified independently, in addition to one of the above three
set(KOTEXT_INCLUDES ${CMAKE_SOURCE_DIR}/libs/kotext/styles ${CMAKE_SOURCE_DIR}/libs/kotext/opendocument ${CMAKE_SOURCE_DIR}/libs/kotext ${CMAKE_SOURCE_DIR}/libs/kotext/changetracker ${CMAKE_BINARY_DIR}/libs/kotext)
set(KFORMULA_INCLUDES ${CMAKE_SOURCE_DIR}/libs/kformula ${CMAKE_BINARY_DIR}/libs/kformula)
set(KOKROSS_INCLUDES ${CMAKE_SOURCE_DIR}/libs/kokross ${CMAKE_BINARY_DIR}/libs/kokross)

# kopageapp
set(KOPAGEAPP_INCLUDES ${CMAKE_SOURCE_DIR}/libs/kopageapp ${CMAKE_SOURCE_DIR}/libs/kopageapp/commands ${CMAKE_BINARY_DIR}/libs/kopageapp )

# Those are included with e.g. <koproperty/set.h>
set(KOPROPERTY_INCLUDES ${CMAKE_SOURCE_DIR}/libs ${CMAKE_BINARY_DIR}/libs)

if(WIN32)
  set(SHOULD_BUILD_KRITA FALSE)
else(WIN32)
  set(SHOULD_BUILD_KRITA TRUE)
endif(WIN32)

if(NOT BLITZ_FOUND OR NOT EIGEN2_FOUND OR NOT EXIV2_FOUND)
   set(SHOULD_BUILD_KRITA FALSE)
endif(NOT BLITZ_FOUND OR NOT EIGEN2_FOUND OR NOT EXIV2_FOUND)

#Check if build kpresenter here to be able to test if we can build filters/kpresenter
if(Boost_FOUND)
    set(SHOULD_BUILD_KPRESENTER TRUE)
else(Boost_FOUND)
    set(SHOULD_BUILD_KPRESENTER FALSE)
endif(Boost_FOUND)

if(WIN32)
    set(LIB_INSTALL_DIR ${LIB_INSTALL_DIR}
                        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
                        LIBRARY ${INSTALL_TARGETS_DEFAULT_ARGS}
                        ARCHIVE ${INSTALL_TARGETS_DEFAULT_ARGS} )
endif(WIN32)

add_subdirectory(libs)
add_subdirectory(interfaces)
add_subdirectory(cmake)

set(SHOULD_BUILD_KARBON TRUE)

macro_optional_add_subdirectory(karbon)

if(EIGEN2_FOUND)
   macro_optional_add_subdirectory(kspread)
    if(BUILD_kspread)
        set(SHOULD_BUILD_KSPREAD TRUE)
    else(BUILD_kspread)
        set(SHOULD_BUILD_KSPREAD FALSE)
    endif(BUILD_kspread)
else(EIGEN2_FOUND)
    set(SHOULD_BUILD_KSPREAD FALSE)
endif(EIGEN2_FOUND)

if(SHOULD_BUILD_KPRESENTER)
    macro_optional_add_subdirectory(kpresenter)
endif(SHOULD_BUILD_KPRESENTER)

macro_optional_add_subdirectory(kdgantt)
macro_optional_add_subdirectory(kchart)

FIND_PROGRAM(BZIP2_EXECUTABLE NAMES bzip2 )
macro_log_feature(BZIP2_EXECUTABLE "bzip2" "High-quality data compressor" "http://www.bzip.org" FALSE "" "Required by Kexi")

OPTION(BUILD_kexi "Build kexi" FALSE)
if(BUILD_kexi)
    if( BZIP2_EXECUTABLE )
        macro_optional_add_subdirectory(kexi)
    endif( BZIP2_EXECUTABLE )
endif(BUILD_kexi)

macro_optional_add_subdirectory(kformula)
macro_optional_add_subdirectory(kivio)
macro_optional_add_subdirectory(kounavail)

if(KdepimLibs_FOUND)
	macro_optional_add_subdirectory(kplato)
endif(KdepimLibs_FOUND)

macro_optional_add_subdirectory(krita)
macro_optional_add_subdirectory(kword)
add_subdirectory(pics)
add_subdirectory(plugins)
add_subdirectory(servicetypes)
add_subdirectory(templates)
add_subdirectory(tools)
add_subdirectory(filters)
macro_optional_add_subdirectory(doc)

MACRO_DISPLAY_FEATURE_LOG()

ADD_CUSTOM_TARGET(apidox doc/api/gendocs.pl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

install( FILES
         ${CMAKE_CURRENT_BINARY_DIR}/config-openexr.h
         DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)
