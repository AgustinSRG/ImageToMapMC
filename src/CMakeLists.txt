# CMakeList.txt: Project
cmake_minimum_required (VERSION 3.8)

project ("ImageToMapMC_core")

# hack for buggy CMake's FindwxWidgets script
if (DEFINED ENV_WX_CONFIG)
    set (ENV{WX_CONFIG} ${ENV_WX_CONFIG})
endif()

set(wxWidgets_USE_STATIC 1)
SET(wxWidgets_USE_LIBS)

find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

include_directories(3rdparty/libnbtplusplus/include)
add_subdirectory(3rdparty/libnbtplusplus)

SET(SOURCES_COMMON 
    "colors/colors.h" "colors/colors.cpp" "colors/cielab.h" "colors/cielab.cpp" 
    "minecraft/mc_common.h" "minecraft/mc_common.h" "minecraft/mc_colors.h" "minecraft/mc_colors.cpp" "minecraft/mc_blocks.h" "minecraft/mc_blocks.h"
    "mapart/map_art.h" "mapart/map_art.cpp"
)

SET(SOURCES_WX 
    "wx/img_display_window.h" "wx/img_display_window.cpp"
)

SET(SOURCES_RC 
    "resources/resources.rc"
)

# Source files
add_executable (ImageToMapMC WIN32 
    "ImageToMapMC.cpp" "ImageToMapMC.h"
    ${SOURCES_COMMON}
    ${SOURCES_WX}
    ${SOURCES_RC}
)

target_link_libraries(ImageToMapMC ${wxWidgets_LIBRARIES})
target_link_libraries(ImageToMapMC nbt++)

# TODO: Tests / Installations.
