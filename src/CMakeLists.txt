# CMakeList.txt: Project
cmake_minimum_required (VERSION 3.8)

project ("ImageToMapMC_core")

# hack for buggy CMake's FindwxWidgets script
if (DEFINED ENV_WX_CONFIG)
    set (ENV{WX_CONFIG} ${ENV_WX_CONFIG})
endif()

set(wxWidgets_USE_STATIC 1)

find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

include_directories(3rdparty/libnbtplusplus/include)
add_subdirectory(3rdparty/libnbtplusplus)

# Source files
add_executable (ImageToMapMC 
    "ImageToMapMC.cpp" "ImageToMapMC.h"
    "colors/colors.h" "colors/colors.cpp" "colors/cielab.h" "colors/cielab.cpp" 
    "minecraft/mc_common.h" "minecraft/mc_common.h" "minecraft/mc_colors.h" "minecraft/mc_colors.cpp" "minecraft/mc_blocks.h" "minecraft/mc_blocks.h"
    "mapart/map_art.h" "mapart/map_art.cpp"
)

target_link_libraries(ImageToMapMC ${wxWidgets_LIBRARIES})
target_link_libraries(ImageToMapMC nbt++)

# TODO: Tests / Installations.
