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

#include "world.h"
#include <filesystem>
#include <io/stream_writer.h>
#include <io/ozlibstream.h>
#include <nbt_tags.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace minecraft;

MinecraftWorld::MinecraftWorld(minecraft::McVersion version, std::string levelName, int offsetY, std::string path)
{
    this->version = version;
    this->levelName = levelName;
    this->path = path;

    filesystem::path wPath(path);

    wPath /= "region";

    this->regionsPath = wPath.string();

    this->offsetY = min(max(0, offsetY), 120);
}

bool MinecraftWorld::saveLevelFile() {
    // Create paths
    filesystem::create_directories(filesystem::path(regionsPath));

    // Create level.dat
    nbt::tag_compound root;
    nbt::tag_compound data;

    data.insert("DataVersion", minecraft::versionToDataVersion(version));

    data.insert("LevelName", nbt::tag_string(levelName));

    data.insert("BorderCenterX", nbt::tag_double(0));
    data.insert("BorderCenterZ", nbt::tag_double(0));
    data.insert("BorderDamagePerBlock", nbt::tag_double(0.2));
    data.insert("BorderSize", nbt::tag_double(60000000));
    data.insert("BorderSizeLerpTarget", nbt::tag_double(60000000));
    data.insert("BorderSizeLerpTime", nbt::tag_long(0));
    data.insert("BorderWarningBlocks", nbt::tag_double(5));
    data.insert("BorderWarningTime", nbt::tag_double(15));

    data.insert("DayTime", nbt::tag_long(0));
    data.insert("Difficulty", nbt::tag_byte(0)); // Peaceful
    data.insert("DifficultyLocked", nbt::tag_byte(0));

    data.insert("GameType", nbt::tag_int(1)); // Creative

    const auto p1 = std::chrono::system_clock::now();
    data.insert("LastPlayed", nbt::tag_long(std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch()).count()));

    data.insert("initialized", nbt::tag_byte(1));
    data.insert("allowCommands", nbt::tag_byte(1));
    data.insert("hardcore", nbt::tag_byte(0));
    data.insert("thundering", nbt::tag_byte(0));
    data.insert("raining", nbt::tag_byte(0));

    data.insert("thunderTime", nbt::tag_int(696969));
    data.insert("rainTime", nbt::tag_int(696969));
    data.insert("clearWeatherTime", nbt::tag_int(696969));


    // Generator options
    data.insert("generatorName", nbt::tag_string("flat"));
    stringstream generatorSS;
    if (version > McVersion::MC_1_12) {
        generatorSS << "minecraft:stone*" << offsetY << ";minecraft:the_void";
    } else {
        generatorSS << "3;minecraft:stone* " << offsetY << ";127;";
    }
    data.insert("generatorOptions", nbt::tag_string("flat"));
    data.insert("GameType", nbt::tag_int(1));

    // Seed
    data.insert("RandomSeed", nbt::tag_long(0));

    // Time
    data.insert("Time", nbt::tag_long(20));

    // Spawn
    data.insert("SpawnX", nbt::tag_int(MAP_WORLD_FIRST_MAP_X - 1));
    data.insert("SpawnY", nbt::tag_int(offsetY + 1));
    data.insert("SpawnZ", nbt::tag_int(MAP_WORLD_FIRST_MAP_Z - 1));

    // Player
    nbt::tag_compound playerTag;
    nbt::tag_list playerPosition;
    playerPosition.push_back(nbt::tag_double(MAP_WORLD_FIRST_MAP_X - 1));
    playerPosition.push_back(nbt::tag_double(offsetY + 1));
    playerPosition.push_back(nbt::tag_double(MAP_WORLD_FIRST_MAP_Z - 1));
    playerTag.insert("Pos", playerPosition.clone());
    data.insert("Player", playerTag.clone());

    // Game rules
    nbt::tag_compound gameRules;
    gameRules.insert("doFireTick", nbt::tag_string("false"));
    gameRules.insert("doMobSpawning", nbt::tag_string("false"));
    gameRules.insert("mobGriefing", nbt::tag_string("false"));
    gameRules.insert("doWeatherCycle", nbt::tag_string("false"));
    gameRules.insert("randomTickSpeed", nbt::tag_string("0"));
    data.insert("GameRules", gameRules.clone());

    // NBT version
    data.insert("version", nbt::tag_int(19133));

    // Game version
    nbt::tag_compound versionTag;
    versionTag.insert("Id", minecraft::versionToDataVersion(version));
    versionTag.insert("Name", minecraft::versionToString(version));
    versionTag.insert("Snapshot", nbt::tag_byte(0));
    data.insert("Version", versionTag.clone());

    // Insert data tag
    root.insert("Data", data.clone());

    // Save
    filesystem::path wPath(path);
    wPath /= "level.dat";
    std::ofstream file(wPath.string(), std::ios::binary);

    if (!file)
    {
        return false;
    }

    try
    {
        zlib::ozlibstream ogzs(file, -1, true);
        nbt::io::write_tag("", root, ogzs);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool MinecraftWorld::saveMapAsRegionFile(std::vector<mapart::MapBuildingBlock> &buildData, int mapX, int mapZ, threading::Progress &p) {
    int offsetX = MAP_WORLD_FIRST_MAP_X + mapX * MAP_WORLD_REGION_SEPARATOR_X;
    int offsetZ = MAP_WORLD_FIRST_MAP_Z + mapZ * MAP_WORLD_REGION_SEPARATOR_Z;

    int regionX = offsetX / (REGION_WIDTH * CHUNK_WIDTH);
    int regionZ = offsetZ / (REGION_HEIGHT * CHUNK_HEIGHT);

    MinecraftRegion region(regionX, regionZ, version, offsetY);

    // Build the region file
    for (int i = 0; i < buildData.size(); i++) {
        region.addBlock(buildData[i].block_ptr, buildData[i].x + offsetX, buildData[i].y, buildData[i].z + offsetZ);

        if (i % 512 == 0) {
            p.setProgress(0, i + 1);
        }
    }

    // Save region file
    return region.writeToFile(regionsPath);
}
