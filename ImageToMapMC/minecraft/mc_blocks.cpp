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

void BlockDescription::addTag(std::string name, std::string value)
{
    size_t size = this->nbtTags.size();
    this->nbtTags.resize(size + 1);
    this->nbtTags[size].name = name;
    this->nbtTags[size].value = value;
}

minecraft::BlockDescription *Block::getBlockDescription(minecraft::McVersion version)
{
    size_t size = this->descriptions.size();

    for (size_t i = 0; i < size; i++)
    {
        minecraft::BlockDescription *bd = &(this->descriptions[i]);

        if (bd->minVersion <= version && bd->maxVersion >= version)
        {
            return bd;
        }
    }

    return NULL;
}

minecraft::BlockDescription *Block::addBlockDescription(std::string name, minecraft::McVersion minVersion, minecraft::McVersion maxVersion, std::string nbtName)
{
    return this->addBlockDescription(name, minVersion, maxVersion, nbtName, 0, 0);
}

minecraft::BlockDescription *Block::addBlockDescription(std::string name, minecraft::McVersion minVersion, minecraft::McVersion maxVersion, std::string nbtName, short blockId, short dataValue)
{
    size_t size = this->descriptions.size();
    this->descriptions.resize(size + 1);

    minecraft::BlockDescription *bd = &(this->descriptions[size]);

    bd->name = name;
    bd->minVersion = minVersion;
    bd->maxVersion = maxVersion;
    bd->nbtName = nbtName;
    bd->blockId = blockId;
    bd->dataValue = dataValue;

    return bd;
}

minecraft::BlockDescription *BlockList::getBlockDescription(minecraft::McVersion version)
{
    minecraft::BlockDescription *res;
    size_t size = this->blocks.size();
    if (this->useBlockIndex >= 0 && this->useBlockIndex < size)
    {
        res = this->blocks[this->useBlockIndex].getBlockDescription(version);
        if (res != NULL)
        {
            return res;
        }
    }
    for (size_t i = 0; i < size; i++)
    {
        res = this->blocks[i].getBlockDescription(version);
        if (res != NULL)
        {
            return res;
        }
    }
    return NULL;
}

minecraft::Block *BlockList::addBlock(std::string id)
{
    size_t size = this->blocks.size();
    this->blocks.resize(size + 1);
    minecraft::Block *res = &(this->blocks[size]);
    res->id = id;
    res->baseColorIndex = this->baseColorIndex;
    return res;
}

