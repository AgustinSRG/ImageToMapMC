/*
 * This file is part of ImageToMapMC project
 * 
 * Copyright (c) 2021 Agustin San Roman
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <string>
#include <vector>
#include "../colors/colors.h"
#include "../minecraft/mc_colors.h"
#include "../minecraft/mc_blocks.h"
#include "../threads/progress.h"

typedef short map_color_t;

#define MAP_WIDTH (128)
#define MAP_HEIGHT (128)

/**
 * @brief  Map art namespace
 * @note   
 * @retval None
 */
namespace mapart {

    /**
     * @brief  Reds map data from nbt file
     * @note   
     * @param  fileName: File name
     * @retval Map data
     */
    std::vector<map_color_t> readMapNBTFile(std::string fileName);

    /**
     * @brief  Saves map data to file
     * @note   
     * @param  fileName: File name
     * @param  &mapColors: Map data
     * @param version: Minecraft version
     * @retval None
     */
    void writeMapNBTFile(std::string fileName, const std::vector<map_color_t> &mapColors, minecraft::McVersion version);

    /**
     * @brief  Turns array of map colors to final colors with RGB info
     * @note   
     * @param  &colorSet: Color set (minecraft)
     * @param  &mapColors: Array of map colors
     * @retval Array of final colors
     */
    std::vector<const minecraft::FinalColor *> mapColorsToRGB(const std::vector<minecraft::FinalColor> &colorSet, const std::vector<map_color_t> &mapColors);

    /**
     * @brief  Gets map data to save to map file
     * @note   
     * @param  &matrix: Color matrix
     * @param  matrixW: Matrix width
     * @param  matrixH: Matrix height
     * @param  mapX: Map X coordinate
     * @param  mapZ: Map Z coordinate
     * @retval Map data
     */
    std::vector<map_color_t> getMapDataFromColorMatrix(const std::vector<const minecraft::FinalColor *> &matrix, size_t matrixW, size_t matrixH, size_t mapX, size_t mapZ);

    /**
     * @brief  Different dithering methods
     * @note   
     * @retval None
     */
    enum class DitheringMethod: short {
        None = 0,

        FloydSteinberg = 1,
        MinAvgErr = 2,
        Burkes = 3,
        SierraLite = 4,
        Stucki = 5,
        Atkinson = 6,

        Bayer44 = 7,
        Bayer22 = 8,
        Ordered33 = 9,

        Unknown = 99
    };

    /**
     * @brief  Gets string representation for dithering method
     * @note   
     * @param  method: Dithering method
     * @retval String
     */
    std::string ditheringMethodToString(mapart::DitheringMethod method);

    /**
     * @brief  Parses dithering method from string
     * @note   
     * @param  str: String
     * @retval Dithering method
     */
    mapart::DitheringMethod parseDitheringMethodFromString(std::string str);

    /**
     * @brief  Generates map art
     * @note   
     * @param  &colorSet: Color set
     * @param  &colorMatrix: Original color matrix
     * @param  width: Image width
     * @param  height: Image height
     * @param  colorDistanceAlgo: Color distance algorithm
     * @param  ditheringMethod: Dithering method
     * @retval Array of final colors
     */
    std::vector<const minecraft::FinalColor *> generateMapArt(const std::vector<minecraft::FinalColor> &colorSet, const std::vector<colors::Color> &colorMatrix, size_t width, size_t height, colors::ColorDistanceAlgorithm colorDistanceAlgo, mapart::DitheringMethod ditheringMethod, size_t threadNum, threading::Progress &progress);

    /**
     * @brief  Map art building method
     * @note   
     * @retval None
     */
    enum class MapBuildMethod {
        None, // No build, only export to map
        Flat, // Flat (2d)
        Staircased, // Staircased
        Chaos // 3D with Y level jumps
    };

    /**
     * @brief  Applies building restrictions
     * @note   
     * @param  &colorSet: Color set
     * @param  method: Build method
     * @retval None
     */
    void applyBuildRestrictions(std::vector<minecraft::FinalColor> &colorSet, MapBuildMethod method);

    /**
     * @brief  Building block
     * @note   
     * @retval None
     */
    struct MapBuildingBlock {
        const minecraft::BlockDescription * block_ptr;
        int x;
        int y;
        int z;
    };

    /**
     * @brief  Represents a plateau
     * @note   
     * @retval None
     */
    struct Plateau {
        size_t start;
        size_t end;
    };

    /**
     * @brief  Builds map row
     * @note   
     * @param  version: 
     * @param  &blockSet: 
     * @param  &blockMatrix: 
     * @param  &matrix: 
     * @param  matrixW: 
     * @param  matrixH: 
     * @param  mapX: 
     * @param  mapZ: 
     * @param  x: 
     * @param  smooth: 
     * @retval None
     */
    void buildMapRow(minecraft::McVersion version, const std::vector<minecraft::BlockList> &blockSet, std::vector<mapart::MapBuildingBlock> &blockMatrix, const std::vector<const minecraft::FinalColor *> &matrix, size_t matrixW, size_t matrixH, size_t mapX, size_t mapZ, size_t x, bool smooth);
    
    /**
     * @brief  Builds map
     * @note   
     * @param  version: 
     * @param  &blockSet: 
     * @param  &matrix: 
     * @param  matrixW: 
     * @param  matrixH: 
     * @param  mapX: 
     * @param  mapZ: 
     * @param  buildMethod: 
     * @param  threadsNum: 
     * @param  &p: 
     * @retval 
     */
    std::vector<mapart::MapBuildingBlock> buildMap(minecraft::McVersion version, const std::vector<minecraft::BlockList> &blockSet, const std::vector<const minecraft::FinalColor *> &matrix, size_t matrixW, size_t matrixH, size_t mapX, size_t mapZ, mapart::MapBuildMethod buildMethod, size_t threadsNum, threading::Progress &p);

    /**
     * @brief  Writes structure to file
     * @note   
     * @param  fileName: File name
     * @param  &buildData: Building data
     * @param  version: Minecraft version
     * @retval None
     */
    void writeStructureNBTFile(std::string fileName, std::vector<mapart::MapBuildingBlock> &buildData, minecraft::McVersion version);
}
