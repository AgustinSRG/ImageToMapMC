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

const minecraft::BlockDescription *Block::getBlockDescription(minecraft::McVersion version) const
{
    size_t size = this->descriptions.size();

    for (size_t i = 0; i < size; i++)
    {
        const minecraft::BlockDescription *bd = &(this->descriptions[i]);

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
    bd->baseColorIndex = this->baseColorIndex;
    bd->minVersion = minVersion;
    bd->maxVersion = maxVersion;
    bd->nbtName = nbtName;
    bd->blockId = blockId;
    bd->dataValue = dataValue;

    return bd;
}

const minecraft::BlockDescription *BlockList::getBlockDescription(minecraft::McVersion version) const
{
    const minecraft::BlockDescription *res;
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

short BlockList::findBlock(std::string id)
{
    size_t size = this->blocks.size();
    for (size_t i = 0; i < size; i++)
    {
        if (id.compare(this->blocks[i].id) == 0)
        {
            return (short)i;
        }
    }

    return -1;
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
        list[i].baseColorIndex = static_cast<short>(i);
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

    // Mushroom Stem (1.13+)
    block = blockList->addBlock(string("mushroom_stem"));

    desc = block->addBlockDescription(
        "Mushroom Stem",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "mushroom_stem" // NBT name
    );

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

    // ---------------
    // ---- COLOR_ORANGE ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_ORANGE]);

    // Orange wool
    block = blockList->addBlock(string("orange_wool"));

    desc = block->addBlockDescription(
        "Orange Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        1       // Data value
    );
    desc->addTag(string("color"), string("orange"));

    desc = block->addBlockDescription(
        "Orange Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "orange_wool" // NBT name
    );

    // Orange carpet
    block = blockList->addBlock(string("orange_carpet"));

    desc = block->addBlockDescription(
        "Orange Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        1         // Data value
    );
    desc->addTag(string("color"), string("orange"));

    desc = block->addBlockDescription(
        "Orange Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "orange_carpet" // NBT name
    );

    // Orange concrete
    block = blockList->addBlock(string("orange_concrete"));

    desc = block->addBlockDescription(
        "Orange Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        1           // Data value
    );
    desc->addTag(string("color"), string("orange"));

    desc = block->addBlockDescription(
        "Orange Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "orange_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_MAGENTA ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_MAGENTA]);

    // Magenta wool
    block = blockList->addBlock(string("magenta_wool"));

    desc = block->addBlockDescription(
        "Magenta Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        2       // Data value
    );
    desc->addTag(string("color"), string("magenta"));

    desc = block->addBlockDescription(
        "Magenta Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "magenta_wool" // NBT name
    );

    // Magenta carpet
    block = blockList->addBlock(string("magenta_carpet"));

    desc = block->addBlockDescription(
        "Magenta Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        2         // Data value
    );
    desc->addTag(string("color"), string("magenta"));

    desc = block->addBlockDescription(
        "Magenta Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "magenta_carpet" // NBT name
    );

    // Magenta concrete
    block = blockList->addBlock(string("magenta_concrete"));

    desc = block->addBlockDescription(
        "Magenta Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        2           // Data value
    );
    desc->addTag(string("color"), string("magenta"));

    desc = block->addBlockDescription(
        "Magenta Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "magenta_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_LIGHT_BLUE ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_LIGHT_BLUE]);

    // Light Blue wool
    block = blockList->addBlock(string("light_blue_wool"));

    desc = block->addBlockDescription(
        "Light Blue Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        3       // Data value
    );
    desc->addTag(string("color"), string("light_blue"));

    desc = block->addBlockDescription(
        "Light Blue Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_blue_wool" // NBT name
    );

    // Light Blue carpet
    block = blockList->addBlock(string("light_blue_carpet"));

    desc = block->addBlockDescription(
        "Light Blue Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        3         // Data value
    );
    desc->addTag(string("color"), string("light_blue"));

    desc = block->addBlockDescription(
        "Light Blue Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_blue_carpet" // NBT name
    );

    // Light Blue concrete
    block = blockList->addBlock(string("light_blue_concrete"));

    desc = block->addBlockDescription(
        "Light Blue Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        3           // Data value
    );
    desc->addTag(string("color"), string("light_blue"));

    desc = block->addBlockDescription(
        "Light Blue Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_blue_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_YELLOW ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_YELLOW]);

    // Yellow wool
    block = blockList->addBlock(string("yellow_wool"));

    desc = block->addBlockDescription(
        "Yellow Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        4       // Data value
    );
    desc->addTag(string("color"), string("yellow"));

    desc = block->addBlockDescription(
        "Yellow Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "yellow_wool" // NBT name
    );

    // Yellow carpet
    block = blockList->addBlock(string("yellow_carpet"));

    desc = block->addBlockDescription(
        "Yellow Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        4         // Data value
    );
    desc->addTag(string("color"), string("yellow"));

    desc = block->addBlockDescription(
        "Yellow Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "yellow_carpet" // NBT name
    );

    // Yellow concrete
    block = blockList->addBlock(string("yellow_concrete"));

    desc = block->addBlockDescription(
        "Yellow Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        4           // Data value
    );
    desc->addTag(string("color"), string("yellow"));

    desc = block->addBlockDescription(
        "Yellow Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "yellow_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_LIGHT_GREEN ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_LIGHT_GREEN]);

    // Lime wool
    block = blockList->addBlock(string("lime_wool"));

    desc = block->addBlockDescription(
        "Lime Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        5       // Data value
    );
    desc->addTag(string("color"), string("lime"));

    desc = block->addBlockDescription(
        "Lime Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "lime_wool" // NBT name
    );

    // Lime carpet
    block = blockList->addBlock(string("lime_carpet"));

    desc = block->addBlockDescription(
        "Lime Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        5         // Data value
    );
    desc->addTag(string("color"), string("lime"));

    desc = block->addBlockDescription(
        "Lime Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "lime_carpet" // NBT name
    );

    // Lime concrete
    block = blockList->addBlock(string("lime_concrete"));

    desc = block->addBlockDescription(
        "Lime Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        5           // Data value
    );
    desc->addTag(string("color"), string("lime"));

    desc = block->addBlockDescription(
        "Lime Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "lime_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_PINK ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_PINK]);

    // Pink wool
    block = blockList->addBlock(string("pink_wool"));

    desc = block->addBlockDescription(
        "Pink Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        6       // Data value
    );
    desc->addTag(string("color"), string("pink"));

    desc = block->addBlockDescription(
        "Pink Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "pink_wool" // NBT name
    );

    // Pink carpet
    block = blockList->addBlock(string("pink_carpet"));

    desc = block->addBlockDescription(
        "Pink Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        6         // Data value
    );
    desc->addTag(string("color"), string("pink"));

    desc = block->addBlockDescription(
        "Pink Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "pink_carpet" // NBT name
    );

    // Pink concrete
    block = blockList->addBlock(string("pink_concrete"));

    desc = block->addBlockDescription(
        "Pink Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        6           // Data value
    );
    desc->addTag(string("color"), string("pink"));

    desc = block->addBlockDescription(
        "Pink Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "pink_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_GRAY ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_GRAY]);

    // Gray wool
    block = blockList->addBlock(string("gray_wool"));

    desc = block->addBlockDescription(
        "Gray Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        7       // Data value
    );
    desc->addTag(string("color"), string("gray"));

    desc = block->addBlockDescription(
        "Gray Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "gray_wool" // NBT name
    );

    // Gray carpet
    block = blockList->addBlock(string("gray_carpet"));

    desc = block->addBlockDescription(
        "Gray Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        7         // Data value
    );
    desc->addTag(string("color"), string("gray"));

    desc = block->addBlockDescription(
        "Gray Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "gray_carpet" // NBT name
    );

    // Gray concrete
    block = blockList->addBlock(string("gray_concrete"));

    desc = block->addBlockDescription(
        "Gray Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        7           // Data value
    );
    desc->addTag(string("color"), string("gray"));

    desc = block->addBlockDescription(
        "Gray Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "gray_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_LIGHT_GRAY ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_LIGHT_GRAY]);

    // Light Gray wool
    block = blockList->addBlock(string("light_gray_wool"));

    desc = block->addBlockDescription(
        "Light Gray Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        8       // Data value
    );
    desc->addTag(string("color"), string("light_gray"));

    desc = block->addBlockDescription(
        "Light Gray Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_gray_wool" // NBT name
    );

    // Light Gray carpet
    block = blockList->addBlock(string("light_gray_carpet"));

    desc = block->addBlockDescription(
        "Light Gray Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        8         // Data value
    );
    desc->addTag(string("color"), string("light_gray"));

    desc = block->addBlockDescription(
        "Light Gray Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_gray_carpet" // NBT name
    );

    // Light Gray concrete
    block = blockList->addBlock(string("light_gray_concrete"));

    desc = block->addBlockDescription(
        "Light Gray Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        8           // Data value
    );
    desc->addTag(string("color"), string("light_gray"));

    desc = block->addBlockDescription(
        "Light Gray Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_gray_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_CYAN ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_CYAN]);

    // Cyan wool
    block = blockList->addBlock(string("cyan_wool"));

    desc = block->addBlockDescription(
        "Cyan Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        9       // Data value
    );
    desc->addTag(string("color"), string("cyan"));

    desc = block->addBlockDescription(
        "Cyan Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "cyan_wool" // NBT name
    );

    // Cyan carpet
    block = blockList->addBlock(string("cyan_carpet"));

    desc = block->addBlockDescription(
        "Cyan Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        9         // Data value
    );
    desc->addTag(string("color"), string("cyan"));

    desc = block->addBlockDescription(
        "Cyan Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "cyan_carpet" // NBT name
    );

    // Cyan concrete
    block = blockList->addBlock(string("cyan_concrete"));

    desc = block->addBlockDescription(
        "Cyan Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        9           // Data value
    );
    desc->addTag(string("color"), string("cyan"));

    desc = block->addBlockDescription(
        "Cyan Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "cyan_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_PURPLE ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_PURPLE]);

    // Purple wool
    block = blockList->addBlock(string("purple_wool"));

    desc = block->addBlockDescription(
        "Purple Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        10      // Data value
    );
    desc->addTag(string("color"), string("purple"));

    desc = block->addBlockDescription(
        "Purple Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "purple_wool" // NBT name
    );

    // Purple carpet
    block = blockList->addBlock(string("purple_carpet"));

    desc = block->addBlockDescription(
        "Purple Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        10        // Data value
    );
    desc->addTag(string("color"), string("purple"));

    desc = block->addBlockDescription(
        "Purple Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "purple_carpet" // NBT name
    );

    // Purple concrete
    block = blockList->addBlock(string("purple_concrete"));

    desc = block->addBlockDescription(
        "Purple Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        10          // Data value
    );
    desc->addTag(string("color"), string("purple"));

    desc = block->addBlockDescription(
        "Purple Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "purple_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_BLUE ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_BLUE]);

    // Blue wool
    block = blockList->addBlock(string("blue_wool"));

    desc = block->addBlockDescription(
        "Blue Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        11      // Data value
    );
    desc->addTag(string("color"), string("blue"));

    desc = block->addBlockDescription(
        "Blue Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "blue_wool" // NBT name
    );

    // Blue carpet
    block = blockList->addBlock(string("blue_carpet"));

    desc = block->addBlockDescription(
        "Blue Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        11        // Data value
    );
    desc->addTag(string("color"), string("blue"));

    desc = block->addBlockDescription(
        "Blue Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "blue_carpet" // NBT name
    );

    // Blue concrete
    block = blockList->addBlock(string("blue_concrete"));

    desc = block->addBlockDescription(
        "Blue Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        11          // Data value
    );
    desc->addTag(string("color"), string("blue"));

    desc = block->addBlockDescription(
        "Blue Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "blue_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_BROWN ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_BROWN]);

    // Brown wool
    block = blockList->addBlock(string("brown_wool"));

    desc = block->addBlockDescription(
        "Brown Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        12      // Data value
    );
    desc->addTag(string("color"), string("brown"));

    desc = block->addBlockDescription(
        "Brown Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "brown_wool" // NBT name
    );

    // Brown carpet
    block = blockList->addBlock(string("brown_carpet"));

    desc = block->addBlockDescription(
        "Brown Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        12        // Data value
    );
    desc->addTag(string("color"), string("brown"));

    desc = block->addBlockDescription(
        "Brown Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "brown_carpet" // NBT name
    );

    // Brown concrete
    block = blockList->addBlock(string("brown_concrete"));

    desc = block->addBlockDescription(
        "Brown Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        12          // Data value
    );
    desc->addTag(string("color"), string("brown"));

    desc = block->addBlockDescription(
        "Brown Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "brown_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_GREEN ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_GREEN]);

    // Green wool
    block = blockList->addBlock(string("green_wool"));

    desc = block->addBlockDescription(
        "Green Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        13      // Data value
    );
    desc->addTag(string("color"), string("green"));

    desc = block->addBlockDescription(
        "Green Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "green_wool" // NBT name
    );

    // Green carpet
    block = blockList->addBlock(string("green_carpet"));

    desc = block->addBlockDescription(
        "Green Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        13        // Data value
    );
    desc->addTag(string("color"), string("green"));

    desc = block->addBlockDescription(
        "Green Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "green_carpet" // NBT name
    );

    // Green concrete
    block = blockList->addBlock(string("green_concrete"));

    desc = block->addBlockDescription(
        "Green Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        13          // Data value
    );
    desc->addTag(string("color"), string("green"));

    desc = block->addBlockDescription(
        "Green Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "green_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_RED ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_RED]);

    // Red wool
    block = blockList->addBlock(string("red_wool"));

    desc = block->addBlockDescription(
        "Red Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        14      // Data value
    );
    desc->addTag(string("color"), string("red"));

    desc = block->addBlockDescription(
        "Red Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "red_wool" // NBT name
    );

    // Red carpet
    block = blockList->addBlock(string("red_carpet"));

    desc = block->addBlockDescription(
        "Red Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        14        // Data value
    );
    desc->addTag(string("color"), string("red"));

    desc = block->addBlockDescription(
        "Red Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "red_carpet" // NBT name
    );

    // Red concrete
    block = blockList->addBlock(string("red_concrete"));

    desc = block->addBlockDescription(
        "Red Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        14          // Data value
    );
    desc->addTag(string("color"), string("red"));

    desc = block->addBlockDescription(
        "Red Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "red_concrete" // NBT name
    );

    // ---------------
    // ---- COLOR_BLACK ----
    // ---------------
    blockList = &(list[(size_t)McColors::COLOR_BLACK]);

    // Black wool
    block = blockList->addBlock(string("black_wool"));

    desc = block->addBlockDescription(
        "Black Wool",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wool", // NBT name
        35,     // Block ID
        15      // Data value
    );
    desc->addTag(string("color"), string("black"));

    desc = block->addBlockDescription(
        "Black Wool",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "black_wool" // NBT name
    );

    // Black carpet
    block = blockList->addBlock(string("black_carpet"));

    desc = block->addBlockDescription(
        "Black Carpet",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "carpet", // NBT name
        171,      // Block ID
        15        // Data value
    );
    desc->addTag(string("color"), string("black"));

    desc = block->addBlockDescription(
        "Black Carpet",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "black_carpet" // NBT name
    );

    // Black concrete
    block = blockList->addBlock(string("black_concrete"));

    desc = block->addBlockDescription(
        "Black Concrete",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete", // NBT name
        251,        // Block ID
        15          // Data value
    );
    desc->addTag(string("color"), string("black"));

    desc = block->addBlockDescription(
        "Black Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "black_concrete" // NBT name
    );

    // Coal block
    block = blockList->addBlock(string("coal_block"));

    desc = block->addBlockDescription(
        "Block of Coal",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "coal_block", // NBT name
        173,          // Block ID
        0             // Data value
    );

    // Obsidian
    block = blockList->addBlock(string("obsidian"));

    desc = block->addBlockDescription(
        "Block of Coal",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "obsidian", // NBT name
        49,         // Block ID
        0           // Data value
    );

    // ---------------
    // ---- GOLD ----
    // ---------------
    blockList = &(list[(size_t)McColors::GOLD]);

    // Gold
    block = blockList->addBlock(string("gold_block"));

    desc = block->addBlockDescription(
        "Block of Gold",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "gold_block", // NBT name
        41,           // Block ID
        0             // Data value
    );

    // Gold plate
    block = blockList->addBlock(string("light_weighted_pressure_plate"));

    desc = block->addBlockDescription(
        "Weighted Pressure Plate (Light)",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "light_weighted_pressure_plate", // NBT name
        147,                             // Block ID
        0                                // Data value
    );

    // Raw Gold
    block = blockList->addBlock(string("gold_block"));

    desc = block->addBlockDescription(
        "Block of Raw Gold",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "raw_gold_block" // NBT name
    );

    // ---------------
    // ---- DIAMOND ----
    // ---------------
    blockList = &(list[(size_t)McColors::DIAMOND]);

    // Prismarine Bricks
    block = blockList->addBlock(string("prismarine_bricks"));

    desc = block->addBlockDescription(
        "Prismarine Bricks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "prismarine", // NBT name
        168,          // Block ID
        1             // Data value
    );
    desc->addTag(string("variant"), string("prismarine_bricks"));

    desc = block->addBlockDescription(
        "Prismarine Bricks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "prismarine_bricks" // NBT name
    );

    // Block of Diamond
    block = blockList->addBlock(string("diamond_block"));

    desc = block->addBlockDescription(
        "Block of Diamond",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "diamond_block", // NBT name
        57,              // Block ID
        0                // Data value
    );

    // Beacon
    block = blockList->addBlock(string("beacon"));

    desc = block->addBlockDescription(
        "Beacon",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "beacon", // NBT name
        138,      // Block ID
        0         // Data value
    );

    // ---------------
    // ---- LAPIS ----
    // ---------------
    blockList = &(list[(size_t)McColors::LAPIS]);

    // Lapis Lazuli Block
    block = blockList->addBlock(string("lapis_block"));

    desc = block->addBlockDescription(
        "Lapis Lazuli Block",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "lapis_block", // NBT name
        22,            // Block ID
        0              // Data value
    );

    // ---------------
    // ---- EMERALD ----
    // ---------------
    blockList = &(list[(size_t)McColors::EMERALD]);

    // Block of Emerald
    block = blockList->addBlock(string("emerald_block"));

    desc = block->addBlockDescription(
        "Block of Emerald",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "emerald_block", // NBT name
        133,             // Block ID
        0                // Data value
    );

    // ---------------
    // ---- PODZOL ----
    // ---------------
    blockList = &(list[(size_t)McColors::PODZOL]);

    // Spruce planks
    block = blockList->addBlock(string("spruce_planks"));

    desc = block->addBlockDescription(
        "Spruce Planks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "planks", // NBT name
        5,        // Block ID
        1         // Data value
    );
    desc->addTag(string("variant"), string("spruce"));

    desc = block->addBlockDescription(
        "Spruce Planks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "spruce_planks" // NBT name
    );

    // Podzol
    block = blockList->addBlock(string("podzol"));

    desc = block->addBlockDescription(
        "Podzol",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "dirt", // NBT name
        3,      // Block ID
        2       // Data value
    );
    desc->addTag(string("variant"), string("podzol"));

    desc = block->addBlockDescription(
        "Podzol",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "podzol" // NBT name
    );

    // Spruce log
    block = blockList->addBlock(string("spruce_log"));

    desc = block->addBlockDescription(
        "Spruce Log (vertical)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log", // NBT name
        17,    // Block ID
        1      // Data value
    );
    desc->addTag(string("variant"), string("spruce"));
    desc->addTag(string("axis"), string("y"));

    desc = block->addBlockDescription(
        "Spruce Log (vertical)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "spruce_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // ---------------
    // ---- NETHER ----
    // ---------------
    blockList = &(list[(size_t)McColors::NETHER]);

    // Netherrack
    block = blockList->addBlock(string("netherrack"));

    desc = block->addBlockDescription(
        "Netherrack",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "netherrack", // NBT name
        87,           // Block ID
        0             // Data value
    );

    // ---------------
    // ---- TERRACOTTA_WHITE ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_WHITE]);

    // White Terracotta
    block = blockList->addBlock(string("white_terracotta"));

    desc = block->addBlockDescription(
        "White Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        0                        // Data value
    );
    desc->addTag(string("color"), string("white"));

    desc = block->addBlockDescription(
        "White Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "white_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_ORANGE ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_ORANGE]);

    // Orange Terracotta
    block = blockList->addBlock(string("orange_terracotta"));

    desc = block->addBlockDescription(
        "Orange Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        1                        // Data value
    );
    desc->addTag(string("color"), string("orange"));

    desc = block->addBlockDescription(
        "Orange Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "orange_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_MAGENTA ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_MAGENTA]);

    // Magenta Terracotta
    block = blockList->addBlock(string("magenta_terracotta"));

    desc = block->addBlockDescription(
        "Magenta Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        2                        // Data value
    );
    desc->addTag(string("color"), string("magenta"));

    desc = block->addBlockDescription(
        "Magenta Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "magenta_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_LIGHT_BLUE ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_LIGHT_BLUE]);

    // Light Blue Terracotta
    block = blockList->addBlock(string("light_blue_terracotta"));

    desc = block->addBlockDescription(
        "Light Blue Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        3                        // Data value
    );
    desc->addTag(string("color"), string("light_blue"));

    desc = block->addBlockDescription(
        "Light Blue Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_blue_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_YELLOW ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_YELLOW]);

    // Yellow Terracotta
    block = blockList->addBlock(string("yellow_terracotta"));

    desc = block->addBlockDescription(
        "Yellow Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        4                        // Data value
    );
    desc->addTag(string("color"), string("yellow"));

    desc = block->addBlockDescription(
        "Yellow Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "yellow_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_LIGHT_GREEN ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_LIGHT_GREEN]);

    // Lime Terracotta
    block = blockList->addBlock(string("lime_terracotta"));

    desc = block->addBlockDescription(
        "Lime Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        5                        // Data value
    );
    desc->addTag(string("color"), string("lime"));

    desc = block->addBlockDescription(
        "Lime Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "lime_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_PINK ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_PINK]);

    // Pink Terracotta
    block = blockList->addBlock(string("pink_terracotta"));

    desc = block->addBlockDescription(
        "Pink Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        6                        // Data value
    );
    desc->addTag(string("color"), string("pink"));

    desc = block->addBlockDescription(
        "Pink Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "pink_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_GRAY ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_GRAY]);

    // Gray Terracotta
    block = blockList->addBlock(string("gray_terracotta"));

    desc = block->addBlockDescription(
        "Gray Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        7                        // Data value
    );
    desc->addTag(string("color"), string("gray"));

    desc = block->addBlockDescription(
        "Gray Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "gray_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_LIGHT_GRAY ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_LIGHT_GRAY]);

    // Light Gray Terracotta
    block = blockList->addBlock(string("light_gray_terracotta"));

    desc = block->addBlockDescription(
        "Light Gray Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        8                        // Data value
    );
    desc->addTag(string("color"), string("light_gray"));

    desc = block->addBlockDescription(
        "Light Gray Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_gray_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_CYAN ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_CYAN]);

    // Cyan Terracotta
    block = blockList->addBlock(string("cyan_terracotta"));

    desc = block->addBlockDescription(
        "Cyan Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        9                        // Data value
    );
    desc->addTag(string("color"), string("cyan"));

    desc = block->addBlockDescription(
        "Cyan Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "cyan_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_PURPLE ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_PURPLE]);

    // Purple Terracotta
    block = blockList->addBlock(string("purple_terracotta"));

    desc = block->addBlockDescription(
        "Purple Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        10                       // Data value
    );
    desc->addTag(string("color"), string("purple"));

    desc = block->addBlockDescription(
        "Purple Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "purple_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_BLUE ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_BLUE]);

    // Blue Terracotta
    block = blockList->addBlock(string("blue_terracotta"));

    desc = block->addBlockDescription(
        "Blue Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        11                       // Data value
    );
    desc->addTag(string("color"), string("blue"));

    desc = block->addBlockDescription(
        "Blue Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "blue_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_BROWN ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_BROWN]);

    // Brown Terracotta
    block = blockList->addBlock(string("brown_terracotta"));

    desc = block->addBlockDescription(
        "Brown Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        12                       // Data value
    );
    desc->addTag(string("color"), string("brown"));

    desc = block->addBlockDescription(
        "Brown Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "brown_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_GREEN ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_GREEN]);

    // Green Terracotta
    block = blockList->addBlock(string("green_terracotta"));

    desc = block->addBlockDescription(
        "Green Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        13                       // Data value
    );
    desc->addTag(string("color"), string("green"));

    desc = block->addBlockDescription(
        "Green Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "green_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_RED ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_RED]);

    // Green Terracotta
    block = blockList->addBlock(string("red_terracotta"));

    desc = block->addBlockDescription(
        "Red Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        14                       // Data value
    );
    desc->addTag(string("color"), string("red"));

    desc = block->addBlockDescription(
        "Red Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "red_terracotta" // NBT name
    );

    // ---------------
    // ---- TERRACOTTA_BLACK ----
    // ---------------
    blockList = &(list[(size_t)McColors::TERRACOTTA_BLACK]);

    // Black Terracotta
    block = blockList->addBlock(string("black_terracotta"));

    desc = block->addBlockDescription(
        "Black Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_hardened_clay", // NBT name
        159,                     // Block ID
        15                       // Data value
    );
    desc->addTag(string("color"), string("black"));

    desc = block->addBlockDescription(
        "Black Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "black_terracotta" // NBT name
    );

    if (size > (size_t)McColors::CRIMSON_NYLIUM)
    {
        // 1.16+

        // ---------------
        // ---- CRIMSON_NYLIUM ----
        // ---------------
        blockList = &(list[(size_t)McColors::CRIMSON_NYLIUM]);

        // Crimson Nylium
        block = blockList->addBlock(string("crimson_nylium"));

        desc = block->addBlockDescription(
            "Crimson Nylium",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "crimson_nylium" // NBT name
        );

        // ---------------
        // ---- CRIMSON_STEM ----
        // ---------------
        blockList = &(list[(size_t)McColors::CRIMSON_STEM]);

        // Crimson Planks
        block = blockList->addBlock(string("crimson_planks"));

        desc = block->addBlockDescription(
            "Crimson Planks",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "crimson_planks" // NBT name
        );

        // ---------------
        // ---- CRIMSON_HYPHAE ----
        // ---------------
        blockList = &(list[(size_t)McColors::CRIMSON_HYPHAE]);

        // Crimson Hyphae
        block = blockList->addBlock(string("crimson_hyphae"));

        desc = block->addBlockDescription(
            "Crimson Hyphae",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "crimson_hyphae" // NBT name
        );

        // ---------------
        // ---- WARPED_NYLIUM ----
        // ---------------
        blockList = &(list[(size_t)McColors::WARPED_NYLIUM]);

        // Warped Nylium
        block = blockList->addBlock(string("warped_nylium"));

        desc = block->addBlockDescription(
            "Warped Nylium",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "warped_nylium" // NBT name
        );

        // ---------------
        // ---- WARPED_STEM ----
        // ---------------
        blockList = &(list[(size_t)McColors::WARPED_STEM]);

        // Warped Planks
        block = blockList->addBlock(string("warped_planks"));

        desc = block->addBlockDescription(
            "Warped Planks",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "warped_planks" // NBT name
        );

        // ---------------
        // ---- WARPED_HYPHAE ----
        // ---------------
        blockList = &(list[(size_t)McColors::WARPED_HYPHAE]);

        // Warped Hyphae
        block = blockList->addBlock(string("warped_hyphae"));

        desc = block->addBlockDescription(
            "Warped Hyphae",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "warped_hyphae" // NBT name
        );

        // ---------------
        // ---- WARPED_WART_BLOCK ----
        // ---------------
        blockList = &(list[(size_t)McColors::WARPED_WART_BLOCK]);

        // Warped Wart Block
        block = blockList->addBlock(string("warped_wart_block"));

        desc = block->addBlockDescription(
            "Warped Wart Block",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "warped_wart_block" // NBT name
        );

        if (size > (size_t)McColors::DEEPSLATE)
        {
            // 1.17+

            // ---------------
            // ---- DEEPSLATE ----
            // ---------------
            blockList = &(list[(size_t)McColors::DEEPSLATE]);

            // Deepslate
            block = blockList->addBlock(string("deepslate"));

            desc = block->addBlockDescription(
                "Deepslate",
                McVersion::MC_1_17, MC_LAST_VERSION,
                "deepslate" // NBT name
            );

            // ---------------
            // ---- RAW_IRON ----
            // ---------------
            blockList = &(list[(size_t)McColors::RAW_IRON]);

            // Block of Raw Iron
            block = blockList->addBlock(string("raw_iron_block"));

            desc = block->addBlockDescription(
                "Block of Raw Iron",
                McVersion::MC_1_17, MC_LAST_VERSION,
                "raw_iron_block" // NBT name
            );

            // ---------------
            // ---- GLOW_LICHEN ----
            // ---------------
            blockList = &(list[(size_t)McColors::GLOW_LICHEN]);

            // Glow Lichen
            block = blockList->addBlock(string("glow_lichen"));

            desc = block->addBlockDescription(
                "Glow Lichen",
                McVersion::MC_1_17, MC_LAST_VERSION,
                "glow_lichen" // NBT name
            );
            desc->addTag(string("down"), string("true"));
        }
    }

    return list; // Return the list of blocks
}
