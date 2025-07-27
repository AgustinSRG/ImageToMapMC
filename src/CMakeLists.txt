# CMakeList.txt: Project
cmake_minimum_required (VERSION 3.8)

project ("ImageToMapMC")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++17 -lstdc++fs") 

# hack for buggy CMake's FindwxWidgets script
if (DEFINED ENV_WX_CONFIG)
    set (ENV{WX_CONFIG} ${ENV_WX_CONFIG})
endif()

set(wxWidgets_USE_STATIC 1)
SET(wxWidgets_USE_LIBS)

find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})
find_package(libzip REQUIRED)

include_directories(3rdparty/libnbtplusplus/include)
add_subdirectory(3rdparty/libnbtplusplus)

SET(SOURCES_COMMON 
    "colors/colors.h" "colors/colors.cpp" 
    "colors/cielab.h" "colors/cielab.cpp" 

    "threads/progress.h" "threads/progress.cpp"

    "minecraft/mc_common.h" "minecraft/mc_common.cpp"
    "minecraft/mc_colors.h" "minecraft/mc_colors.cpp" 
    "minecraft/mc_blocks.h" "minecraft/mc_blocks.cpp"
    "minecraft/schematic.h" "minecraft/schematic.cpp"
    "minecraft/structure.h" "minecraft/structure.cpp"
    "minecraft/mcfunction.h" "minecraft/mcfunction.cpp"

    "mapart/map_art.h" 
    "mapart/common.h" "mapart/common.cpp" 
    "mapart/map_image.h" "mapart/map_image.cpp" 
    "mapart/dithering.h" 
    "mapart/map_generate.h" "mapart/map_generate.cpp"
    "mapart/map_build.h" "mapart/map_build.cpp"
    "mapart/map_nbt.h" "mapart/map_nbt.cpp"
    "mapart/map_color_set.h" "mapart/map_color_set.cpp"
    "mapart/materials.h" "mapart/materials.cpp"

    "tools/basedir.h" "tools/basedir.cpp"
    "tools/text_file.h" "tools/text_file.cpp"
    "tools/image_edit.h" "tools/image_edit.cpp"

    "version.h"
)

SET(SOURCES_WX 
    "wx/main_window.h" "wx/main_window.cpp"
    "wx/img_display_window.h" "wx/img_display_window.cpp"
    "wx/materials_window.h" "wx/materials_window.cpp"
    "wx/map_export_dialog.h" "wx/map_export_dialog.cpp"
    "wx/structure_export_dialog.h" "wx/structure_export_dialog.cpp"
    "wx/image_resize_dialog.h" "wx/image_resize_dialog.cpp"
    "wx/image_edit_dialog.h" "wx/image_edit_dialog.cpp"
    "wx/worker_thread.h" "wx/worker_thread.cpp"
    "mapart/project.h" "mapart/project.cpp"
    "tools/open_desktop.h" "tools/open_desktop.cpp"
    "tools/value_remember.h" "tools/value_remember.cpp"
)

SET(SOURCES_RC 
    "resources/resources.rc"
)

# Win app
add_executable (mcmap-gui WIN32 
    "main_win.cpp" "main_win.h"
    ${SOURCES_COMMON}
    ${SOURCES_WX}
    ${SOURCES_RC}
)

target_link_libraries(mcmap-gui ${wxWidgets_LIBRARIES})
target_link_libraries(mcmap-gui libzip::zip)
target_link_libraries(mcmap-gui nbt++)

if (WIN_RELEASE)
    file(GLOB MSVC_DLL_LIST $ENV{MSVC_CRT_DLL_PATH}/*.dll)
    file(COPY ${MSVC_DLL_LIST} DESTINATION Release)
endif()

# Console app
add_executable (mcmap 
    "main_console.cpp" "main_console.h"
    ${SOURCES_COMMON}
)

target_link_libraries(mcmap ${wxWidgets_LIBRARIES})
target_link_libraries(mcmap libzip::zip)
target_link_libraries(mcmap nbt++)
