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

    // Sandstone Slab
    block = blockList->addBlock(string("sandstone_slab"));

    desc = block->addBlockDescription(
        "Sandstone Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stone_slab", // NBT name
        44,           // Block ID
        1             // Data value
    );

    desc->addTag(string("variant"), string("sandstone"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Sandstone Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "sandstone_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Birch Log (Vertical)
    block = blockList->addBlock(string("birch_log"));

    desc = block->addBlockDescription(
        "Birch Log (Vertical)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log", // NBT name
        17,    // Block ID
        2      // Data value
    );
    desc->addTag(string("variant"), string("birch"));
    desc->addTag(string("axis"), string("y"));

    desc = block->addBlockDescription(
        "Birch Log (Vertical)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "birch_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // Birch planks
    block = blockList->addBlock(string("birch_planks"));

    desc = block->addBlockDescription(
        "Birch Planks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "planks", // NBT name
        5,        // Block ID
        2         // Data value
    );
    desc->addTag(string("variant"), string("birch"));

    desc = block->addBlockDescription(
        "Birch Planks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "birch_planks" // NBT name
    );

    // Birch slab
    block = blockList->addBlock(string("birch_slab"));

    desc = block->addBlockDescription(
        "Birch Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wooden_slab", // NBT name
        126,           // Block ID
        2              // Data value
    );
    desc->addTag(string("variant"), string("birch"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Birch Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "birch_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Birch pressure plate
    block = blockList->addBlock(string("birch_pressure_plate"));

    desc = block->addBlockDescription(
        "Birch Pressure Plate",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "birch_pressure_plate" // NBT name
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

    // End Stone Bricks
    block = blockList->addBlock(string("end_stone_bricks"));

    desc = block->addBlockDescription(
        "End Stone Bricks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "end_bricks", // NBT name
        206,          // Block ID
        0             // Data value
    );

    desc = block->addBlockDescription(
        "End Stone Bricks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "end_stone_bricks" // NBT name
    );

    // End Stone Brick Slab
    block = blockList->addBlock(string("end_stone_brick_slab"));

    desc = block->addBlockDescription(
        "End Stone Brick Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "end_stone_brick_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Bone Block
    block = blockList->addBlock(string("bone_block"));

    desc = block->addBlockDescription(
        "Bone Block",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "bone_block", // NBT name
        216,          // Block ID
        0             // Data value
    );

    // Ochre Lichen
    block = blockList->addBlock(string("ochre_froglight"));

    desc = block->addBlockDescription(
        "Ochre Froglight",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "ochre_froglight" // NBT name
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

    // Brewing stand
    block = blockList->addBlock(string("brewing_stand"));

    desc = block->addBlockDescription(
        "Brewing Stand",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "brewing_stand", // NBT name
        379,             // Block ID
        0                // Data value
    );

    // Grindstone
    block = blockList->addBlock(string("grindstone"));

    desc = block->addBlockDescription(
        "Grindstone",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "grindstone" // NBT name
    );

    // Pale Oak Leaves
    block = blockList->addBlock(string("pale_oak_leaves"));

    desc = block->addBlockDescription(
        "Pale Oak Leaves",
        McVersion::MC_1_21_4, MC_LAST_VERSION,
        "pale_oak_leaves" // NBT name
    );

    // ---------------
    // ---- PLANT ----
    // ---------------
    blockList = &(list[(size_t)McColors::PLANT]);

    // Oak Leaves
    block = blockList->addBlock(string("oak_leaves"));

    desc = block->addBlockDescription(
        "Oak Leaves",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "leaves", // NBT name
        18,       // Block ID
        0         // Data value
    );
    desc->addTag(string("decayable"), string("false"));
    desc->addTag(string("check_decay"), string("false"));
    desc->addTag(string("variant"), string("oak"));

    desc = block->addBlockDescription(
        "Oak Leaves",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "oak_leaves" // NBT name
    );
    desc->addTag(string("persistent"), string("true"));

    // Spruce Leaves
    block = blockList->addBlock(string("spruce_leaves"));

    desc = block->addBlockDescription(
        "Spruce Leaves",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "leaves", // NBT name
        18,       // Block ID
        1         // Data value
    );
    desc->addTag(string("decayable"), string("false"));
    desc->addTag(string("check_decay"), string("false"));
    desc->addTag(string("variant"), string("spruce"));

    desc = block->addBlockDescription(
        "Spruce Leaves",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "spruce_leaves" // NBT name
    );
    desc->addTag(string("persistent"), string("true"));

    // Birch Leaves
    block = blockList->addBlock(string("birch_leaves"));

    desc = block->addBlockDescription(
        "Birch Leaves",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "leaves", // NBT name
        18,       // Block ID
        2         // Data value
    );
    desc->addTag(string("decayable"), string("false"));
    desc->addTag(string("check_decay"), string("false"));
    desc->addTag(string("variant"), string("birch"));

    desc = block->addBlockDescription(
        "Birch Leaves",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "birch_leaves" // NBT name
    );
    desc->addTag(string("persistent"), string("true"));

    // Jungle Leaves
    block = blockList->addBlock(string("jungle_leaves"));

    desc = block->addBlockDescription(
        "Jungle Leaves",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "leaves", // NBT name
        18,       // Block ID
        3         // Data value
    );
    desc->addTag(string("decayable"), string("false"));
    desc->addTag(string("check_decay"), string("false"));
    desc->addTag(string("variant"), string("jungle"));

    desc = block->addBlockDescription(
        "Jungle Leaves",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "jungle_leaves" // NBT name
    );
    desc->addTag(string("persistent"), string("true"));

    // Acacia Leaves
    block = blockList->addBlock(string("acacia_leaves"));

    desc = block->addBlockDescription(
        "Acacia Leaves",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "leaves2", // NBT name
        161,       // Block ID
        0          // Data value
    );
    desc->addTag(string("decayable"), string("false"));
    desc->addTag(string("check_decay"), string("false"));
    desc->addTag(string("variant"), string("acacia"));

    desc = block->addBlockDescription(
        "Acacia Leaves",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "acacia_leaves" // NBT name
    );
    desc->addTag(string("persistent"), string("true"));

    // Dark Oak Leaves
    block = blockList->addBlock(string("dark_oak_leaves"));

    desc = block->addBlockDescription(
        "Dark Oak Leaves",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "leaves2", // NBT name
        161,       // Block ID
        1          // Data value
    );
    desc->addTag(string("decayable"), string("false"));
    desc->addTag(string("check_decay"), string("false"));
    desc->addTag(string("variant"), string("dark_oak"));

    desc = block->addBlockDescription(
        "Dark Oak Leaves",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dark_oak_leaves" // NBT name
    );
    desc->addTag(string("persistent"), string("true"));

    // Azalea Leaves
    block = blockList->addBlock(string("azalea_leaves"));

    desc = block->addBlockDescription(
        "Azalea Leaves",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "azalea_leaves" // NBT name
    );
    desc->addTag(string("persistent"), string("true"));

    // Mangrove Leaves
    block = blockList->addBlock(string("mangrove_leaves"));

    desc = block->addBlockDescription(
        "Mangrove Leaves",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "mangrove_leaves" // NBT name
    );
    desc->addTag(string("persistent"), string("true"));

    // Block of Bamboo (Horizontal)
    block = blockList->addBlock(string("bamboo_block_horizontal"));

    desc = block->addBlockDescription(
        "Block of Bamboo (Horizontal)",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "bamboo_block" // NBT name
    );
    desc->addTag(string("axis"), string("x"));

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

    // Snow layer
    block = blockList->addBlock(string("snow_layer"));

    desc = block->addBlockDescription(
        "Snow Layer",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "snow" // NBT name
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

    // White concrete powder
    block = blockList->addBlock(string("white_concrete_powder"));

    desc = block->addBlockDescription(
        "White Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        0                  // Data value
    );
    desc->addTag(string("color"), string("white"));

    desc = block->addBlockDescription(
        "White Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "white_concrete_powder" // NBT name
    );

    // White Glazed Terracota
    block = blockList->addBlock(string("white_glazed_terracota"));

    desc = block->addBlockDescription(
        "White Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "white_glazed_terracota", // NBT name
        235,                      // Block ID
        0                         // Data value
    );

    // White Stained Glass
    block = blockList->addBlock(string("white_stained_glass"));

    desc = block->addBlockDescription(
        "White Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        0                // Data value
    );
    desc->addTag(string("color"), string("white"));

    desc = block->addBlockDescription(
        "White Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "white_stained_glass" // NBT name
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

    // Granite Slab
    block = blockList->addBlock(string("granite_slab"));

    desc = block->addBlockDescription(
        "Granite Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "granite_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

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

    // Jungle slab
    block = blockList->addBlock(string("jungle_slab"));

    desc = block->addBlockDescription(
        "Jungle Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wooden_slab", // NBT name
        126,           // Block ID
        3              // Data value
    );
    desc->addTag(string("variant"), string("jungle"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Jungle Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "jungle_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Jungle pressure plate
    block = blockList->addBlock(string("jungle_pressure_plate"));

    desc = block->addBlockDescription(
        "Jungle Pressure Plate",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "jungle_pressure_plate" // NBT name
    );

    // Jungle Log (Vertical)
    block = blockList->addBlock(string("jungle_log"));

    desc = block->addBlockDescription(
        "Jungle Log (Vertical)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log", // NBT name
        17,    // Block ID
        3      // Data value
    );
    desc->addTag(string("variant"), string("jungle"));
    desc->addTag(string("axis"), string("y"));

    desc = block->addBlockDescription(
        "Jungle Log (Vertical)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "jungle_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // Dirt
    block = blockList->addBlock(string("dirt"));

    desc = block->addBlockDescription(
        "Dirt",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "dirt", // NBT name
        3,      // Block ID
        0       // Data value
    );

    // Coarse Dirt
    block = blockList->addBlock(string("coarse_dirt"));

    desc = block->addBlockDescription(
        "Coarse Dirt",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "dirt", // NBT name
        3,      // Block ID
        1       // Data value
    );

    desc = block->addBlockDescription(
        "Coarse Dirt",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "coarse_dirt" // NBT name
    );

    // Rooted Dirt
    block = blockList->addBlock(string("rooted_dirt"));

    desc = block->addBlockDescription(
        "Rooted Dirt",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "rooted_dirt" // NBT name
    );

    // Packed mud
    block = blockList->addBlock(string("packed_mud"));

    desc = block->addBlockDescription(
        "Packed Dirt",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "packed_mud" // NBT name
    );

    // Brown Mushroom Block
    block = blockList->addBlock(string("brown_mushroom_block"));

    desc = block->addBlockDescription(
        "Brown Mushroom Block",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "brown_mushroom_block" // NBT name
    );

    // Jukebox
    block = blockList->addBlock(string("jukebox"));

    desc = block->addBlockDescription(
        "Jukebox",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "jukebox", // NBT name
        84,        // Block ID
        0          // Data value
    );

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

    // Cobblestone Slab
    block = blockList->addBlock(string("cobblestone_slab"));

    desc = block->addBlockDescription(
        "Cobblestone Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stone_slab", // NBT name
        44,           // Block ID
        3             // Data value
    );

    desc->addTag(string("variant"), string("cobblestone"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Cobblestone Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "cobblestone_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Mossy Cobblestone
    block = blockList->addBlock(string("mossy_cobblestone"));

    desc = block->addBlockDescription(
        "Mossy Cobblestone",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "mossy_cobblestone", // NBT name
        48,                  // Block ID
        0                    // Data value
    );

    desc = block->addBlockDescription(
        "Mossy Cobblestone",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "mossy_cobblestone" // NBT name
    );

    // Mossy Cobblestone Slab
    block = blockList->addBlock(string("mossy_cobblestone_slab"));

    desc = block->addBlockDescription(
        "Mossy Cobblestone Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "mossy_cobblestone_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Stone
    block = blockList->addBlock(string("stone"));

    desc = block->addBlockDescription(
        "Stone",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "stone", // NBT name
        1,       // Block ID
        0        // Data value
    );

    // Stone Slab
    block = blockList->addBlock(string("stone_slab"));

    desc = block->addBlockDescription(
        "Stone Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "stone_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Stone Pressure Plate
    block = blockList->addBlock(string("stone_pressure_plate"));

    desc = block->addBlockDescription(
        "Stone Pressure Plate",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "stone_pressure_plate", // NBT name
        70,                     // Block ID
        0                       // Data value
    );

    // Smooth Stone
    block = blockList->addBlock(string("smooth_stone"));

    desc = block->addBlockDescription(
        "Smooth Stone",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "smooth_stone" // NBT name
    );

    // Smooth Stone Slab
    block = blockList->addBlock(string("smooth_stone_slab"));

    desc = block->addBlockDescription(
        "Smooth Stone Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "smooth_stone_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Stone Bricks
    block = blockList->addBlock(string("stone_bricks"));

    desc = block->addBlockDescription(
        "Stone Bricks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stonebrick", // NBT name
        98,           // Block ID
        0             // Data value
    );

    desc = block->addBlockDescription(
        "Stone Bricks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "stone_bricks" // NBT name
    );

    // Stone Brick Slab
    block = blockList->addBlock(string("stone_brick_slab"));

    desc = block->addBlockDescription(
        "Stone Brick Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stone_slab", // NBT name
        44,           // Block ID
        5             // Data value
    );

    desc->addTag(string("variant"), string("stone_brick"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Stone Brick Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "stone_brick_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

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

    // Andesite Slab
    block = blockList->addBlock(string("andesite_slab"));

    desc = block->addBlockDescription(
        "Andesite Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "andesite_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Gravel
    block = blockList->addBlock(string("gravel"));

    desc = block->addBlockDescription(
        "Gravel",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "gravel", // NBT name
        13,       // Block ID
        0         // Data value
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

    // Oak slab
    block = blockList->addBlock(string("oak_slab"));

    desc = block->addBlockDescription(
        "Oak Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wooden_slab", // NBT name
        126,           // Block ID
        0              // Data value
    );
    desc->addTag(string("variant"), string("oak"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Oak Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "oak_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Oak pressure plate
    block = blockList->addBlock(string("oak_pressure_plate"));

    desc = block->addBlockDescription(
        "Oak Pressure Plate",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "oak_pressure_plate" // NBT name
    );

    // Oak Log (Vertical)
    block = blockList->addBlock(string("oak_log"));

    desc = block->addBlockDescription(
        "Oak Log (Vertical)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log", // NBT name
        17,    // Block ID
        0      // Data value
    );
    desc->addTag(string("variant"), string("oak"));
    desc->addTag(string("axis"), string("y"));

    desc = block->addBlockDescription(
        "Oak Log (Vertical)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "oak_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // Crafting Table
    block = blockList->addBlock(string("crafting_table"));

    desc = block->addBlockDescription(
        "Crafting Table",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "crafting_table", // NBT name
        58,               // Block ID
        0                 // Data value
    );

    // Bookshelf
    block = blockList->addBlock(string("bookshelf"));

    desc = block->addBlockDescription(
        "Bookshelf",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "bookshelf", // NBT name
        47,          // Block ID
        0            // Data value
    );

    // ----------------
    // ---- QUARTZ ----
    // ----------------
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

    // Diorite Slab
    block = blockList->addBlock(string("diorite_slab"));

    desc = block->addBlockDescription(
        "Diorite Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "diorite_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Quartz block
    block = blockList->addBlock(string("quartz_block"));

    desc = block->addBlockDescription(
        "Quartz Block",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "quartz_block", // NBT name
        155,            // Block ID
        0               // Data value
    );

    // Quartz Slab
    block = blockList->addBlock(string("quartz_slab"));

    desc = block->addBlockDescription(
        "Quartz Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stone_slab", // NBT name
        44,           // Block ID
        7             // Data value
    );

    desc->addTag(string("variant"), string("quartz"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Quartz Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "quartz_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Sea lantern
    block = blockList->addBlock(string("sea_lantern"));

    desc = block->addBlockDescription(
        "Sea Lantern",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "sea_lantern", // NBT name
        169,           // Block ID
        0              // Data value
    );

    // Pale Oak Log (Vertical)
    block = blockList->addBlock(string("pale_oak_log"));

    desc = block->addBlockDescription(
        "Pale Oak Log (Vertical)",
        McVersion::MC_1_21_4, MC_LAST_VERSION,
        "pale_oak_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // Pale Oak Planks
    block = blockList->addBlock(string("pale_oak_planks"));

    desc = block->addBlockDescription(
        "Pale Oak Planks",
        McVersion::MC_1_21_4, MC_LAST_VERSION,
        "pale_oak_planks" // NBT name
    );

    // Pale Oak Slab
    block = blockList->addBlock(string("pale_oak_slab"));

    desc = block->addBlockDescription(
        "Pale Oak Slab",
        McVersion::MC_1_21_4, MC_LAST_VERSION,
        "pale_oak_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Pale Oak Pressure Plate
    block = blockList->addBlock(string("pale_oak_pressure_plate"));

    desc = block->addBlockDescription(
        "Pale Oak Pressure Plate",
        McVersion::MC_1_21_4, MC_LAST_VERSION,
        "pale_oak_pressure_plate" // NBT name
    );

    // Target Block
    block = blockList->addBlock(string("target"));

    desc = block->addBlockDescription(
        "Target Block",
        McVersion::MC_1_16, MC_LAST_VERSION,
        "target" // NBT name
    );

    // ----------------------
    // ---- COLOR_ORANGE ----
    // ----------------------
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

    // Orange concrete powder
    block = blockList->addBlock(string("orange_concrete_powder"));

    desc = block->addBlockDescription(
        "Orange Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        1                  // Data value
    );
    desc->addTag(string("color"), string("orange"));

    desc = block->addBlockDescription(
        "Orange Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "orange_concrete_powder" // NBT name
    );

    // Orange Glazed Terracota
    block = blockList->addBlock(string("orange_glazed_terracota"));

    desc = block->addBlockDescription(
        "Orange Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "orange_glazed_terracota", // NBT name
        236,                       // Block ID
        0                          // Data value
    );

    // Orange Stained Glass
    block = blockList->addBlock(string("orange_stained_glass"));

    desc = block->addBlockDescription(
        "Orange Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        1                // Data value
    );
    desc->addTag(string("color"), string("orange"));

    desc = block->addBlockDescription(
        "Orange Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "orange_stained_glass" // NBT name
    );

    // Pumpkin
    block = blockList->addBlock(string("pumpkin"));

    desc = block->addBlockDescription(
        "Pumpkin",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "pumpkin", // NBT name
        86,        // Block ID
        0          // Data value
    );

    // Acacia Log (Vertical)
    block = blockList->addBlock(string("acacia_log"));

    desc = block->addBlockDescription(
        "Acacia Log (Vertical)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log2", // NBT name
        162,    // Block ID
        0       // Data value
    );
    desc->addTag(string("variant"), string("acacia"));
    desc->addTag(string("axis"), string("y"));

    desc = block->addBlockDescription(
        "Acacia Log (Vertical)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "acacia_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // Acacia planks
    block = blockList->addBlock(string("acacia_planks"));

    desc = block->addBlockDescription(
        "Acacia Planks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "planks", // NBT name
        5,        // Block ID
        4         // Data value
    );
    desc->addTag(string("variant"), string("acacia"));

    desc = block->addBlockDescription(
        "Acacia Planks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "acacia_planks" // NBT name
    );

    // Acacia slab
    block = blockList->addBlock(string("acacia_slab"));

    desc = block->addBlockDescription(
        "Acacia Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wooden_slab", // NBT name
        126,           // Block ID
        4              // Data value
    );
    desc->addTag(string("variant"), string("acacia"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Acacia Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "acacia_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Acacia pressure plate
    block = blockList->addBlock(string("acacia_pressure_plate"));

    desc = block->addBlockDescription(
        "Acacia Pressure Plate",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "acacia_pressure_plate" // NBT name
    );

    // Red Sand
    block = blockList->addBlock(string("red_sand"));

    desc = block->addBlockDescription(
        "Red Sand",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "sand", // NBT name
        12,     // Block ID
        1       // Data value
    );
    desc->addTag(string("variant"), string("red_sand"));

    desc = block->addBlockDescription(
        "Red Sand",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "red_sand" // NBT name
    );

    // Red Sandstone
    block = blockList->addBlock(string("red_sandstone"));

    desc = block->addBlockDescription(
        "Red Sandstone",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "red_sandstone", // NBT name
        179,             // Block ID
        0                // Data value
    );

    // Red Sandstone Slab
    block = blockList->addBlock(string("red_sandstone_slab"));

    desc = block->addBlockDescription(
        "Red Sandstone Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stone_slab2", // NBT name
        182,           // Block ID
        0              // Data value
    );

    desc->addTag(string("variant"), string("red_sandstone"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Red Sandstone Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "red_sandstone_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Terracotta
    block = blockList->addBlock(string("terracotta"));

    desc = block->addBlockDescription(
        "Terracotta",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "hardened_clay", // NBT name
        172,             // Block ID
        0                // Data value
    );

    desc = block->addBlockDescription(
        "Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "terracotta" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Honey Block
    block = blockList->addBlock(string("honey_block"));

    desc = block->addBlockDescription(
        "Honey Block",
        McVersion::MC_1_15, MC_LAST_VERSION,
        "honey_block" // NBT name
    );

    // Honeycomb Block
    block = blockList->addBlock(string("honeycomb_block"));

    desc = block->addBlockDescription(
        "Honeycomb Block",
        McVersion::MC_1_15, MC_LAST_VERSION,
        "honeycomb_block" // NBT name
    );

    // Block of Raw Copper
    block = blockList->addBlock(string("raw_copper_block"));

    desc = block->addBlockDescription(
        "Block of Raw Copper",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "raw_copper_block" // NBT name
    );

    // Waxed Copper Block
    block = blockList->addBlock(string("waxed_copper_block"));

    desc = block->addBlockDescription(
        "Waxed Copper Block",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "waxed_copper_block" // NBT name
    );

    // Waxed Cut Copper Slab
    block = blockList->addBlock(string("waxed_cut_copper_slab"));

    desc = block->addBlockDescription(
        "Waxed Cut Copper Slab",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "waxed_cut_copper_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // -----------------------
    // ---- COLOR_MAGENTA ----
    // -----------------------
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

    // Magenta concrete powder
    block = blockList->addBlock(string("magenta_concrete_powder"));

    desc = block->addBlockDescription(
        "Magenta Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        2                  // Data value
    );
    desc->addTag(string("color"), string("magenta"));

    desc = block->addBlockDescription(
        "Magenta Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "magenta_concrete_powder" // NBT name
    );

    // Magenta Glazed Terracota
    block = blockList->addBlock(string("magenta_glazed_terracota"));

    desc = block->addBlockDescription(
        "Magenta Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "magenta_glazed_terracota", // NBT name
        237,                        // Block ID
        0                           // Data value
    );

    // Magenta Stained Glass
    block = blockList->addBlock(string("magenta_stained_glass"));

    desc = block->addBlockDescription(
        "Magenta Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        2                // Data value
    );
    desc->addTag(string("color"), string("magenta"));

    desc = block->addBlockDescription(
        "Magenta Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "magenta_stained_glass" // NBT name
    );

    // Purpur Block
    block = blockList->addBlock(string("purpur_block"));

    desc = block->addBlockDescription(
        "Purpur Block",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "purpur_block", // NBT name
        201,            // Block ID
        0               // Data value
    );

    // Purpur Slab
    block = blockList->addBlock(string("purpur_slab"));

    desc = block->addBlockDescription(
        "Purpur Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "purpur_slab", // NBT name
        205,           // Block ID
        0              // Data value
    );

    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Purpur Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "purpur_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // --------------------------
    // ---- COLOR_LIGHT_BLUE ----
    // --------------------------
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

    // Light Blue concrete powder
    block = blockList->addBlock(string("light_blue_concrete_powder"));

    desc = block->addBlockDescription(
        "Light Blue Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        3                  // Data value
    );
    desc->addTag(string("color"), string("light_blue"));

    desc = block->addBlockDescription(
        "Light Blue Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_blue_concrete_powder" // NBT name
    );

    // Light Blue Glazed Terracota
    block = blockList->addBlock(string("light_blue_glazed_terracota"));

    desc = block->addBlockDescription(
        "Light Blue Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "light_blue_glazed_terracota", // NBT name
        238,                           // Block ID
        0                              // Data value
    );

    // Light Blue Stained Glass
    block = blockList->addBlock(string("light_blue_stained_glass"));

    desc = block->addBlockDescription(
        "Light Blue Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        3                // Data value
    );
    desc->addTag(string("color"), string("light_blue"));

    desc = block->addBlockDescription(
        "Light Blue Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_blue_stained_glass" // NBT name
    );

    // ----------------------
    // ---- COLOR_YELLOW ----
    // ----------------------
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

    // Yellow concrete powder
    block = blockList->addBlock(string("yellow_concrete_powder"));

    desc = block->addBlockDescription(
        "Yellow Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        4                  // Data value
    );
    desc->addTag(string("color"), string("yellow"));

    desc = block->addBlockDescription(
        "Yellow Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "yellow_concrete_powder" // NBT name
    );

    // Yellow Glazed Terracota
    block = blockList->addBlock(string("yellow_glazed_terracota"));

    desc = block->addBlockDescription(
        "Yellow Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "yellow_glazed_terracota", // NBT name
        239,                       // Block ID
        0                          // Data value
    );

    // Yellow Stained Glass
    block = blockList->addBlock(string("yellow_stained_glass"));

    desc = block->addBlockDescription(
        "Yellow Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        4                // Data value
    );
    desc->addTag(string("color"), string("yellow"));

    desc = block->addBlockDescription(
        "Yellow Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "yellow_stained_glass" // NBT name
    );

    // Hay Bale
    block = blockList->addBlock(string("hay_block"));

    desc = block->addBlockDescription(
        "Hay Bale",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "hay_block", // NBT name
        170,         // Block ID
        0            // Data value
    );

    // Sponge
    block = blockList->addBlock(string("sponge"));

    desc = block->addBlockDescription(
        "Sponge",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "sponge", // NBT name
        19,       // Block ID
        0         // Data value
    );

    // Block of Bamboo (Vertical)
    block = blockList->addBlock(string("bamboo_block"));

    desc = block->addBlockDescription(
        "Block of Bamboo (Vertical)",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "bamboo_block" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // Bamboo Planks
    block = blockList->addBlock(string("bamboo_planks"));

    desc = block->addBlockDescription(
        "Bamboo Planks",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "bamboo_planks" // NBT name
    );

    // Bamboo Slab
    block = blockList->addBlock(string("bamboo_slab"));

    desc = block->addBlockDescription(
        "Bamboo Slab",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "bamboo_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Bamboo Pressure Plate
    block = blockList->addBlock(string("bamboo_pressure_plate"));

    desc = block->addBlockDescription(
        "Bamboo Pressure Plate",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "bamboo_pressure_plate" // NBT name
    );

    // ---------------------------
    // ---- COLOR_LIGHT_GREEN ----
    // ---------------------------
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

    // Lime concrete powder
    block = blockList->addBlock(string("lime_concrete_powder"));

    desc = block->addBlockDescription(
        "Lime Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        5                  // Data value
    );
    desc->addTag(string("color"), string("lime"));

    desc = block->addBlockDescription(
        "Lime Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "lime_concrete_powder" // NBT name
    );

    // Lime Glazed Terracota
    block = blockList->addBlock(string("lime_glazed_terracota"));

    desc = block->addBlockDescription(
        "Lime Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "lime_glazed_terracota", // NBT name
        240,                     // Block ID
        0                        // Data value
    );

    // Lime Stained Glass
    block = blockList->addBlock(string("lime_stained_glass"));

    desc = block->addBlockDescription(
        "Lime Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        5                // Data value
    );
    desc->addTag(string("color"), string("lime"));

    desc = block->addBlockDescription(
        "Lime Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "lime_stained_glass" // NBT name
    );

    // Melon
    block = blockList->addBlock(string("melon"));

    desc = block->addBlockDescription(
        "Melon",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "melon_block", // NBT name
        103,           // Block ID
        0              // Data value
    );

    desc = block->addBlockDescription(
        "Melon",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "melon" // NBT name
    );

    // --------------------
    // ---- COLOR_PINK ----
    // --------------------
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

    // Pink concrete powder
    block = blockList->addBlock(string("pink_concrete_powder"));

    desc = block->addBlockDescription(
        "Pink Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        6                  // Data value
    );
    desc->addTag(string("color"), string("pink"));

    desc = block->addBlockDescription(
        "Pink Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "pink_concrete_powder" // NBT name
    );

    // Pink Glazed Terracota
    block = blockList->addBlock(string("pink_glazed_terracota"));

    desc = block->addBlockDescription(
        "Pink Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "pink_glazed_terracota", // NBT name
        241,                     // Block ID
        0                        // Data value
    );

    // Pink Stained Glass
    block = blockList->addBlock(string("pink_stained_glass"));

    desc = block->addBlockDescription(
        "Pink Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        6                // Data value
    );
    desc->addTag(string("color"), string("pink"));

    desc = block->addBlockDescription(
        "Pink Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "pink_stained_glass" // NBT name
    );

    // Pearlescent Froglight
    block = blockList->addBlock(string("pearlescent_froglight"));

    desc = block->addBlockDescription(
        "Pearlescent Froglight",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "pearlescent_froglight" // NBT name
    );

    // Cherry Leaves
    block = blockList->addBlock(string("cherry_leaves"));

    desc = block->addBlockDescription(
        "Cherry Leaves",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "cherry_leaves" // NBT name
    );

    // --------------------
    // ---- COLOR_GRAY ----
    // --------------------
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

    // Gray concrete powder
    block = blockList->addBlock(string("gray_concrete_powder"));

    desc = block->addBlockDescription(
        "Gray Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        7                  // Data value
    );
    desc->addTag(string("color"), string("gray"));

    desc = block->addBlockDescription(
        "Gray Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "gray_concrete_powder" // NBT name
    );

    // Gray Glazed Terracota
    block = blockList->addBlock(string("gray_glazed_terracota"));

    desc = block->addBlockDescription(
        "Gray Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "gray_glazed_terracota", // NBT name
        242,                     // Block ID
        0                        // Data value
    );

    // Gray Stained Glass
    block = blockList->addBlock(string("gray_stained_glass"));

    desc = block->addBlockDescription(
        "Gray Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        7                // Data value
    );
    desc->addTag(string("color"), string("gray"));

    desc = block->addBlockDescription(
        "Gray Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "gray_stained_glass" // NBT name
    );

    // Dead Tube Coral Block
    block = blockList->addBlock(string("dead_tube_coral_block"));

    desc = block->addBlockDescription(
        "Dead Tube Coral Block",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dead_tube_coral_block" // NBT name
    );

    // Dead Brain Coral Block
    block = blockList->addBlock(string("dead_brain_coral_block"));

    desc = block->addBlockDescription(
        "Dead Brain Coral Block",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dead_brain_coral_block" // NBT name
    );

    // Dead Bubble Coral Block
    block = blockList->addBlock(string("dead_bubble_coral_block"));

    desc = block->addBlockDescription(
        "Dead Bubble Coral Block",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dead_bubble_coral_block" // NBT name
    );

    // Dead Fire Coral Block
    block = blockList->addBlock(string("dead_fire_coral_block"));

    desc = block->addBlockDescription(
        "Dead Fire Coral Block",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dead_fire_coral_block" // NBT name
    );

    // Dead Horn Coral Block
    block = blockList->addBlock(string("dead_horn_coral_block"));

    desc = block->addBlockDescription(
        "Dead Horn Coral Block",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dead_horn_coral_block" // NBT name
    );

    // Tinted Glass
    block = blockList->addBlock(string("tinted_glass"));

    desc = block->addBlockDescription(
        "Tinted Glass",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "tinted_glass" // NBT name
    );

    // Acacia Wood
    block = blockList->addBlock(string("acacia_wood"));

    desc = block->addBlockDescription(
        "Acacia Wood",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "acacia_wood" // NBT name
    );

    // --------------------------
    // ---- COLOR_LIGHT_GRAY ----
    // --------------------------
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
    desc->addTag(string("color"), string("silver"));

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
    desc->addTag(string("color"), string("silver"));

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
    desc->addTag(string("color"), string("silver"));

    desc = block->addBlockDescription(
        "Light Gray Concrete",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_gray_concrete" // NBT name
    );

    // Light Gray concrete powder
    block = blockList->addBlock(string("light_gray_concrete_powder"));

    desc = block->addBlockDescription(
        "Light Gray Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        8                  // Data value
    );
    desc->addTag(string("color"), string("silver"));

    desc = block->addBlockDescription(
        "Light Gray Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_gray_concrete_powder" // NBT name
    );

    // Light Gray Glazed Terracota
    block = blockList->addBlock(string("light_gray_glazed_terracota"));

    desc = block->addBlockDescription(
        "Light Gray Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "light_gray_glazed_terracota", // NBT name
        243,                           // Block ID
        0                              // Data value
    );

    // Light Gray Stained Glass
    block = blockList->addBlock(string("light_gray_stained_glass"));

    desc = block->addBlockDescription(
        "Light Gray Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        8                // Data value
    );
    desc->addTag(string("color"), string("silver"));

    desc = block->addBlockDescription(
        "Light Gray Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_gray_stained_glass" // NBT name
    );

    // Pale Moss Block
    block = blockList->addBlock(string("pale_moss_block"));

    desc = block->addBlockDescription(
        "Pale Moss Block",
        McVersion::MC_1_21_4, MC_LAST_VERSION,
        "pale_moss_block" // NBT name
    );

    // Pale Moss Carpet
    block = blockList->addBlock(string("pale_moss_carpet"));

    desc = block->addBlockDescription(
        "Pale Moss Carpet",
        McVersion::MC_1_21_4, MC_LAST_VERSION,
        "pale_moss_carpet" // NBT name
    );

    // --------------------
    // ---- COLOR_CYAN ----
    // --------------------
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

    // Cyan concrete powder
    block = blockList->addBlock(string("cyan_concrete_powder"));

    desc = block->addBlockDescription(
        "Cyan Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        9                  // Data value
    );
    desc->addTag(string("color"), string("cyan"));

    desc = block->addBlockDescription(
        "Cyan Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "cyan_concrete_powder" // NBT name
    );

    // Cyan Glazed Terracota
    block = blockList->addBlock(string("cyan_glazed_terracota"));

    desc = block->addBlockDescription(
        "Cyan Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "cyan_glazed_terracota", // NBT name
        244,                     // Block ID
        0                        // Data value
    );

    // Cyan Stained Glass
    block = blockList->addBlock(string("cyan_stained_glass"));

    desc = block->addBlockDescription(
        "Cyan Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        9                // Data value
    );
    desc->addTag(string("color"), string("cyan"));

    desc = block->addBlockDescription(
        "Cyan Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "cyan_stained_glass" // NBT name
    );

    // Prismarine
    block = blockList->addBlock(string("prismarine"));

    desc = block->addBlockDescription(
        "Prismarine",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "prismarine", // NBT name
        168,          // Block ID
        0             // Data value
    );

    // Prismarine Slab
    block = blockList->addBlock(string("prismarine_slab"));

    desc = block->addBlockDescription(
        "Prismarine Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "prismarine_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // ----------------------
    // ---- COLOR_PURPLE ----
    // ----------------------
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

    // Purple concrete powder
    block = blockList->addBlock(string("purple_concrete_powder"));

    desc = block->addBlockDescription(
        "Purple Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        10                 // Data value
    );
    desc->addTag(string("color"), string("purple"));

    desc = block->addBlockDescription(
        "Purple Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "purple_concrete_powder" // NBT name
    );

    // Purple Glazed Terracota
    block = blockList->addBlock(string("purple_glazed_terracota"));

    desc = block->addBlockDescription(
        "Purple Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "purple_glazed_terracota", // NBT name
        245,                       // Block ID
        0                          // Data value
    );

    // Purple Stained Glass
    block = blockList->addBlock(string("purple_stained_glass"));

    desc = block->addBlockDescription(
        "Purple Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        10               // Data value
    );
    desc->addTag(string("color"), string("purple"));

    desc = block->addBlockDescription(
        "Purple Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "purple_stained_glass" // NBT name
    );

    // Mycelium
    block = blockList->addBlock(string("mycelium"));

    desc = block->addBlockDescription(
        "Mycelium",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "mycelium", // NBT name
        110,        // Block ID
        0           // Data value
    );

    // Amethyst Block
    block = blockList->addBlock(string("amethyst_block"));

    desc = block->addBlockDescription(
        "Amethyst Block",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "amethyst_block" // NBT name
    );

    // --------------------
    // ---- COLOR_BLUE ----
    // --------------------
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

    // Blue concrete powder
    block = blockList->addBlock(string("blue_concrete_powder"));

    desc = block->addBlockDescription(
        "Blue Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        11                 // Data value
    );
    desc->addTag(string("color"), string("blue"));

    desc = block->addBlockDescription(
        "Blue Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "blue_concrete_powder" // NBT name
    );

    // Blue Glazed Terracota
    block = blockList->addBlock(string("blue_glazed_terracota"));

    desc = block->addBlockDescription(
        "Blue Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "blue_glazed_terracota", // NBT name
        246,                     // Block ID
        0                        // Data value
    );

    // Blue Stained Glass
    block = blockList->addBlock(string("blue_stained_glass"));

    desc = block->addBlockDescription(
        "Blue Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        11               // Data value
    );
    desc->addTag(string("color"), string("blue"));

    desc = block->addBlockDescription(
        "Blue Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "blue_stained_glass" // NBT name
    );

    // ---------------------
    // ---- COLOR_BROWN ----
    // ---------------------
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

    // Brown concrete powder
    block = blockList->addBlock(string("brown_concrete_powder"));

    desc = block->addBlockDescription(
        "Brown Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        12                 // Data value
    );
    desc->addTag(string("color"), string("brown"));

    desc = block->addBlockDescription(
        "Brown Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "brown_concrete_powder" // NBT name
    );

    // Brown Glazed Terracota
    block = blockList->addBlock(string("brown_glazed_terracota"));

    desc = block->addBlockDescription(
        "Brown Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "brown_glazed_terracota", // NBT name
        247,                      // Block ID
        0                         // Data value
    );

    // Brown Stained Glass
    block = blockList->addBlock(string("brown_stained_glass"));

    desc = block->addBlockDescription(
        "Brown Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        12               // Data value
    );
    desc->addTag(string("color"), string("brown"));

    desc = block->addBlockDescription(
        "Brown Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "brown_stained_glass" // NBT name
    );

    // Dark Oak planks
    block = blockList->addBlock(string("dark_oak_planks"));

    desc = block->addBlockDescription(
        "Dark Oak Planks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "planks", // NBT name
        5,        // Block ID
        5         // Data value
    );
    desc->addTag(string("variant"), string("dark_oak"));

    desc = block->addBlockDescription(
        "Dark Oak Planks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dark_oak_planks" // NBT name
    );

    // Dark Oak slab
    block = blockList->addBlock(string("dark_oak_slab"));

    desc = block->addBlockDescription(
        "Dark Oak Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wooden_slab", // NBT name
        126,           // Block ID
        5              // Data value
    );
    desc->addTag(string("variant"), string("dark_oak"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Dark Oak Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dark_oak_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Dark Oak pressure plate
    block = blockList->addBlock(string("dark_oak_pressure_plate"));

    desc = block->addBlockDescription(
        "Dark Oak Pressure Plate",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dark_oak_pressure_plate" // NBT name
    );

    // Dark Oak Log
    block = blockList->addBlock(string("dark_oak_log"));

    desc = block->addBlockDescription(
        "Dark Oak Log",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log2", // NBT name
        162,    // Block ID
        1       // Data value
    );
    desc->addTag(string("variant"), string("dark_oak"));
    desc->addTag(string("axis"), string("y"));

    desc = block->addBlockDescription(
        "Dark Oak Log",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dark_oak_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // Spruce Log (Horizontal)
    block = blockList->addBlock(string("spruce_log_horizontal"));

    desc = block->addBlockDescription(
        "Spruce Log (Horizontal)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log", // NBT name
        17,    // Block ID
        1      // Data value
    );
    desc->addTag(string("variant"), string("spruce"));
    desc->addTag(string("axis"), string("x"));

    desc = block->addBlockDescription(
        "Spruce Log (Horizontal)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "spruce_log" // NBT name
    );
    desc->addTag(string("axis"), string("x"));

    // Soul Sand
    block = blockList->addBlock(string("soul_sand"));

    desc = block->addBlockDescription(
        "Soul Sand",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "soul_sand", // NBT name
        88,          // Block ID
        0            // Data value
    );

    // Soul Soil
    block = blockList->addBlock(string("soul_soil"));

    desc = block->addBlockDescription(
        "Soul Sand",
        McVersion::MC_1_16, MC_LAST_VERSION,
        "soul_soil" // NBT name
    );

    // ---------------------
    // ---- COLOR_GREEN ----
    // ---------------------
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

    // Green concrete powder
    block = blockList->addBlock(string("green_concrete_powder"));

    desc = block->addBlockDescription(
        "Green Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        13                 // Data value
    );
    desc->addTag(string("color"), string("green"));

    desc = block->addBlockDescription(
        "Green Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "green_concrete_powder" // NBT name
    );

    // Green Glazed Terracota
    block = blockList->addBlock(string("green_glazed_terracota"));

    desc = block->addBlockDescription(
        "Green Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "green_glazed_terracota", // NBT name
        248,                      // Block ID
        0                         // Data value
    );

    // Green Stained Glass
    block = blockList->addBlock(string("green_stained_glass"));

    desc = block->addBlockDescription(
        "Green Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        13               // Data value
    );
    desc->addTag(string("color"), string("green"));

    desc = block->addBlockDescription(
        "Green Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "green_stained_glass" // NBT name
    );

    // Dried Kelp Block
    block = blockList->addBlock(string("dried_kelp_block"));

    desc = block->addBlockDescription(
        "Dried Kelp Block",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dried_kelp_block" // NBT name
    );

    // Moss Block
    block = blockList->addBlock(string("moss_block"));

    desc = block->addBlockDescription(
        "Moss Block",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "moss_block" // NBT name
    );

    // Moss Carpet
    block = blockList->addBlock(string("moss_carpet"));

    desc = block->addBlockDescription(
        "Moss Carpet",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "moss_carpet" // NBT name
    );

    // -------------------
    // ---- COLOR_RED ----
    // -------------------
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

    // Red concrete powder
    block = blockList->addBlock(string("red_concrete_powder"));

    desc = block->addBlockDescription(
        "Red Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        14                 // Data value
    );
    desc->addTag(string("color"), string("red"));

    desc = block->addBlockDescription(
        "Red Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "red_concrete_powder" // NBT name
    );

    // Red Glazed Terracota
    block = blockList->addBlock(string("red_glazed_terracota"));

    desc = block->addBlockDescription(
        "Red Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "red_glazed_terracota", // NBT name
        249,                    // Block ID
        0                       // Data value
    );

    // Red Stained Glass
    block = blockList->addBlock(string("red_stained_glass"));

    desc = block->addBlockDescription(
        "Red Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        14               // Data value
    );
    desc->addTag(string("color"), string("red"));

    desc = block->addBlockDescription(
        "Red Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "red_stained_glass" // NBT name
    );

    // Bricks
    block = blockList->addBlock(string("bricks"));

    desc = block->addBlockDescription(
        "Bricks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "brick_block", // NBT name
        45,            // Block ID
        0              // Data value
    );

    desc = block->addBlockDescription(
        "Bricks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "bricks" // NBT name
    );

    // Brick Slab
    block = blockList->addBlock(string("brick_slab"));

    desc = block->addBlockDescription(
        "Brick Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stone_slab", // NBT name
        44,           // Block ID
        4             // Data value
    );

    desc->addTag(string("variant"), string("brick"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Brick Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "brick_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Nether Wart Block
    block = blockList->addBlock(string("nether_wart_block"));

    desc = block->addBlockDescription(
        "Nether Wart Block",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "nether_wart_block", // NBT name
        214,                 // Block ID
        0                    // Data value
    );

    // Shroomlight
    block = blockList->addBlock(string("shroomlight"));

    desc = block->addBlockDescription(
        "Shroomlight",
        McVersion::MC_1_16, MC_LAST_VERSION,
        "shroomlight" // NBT name
    );

    // Mangrove Log (Vertical)
    block = blockList->addBlock(string("mangrove_log"));

    desc = block->addBlockDescription(
        "Mangrove Log (Vertical)",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "mangrove_log" // NBT name
    );

    desc->addTag(string("axis"), string("y"));

    // Mangrove Planks
    block = blockList->addBlock(string("mangrove_planks"));

    desc = block->addBlockDescription(
        "Mangrove Planks",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "mangrove_planks" // NBT name
    );

    // Mangrove Slab
    block = blockList->addBlock(string("mangrove_slab"));

    desc = block->addBlockDescription(
        "Mangrove Slab",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "mangrove_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Mangrove Pressure Plate
    block = blockList->addBlock(string("mangrove_pressure_plate"));

    desc = block->addBlockDescription(
        "Mangrove Pressure Plate",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "mangrove_pressure_plate" // NBT name
    );

    // Red Mushroom Block
    block = blockList->addBlock(string("red_mushroom_block"));

    desc = block->addBlockDescription(
        "Red Mushroom Block",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "red_mushroom_block" // NBT name
    );

    // ---------------------
    // ---- COLOR_BLACK ----
    // ---------------------
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

    // Black concrete powder
    block = blockList->addBlock(string("black_concrete_powder"));

    desc = block->addBlockDescription(
        "Black Concrete Powder",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "concrete_powder", // NBT name
        252,               // Block ID
        15                 // Data value
    );
    desc->addTag(string("color"), string("black"));

    desc = block->addBlockDescription(
        "Black Concrete Powder",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "black_concrete_powder" // NBT name
    );

    // Black Glazed Terracota
    block = blockList->addBlock(string("black_glazed_terracota"));

    desc = block->addBlockDescription(
        "Black Glazed Terracota",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "black_glazed_terracota", // NBT name
        250,                      // Block ID
        0                         // Data value
    );

    // Black Stained Glass
    block = blockList->addBlock(string("black_stained_glass"));

    desc = block->addBlockDescription(
        "Black Stained Glass",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stained_glass", // NBT name
        95,              // Block ID
        15               // Data value
    );
    desc->addTag(string("color"), string("black"));

    desc = block->addBlockDescription(
        "Black Stained Glass",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "black_stained_glass" // NBT name
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
        "Obsidian",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "obsidian", // NBT name
        49,         // Block ID
        0           // Data value
    );

    // Crying Obsidian
    block = blockList->addBlock(string("crying_obsidian"));

    desc = block->addBlockDescription(
        "Crying Obsidian",
        McVersion::MC_1_16, MC_LAST_VERSION,
        "crying_obsidian" // NBT name
    );

    // Blackstone
    block = blockList->addBlock(string("blackstone"));

    desc = block->addBlockDescription(
        "Blackstone",
        McVersion::MC_1_16, MC_LAST_VERSION,
        "blackstone" // NBT name
    );

    // Blackstone Slab
    block = blockList->addBlock(string("blackstone_slab"));

    desc = block->addBlockDescription(
        "Blackstone Slab",
        McVersion::MC_1_16, MC_LAST_VERSION,
        "blackstone_slab" // NBT name
    );

    // Polished Blackstone Pressure Plate
    block = blockList->addBlock(string("polished_blackstone_pressure_plate"));

    desc = block->addBlockDescription(
        "Polished Blackstone Pressure Plate",
        McVersion::MC_1_16, MC_LAST_VERSION,
        "polished_blackstone_pressure_plate" // NBT name
    );

    // Basalt
    block = blockList->addBlock(string("basalt"));

    desc = block->addBlockDescription(
        "Basalt",
        McVersion::MC_1_16, MC_LAST_VERSION,
        "basalt" // NBT name
    );

    // Smooth Basalt
    block = blockList->addBlock(string("smooth_basalt"));

    desc = block->addBlockDescription(
        "Smooth Basalt",
        McVersion::MC_1_16, MC_LAST_VERSION,
        "smooth_basalt" // NBT name
    );

    // Block of Netherite
    block = blockList->addBlock(string("netherite_block"));

    desc = block->addBlockDescription(
        "Block of Netherite",
        McVersion::MC_1_16, MC_LAST_VERSION,
        "netherite_block" // NBT name
    );

    // Sculk
    block = blockList->addBlock(string("sculk"));

    desc = block->addBlockDescription(
        "Sculk",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "sculk" // NBT name
    );

    // ---------------
    // ---- GOLD -----
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

    // -----------------
    // ---- DIAMOND ----
    // -----------------
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

    // Prismarine Brick Slab
    block = blockList->addBlock(string("prismarine_brick_slab"));

    desc = block->addBlockDescription(
        "Prismarine Brick Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "prismarine_brick_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Dark Prismarine
    block = blockList->addBlock(string("dark_prismarine"));

    desc = block->addBlockDescription(
        "Dark Prismarine",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "prismarine", // NBT name
        168,          // Block ID
        2             // Data value
    );
    desc->addTag(string("variant"), string("dark_prismarine"));

    desc = block->addBlockDescription(
        "Dark Prismarine",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "dark_prismarine" // NBT name
    );

    // Dark Prismarine Slab
    block = blockList->addBlock(string("dark_prismarine_slab"));

    desc = block->addBlockDescription(
        "Dark Prismarine Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "dark_prismarine_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

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

    // -----------------
    // ---- EMERALD ----
    // -----------------
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

    // ----------------
    // ---- PODZOL ----
    // ----------------
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

    // Spruce slab
    block = blockList->addBlock(string("spruce_slab"));

    desc = block->addBlockDescription(
        "Spruce Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "wooden_slab", // NBT name
        126,           // Block ID
        1              // Data value
    );
    desc->addTag(string("variant"), string("spruce"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Spruce Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "spruce_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Spruce pressure plate
    block = blockList->addBlock(string("spruce_pressure_plate"));

    desc = block->addBlockDescription(
        "Spruce Pressure Plate",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "spruce_pressure_plate" // NBT name
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

    // Spruce log (Vertical)
    block = blockList->addBlock(string("spruce_log"));

    desc = block->addBlockDescription(
        "Spruce Log (Vertical)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log", // NBT name
        17,    // Block ID
        1      // Data value
    );
    desc->addTag(string("variant"), string("spruce"));
    desc->addTag(string("axis"), string("y"));

    desc = block->addBlockDescription(
        "Spruce Log (Vertical)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "spruce_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // Oak Log (Horizontal)
    block = blockList->addBlock(string("oak_log_horizontal"));

    desc = block->addBlockDescription(
        "Oak Log (Horizontal)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log", // NBT name
        17,    // Block ID
        0      // Data value
    );
    desc->addTag(string("variant"), string("oak"));
    desc->addTag(string("axis"), string("x"));

    desc = block->addBlockDescription(
        "Oak Log (Horizontal)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "oak_log" // NBT name
    );
    desc->addTag(string("axis"), string("x"));

    // Jungle Log (Horizontal)
    block = blockList->addBlock(string("jungle_log_horizontal"));

    desc = block->addBlockDescription(
        "Jungle Log (Horizontal)",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "log", // NBT name
        17,    // Block ID
        3      // Data value
    );
    desc->addTag(string("variant"), string("jungle"));
    desc->addTag(string("axis"), string("x"));

    desc = block->addBlockDescription(
        "Jungle Log (Horizontal)",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "jungle_log" // NBT name
    );
    desc->addTag(string("axis"), string("x"));

    // Mangrove Log (Horizontal)
    block = blockList->addBlock(string("mangrove_log_horizontal"));

    desc = block->addBlockDescription(
        "Mangrove Log (Horizontal)",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "mangrove_log" // NBT name
    );
    desc->addTag(string("axis"), string("x"));

    // Mangrove Roots
    block = blockList->addBlock(string("mangrove_roots"));

    desc = block->addBlockDescription(
        "Mangrove Roots",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "mangrove_roots" // NBT name
    );

    // ----------------
    // ---- NETHER ----
    // ----------------
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

    // Nether Bricks
    block = blockList->addBlock(string("nether_bricks"));

    desc = block->addBlockDescription(
        "Nether Bricks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "nether_brick", // NBT name
        112,            // Block ID
        0               // Data value
    );

    desc = block->addBlockDescription(
        "Nether Bricks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "nether_bricks" // NBT name
    );

    // Nether Brick Slab
    block = blockList->addBlock(string("nether_brick_slab"));

    desc = block->addBlockDescription(
        "Nether Brick Slab",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "stone_slab", // NBT name
        44,           // Block ID
        6             // Data value
    );

    desc->addTag(string("variant"), string("nether_brick"));
    desc->addTag(string("half"), string("bottom"));

    desc = block->addBlockDescription(
        "Nether Brick Slab",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "nether_brick_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Magma Block
    block = blockList->addBlock(string("magma_block"));

    desc = block->addBlockDescription(
        "Magma Block",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "magma", // NBT name
        213,     // Block ID
        0        // Data value
    );

    desc = block->addBlockDescription(
        "Magma Block",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "magma_block" // NBT name
    );

    // Red Nether Bricks
    block = blockList->addBlock(string("red_nether_bricks"));

    desc = block->addBlockDescription(
        "Red Nether Bricks",
        McVersion::MC_1_12, McVersion::MC_1_12,
        "red_nether_brick", // NBT name
        215,                // Block ID
        0                   // Data value
    );

    desc = block->addBlockDescription(
        "Red Nether Bricks",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "red_nether_bricks" // NBT name
    );

    // Red Nether Brick Slab
    block = blockList->addBlock(string("red_nether_brick_slab"));

    desc = block->addBlockDescription(
        "Red Nether Brick Slab",
        McVersion::MC_1_14, MC_LAST_VERSION,
        "red_nether_brick_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // --------------------------
    // ---- TERRACOTTA_WHITE ----
    // --------------------------
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

    // Calcite
    block = blockList->addBlock(string("calcite"));

    desc = block->addBlockDescription(
        "Calcite",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "calcite" // NBT name
    );

    // Cherry Log (Vertical)
    block = blockList->addBlock(string("cherry_log"));

    desc = block->addBlockDescription(
        "Cherry Log (Vertical)",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "cherry_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // Stripped Cherry Log (Vertical)
    block = blockList->addBlock(string("stripped_cherry_log"));

    desc = block->addBlockDescription(
        "Stripped Cherry Log (Vertical)",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "stripped_cherry_log" // NBT name
    );
    desc->addTag(string("axis"), string("y"));

    // Cherry Planks
    block = blockList->addBlock(string("cherry_planks"));

    desc = block->addBlockDescription(
        "Cherry Planks",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "cherry_planks" // NBT name
    );

    // Cherry Slab
    block = blockList->addBlock(string("cherry_slab"));

    desc = block->addBlockDescription(
        "Cherry Slab",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "cherry_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Cherry Pressure Plate
    block = blockList->addBlock(string("cherry_pressure_plate"));

    desc = block->addBlockDescription(
        "Cherry Pressure Plate",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "cherry_pressure_plate" // NBT name
    );

    // ---------------------------
    // ---- TERRACOTTA_ORANGE ----
    // ---------------------------
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

    // Redstone Lamp
    block = blockList->addBlock(string("redstone_lamp"));

    desc = block->addBlockDescription(
        "Redstone Lamp",
        McVersion::MC_1_12, MC_LAST_VERSION,
        "redstone_lamp", // NBT name
        123,             // Block ID
        0                // Data value
    );

    // Block of Resin
    block = blockList->addBlock(string("resin_block"));

    desc = block->addBlockDescription(
        "Block of Resin",
        McVersion::MC_1_21_4, MC_LAST_VERSION,
        "resin_block" // NBT name
    );

    // Resin Bricks
    block = blockList->addBlock(string("resin_bricks"));

    desc = block->addBlockDescription(
        "Resin Bricks",
        McVersion::MC_1_21_4, MC_LAST_VERSION,
        "resin_bricks" // NBT name
    );

    // Resin Brick Slab
    block = blockList->addBlock(string("resin_brick_slab"));

    desc = block->addBlockDescription(
        "Resin Brick Slab",
        McVersion::MC_1_21_4, MC_LAST_VERSION,
        "resin_brick_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // ----------------------------
    // ---- TERRACOTTA_MAGENTA ----
    // ----------------------------
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

    // -------------------------------
    // ---- TERRACOTTA_LIGHT_BLUE ----
    // -------------------------------
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

    // ---------------------------
    // ---- TERRACOTTA_YELLOW ----
    // ---------------------------
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

    // --------------------------------
    // ---- TERRACOTTA_LIGHT_GREEN ----
    // --------------------------------
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

    // -------------------------
    // ---- TERRACOTTA_PINK ----
    // -------------------------
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

    // Stripped Cherry Log (horizontal)
    block = blockList->addBlock(string("stripped_cherry_log_horizontal"));

    desc = block->addBlockDescription(
        "Stripped Cherry Log (Horizontal)",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "stripped_cherry_log" // NBT name
    );
    desc->addTag(string("axis"), string("x"));

    // Stripped Cherry Wood
    block = blockList->addBlock(string("stripped_cherry_wood"));

    desc = block->addBlockDescription(
        "Stripped Cherry Wood",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "stripped_cherry_wood" // NBT name
    );

    // -------------------------
    // ---- TERRACOTTA_GRAY ----
    // -------------------------
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

    // Tuff
    block = blockList->addBlock(string("tuff"));

    desc = block->addBlockDescription(
        "Tuff",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "tuff" // NBT name
    );

    // Tuff Slab
    block = blockList->addBlock(string("tuff_slab"));

    desc = block->addBlockDescription(
        "Tuff Slab",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "tuff_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Cherry Log (horizontal)
    block = blockList->addBlock(string("cherry_log_horizontal"));

    desc = block->addBlockDescription(
        "Cherry Log (Horizontal)",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "cherry_log" // NBT name
    );
    desc->addTag(string("axis"), string("x"));

    // Cherry Wood
    block = blockList->addBlock(string("cherry_wood"));

    desc = block->addBlockDescription(
        "Cherry Wood",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "cherry_wood" // NBT name
    );

    // -------------------------------
    // ---- TERRACOTTA_LIGHT_GRAY ----
    // -------------------------------
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
    desc->addTag(string("color"), string("silver"));

    desc = block->addBlockDescription(
        "Light Gray Terracotta",
        McVersion::MC_1_13, MC_LAST_VERSION,
        "light_gray_terracotta" // NBT name
    );

    // Waxed Exposed Copper
    block = blockList->addBlock(string("waxed_exposed_copper"));

    desc = block->addBlockDescription(
        "Waxed Exposed Copper",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "waxed_exposed_copper" // NBT name
    );

    // Waxed Exposed Cut Copper Slab
    block = blockList->addBlock(string("waxed_exposed_cut_copper_slab"));

    desc = block->addBlockDescription(
        "Waxed Exposed Cut Copper Slab",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "waxed_exposed_cut_copper_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // Mud Bricks
    block = blockList->addBlock(string("mud_bricks"));

    desc = block->addBlockDescription(
        "Mud Bricks",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "mud_bricks" // NBT name
    );

    // Mud Brick Slab
    block = blockList->addBlock(string("mud_brick_slab"));

    desc = block->addBlockDescription(
        "Mud Brick Slab",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "mud_brick_slab" // NBT name
    );

    desc->addTag(string("type"), string("bottom"));

    // -------------------------
    // ---- TERRACOTTA_CYAN ----
    // -------------------------
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

    // Mud
    block = blockList->addBlock(string("mud"));

    desc = block->addBlockDescription(
        "Mud",
        McVersion::MC_1_19, MC_LAST_VERSION,
        "mud" // NBT name
    );

    // ---------------------------
    // ---- TERRACOTTA_PURPLE ----
    // ---------------------------
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

    // -------------------------
    // ---- TERRACOTTA_BLUE ----
    // -------------------------
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

    // --------------------------
    // ---- TERRACOTTA_BROWN ----
    // --------------------------
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

    // Dripstone Block
    block = blockList->addBlock(string("dripstone_block"));

    desc = block->addBlockDescription(
        "Dripstone Block",
        McVersion::MC_1_17, MC_LAST_VERSION,
        "dripstone_block" // NBT name
    );

    // --------------------------
    // ---- TERRACOTTA_GREEN ----
    // --------------------------
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

    // ------------------------
    // ---- TERRACOTTA_RED ----
    // ------------------------
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

    // Decorated Pot
    block = blockList->addBlock(string("decorated_pot"));

    desc = block->addBlockDescription(
        "Decorated Pot",
        McVersion::MC_1_20, MC_LAST_VERSION,
        "decorated_pot" // NBT name
    );

    // --------------------------
    // ---- TERRACOTTA_BLACK ----
    // --------------------------
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

        // ------------------------
        // ---- CRIMSON_NYLIUM ----
        // ------------------------
        blockList = &(list[(size_t)McColors::CRIMSON_NYLIUM]);

        // Crimson Nylium
        block = blockList->addBlock(string("crimson_nylium"));

        desc = block->addBlockDescription(
            "Crimson Nylium",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "crimson_nylium" // NBT name
        );

        // ----------------------
        // ---- CRIMSON_STEM ----
        // ----------------------
        blockList = &(list[(size_t)McColors::CRIMSON_STEM]);

        // Crimson Planks
        block = blockList->addBlock(string("crimson_planks"));

        desc = block->addBlockDescription(
            "Crimson Planks",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "crimson_planks" // NBT name
        );

        // Crimson Slab
        block = blockList->addBlock(string("crimson_slab"));

        desc = block->addBlockDescription(
            "Crimson Slab",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "crimson_slab" // NBT name
        );

        desc->addTag(string("type"), string("bottom"));

        // Crimson Pressure Plate
        block = blockList->addBlock(string("crimson_pressure_plate"));

        desc = block->addBlockDescription(
            "Crimson Pressure Plate",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "crimson_pressure_plate" // NBT name
        );

        // Crimson Stem
        block = blockList->addBlock(string("crimson_stem"));

        desc = block->addBlockDescription(
            "Crimson Stem",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "crimson_stem" // NBT name
        );
        desc->addTag(string("axis"), string("y"));

        // Stripped Crimson Stem
        block = blockList->addBlock(string("stripped_crimson_stem"));

        desc = block->addBlockDescription(
            "Stripped Crimson Stem",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "stripped_crimson_stem" // NBT name
        );
        desc->addTag(string("axis"), string("y"));

        // ------------------------
        // ---- CRIMSON_HYPHAE ----
        // ------------------------
        blockList = &(list[(size_t)McColors::CRIMSON_HYPHAE]);

        // Crimson Hyphae
        block = blockList->addBlock(string("crimson_hyphae"));

        desc = block->addBlockDescription(
            "Crimson Hyphae",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "crimson_hyphae" // NBT name
        );

        // Stripped Crimson Hyphae
        block = blockList->addBlock(string("stripped_crimson_hyphae"));

        desc = block->addBlockDescription(
            "Stripped Crimson Hyphae",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "stripped_crimson_hyphae" // NBT name
        );

        // -----------------------
        // ---- WARPED_NYLIUM ----
        // -----------------------
        blockList = &(list[(size_t)McColors::WARPED_NYLIUM]);

        // Warped Nylium
        block = blockList->addBlock(string("warped_nylium"));

        desc = block->addBlockDescription(
            "Warped Nylium",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "warped_nylium" // NBT name
        );

        // Waxed Oxidized Copper
        block = blockList->addBlock(string("waxed_oxidized_copper"));

        desc = block->addBlockDescription(
            "Waxed Oxidized Copper",
            McVersion::MC_1_17, MC_LAST_VERSION,
            "waxed_oxidized_copper" // NBT name
        );

        // Waxed Oxidized Cut Copper Slab
        block = blockList->addBlock(string("waxed_oxidized_cut_copper_slab"));

        desc = block->addBlockDescription(
            "Waxed Oxidized Cut Copper Slab",
            McVersion::MC_1_17, MC_LAST_VERSION,
            "waxed_oxidized_cut_copper_slab" // NBT name
        );

        desc->addTag(string("type"), string("bottom"));

        // ---------------------
        // ---- WARPED_STEM ----
        // ---------------------
        blockList = &(list[(size_t)McColors::WARPED_STEM]);

        // Warped Planks
        block = blockList->addBlock(string("warped_planks"));

        desc = block->addBlockDescription(
            "Warped Planks",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "warped_planks" // NBT name
        );

        // Warped Slab
        block = blockList->addBlock(string("warped_slab"));

        desc = block->addBlockDescription(
            "Warped Slab",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "warped_slab" // NBT name
        );

        desc->addTag(string("type"), string("bottom"));

        // Warped Pressure Plate
        block = blockList->addBlock(string("warped_pressure_plate"));

        desc = block->addBlockDescription(
            "Warped Pressure Plate",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "warped_pressure_plate" // NBT name
        );

        // Warped Stem
        block = blockList->addBlock(string("warped_stem"));

        desc = block->addBlockDescription(
            "Warped Stem",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "warped_stem" // NBT name
        );
        desc->addTag(string("axis"), string("y"));

        // Stripped Warped Stem
        block = blockList->addBlock(string("stripped_warped_stem"));

        desc = block->addBlockDescription(
            "Stripped Warped Stem",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "stripped_warped_stem" // NBT name
        );
        desc->addTag(string("axis"), string("y"));

        // Waxed Weathered Copper
        block = blockList->addBlock(string("waxed_weathered_copper"));

        desc = block->addBlockDescription(
            "Waxed Weathered Copper",
            McVersion::MC_1_17, MC_LAST_VERSION,
            "waxed_weathered_copper" // NBT name
        );

        // Waxed Weathered Cut Copper Slab
        block = blockList->addBlock(string("waxed_weathered_cut_copper_slab"));

        desc = block->addBlockDescription(
            "Waxed Weathered Cut Copper Slab",
            McVersion::MC_1_17, MC_LAST_VERSION,
            "waxed_weathered_cut_copper_slab" // NBT name
        );

        desc->addTag(string("type"), string("bottom"));

        // -----------------------
        // ---- WARPED_HYPHAE ----
        // -----------------------
        blockList = &(list[(size_t)McColors::WARPED_HYPHAE]);

        // Warped Hyphae
        block = blockList->addBlock(string("warped_hyphae"));

        desc = block->addBlockDescription(
            "Warped Hyphae",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "warped_hyphae" // NBT name
        );

        // Stripped Warped Hyphae
        block = blockList->addBlock(string("stripped_warped_hyphae"));

        desc = block->addBlockDescription(
            "Stripped Warped Hyphae",
            McVersion::MC_1_16, MC_LAST_VERSION,
            "stripped_warped_hyphae" // NBT name
        );

        // ---------------------------
        // ---- WARPED_WART_BLOCK ----
        // ---------------------------
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

            // -------------------
            // ---- DEEPSLATE ----
            // -------------------
            blockList = &(list[(size_t)McColors::DEEPSLATE]);

            // Deepslate
            block = blockList->addBlock(string("deepslate"));

            desc = block->addBlockDescription(
                "Deepslate",
                McVersion::MC_1_17, MC_LAST_VERSION,
                "deepslate" // NBT name
            );

            // Cobbled Deepslate
            block = blockList->addBlock(string("cobbled_deepslate"));

            desc = block->addBlockDescription(
                "Cobbled Deepslate",
                McVersion::MC_1_17, MC_LAST_VERSION,
                "cobbled_deepslate" // NBT name
            );

            // Cobbled Deepslate Slab
            block = blockList->addBlock(string("cobbled_deepslate_slab"));

            desc = block->addBlockDescription(
                "Cobbled Deepslate Slab",
                McVersion::MC_1_17, MC_LAST_VERSION,
                "cobbled_deepslate_slab" // NBT name
            );

            desc->addTag(string("type"), string("bottom"));

            // ------------------
            // ---- RAW_IRON ----
            // ------------------
            blockList = &(list[(size_t)McColors::RAW_IRON]);

            // Block of Raw Iron
            block = blockList->addBlock(string("raw_iron_block"));

            desc = block->addBlockDescription(
                "Block of Raw Iron",
                McVersion::MC_1_17, MC_LAST_VERSION,
                "raw_iron_block" // NBT name
            );

            // ---------------------
            // ---- GLOW_LICHEN ----
            // ---------------------
            blockList = &(list[(size_t)McColors::GLOW_LICHEN]);

            // Verdant Froglight
            block = blockList->addBlock(string("verdant_froglight"));

            desc = block->addBlockDescription(
                "Verdant Froglight",
                McVersion::MC_1_19, MC_LAST_VERSION,
                "verdant_froglight" // NBT name
            );

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
