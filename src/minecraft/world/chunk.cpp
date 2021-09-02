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

#include "chunk.h"

#include <algorithm>

#include <io/stream_writer.h>
#include <io/ozlibstream.h>
#include <nbt_tags.h>
#include <sstream>

using namespace std;
using namespace minecraft;

ChunkSection::ChunkSection() {
    y = 0;
}

ChunkSection::ChunkSection(int y) {
    // Init palette
    palette.resize(2);

    BlockDescription descAir;
    descAir.name = "Air";
    descAir.nbtName = "air";
    descAir.baseColorIndex = 0;

    palette[0] = descAir; // First is air

    BlockDescription descStone;
    descStone.name = "Stone";
    descStone.nbtName = "stone";
    descStone.baseColorIndex = 0;

    palette[1] = descStone; // Second is stone

    // Init palette map
    paletteMap.resize(MAX_COLOR_GROUPS);

    for (int i = 0; i < paletteMap.size(); i++) {
        paletteMap[i] = -1;
    }

    paletteMap[0] = 0;

    // Init blocks
    blocks.resize(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_SECTION_HEIGHT);

    for (int i = 0; i < blocks.size(); i++) {
        blocks[i] = 0;
    }
}

void ChunkSection::setBlock(const minecraft::BlockDescription *block, int x, int y, int z) {
    x = min(max(0, x), CHUNK_WIDTH - 1);
    y = min(max(0, y), CHUNK_SECTION_HEIGHT - 1);
    z = min(max(0, z), CHUNK_HEIGHT - 1);

    uint8_t palleteIndex;

    if (block == NULL) {
        palleteIndex = 1; // Stone
    } else {
        if (paletteMap[block->baseColorIndex] > 0) {
            // Already in palette
            palleteIndex = paletteMap[block->baseColorIndex];
        } else {
            palette.resize(palette.size() + 1);
            palette[palette.size() - 1] = *block;
            paletteMap[block->baseColorIndex] = palette.size() - 1;
        }
    }


    blocks[BLOCK_POS_IN_SECTION(x, y, z)] = palleteIndex;
}

std::vector<uint64_t> ChunkSection::getBlockStates(minecraft::McVersion version) {
    BlockStatesArrayMaker am(palette.size(), version > McVersion::MC_1_16 /* Padding starts at version 1.16 */);

    for (size_t i = 0; i < blocks.size(); i++) {
        am.insertState(blocks[i]);
    }

    return am.array;
}

MinecraftChunk::MinecraftChunk(minecraft::McVersion version, int x, int z) {
    this->x = x;
    this->z = z;
    this->version = version;
}

MinecraftChunk::MinecraftChunk() {
    x = 0;
    z = 0;
    version = MC_LAST_VERSION;
}

minecraft::ChunkSection * MinecraftChunk::getSection(int y) {
    int sectionY = y / CHUNK_SECTION_HEIGHT;
    sectionY * CHUNK_SECTION_HEIGHT;

    for (int i = 0; i < sections.size(); i++) {
        if (sections[i].y == sectionY) {
            return &(sections[i]);
        }
    }

    sections.resize(sections.size() + 1);

    sections[sections.size() - 1] = ChunkSection(sectionY);

    return &(sections[sections.size() - 1]);
}

void MinecraftChunk::addBlock(const minecraft::BlockDescription * block, int x, int y, int z) {
    minecraft::ChunkSection * section = getSection(y);

    // Limit inside coordinates
    y = min(max(0, y - section->y), CHUNK_SECTION_HEIGHT);

    section->setBlock(block, x, y, z);
}

void MinecraftChunk::addBasePlatform(int offsetY) {
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_HEIGHT; z++) {
            for (int y = 0; y <= offsetY; y++) {
                addBlock(NULL, x, y, z);
            }
        }
    }
}

std::string MinecraftChunk::getBytes() {
    nbt::tag_compound root;
    nbt::tag_compound levelTag;

    levelTag.insert("xPos", nbt::tag_int(x));
    levelTag.insert("zPos", nbt::tag_int(z));
    levelTag.insert("Status", nbt::tag_string("full"));

    nbt::tag_list sectionsTag;

    for (int i = 0; i < sections.size(); i++) {
        nbt::tag_compound sectionTag;
        ChunkSection * section = &(sections[i]);

        // Set Y
        sectionTag.insert("Y", nbt::tag_int(section->y));

        // Palette
        nbt::tag_list palette;
        for (int j = 0; j < section->palette.size(); j++) {
            palette.push_back(blockDescriptionToTag(&(section->palette[j])));
        }
        sectionTag.insert("Palette", palette.clone());

        // BlockLight (0)
        nbt::tag_byte_array blockLight;
        for (int j = 0; j < 2048; j++) {
            blockLight.push_back(0x00);
        }
        sectionTag.insert("BlockLight", blockLight.clone());

        // SkyLight (15)
        nbt::tag_byte_array skyLight;
        for (int j = 0; j < 2048; j++) {
            skyLight.push_back(0xFF);
        }
        sectionTag.insert("SkyLight", skyLight.clone());

        // BlockStates

        nbt::tag_long_array blockStates;
        std::vector<uint64_t> statesBS = section->getBlockStates(version);
        for (int j = 0; j < statesBS.size(); j++) {
            blockStates.push_back(statesBS[i]);
        }
        sectionTag.insert("BlockStates", blockStates.clone());
        

        sectionsTag.push_back(sectionTag.clone());
    }

    levelTag.insert("Sections", sectionsTag.clone());
    levelTag.insert("LastUpdate", nbt::tag_int(0));
    levelTag.insert("InhabitedTime", nbt::tag_int(0));

    root.insert("DataVersion", minecraft::versionToDataVersion(version));
    root.insert("Level", levelTag.clone());

    ostringstream os;
    zlib::ozlibstream ogzs(os, -1, false);

    nbt::io::write_tag("", root, ogzs);

    return os.str();
}