std::vector<minecraft::BlockList> minecraft::loadBlocks(std::vector<colors::Color> &baseColors)
{
    size_t size = baseColors.size();
    std::vector<minecraft::BlockList> list(size);
    BlockList *blockList;
    Block *block;
    BlockDescription *desc;

    /* Initialize */
    for (size_t i = 0; i < size; i++)
    {
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
        2,       // Block ID
        0        // Data value
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
        165,     // Block ID
        0        // Data value
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
        12,     // Block ID
        0       // Data value
    );

    // Sandstone
    block = blockList->addBlock(string("sandstone"));

    desc = block->addBlockDescription(
        "Sandstone",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "sandstone", // NBT name
        24,          // Block ID
        0            // Data value
    );

    // Glowstone
    block = blockList->addBlock(string("glowstone"));

    desc = block->addBlockDescription(
        "Glowstone",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "glowstone", // NBT name
        89,          // Block ID
        0            // Data value
    );

    // End Stone
    block = blockList->addBlock(string("end_stone"));

    desc = block->addBlockDescription(
        "End Stone",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "end_stone", // NBT name
        121,         // Block ID
        0            // Data value
    );

    // Bone Block
    block = blockList->addBlock(string("bone_block"));

    desc = block->addBlockDescription(
        "Bone Block",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "bone_block", // NBT name
        216,          // Block ID
        0             // Data value
    );

    // ---------------
    // ---- WOOL ----
    // ---------------
    blockList = &(list[(size_t)McColors::WOOL]);

    // Cobweb
    block = blockList->addBlock(string("web"));

    desc = block->addBlockDescription(
        "Cobweb",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "web", // NBT name
        30,    // Block ID
        0      // Data value
    );

    desc = block->addBlockDescription(
        "Cobweb",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "cobweb" // NBT name
    );

    // Mushroom Stem (1.13+)
    block = blockList->addBlock(string("mushroom_stem"));

    desc = block->addBlockDescription(
        "Mushroom Stem",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "mushroom_stem" // NBT name
    );

    // White candle (1.17+)
    block = blockList->addBlock(string("white_candle"));

    desc = block->addBlockDescription(
        "White Candle",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "white_candle" // NBT name
    );

    // ---------------
    // ---- FIRE ----
    // ---------------
    blockList = &(list[(size_t)McColors::FIRE]);

    // Redstone block
    block = blockList->addBlock(string("redstone_block"));

    desc = block->addBlockDescription(
        "Block of Redstone",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "redstone_block", // NBT name
        152,              // Block ID
        0                 // Data value
    );

    // TNT
    block = blockList->addBlock(string("tnt"));

    desc = block->addBlockDescription(
        "TNT",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "tnt", // NBT name
        46,    // Block ID
        0      // Data value
    );

    // ---------------
    // ---- ICE ----
    // ---------------
    blockList = &(list[(size_t)McColors::ICE]);

    // Packed Ice
    block = blockList->addBlock(string("packed_ice"));

    desc = block->addBlockDescription(
        "Packed Ice",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "packed_ice", // NBT name
        174,          // Block ID
        0             // Data value
    );

    // Blue Ice
    block = blockList->addBlock(string("blue_ice"));

    desc = block->addBlockDescription(
        "Blue Ice",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "blue_ice" // NBT name
    );

    // ---------------
    // ---- METAL ----
    // ---------------
    blockList = &(list[(size_t)McColors::METAL]);

    // Iron block
    block = blockList->addBlock(string("iron_block"));

    desc = block->addBlockDescription(
        "Block of Iron",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "iron_block", // NBT name
        42,           // Block ID
        0             // Data value
    );

    // Iron Trapdoor
    block = blockList->addBlock(string("iron_trapdoor"));

    desc = block->addBlockDescription(
        "Iron Trapdoor",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "iron_trapdoor", // NBT name
        167,             // Block ID
        0                // Data value
    );
    desc->addTag(string("half"), string("bottom"));

    // Iron pressure plate
    block = blockList->addBlock(string("heavy_weighted_pressure_plate"));

    desc = block->addBlockDescription(
        "Weighted Pressure Plate (Heavy)",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "heavy_weighted_pressure_plate", // NBT name
        148,                             // Block ID
        0                                // Data value
    );

    // ---------------
    // ---- PLANT ----
    // ---------------
    blockList = &(list[(size_t)McColors::PLANT]);

    // Leaves
    block = blockList->addBlock(string("leaves"));

    desc = block->addBlockDescription(
        "Leaves",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "leaves", // NBT name
        18,       // Block ID
        0         // Data value
    );
    desc->addTag(string("decayable"), string("false"));
    desc->addTag(string("check_decay"), string("false"));
    desc->addTag(string("variant"), string("oak"));

    desc = block->addBlockDescription(
        "Leaves",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "oak_leaves" // NBT name
    );
    desc->addTag(string("persistent"), string("true"));

    // ---------------
    // ---- SNOW ----
    // ---------------
    blockList = &(list[(size_t)McColors::SNOW]);

    // Snow block
    block = blockList->addBlock(string("snow"));

    desc = block->addBlockDescription(
        "Snow Block",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "snow", // NBT name
        80,     // Block ID
        0       // Data value
    );

    desc = block->addBlockDescription(
        "Snow Block",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "snow_block" // NBT name
    );

    // White wool
    block = blockList->addBlock(string("white_wool"));

    desc = block->addBlockDescription(
        "White Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        0       // Data value
    );
    desc->addTag(string("color"), string("white"));

    desc = block->addBlockDescription(
        "White Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "white_wool" // NBT name
    );

    // White carpet
    block = blockList->addBlock(string("white_carpet"));

    desc = block->addBlockDescription(
        "White Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        0         // Data value
    );
    desc->addTag(string("color"), string("white"));

    desc = block->addBlockDescription(
        "White Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "white_carpet" // NBT name
    );

    // White concrete
    block = blockList->addBlock(string("white_concrete"));

    desc = block->addBlockDescription(
        "White Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        0           // Data value
    );
    desc->addTag(string("color"), string("white"));

    desc = block->addBlockDescription(
        "White Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "white_concrete" // NBT name
    );

    // ---------------
    // ---- CLAY ----
    // ---------------
    blockList = &(list[(size_t)McColors::CLAY]);

    // Clay
    block = blockList->addBlock(string("clay"));

    desc = block->addBlockDescription(
        "Clay",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "clay", // NBT name
        82,     // Block ID
        0       // Data value
    );

    // ---------------
    // ---- DIRT ----
    // ---------------
    blockList = &(list[(size_t)McColors::DIRT]);

    // Jungle planks
    block = blockList->addBlock(string("jungle_planks"));

    desc = block->addBlockDescription(
        "Jungle Planks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "planks", // NBT name
        5,        // Block ID
        3         // Data value
    );
    desc->addTag(string("variant"), string("jungle"));

    desc = block->addBlockDescription(
        "Jungle Planks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "jungle_planks" // NBT name
    );

    // Granite
    block = blockList->addBlock(string("granite"));

    desc = block->addBlockDescription(
        "Granite",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stone", // NBT name
        1,       // Block ID
        1        // Data value
    );
    desc->addTag(string("variant"), string("granite"));

    desc = block->addBlockDescription(
        "Granite",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "granite" // NBT name
    );

    // Jungle log
    block = blockList->addBlock(string("jungle_log"));

    desc = block->addBlockDescription(
        "Jungle Log (vertical)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log", // NBT name
        17,    // Block ID
        3      // Data value
    );
    desc->addTag(string("variant"), string("jungle"));
    desc->addTag(string("axis"), string("y"));

    desc = block->addBlockDescription(
        "Jungle Log (vertical)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "jungle_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // ---------------
    // ---- STONE ----
    // ---------------
    blockList = &(list[(size_t)McColors::STONE]);

    // Cobblestone
    block = blockList->addBlock(string("cobblestone"));

    desc = block->addBlockDescription(
        "Cobblestone",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "cobblestone", // NBT name
        4,             // Block ID
        0              // Data value
    );

    // Stone
    block = blockList->addBlock(string("stone"));

    desc = block->addBlockDescription(
        "Stone",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "stone", // NBT name
        1,       // Block ID
        0        // Data value
    );

    // Andesite
    block = blockList->addBlock(string("andesite"));

    desc = block->addBlockDescription(
        "Andesite",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stone", // NBT name
        1,       // Block ID
        5        // Data value
    );
    desc->addTag(string("variant"), string("andesite"));

    desc = block->addBlockDescription(
        "Andesite",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "andesite" // NBT name
    );

    // ---------------
    // ---- WATER ----
    // ---------------
    blockList = &(list[(size_t)McColors::WATER]);

    // Water
    block = blockList->addBlock(string("water"));

    desc = block->addBlockDescription(
        "Water",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "water", // NBT name
        9,       // Block ID
        0        // Data value
    );

    // ---------------
    // ---- WOOD ----
    // ---------------
    blockList = &(list[(size_t)McColors::WOOD]);

    // Oak planks
    block = blockList->addBlock(string("oak_planks"));

    desc = block->addBlockDescription(
        "Oak Planks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "planks", // NBT name
        5,        // Block ID
        0         // Data value
    );
    desc->addTag(string("variant"), string("oak"));

    desc = block->addBlockDescription(
        "Oak Planks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "oak_planks" // NBT name
    );

    // Oak log
    block = blockList->addBlock(string("oak_log"));

    desc = block->addBlockDescription(
        "Oak Log (vertical)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log", // NBT name
        17,    // Block ID
        0      // Data value
    );
    desc->addTag(string("variant"), string("oak"));
    desc->addTag(string("axis"), string("y"));

    desc = block->addBlockDescription(
        "Oak Log (vertical)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "oak_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // ---------------
    // ---- QUARTZ ----
    // ---------------
    blockList = &(list[(size_t)McColors::QUARTZ]);

    // Diorite
    block = blockList->addBlock(string("diorite"));

    desc = block->addBlockDescription(
        "Diorite",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stone", // NBT name
        1,       // Block ID
        3        // Data value
    );
    desc->addTag(string("variant"), string("diorite"));

    desc = block->addBlockDescription(
        "Diorite",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "diorite" // NBT name
    );

    // Quartz block
    block = blockList->addBlock(string("quartz_block"));

    desc = block->addBlockDescription(
        "Quartz Block",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "quartz_block", // NBT name
        155,            // Block ID
        0               // Data value
    );

    // Sea lantern
    block = blockList->addBlock(string("sea_lantern"));

    desc = block->addBlockDescription(
        "Sea Lantern",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "sea_lantern", // NBT name
        169,           // Block ID
        0              // Data value
    );

    return list; // Return the list of blocks
}
