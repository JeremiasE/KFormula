project(kritafilters)

macro_optional_find_package(PNG)
macro_log_feature(PNG_FOUND "png" "Official PNG reference library" "http://www.libpng.org/pub/png/libpng.html" FALSE "" "Required by the Krita PNG filter")

macro_optional_find_package(OpenEXR)
macro_log_feature(OPENEXR_FOUND "OpenEXR" "High dynamic-range (HDR) image file format" "http://www.openexr.com" FALSE "" "Required by the Krita OpenEXR filter")

macro_optional_find_package(TIFF)
macro_log_feature(TIFF_FOUND "tiff" "TIFF Library and Utilities" "http://www.remotesensing.org/libtiff" FALSE "" "Required by the Krita TIFF filter")

macro_optional_find_package(JPEG)
macro_log_feature(JPEG_FOUND "jpeg" "Free library for JPEG image compression" "http://www.ijg.org" FALSE "" "Required by the Krita JPEG filter")

macro_optional_find_package(Poppler)
macro_log_feature( POPPLER_FOUND "poppler-qt4" "The Poppler Qt4 interface library" "http://poppler.freedesktop.org" FALSE "" "Required by the Krita PDF filter (version 0.6.x is currently required)")

macro_optional_find_package(Kdcraw)
macro_log_feature( KDCRAW_FOUND "kdcrw" "The KDCraw library" "http://www.digikam.org/" FALSE "" "Required by the Krita RAW filter")

macro_optional_find_package(GraphicsMagick)
macro_log_feature(GRAPHICSMAGICK_FOUND "GraphicsMagick" "GraphicsMagick Image Processing System" "http://www.graphicsmagick.org" FALSE "1.1" "Required by the Krita GraphicsMagick filter for various file formats (including PSD, XCF)")

include_directories(${KRITA_INCLUDES})

if(KDCRAW_FOUND)
    add_subdirectory( raw )
endif(KDCRAW_FOUND)

if(JPEG_FOUND)
	add_subdirectory(jpeg)
endif(JPEG_FOUND)

if(TIFF_FOUND)
	add_subdirectory(tiff)
endif(TIFF_FOUND )

if(PNG_FOUND)
    add_subdirectory(png)
endif(PNG_FOUND)

# OpenCTL is needed for having a RGB16/32f color space
if(OPENEXR_FOUND AND OPENCTL_FOUND)
    add_subdirectory(openexr)
endif(OPENEXR_FOUND AND OPENCTL_FOUND)

if(GRAPHICSMAGICK_FOUND)
    add_subdirectory(gmagick)
endif(GRAPHICSMAGICK_FOUND)

if(POPPLER_FOUND AND HAVE_POPPLER_0_6)
    add_subdirectory(pdf)
endif(POPPLER_FOUND AND HAVE_POPPLER_0_6)

add_subdirectory(bmp)
add_subdirectory(ora)
