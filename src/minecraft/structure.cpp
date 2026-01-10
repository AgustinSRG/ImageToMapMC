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

#include "structure.h"

#include <io/stream_writer.h>
#include <io/ozlibstream.h>
#include <nbt_tags.h>
#include <fstream>

#include <cstring>

using namespace std;
using namespace mapart;
using namespace minecraft;

nbt::tag_compound blockDescriptionToTag(const minecraft::BlockDescription *desc)
{
    nbt::tag_compound tag;

    tag.insert("Name", desc->nbtName);
    if (desc->nbtTags.size() > 0)
    {
        nbt::tag_compound props;
        for (size_t i = 0; i < desc->nbtTags.size(); i++)
        {
            props.insert(desc->nbtTags[i].name, desc->nbtTags[i].value);
        }
        tag.insert("Properties", props.clone());
    }

    return tag;
}

nbt::tag_compound blockToTag(const mapart::MapBuildingBlock &block, vector<int> &palette, bool base)
{
    nbt::tag_compound tag;

    // Position
    nbt::tag_list posTag;
    posTag.push_back(nbt::tag_int(block.x));
    posTag.push_back(nbt::tag_int(base ? (block.y) : (block.y + 1)));
    posTag.push_back(nbt::tag_int(block.z));
    tag.insert("pos", posTag.clone());

    // State
    if (base)
    {
        tag.insert("state", nbt::tag_int(0));
    }
    else if (block.block_ptr != NULL)
    {
        tag.insert("state", nbt::tag_int(palette[block.block_ptr->baseColorIndex]));
    }
    else
    {
        tag.insert("state", nbt::tag_int(0));
    }

    return tag;
}

nbt::tag_compound blockToTagOffset(const mapart::MapBuildingBlock &block, vector<int> &palette, bool base, size_t offset_x, size_t offset_z)
{
    nbt::tag_compound tag;

    // Position
    nbt::tag_list posTag;
    posTag.push_back(nbt::tag_int(static_cast<int32_t>(offset_x + block.x)));
    posTag.push_back(nbt::tag_int(base ? (block.y) : (block.y + 1)));
    posTag.push_back(nbt::tag_int(static_cast<int32_t>(offset_z + block.z)));
    tag.insert("pos", posTag.clone());

    // State
    if (base)
    {
        tag.insert("state", nbt::tag_int(0));
    }
    else if (block.block_ptr != NULL)
    {
        tag.insert("state", nbt::tag_int(palette[block.block_ptr->baseColorIndex]));
    }
    else
    {
        tag.insert("state", nbt::tag_int(0));
    }

    return tag;
}

