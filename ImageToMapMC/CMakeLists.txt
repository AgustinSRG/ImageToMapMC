# CMakeList.txt: Project
cmake_minimum_required (VERSION 3.8)

# Source files
add_executable (ImageToMapMC "ImageToMapMC.cpp" "ImageToMapMC.h" "colors/colors.h" "colors/colors.cpp" "colors/cielab.h" "colors/cielab.cpp" "blocks/blocks.h")

# TODO: Tests / Installations.
