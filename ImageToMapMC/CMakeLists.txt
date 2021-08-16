# CMakeList.txt: Project
cmake_minimum_required (VERSION 3.8)

# Source files
add_executable (ImageToMapMC 
    "ImageToMapMC.cpp" "ImageToMapMC.h" 
    "colors/colors.h" "colors/colors.cpp" "colors/cielab.h" "colors/cielab.cpp" 
    "minecraft/mc_common.h" "minecraft/mc_common.h" "minecraft/mc_colors.h" "minecraft/mc_colors.cpp" "minecraft/mc_blocks.h" "minecraft/mc_blocks.h"
    "mapart/map_art.h" "mapart/map_art.cpp"
)

# TODO: Tests / Installations.
