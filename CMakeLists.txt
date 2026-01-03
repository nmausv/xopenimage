cmake_minimum_required(VERSION 3.14)
#
project(xopenimage_project LANGUAGES C)
#
set(CMAKE_VERBOSE_MAKEFILE ON)
#
# depends on (needs to link against libraries):
#  X11
#  libTIFF
#  libJPEG
#  libPNG (which depends on ZLIB)
#  libWebP (which depends on pthread)
#
# some architectures need explicit math library
#
find_package(X11 REQUIRED)
#
find_package(TIFF REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
#
find_package(Threads REQUIRED)
find_library(WEBP_LIBRARY webp)
#
find_library(STDMATH_LIBRARY m)
#
#
# compilation configuration
#
add_custom_command(OUTPUT build.c COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build-info)
#
add_compile_options(
 -Wall
 -Wextra
 -pedantic
)
#
add_executable(xopenimage
 xopenimage.c
 build.c
 error.c
 fileformats.c
 gimage.c
 options.c
 usageHelp.c
 formats/xbitmap_fmt.c
 formats/netpbm_fmt.c
 formats/tiff_fmt.c
 formats/jpeg_fmt.c
 formats/png_fmt.c
 formats/webp_fmt.c
 X11_interface/gdisplay.c
 transforms/gamma.c
 transforms/rotate.c
 transforms/zoom.c
 transforms/colorspace.c
 transforms/downscale.c
 transforms/bitdownscale.c
)
#
# compilation: include library headers
target_include_directories(xopenimage PRIVATE ${X11_INCLUDE_DIR})
target_include_directories(xopenimage PRIVATE ${TIFF_INCLUDE_DIR})
target_include_directories(xopenimage PRIVATE ${JPEG_INCLUDE_DIR})
target_include_directories(xopenimage PRIVATE ${PNG_INCLUDE_DIR})
target_include_directories(xopenimage PRIVATE ${WEBP_INCLUDE_DIR})
#
#
# linking: link against these libraries
target_link_libraries(xopenimage
  ${X11_LIBRARIES}
  ${STDMATH_LIBRARY}
  #${TIFF_LIBRARY} # stopped working on x86-64 arch linux
  TIFF::TIFF
  ${JPEG_LIBRARY}
  ${PNG_LIBRARY}
  ${WEBP_LIBRARY}
  Threads::Threads
)