void minecraft::writeStructureNBTFileCompact(std::string fileName, std::vector<std::vector<mapart::MapBuildingBlock>> &chunks, mapart::MapBuildingSupportBlock &supportBlocks, minecraft::McVersion version, threading::Progress &progress)
{
    nbt::tag_compound root;
    nbt::tag_list blocksTag;
    nbt::tag_list paletteTag;
    vector<int> palette(64);
    int nextPaletteItem = 1;
    int maxYlevel = 0;

    for (size_t i = 0; i < palette.size(); i++)
    {
        palette[i] = -1; // Initialize
    }

    // Add base blocks to palette
    palette[0] = 0;

    if (supportBlocks.block_ptr != NULL)
    {
        paletteTag.push_back(blockDescriptionToTag(supportBlocks.block_ptr));
    }
    else
    {
        minecraft::BlockDescription baseBlockDesc;
        baseBlockDesc.name = DEFAULT_SUPPORT_BLOCK_NAME;
        baseBlockDesc.minVersion = McVersion::MC_1_12;
        baseBlockDesc.maxVersion = MC_LAST_VERSION;
        baseBlockDesc.nbtName = DEFAULT_SUPPORT_BLOCK_ID;
        paletteTag.push_back(blockDescriptionToTag(&baseBlockDesc));
    }

    size_t chunk_count = chunks.size();
    size_t total_width = chunk_count * MAP_WIDTH;
    size_t total_height = MAP_HEIGHT + 1;

    for (size_t chunk_i = 0; chunk_i < chunk_count; chunk_i++)
    {
        std::vector<mapart::MapBuildingBlock> *buildData = &chunks[chunk_i];

        size_t offset_x = chunk_i * MAP_WIDTH;

        size_t size = buildData->size();
        for (size_t i = 0; i < size; i++)
        {
            // Compute max y level for size
            int y = buildData->at(i).y + 1;
            if (y > maxYlevel)
            {
                maxYlevel = y;
            }

            // Add the blocks
            const minecraft::BlockDescription *blockPtr = buildData->at(i).block_ptr;

            if (blockPtr == NULL)
            {
                // First line, no base blocks
                blocksTag.push_back(blockToTagOffset(buildData->at(i), palette, false, offset_x, 0));
            }
            else
            {
                short blockIndex = blockPtr->baseColorIndex;
                if (palette[blockIndex] < 0)
                {
                    // Add to palette
                    palette[blockIndex] = nextPaletteItem++;
                    paletteTag.push_back(blockDescriptionToTag(blockPtr));
                }

                // Base block
                if (supportBlocks.placeAlways || blockPtr->requiresSupportBlock)
                {
                    blocksTag.push_back(blockToTagOffset(buildData->at(i), palette, true, offset_x, 0));
                }

                // Real block
                blocksTag.push_back(blockToTagOffset(buildData->at(i), palette, false, offset_x, 0));
            }
        }

        try
        {
            progress.setProgress(0, static_cast<unsigned int>(chunk_i + 1));
        }
        catch (int)
        {
            return;
        }
    }

    // Data version and author tags
    root.insert("DataVersion", minecraft::versionToDataVersion(version));
    root.insert("author", "mcmap");

    // Insert blocks
    root.insert("blocks", blocksTag.clone());

    // Empty entitities
    nbt::tag_list entitiesTag;
    root.insert("entities", entitiesTag.clone());

    // Insert palette
    root.insert("palette", paletteTag.clone());

    // Insert size tag
    nbt::tag_list sizeTag;
    sizeTag.push_back(nbt::tag_int(static_cast<int32_t>(total_width)));
    sizeTag.push_back(nbt::tag_int(maxYlevel + 1));
    sizeTag.push_back(nbt::tag_int(static_cast<int32_t>(total_height)));
    root.insert("size", sizeTag.clone());

    // Save
    std::ofstream file(fileName, std::ios::binary);

    if (!file)
    {
        throw -1;
    }

    try
    {
        zlib::ozlibstream ogzs(file, -1, true);
        nbt::io::write_tag("", root, ogzs);
    }
    catch (...)
    {
        throw -2;
    }
}

void minecraft::writeStructureNBTFileCompactFlat(std::string fileName, std::vector<std::vector<mapart::MapBuildingBlock>> &chunks, mapart::MapBuildingSupportBlock &supportBlocks, size_t width, minecraft::McVersion version, threading::Progress &progress)
{
    nbt::tag_compound root;
    nbt::tag_list blocksTag;
    nbt::tag_list paletteTag;
    vector<int> palette(64);
    int nextPaletteItem = 1;
    int maxYlevel = 0;

    for (size_t i = 0; i < palette.size(); i++)
    {
        palette[i] = -1; // Initialize
    }

    // Add base blocks to palette
    palette[0] = 0;

    if (supportBlocks.block_ptr != NULL)
    {
        paletteTag.push_back(blockDescriptionToTag(supportBlocks.block_ptr));
    }
    else
    {
        minecraft::BlockDescription baseBlockDesc;
        baseBlockDesc.name = DEFAULT_SUPPORT_BLOCK_NAME;
        baseBlockDesc.minVersion = McVersion::MC_1_12;
        baseBlockDesc.maxVersion = MC_LAST_VERSION;
        baseBlockDesc.nbtName = DEFAULT_SUPPORT_BLOCK_ID;
        paletteTag.push_back(blockDescriptionToTag(&baseBlockDesc));
    }

    size_t chunk_count = chunks.size();
    size_t chunks_height = chunk_count / width;
    size_t total_width = width * MAP_WIDTH;
    size_t total_height = (chunks_height * MAP_HEIGHT) + 1;

    for (size_t chunk_i = 0; chunk_i < chunk_count; chunk_i++)
    {
        std::vector<mapart::MapBuildingBlock> *buildData = &chunks[chunk_i];

        size_t chunk_x = chunk_i % width;
        size_t chunk_z = chunk_i / width;

        size_t offset_z = chunk_z * MAP_HEIGHT;
        size_t offset_x = chunk_x * MAP_WIDTH;

        size_t size = buildData->size();
        for (size_t i = 0; i < size; i++)
        {
            // Compute max y level for size
            int y = buildData->at(i).y + 1;
            if (y > maxYlevel)
            {
                maxYlevel = y;
            }

            // Add the blocks
            const minecraft::BlockDescription *blockPtr = buildData->at(i).block_ptr;

            if (blockPtr == NULL)
            {
                // First line, no base blocks
                if (chunk_z == 0)
                {
                    blocksTag.push_back(blockToTagOffset(buildData->at(i), palette, false, offset_x, offset_z));
                }
            }
            else
            {
                short blockIndex = blockPtr->baseColorIndex;
                if (palette[blockIndex] < 0)
                {
                    // Add to palette
                    palette[blockIndex] = nextPaletteItem++;
                    paletteTag.push_back(blockDescriptionToTag(blockPtr));
                }

                // Base block
                if (supportBlocks.placeAlways || blockPtr->requiresSupportBlock)
                {
                    blocksTag.push_back(blockToTagOffset(buildData->at(i), palette, true, offset_x, offset_z));
                }

                // Real block
                blocksTag.push_back(blockToTagOffset(buildData->at(i), palette, false, offset_x, offset_z));
            }
        }

        try
        {
            progress.setProgress(0, static_cast<unsigned int>(chunk_i + 1));
        }
        catch (int)
        {
            return;
        }
    }

    // Data version and author tags
    root.insert("DataVersion", minecraft::versionToDataVersion(version));
    root.insert("author", "mcmap");

    // Insert blocks
    root.insert("blocks", blocksTag.clone());

    // Empty entitities
    nbt::tag_list entitiesTag;
    root.insert("entities", entitiesTag.clone());

    // Insert palette
    root.insert("palette", paletteTag.clone());

    // Insert size tag
    nbt::tag_list sizeTag;
    sizeTag.push_back(nbt::tag_int(static_cast<int32_t>(total_width)));
    sizeTag.push_back(nbt::tag_int(maxYlevel + 1));
    sizeTag.push_back(nbt::tag_int(static_cast<int32_t>(total_height)));
    root.insert("size", sizeTag.clone());

    // Save
    std::ofstream file(fileName, std::ios::binary);

    if (!file)
    {
        throw -1;
    }

    try
    {
        zlib::ozlibstream ogzs(file, -1, true);
        nbt::io::write_tag("", root, ogzs);
    }
    catch (...)
    {
        throw -2;
    }
}

