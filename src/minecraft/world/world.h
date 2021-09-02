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

#include "region.h"

#include "../../threads/progress.h"

#define MAP_WORLD_FIRST_MAP_X (64)
#define MAP_WORLD_FIRST_MAP_Z (63)

#define MAP_WORLD_REGION_SEPARATOR_X (MAP_WIDTH * 2)
#define MAP_WORLD_REGION_SEPARATOR_Z (MAP_HEIGHT * 2)

namespace minecraft
{

    class MinecraftWorld
    {
    public:
        MinecraftWorld(minecraft::McVersion version, std::string levelName, int offsetY, std::string path);

        bool saveLevelFile();

        bool saveMapAsRegionFile(std::vector<mapart::MapBuildingBlock> &buildData, int mapX, int mapZ, threading::Progress &p);

    private:
        minecraft::McVersion version;
        std::string levelName;
        std::string path;
        std::string regionsPath;
        int offsetY;
    };
}
