# -----------------------------------------------------------------------------
# S16 Sprite Viewer CMake setup
#
# For ports, please try not to edit this file directly.
# Everything should go into a win64.cmake equivalent.
# -----------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.3.2) # Might compile lower, but untested
project (s16_viewer)

# -----------------------------------------------------------------------------
# Directory Setup
# -----------------------------------------------------------------------------

# CMake default file (override with -DTARGET=file)
set(DCMAKE win64.cmake)

# Source location
set(main_cpp_base ../src)

# -----------------------------------------------------------------------------
# Platform Specific Setup
# -----------------------------------------------------------------------------

# Include relevant CMAKE file (default, mingw, pandora etc.)
if(TARGET)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${TARGET}")
        message("CMake files for ${TARGET} found")
        include(${TARGET})
    else()
        message("CMake files for ${TARGET} not found, using ${DCMAKE}")
        include(${DCMAKE})
    endif()
else()
    message("TARGET not passed, using ${DCMAKE}")
    include(${DCMAKE})
endif(TARGET)

# -----------------------------------------------------------------------------
# Project Source Files
# -----------------------------------------------------------------------------
    
set(src_main
    "${main_cpp_base}/globals.hpp"
    "${main_cpp_base}/hwsprites.hpp"
    "${main_cpp_base}/input.hpp"
    "${main_cpp_base}/palette.hpp"
    "${main_cpp_base}/roms.hpp"
    "${main_cpp_base}/stdint.hpp"
    "${main_cpp_base}/romloader.hpp"
    "${main_cpp_base}/video.hpp"

    "${main_cpp_base}/hwsprites.cpp"
    "${main_cpp_base}/input.cpp"
    "${main_cpp_base}/main.cpp"
    "${main_cpp_base}/romloader.cpp"
    "${main_cpp_base}/roms.cpp"
    "${main_cpp_base}/video.cpp"
    )


set(SRCS
    ${src_main}
)

# Group files into relevant filters for Visual Studio build
SOURCE_GROUP(src FILES ${src_main})

add_executable(s16_viewer ${SRCS})

# -----------------------------------------------------------------------------
# Library Setup
# -----------------------------------------------------------------------------

set(BOOST_INCLUDEDIR ${boost_dir})
find_package(Boost REQUIRED)

set(SDL2_DIR ${sdl2_dir})
find_package(SDL2 REQUIRED)
message("SDL2 Include  : ${SDL2_INCLUDE_DIRS}")
message("SDL2 Libraries: ${SDL2_LIBRARIES}")

# Include
include_directories(
    ${main_cpp_base}
    ${BOOST_INCLUDEDIR}
    ${SDL2_INCLUDE_DIRS}
)

link_directories(
    ${platform_link_dirs}
)

target_link_libraries(s16_viewer
    ${SDL2_LIBRARIES}
    ${platform_link_libs}
)