void minecraft::writeStructureNBTFile(std::string fileName, std::vector<mapart::MapBuildingBlock> &buildData, mapart::MapBuildingSupportBlock &supportBlocks, minecraft::McVersion version, bool isBase)
{
    nbt::tag_compound root;
    nbt::tag_list blocksTag;
    nbt::tag_list paletteTag;
    vector<int> palette(64);
    int nextPaletteItem = 1;
    int maxYlevel = 0;

    for (size_t i = 0; i < palette.size(); i++)
    {
        palette[i] = -1; // Initialize
    }

    // Add base blocks to palette
    palette[0] = 0;

    if (supportBlocks.block_ptr != NULL)
    {
        paletteTag.push_back(blockDescriptionToTag(supportBlocks.block_ptr));
    }
    else
    {
        minecraft::BlockDescription baseBlockDesc;
        baseBlockDesc.name = DEFAULT_SUPPORT_BLOCK_NAME;
        baseBlockDesc.minVersion = McVersion::MC_1_12;
        baseBlockDesc.maxVersion = MC_LAST_VERSION;
        baseBlockDesc.nbtName = DEFAULT_SUPPORT_BLOCK_ID;
        paletteTag.push_back(blockDescriptionToTag(&baseBlockDesc));
    }

    // Blocks parsing
    size_t size = buildData.size();
    for (size_t i = 0; i < size; i++)
    {
        // Compute max y level for size
        int y = buildData[i].y + 1;
        if (y > maxYlevel)
        {
            maxYlevel = y;
        }

        // Add the blocks
        const minecraft::BlockDescription *blockPtr = buildData[i].block_ptr;

        if (blockPtr == NULL)
        {
            // First line, no base blocks
            blocksTag.push_back(blockToTag(buildData[i], palette, false));
        }
        else
        {
            short blockIndex = blockPtr->baseColorIndex;
            if (palette[blockIndex] < 0)
            {
                // Add to palette
                palette[blockIndex] = nextPaletteItem++;
                paletteTag.push_back(blockDescriptionToTag(blockPtr));
            }

            // Base block
            if (supportBlocks.placeAlways || blockPtr->requiresSupportBlock)
            {
                blocksTag.push_back(blockToTag(buildData[i], palette, true));
            }

            // Real block
            if (!isBase)
            {
                blocksTag.push_back(blockToTag(buildData[i], palette, false));
            }
        }
    }

    // Data version and author tags
    root.insert("DataVersion", minecraft::versionToDataVersion(version));
    root.insert("author", "mcmap");

    // Insert blocks
    root.insert("blocks", blocksTag.clone());

    // Empty entitities
    nbt::tag_list entitiesTag;
    root.insert("entities", entitiesTag.clone());

    // Insert palette
    root.insert("palette", paletteTag.clone());

    // Insert size tag
    nbt::tag_list sizeTag;
    sizeTag.push_back(nbt::tag_int(MAP_WIDTH));
    sizeTag.push_back(nbt::tag_int(maxYlevel + 1));
    sizeTag.push_back(nbt::tag_int(MAP_HEIGHT + 1));
    root.insert("size", sizeTag.clone());

    // Save
    std::ofstream file(fileName, std::ios::binary);

    if (!file)
    {
        throw -1;
    }

    try
    {
        zlib::ozlibstream ogzs(file, -1, true);
        nbt::io::write_tag("", root, ogzs);
    }
    catch (...)
    {
        throw -2;
    }
}

