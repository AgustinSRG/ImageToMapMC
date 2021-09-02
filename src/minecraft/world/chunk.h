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

#include "tools.h"

#include "../../mapart/common.h"

#define CHUNK_WIDTH (16)
#define CHUNK_HEIGHT (16)

#define CHUNK_SECTION_HEIGHT (16)

#define BLOCK_POS_IN_SECTION(x, y, z) ((y) * CHUNK_SECTION_HEIGHT * CHUNK_HEIGHT + (z) * CHUNK_WIDTH + (x))

namespace minecraft
{

    /**
     * @brief  Chunk Section
     * @note   
     * @retval None
     */
    class ChunkSection
    {
    public:
        ChunkSection();
        ChunkSection(int y);

        /**
         * @brief  Sets block
         * @note   
         * @param  *block: Block info
         * @param  x: X relative to section 0-15
         * @param  y: Y relative to section 0-15
         * @param  z: Z relative to section 0-15
         * @retval None
         */
        void setBlock(const minecraft::BlockDescription *block, int x, int y, int z);

        int y;
        std::vector<minecraft::BlockDescription> palette;
        std::vector<uint8_t> blocks;

        std::vector<uint64_t> getBlockStates(minecraft::McVersion version);
    private:
        std::vector<uint8_t> paletteMap;
    };

    /**
     * @brief  Chunk
     * @note   
     * @retval None
     */
    class MinecraftChunk
    {
    public:
        MinecraftChunk();
        MinecraftChunk(minecraft::McVersion version, int x, int z);

        /**
         * @brief  Gets chunk bytes
         * @note   
         * @retval byte string
         */
        std::string getBytes();

        /**
         * @brief  Gets section
         * @note   
         * @param  y: Y level
         * @retval 
         */
        minecraft::ChunkSection * getSection(int y);

        /**
         * @brief  Adds block
         * @note   
         * @param  *block: Block
         * @param  x: X relative to chunk 0-15
         * @param  y: Y (0-256)
         * @param  z: Z relative to chunk 0-15
         * @retval None
         */
        void addBlock(const minecraft::BlockDescription *block, int x, int y, int z);

        /**
         * @brief  Adds base platform
         * @note   
         * @param  offsetY: Y level
         * @retval None
         */
        void addBasePlatform(int offsetY);
    private:
        minecraft::McVersion version;
        int x;
        int z;
        std::vector<ChunkSection> sections;
    };
}
