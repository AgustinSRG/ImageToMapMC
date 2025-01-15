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

#include "mc_common.h"
#include "mc_colors.h"

#include <vector>
#include <string>

namespace minecraft
{

    /**
     * @brief  NBT Property
     * @note   
     * @retval None
     */
    class NBTProp
    {
    public:
        std::string name;
        std::string value;
    };

    /**
     * @brief  Block description
     * @note   Needed to create the final results
     * @retval None
     */
    class BlockDescription
    {
    public:
        // Name
        std::string name;

        // Base color
        short baseColorIndex;

        // Version
        minecraft::McVersion minVersion;
        minecraft::McVersion maxVersion;

        // NBT data
        std::string nbtName;
        std::vector<minecraft::NBTProp> nbtTags;

        // Id and data value for 1.12
        short blockId;
        short dataValue;

        /**
         * @brief  Adds NBT tag
         * @note   
         * @param  name: Tag name
         * @param  value: Tag value
         * @retval None
         */
        void addTag(std::string name, std::string value);
    };

    /**
     * @brief  Represents a minecraft block
     * @note   
     * @retval None
     */
    class Block
    {
    public:
        // Unique identifier for the block
        std::string id;

        // Index of base color
        short baseColorIndex;

        // Block descriptions to create it
        std::vector<minecraft::BlockDescription> descriptions;

        /**
             * @brief  Gets the block description for a minecraft version
             * @note   
             * @param  version: Version
             * @retval The block description. Returns NULL if it is not available for the version.
             */
        const minecraft::BlockDescription *getBlockDescription(minecraft::McVersion version) const;

        /**
             * @brief  Adds block description
             * @note   
             * @param  name: Block name
             * @param  minVersion: Min version 
             * @param  maxVersion: Max version
             * @param  nbtName: NBT name for nbt files
             * @retval The created block description
             */
        minecraft::BlockDescription *addBlockDescription(std::string name, minecraft::McVersion minVersion, minecraft::McVersion maxVersion, std::string nbtName);

        /**
             * @brief  Adds block description
             * @note   
             * @param  name: Block name
             * @param  minVersion: Min version
             * @param  maxVersion: Max version
             * @param  nbtName: NBT name for nbt files
             * @param  blockId: Block ID
             * @param  dataValue: Data value
             * @retval The created block description
             */
        minecraft::BlockDescription *addBlockDescription(std::string name, minecraft::McVersion minVersion, minecraft::McVersion maxVersion, std::string nbtName, short blockId, short dataValue);
    };

    /**
     * @brief  List of blocks for each minecraft color
     * @note   
     * @retval None
     */
    class BlockList
    {
    public:
        // Base color
        short baseColorIndex;

        // List of blocks that produce the color
        std::vector<minecraft::Block> blocks;

        // Default block to use
        short useBlockIndex;

        /**
             * @brief  Gets the block description 
             * @note   
             * @param  version: Minecraft version
             * @retval Th block description
             */
        const minecraft::BlockDescription *getBlockDescription(minecraft::McVersion version) const;

        /**
             * @brief  Adds a block
             * @note   
             * @param  id: Block ID
             * @retval The block
             */
        minecraft::Block *addBlock(std::string id);

        /**
             * @brief  Finds block by ID
             * @note   
             * @param  id: Block ID
             * @retval The block index in the list or -1 if not found
             */
        short findBlock(std::string id);
    };

    /**
     * @brief  Loads all blocks
     * @note   
     * @param  &baseColors: Base colors
     * @retval List of blocks
     */
    std::vector<minecraft::BlockList> loadBlocks(std::vector<colors::Color> &baseColors);
}