void minecraft::writeStructureNBTFileZip(std::string fileName, zip_t *zipper, std::vector<mapart::MapBuildingBlock> &buildData, mapart::MapBuildingSupportBlock &supportBlocks, minecraft::McVersion version, bool isBase)
{
    nbt::tag_compound root;
    nbt::tag_list blocksTag;
    nbt::tag_list paletteTag;
    vector<int> palette(64);
    int nextPaletteItem = 1;
    int maxYlevel = 0;

    for (size_t i = 0; i < palette.size(); i++)
    {
        palette[i] = -1; // Initiallize
    }

    // Add base blocks to palette
    palette[0] = 0;

    if (supportBlocks.block_ptr != NULL)
    {
        paletteTag.push_back(blockDescriptionToTag(supportBlocks.block_ptr));
    }
    else
    {
        minecraft::BlockDescription baseBlockDesc;
        baseBlockDesc.name = DEFAULT_SUPPORT_BLOCK_NAME;
        baseBlockDesc.minVersion = McVersion::MC_1_12;
        baseBlockDesc.maxVersion = MC_LAST_VERSION;
        baseBlockDesc.nbtName = DEFAULT_SUPPORT_BLOCK_ID;
        paletteTag.push_back(blockDescriptionToTag(&baseBlockDesc));
    }

    // Blocks parsing
    size_t size = buildData.size();
    for (size_t i = 0; i < size; i++)
    {
        // Compute max y level for size
        int y = buildData[i].y + 1;
        if (y > maxYlevel)
        {
            maxYlevel = y;
        }

        // Add the blocks
        const minecraft::BlockDescription *blockPtr = buildData[i].block_ptr;

        if (blockPtr == NULL)
        {
            // First line, no base blocks
            blocksTag.push_back(blockToTag(buildData[i], palette, false));
        }
        else
        {
            short blockIndex = blockPtr->baseColorIndex;
            if (palette[blockIndex] < 0)
            {
                // Add to palette
                palette[blockIndex] = nextPaletteItem++;
                paletteTag.push_back(blockDescriptionToTag(blockPtr));
            }

            // Base block
            if (supportBlocks.placeAlways || blockPtr->requiresSupportBlock) {
                blocksTag.push_back(blockToTag(buildData[i], palette, true));
            }

            // Real block
            if (!isBase)
            {
                blocksTag.push_back(blockToTag(buildData[i], palette, false));
            }
        }
    }

    // Data version and author tags
    root.insert("DataVersion", minecraft::versionToDataVersion(version));
    root.insert("author", "mcmap");

    // Insert blocks
    root.insert("blocks", blocksTag.clone());

    // Empty entitities
    nbt::tag_list entitiesTag;
    root.insert("entities", entitiesTag.clone());

    // Insert palette
    root.insert("palette", paletteTag.clone());

    // Insert size tag
    nbt::tag_list sizeTag;
    sizeTag.push_back(nbt::tag_int(MAP_WIDTH));
    sizeTag.push_back(nbt::tag_int(maxYlevel + 1));
    sizeTag.push_back(nbt::tag_int(MAP_HEIGHT + 1));
    root.insert("size", sizeTag.clone());

    // Save
    ostringstream ss;

    try
    {
        zlib::ozlibstream ogzs(ss, -1, true);
        nbt::io::write_tag("", root, ogzs);

        ogzs.close();

        std::string data = ss.str();

        size_t len = data.size();

        char *buffer = new char[len];

        memcpy(buffer, data.c_str(), len);

        zip_source_t *bsource = zip_source_buffer(zipper, buffer, len, 1);

        zip_file_add(zipper, fileName.c_str(), bsource, ZIP_FL_ENC_UTF_8 | ZIP_FL_OVERWRITE);
    }
    catch (...)
    {
        throw -2;
    }
}
