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
        for (int i = 0; i < desc->nbtTags.size(); i++)
        {
            props.insert(desc->nbtTags[i].name, desc->nbtTags[i].value);
        }
        tag.insert("Properties", props.clone());
    }

    return tag;
}

nbt::tag_compound blockToTag(const mapart::MapBuildingBlock &block, vector<int> &palette, bool placeholder)
{
    nbt::tag_compound tag;

    // Position
    nbt::tag_list posTag;
    posTag.push_back(nbt::tag_int(block.x));
    posTag.push_back(nbt::tag_int(placeholder ? (block.y) : (block.y + 1)));
    posTag.push_back(nbt::tag_int(block.z));
    tag.insert("pos", posTag.clone());

    // State
    if (placeholder)
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

void minecraft::writeStructureNBTFile(std::string fileName, std::vector<mapart::MapBuildingBlock> &buildData, minecraft::McVersion version)
{
    nbt::tag_compound root;
    nbt::tag_list blocksTag;
    nbt::tag_list paletteTag;
    vector<int> palette(64);
    int nextPaletteItem = 1;
    int maxYlevel = 0;

    for (int i = 0; i < palette.size(); i++) {
        palette[i] = -1; // Initiallize
    }

    // Add placeholder blocks to palette
    minecraft::BlockDescription placeholderBlockDesc;
    placeholderBlockDesc.name = "Placeholder";
    placeholderBlockDesc.minVersion = McVersion::MC_1_12;
    placeholderBlockDesc.maxVersion = MC_LAST_VERSION;
    placeholderBlockDesc.nbtName = "stone";
    palette[0] = 0;

    paletteTag.push_back(blockDescriptionToTag(&placeholderBlockDesc));

    // Blocks parsing
    size_t size = buildData.size();
    for (int i = 0; i < size; i++)
    {
        // Compute max y level for size
        int y = buildData[i].y + 1;
        if (y > maxYlevel) {
            maxYlevel = y;
        }

        // Add the blocks
        const minecraft::BlockDescription *blockPtr = buildData[i].block_ptr;
        
        if (blockPtr == NULL)
        {
            // First line, no placeholder
            blocksTag.push_back(blockToTag(buildData[i], palette, false));
        } else {
            short blockIndex = blockPtr->baseColorIndex;
            if (palette[blockIndex] < 0) {
                // Add to palette
                palette[blockIndex] = nextPaletteItem++;
                paletteTag.push_back(blockDescriptionToTag(blockPtr));
            }

            // Placeholder block
            blocksTag.push_back(blockToTag(buildData[i], palette, true));

            // Real block
            blocksTag.push_back(blockToTag(buildData[i], palette, false));
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
