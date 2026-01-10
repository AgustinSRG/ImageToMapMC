/*
 * This file is part of ImageToMapMC project
 * 
 * Copyright (c) 2025 Agustin San Roman
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

#include "../mapart/common.h"
#include "../threads/progress.h"
#include <zip.h>

namespace minecraft {
    /**
     * @brief  Writes schematic to file
     * @note   
     * @param  fileName File name
     * @param  buildData Building data
     * @param  supportBlocks Support block options
     * @param  version Minecraft version
     * @param  isBase Set to true to only save the base blocks (stone) 
     * @retval None
     */
    void writeSchematicNBTFile(std::string fileName, std::vector<mapart::MapBuildingBlock> &buildData, mapart::MapBuildingSupportBlock &supportBlocks, minecraft::McVersion version, bool isBase);

    /**
     * @brief  Writes schematic to file (compact, single file)
     * @note   
     * @param  fileName File name
     * @param  chunks Building chunks
     * @param  supportBlocks Support block options
     * @param  version Minecraft version
     * @param  progress progress reporter
     * @retval None
     */
    void writeSchematicNBTFileCompact(std::string fileName, std::vector<std::vector<mapart::MapBuildingBlock>> &chunks, mapart::MapBuildingSupportBlock &supportBlocks, minecraft::McVersion version, threading::Progress &progress);

     /**
     * @brief  Writes schematic to file (compact, single file) (for flat maps only)
     * @note   
     * @param  fileName File name
     * @param  chunks Building chunks
     * @param  supportBlocks Support block options
     * @param  width Matrix width
     * @param  version Minecraft version
     * @param  progress progress reporter
     * @retval None
     */
    void writeSchematicNBTFileCompactFlat(std::string fileName, std::vector<std::vector<mapart::MapBuildingBlock>> &chunks, mapart::MapBuildingSupportBlock &supportBlocks, size_t width, minecraft::McVersion version, threading::Progress &progress);

    /**
     * @brief  Writes schematic to file
     * @note   
     * @param  fileName File name
     * @param  zipper Zip file descriptor
     * @param  buildData Building data
     * @param  supportBlocks Support block options
     * @param  version Minecraft version
     * @param  isBase Set to true to only save the base blocks (stone) 
     * @retval None
     */
    void writeSchematicNBTFileZip(std::string fileName, zip_t *zipper, std::vector<mapart::MapBuildingBlock> &buildData, mapart::MapBuildingSupportBlock &supportBlocks, minecraft::McVersion version, bool isBase);
}
