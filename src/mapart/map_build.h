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

#include "common.h"
#include "../threads/progress.h"

namespace mapart {
    /**
     * @brief  Applies building restrictions
     * @note   
     * @param  &colorSet: Color set
     * @param  method: Build method
     * @retval None
     */
    void applyBuildRestrictions(std::vector<minecraft::FinalColor> &colorSet, MapBuildMethod method);

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
}
