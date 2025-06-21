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

#include "schematic.h"

#include <io/stream_writer.h>
#include <io/ozlibstream.h>
#include <nbt_tags.h>
#include <fstream>
#include <sstream>

#include <cstring>

using namespace std;
using namespace mapart;
using namespace minecraft;

std::string blockDescriptionToTagName(const minecraft::BlockDescription *desc)
{
    nbt::tag_compound tag;

    stringstream ss;

    ss << "minecraft:";
    ss << desc->nbtName;

    if (desc->nbtTags.size() > 0)
    {
        ss << "[";

        for (size_t i = 0; i < desc->nbtTags.size(); i++)
        {
            if (i > 0)
            {
                ss << ",";
            }

            ss << desc->nbtTags[i].name;
            ss << "=";
            ss << desc->nbtTags[i].value;
        }

        ss << "]";
    }

    return ss.str();
}

void minecraft::writeSchematicNBTFile(std::string fileName, std::vector<mapart::MapBuildingBlock> &buildData, minecraft::McVersion version, bool isBase)
{
    nbt::tag_compound root;

    nbt::tag_compound paletteTag;

    vector<int> palette(64);
    int nextPaletteItem = 2;
    int maxYlevel = 0;

    for (size_t i = 0; i < palette.size(); i++)
    {
        palette[i] = -1; // Initialize
    }

    // Add base blocks to palette
    minecraft::BlockDescription baseBlockDesc;
    baseBlockDesc.name = "Base Block";
    baseBlockDesc.minVersion = McVersion::MC_1_12;
    baseBlockDesc.maxVersion = MC_LAST_VERSION;
    baseBlockDesc.nbtName = "stone";
    palette[0] = 1;

    paletteTag.insert("minecraft:air", 0);
    paletteTag.insert(blockDescriptionToTagName(&baseBlockDesc), 1);

    size_t total_width = MAP_WIDTH;
    size_t total_length = MAP_HEIGHT + 1;

    // Compute y level

    size_t size = buildData.size();
    for (size_t i = 0; i < size; i++)
    {
        // Compute max y level for size
        int y = buildData[i].y + 1;
        if (y > maxYlevel)
        {
            maxYlevel = y;
        }
    }

    // Compute blocks

    size_t total_height = maxYlevel + 1;

    vector<int32_t> &&blocks = vector<int32_t>(total_width * total_length * total_height, 0);

    size_t size = buildData.size();
    for (size_t i = 0; i < size; i++)
    {
        size_t x = buildData[i].x;
        size_t y = buildData[i].y;
        size_t z = buildData[i].z;

        size_t block_index_base = x + z * total_width + y * total_width * total_length;
        size_t block_index = x + z * total_width + (y + 1) * total_width * total_length;

        // Add the blocks
        const minecraft::BlockDescription *blockPtr = buildData[i].block_ptr;

        if (blockPtr == NULL)
        {
            // First line, no base blocks
            blocks[block_index] = 1;
        }
        else
        {
            short blockIndex = blockPtr->baseColorIndex;
            if (palette[blockIndex] < 0)
            {
                // Add to palette
                palette[blockIndex] = nextPaletteItem++;
                paletteTag.insert(blockDescriptionToTagName(blockPtr), palette[blockIndex]);
            }

            // Base block
            blocks[block_index_base] = 1;

            // Real block
            if (!isBase)
            {
                blocks[block_index_base] = palette[blockIndex];
            }
        }
    }

    // Data version and author tags
    root.insert("Version", 3);
    root.insert("DataVersion", minecraft::versionToDataVersion(version));

    // Insert blocks
    nbt::tag_compound blocksTag;

    nbt::tag_int_array &&blocksData(std::move(blocks));

    blocksTag.insert("Data", std::move(blocksData));
    blocksTag.insert("Palette", paletteTag.clone());

    root.insert("Blocks", blocksTag.clone());

    // Insert size tags
    root.insert("Width", static_cast<int32_t>(total_width));
    root.insert("Height", static_cast<int32_t>(total_height));
    root.insert("Length", static_cast<int32_t>(total_length));

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

void minecraft::writeSchematicNBTFileCompact(std::string fileName, std::vector<std::vector<mapart::MapBuildingBlock>> &chunks, minecraft::McVersion version, threading::Progress &progress)
{
    nbt::tag_compound root;

    nbt::tag_compound paletteTag;

    vector<int> palette(64);
    int nextPaletteItem = 2;
    int maxYlevel = 0;

    for (size_t i = 0; i < palette.size(); i++)
    {
        palette[i] = -1; // Initialize
    }

    // Add base blocks to palette
    minecraft::BlockDescription baseBlockDesc;
    baseBlockDesc.name = "Base Block";
    baseBlockDesc.minVersion = McVersion::MC_1_12;
    baseBlockDesc.maxVersion = MC_LAST_VERSION;
    baseBlockDesc.nbtName = "stone";
    palette[0] = 1;

    paletteTag.insert("minecraft:air", 0);
    paletteTag.insert(blockDescriptionToTagName(&baseBlockDesc), 1);

    size_t chunk_count = chunks.size();
    size_t total_width = chunk_count * MAP_WIDTH;
    size_t total_length = MAP_HEIGHT + 1;

    // Compute y level

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
        }
    }

    // Compute blocks

    size_t total_height = maxYlevel + 1;

    vector<int32_t> &&blocks = vector<int32_t>(total_width * total_length * total_height, 0);

    for (size_t chunk_i = 0; chunk_i < chunk_count; chunk_i++)
    {
        std::vector<mapart::MapBuildingBlock> *buildData = &chunks[chunk_i];

        size_t offset_x = chunk_i * MAP_WIDTH;

        size_t size = buildData->size();
        for (size_t i = 0; i < size; i++)
        {
            size_t x = buildData->at(i).x + offset_x;
            size_t y = buildData->at(i).y;
            size_t z = buildData->at(i).z;

            size_t block_index_base = x + z * total_width + y * total_width * total_length;
            size_t block_index = x + z * total_width + (y + 1) * total_width * total_length;

            // Add the blocks
            const minecraft::BlockDescription *blockPtr = buildData->at(i).block_ptr;

            if (blockPtr == NULL)
            {
                // First line, no base blocks
                blocks[block_index] = 1;
            }
            else
            {
                short blockIndex = blockPtr->baseColorIndex;
                if (palette[blockIndex] < 0)
                {
                    // Add to palette
                    palette[blockIndex] = nextPaletteItem++;
                    paletteTag.insert(blockDescriptionToTagName(blockPtr), palette[blockIndex]);
                }

                // Base block
                blocks[block_index_base] = 1;

                // Real block
                blocks[block_index_base] = palette[blockIndex];
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
    root.insert("Version", 3);
    root.insert("DataVersion", minecraft::versionToDataVersion(version));

    // Insert blocks
    nbt::tag_compound blocksTag;

    nbt::tag_int_array &&blocksData(std::move(blocks));

    blocksTag.insert("Data", std::move(blocksData));
    blocksTag.insert("Palette", paletteTag.clone());

    root.insert("Blocks", blocksTag.clone());

    // Insert size tags
    root.insert("Width", static_cast<int32_t>(total_width));
    root.insert("Height", static_cast<int32_t>(total_height));
    root.insert("Length", static_cast<int32_t>(total_length));

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

void minecraft::writeSchematicNBTFileCompactFlat(std::string fileName, std::vector<std::vector<mapart::MapBuildingBlock>> &chunks, size_t width, minecraft::McVersion version, threading::Progress &progress)
{
    nbt::tag_compound root;

    nbt::tag_compound paletteTag;

    vector<int> palette(64);
    int nextPaletteItem = 2;
    int maxYlevel = 0;

    for (size_t i = 0; i < palette.size(); i++)
    {
        palette[i] = -1; // Initialize
    }

    // Add base blocks to palette
    minecraft::BlockDescription baseBlockDesc;
    baseBlockDesc.name = "Base Block";
    baseBlockDesc.minVersion = McVersion::MC_1_12;
    baseBlockDesc.maxVersion = MC_LAST_VERSION;
    baseBlockDesc.nbtName = "stone";
    palette[0] = 1;

    paletteTag.insert("minecraft:air", 0);
    paletteTag.insert(blockDescriptionToTagName(&baseBlockDesc), 1);

    size_t chunk_count = chunks.size();
    size_t chunks_length = chunk_count / width;
    size_t total_width = width * MAP_WIDTH;
    size_t total_length = (chunks_length * MAP_HEIGHT) + 1;

    // Compute y level

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
        }
    }

    // Compute blocks

    size_t total_height = maxYlevel + 1;

    vector<int32_t> &&blocks = vector<int32_t>(total_width * total_length * total_height, 0);

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
            size_t x = buildData->at(i).x + offset_x;
            size_t y = buildData->at(i).y;
            size_t z = buildData->at(i).z + offset_z;

            size_t block_index_base = x + z * total_width + y * total_width * total_length;
            size_t block_index = x + z * total_width + (y + 1) * total_width * total_length;

            // Add the blocks
            const minecraft::BlockDescription *blockPtr = buildData->at(i).block_ptr;

            if (blockPtr == NULL)
            {
                // First line, no base blocks
                if (chunk_z == 0)
                {
                    blocks[block_index] = 1;
                }
            }
            else
            {
                short blockIndex = blockPtr->baseColorIndex;
                if (palette[blockIndex] < 0)
                {
                    // Add to palette
                    palette[blockIndex] = nextPaletteItem++;
                    paletteTag.insert(blockDescriptionToTagName(blockPtr), palette[blockIndex]);
                }

                // Base block
                blocks[block_index_base] = 1;

                // Real block
                blocks[block_index_base] = palette[blockIndex];
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
    root.insert("Version", 3);
    root.insert("DataVersion", minecraft::versionToDataVersion(version));

    // Insert blocks
    nbt::tag_compound blocksTag;

    nbt::tag_int_array &&blocksData(std::move(blocks));

    blocksTag.insert("Data", std::move(blocksData));
    blocksTag.insert("Palette", paletteTag.clone());

    root.insert("Blocks", blocksTag.clone());

    // Insert size tags
    root.insert("Width", static_cast<int32_t>(total_width));
    root.insert("Height", static_cast<int32_t>(total_height));
    root.insert("Length", static_cast<int32_t>(total_length));

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

void minecraft::writeSchematicNBTFileZip(std::string fileName, zip_t *zipper, std::vector<mapart::MapBuildingBlock> &buildData, minecraft::McVersion version, bool isBase)
{
    nbt::tag_compound root;

    nbt::tag_compound paletteTag;

    vector<int> palette(64);
    int nextPaletteItem = 2;
    int maxYlevel = 0;

    for (size_t i = 0; i < palette.size(); i++)
    {
        palette[i] = -1; // Initialize
    }

    // Add base blocks to palette
    minecraft::BlockDescription baseBlockDesc;
    baseBlockDesc.name = "Base Block";
    baseBlockDesc.minVersion = McVersion::MC_1_12;
    baseBlockDesc.maxVersion = MC_LAST_VERSION;
    baseBlockDesc.nbtName = "stone";
    palette[0] = 1;

    paletteTag.insert("minecraft:air", 0);
    paletteTag.insert(blockDescriptionToTagName(&baseBlockDesc), 1);

    size_t total_width = MAP_WIDTH;
    size_t total_length = MAP_HEIGHT + 1;

    // Compute y level

    size_t size = buildData.size();
    for (size_t i = 0; i < size; i++)
    {
        // Compute max y level for size
        int y = buildData[i].y + 1;
        if (y > maxYlevel)
        {
            maxYlevel = y;
        }
    }

    // Compute blocks

    size_t total_height = maxYlevel + 1;

    vector<int32_t> &&blocks = vector<int32_t>(total_width * total_length * total_height, 0);

    size_t size = buildData.size();
    for (size_t i = 0; i < size; i++)
    {
        size_t x = buildData[i].x;
        size_t y = buildData[i].y;
        size_t z = buildData[i].z;

        size_t block_index_base = x + z * total_width + y * total_width * total_length;
        size_t block_index = x + z * total_width + (y + 1) * total_width * total_length;

        // Add the blocks
        const minecraft::BlockDescription *blockPtr = buildData[i].block_ptr;

        if (blockPtr == NULL)
        {
            // First line, no base blocks
            blocks[block_index] = 1;
        }
        else
        {
            short blockIndex = blockPtr->baseColorIndex;
            if (palette[blockIndex] < 0)
            {
                // Add to palette
                palette[blockIndex] = nextPaletteItem++;
                paletteTag.insert(blockDescriptionToTagName(blockPtr), palette[blockIndex]);
            }

            // Base block
            blocks[block_index_base] = 1;

            // Real block
            if (!isBase)
            {
                blocks[block_index_base] = palette[blockIndex];
            }
        }
    }

    // Data version and author tags
    root.insert("Version", 3);
    root.insert("DataVersion", minecraft::versionToDataVersion(version));

    // Insert blocks
    nbt::tag_compound blocksTag;

    nbt::tag_int_array &&blocksData(std::move(blocks));

    blocksTag.insert("Data", std::move(blocksData));
    blocksTag.insert("Palette", paletteTag.clone());

    root.insert("Blocks", blocksTag.clone());

    // Insert size tags
    root.insert("Width", static_cast<int32_t>(total_width));
    root.insert("Height", static_cast<int32_t>(total_height));
    root.insert("Length", static_cast<int32_t>(total_length));

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
