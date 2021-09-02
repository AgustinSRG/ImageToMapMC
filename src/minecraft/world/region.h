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

#include "chunk.h"

#define REGION_PAGE_LENGTH (4096)

#define REGION_WIDTH (32)
#define REGION_HEIGHT (32)

#define MAX_CHUNKS_PER_REGION (REGION_WIDTH * REGION_HEIGHT)

// 4 bytes per entry (uint32_t)
#define REGION_HEADER_TABLE_SIZE (MAX_CHUNKS_PER_REGION * 4)

#define REGION_LOCATIONS_START (0)
#define REGION_TIMSTAMPS_START (REGION_LOCATIONS_START + REGION_HEADER_TABLE_SIZE)
#define REGION_CHUNKS_START (REGION_TIMSTAMPS_START + REGION_HEADER_TABLE_SIZE)

namespace minecraft
{
    /**
     * @brief  Get chunk location to store in the region file
     * @note   
     * @param  pageOffset: Page offset
     * @param  pageCount: Page count
     * @retval 
     */
    uint32_t getChunkLocation(uint32_t pageOffset, uint32_t pageCount);

    struct RegionCoord
    {
        int x, y;

        bool const operator==(const RegionCoord &o) const
        {
            return x == o.x && y == o.y;
        }

        bool const operator<(const RegionCoord &o) const
        {
            return x < o.x || (x == o.x && y < o.y);
        }
    };

    /**
     * @brief  Minecraft region
     * @note   
     * @retval None
     */
    class MinecraftRegion
    {
    public:
        MinecraftRegion();
        MinecraftRegion(int x, int z, minecraft::McVersion version, int offsetY);

        /**
         * @brief  Gets chunk pointer
         * @note   
         * @param  x: Block X
         * @param  z: Block Z
         * @retval 
         */
        minecraft::MinecraftChunk *getChunk(int x, int z);

        /**
         * @brief  Adds block to region
         * @note   
         * @param  *block: Block
         * @param  x: X
         * @param  y: Y
         * @param  z: Z
         * @retval None
         */
        void addBlock(const minecraft::BlockDescription *block, int x, int y, int z);

        /**
         * @brief  Writes region file
         * @note   
         * @param  regionsPath: Regions folder
         * @retval True if success, False if error
         */
        bool writeToFile(std::string regionsPath);
    private:
        std::vector<minecraft::MinecraftChunk *> chunkMatrix;
        std::vector<minecraft::MinecraftChunk> chunks;

        int x;
        int z;
        minecraft::McVersion version;

        int offsetX;
        int offsetY;
        int offsetZ;
    };
}
