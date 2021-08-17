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

#include "mc_blocks.h"

using namespace minecraft;
using namespace std;

// Implementation

void BlockDescription::addTag(std::string name, std::string value) {
    size_t size = this->nbtTags.size();
    this->nbtTags.resize(size + 1);
    this->nbtTags[size].name = name;
    this->nbtTags[size].value = value;
}


minecraft::BlockDescription * Block::getBlockDescription(minecraft::McVersion version) {
    size_t size = this->descriptions.size();

    for (size_t i = 0; i < size; i++) {
        minecraft::BlockDescription * bd = &(this->descriptions[i]);

        if (bd->minVersion <= version && bd->maxVersion >= version) {
            return bd;
        }
    }

    return NULL;
}

minecraft::BlockDescription * Block::addBlockDescription(std::string name, minecraft::McVersion minVersion, minecraft::McVersion maxVersion, std::string nbtName) {
    return this->addBlockDescription(name, minVersion, maxVersion, nbtName, 0, 0);
}

minecraft::BlockDescription * Block::addBlockDescription(std::string name, minecraft::McVersion minVersion, minecraft::McVersion maxVersion, std::string nbtName, short blockId, short dataValue) {
    size_t size = this->descriptions.size();
    this->descriptions.resize(size + 1);

    minecraft::BlockDescription * bd = &(this->descriptions[size]);

    bd->name = name;
    bd->minVersion = minVersion;
    bd->maxVersion = maxVersion;
    bd->nbtName = nbtName;
    bd->blockId = blockId;
    bd->dataValue = dataValue;

    return bd;
}

minecraft::BlockDescription * BlockList::getBlockDescription(minecraft::McVersion version) {
    minecraft::BlockDescription * res;
    size_t size = this->blocks.size();
    if (this->useBlockIndex >= 0 && this->useBlockIndex < size) {
        res = this->blocks[this->useBlockIndex].getBlockDescription(version);
        if (res != NULL) {
            return res;
        }
    }
    for (size_t i = 0; i < size; i++) {
        res = this->blocks[i].getBlockDescription(version);
        if (res != NULL) {
            return res;
        }
    }
    return NULL;
}


minecraft::Block * BlockList::addBlock(std::string id) {
    size_t size = this->blocks.size();
    this->blocks.resize(size + 1);
    minecraft::Block * res = &(this->blocks[size]);
    res->id = id;
    res->baseColorIndex = this->baseColorIndex;
    return res;
}

std::vector<minecraft::BlockList> minecraft::loadBlocks(std::vector<colors::Color> &baseColors) {
    size_t size = baseColors.size();
    std::vector<minecraft::BlockList> list(size);
    BlockList * blockList;
    Block* block;
    BlockDescription * desc;

    /* Initialize */
    for (size_t i = 0; i < size; i++) {
        list[i].baseColorIndex = i;
        list[i].useBlockIndex = 0;
    }

    // ---------------
    // ---- GRASS ----
    // ---------------
    blockList = &(list[(size_t)McColors::GRASS]);

    // Grass block
    block = blockList->addBlock(string("grass_block"));

    desc = block->addBlockDescription(
        "Grass Block", 
        McVersion::MC_1_12, McVersion::MC_1_12,
        "grass", // NBT name
        2, // Block ID
        0 // Data value
    );

    desc = block->addBlockDescription(
        "Grass Block", 
        McVersion::MC_1_13, MC_LAST_VERSION,
        "grass_block" // NBT name
    );

    // Slime block
    block = blockList->addBlock(string("slime_block"));

    desc = block->addBlockDescription(
        "Slime Block", 
        McVersion::MC_1_12, McVersion::MC_1_12,
        "slime", // NBT name
        165, // Block ID
        0 // Data value
    );

    desc = block->addBlockDescription(
        "Slime Block", 
        McVersion::MC_1_13, MC_LAST_VERSION,
        "slime_block" // NBT name
    );


    // ---------------
    // ---- SAND ----
    // ---------------
    blockList = &(list[(size_t)McColors::SAND]);

    // Sand
    block = blockList->addBlock(string("sand"));

    desc = block->addBlockDescription(
        "Sand", 
        McVersion::MC_1_12, MC_LAST_VERSION,
        "sand", // NBT name
        12, // Block ID
        0 // Data value
    );
    
    return list; // Return the list of blocks
}
