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

#include "map_build.h"
#include <algorithm>

using namespace std;
using namespace mapart;
using namespace colors;
using namespace minecraft;

void mapart::applyBuildRestrictions(std::vector<minecraft::FinalColor> &colorSet, MapBuildMethod method)
{
    if (method == MapBuildMethod::None)
    {
        return;
    }

    // Darker colors are unobtainable with blocks, so cannot be built
    setColorTypesEnabled(colorSet, McColorType::DARKER, false);

    if (method == MapBuildMethod::Flat)
    {
        // If flat, only normal are obtainable
        setColorTypesEnabled(colorSet, McColorType::LIGHT, false);
        setColorTypesEnabled(colorSet, McColorType::DARK, false);
    }
    else
    {
        // If 3d, water is a pain
        setBaseColorEnabled(colorSet, (short)McColors::WATER, false);
    }
}

std::vector<mapart::MapBuildingBlock> mapart::buildMap(minecraft::McVersion version, const std::vector<minecraft::BlockList> &blockSet, const std::vector<const minecraft::FinalColor *> &matrix, size_t matrixW, size_t matrixH, size_t mapX, size_t mapZ, mapart::MapBuildMethod buildMethod, size_t threadsNum, threading::Progress &p)
{
    bool smooth = true;

    if (buildMethod == MapBuildMethod::Chaos)
    {
        smooth = false;
    }

    std::vector<mapart::MapBuildingBlock> blocks(MAP_WIDTH * (MAP_HEIGHT + 1));

    for (size_t x = 0; x < MAP_WIDTH; x++)
    {
        buildMapRow(version, blockSet, blocks, matrix, matrixW, matrixH, mapX, mapZ, x, smooth);
        p.setProgress(0, static_cast<unsigned int>(x + 1));
    }

    return blocks;
}

void mapart::buildMapRow(minecraft::McVersion version, const std::vector<minecraft::BlockList> &blockSet, std::vector<mapart::MapBuildingBlock> &blockMatrix, const std::vector<const minecraft::FinalColor *> &matrix, size_t matrixW, size_t matrixH, size_t mapX, size_t mapZ, size_t x, bool smooth)
{
    size_t offsetX = mapX * MAP_WIDTH;
    size_t offsetZ = mapZ * MAP_HEIGHT;

    vector<Plateau> plateaus(1);
    size_t currentPlateau = 0;
    bool ascending = false;
    size_t currentPlateauStartIndex = 0;

    int lowestY = 0;

    blockMatrix[x].block_ptr = NULL;
    blockMatrix[x].x = static_cast<int>(x);
    blockMatrix[x].z = 0;
    blockMatrix[x].y = 0;

    plateaus[0].start = 0;
    plateaus[0].end = 0;

    for (size_t z = 0; z < MAP_HEIGHT; z++)
    {
        size_t indexBlockMatrix = (z + 1) * MAP_WIDTH + x;
        size_t indexInMatrix = (z + offsetZ) * matrixW + (x + offsetX);

        // Position
        blockMatrix[indexBlockMatrix].x = static_cast<int>(x);
        blockMatrix[indexBlockMatrix].z = static_cast<int>(z + 1);

        // Color
        const minecraft::FinalColor *color = matrix[indexInMatrix];

        // Block
        blockMatrix[indexBlockMatrix].block_ptr = blockSet[color->baseColorIndex].getBlockDescription(version);

        // Y level
        int prevIndex = static_cast<int>((z)*MAP_WIDTH + x);
        int prevY = blockMatrix[prevIndex].y;

        switch (color->colorType)
        {
        case McColorType::LIGHT:
            // 1 up
            blockMatrix[indexBlockMatrix].y = prevY + 1;
            ascending = true;
            currentPlateauStartIndex = z + 1;
            break;
        case McColorType::DARK:
            // 1 down
            blockMatrix[indexBlockMatrix].y = prevY - 1;
            if (lowestY > prevY - 1)
            {
                lowestY = prevY - 1;
            }
            if (ascending)
            {
                ascending = false;
                currentPlateau++;
                plateaus.resize(currentPlateau + 1);
                plateaus[currentPlateau].start = currentPlateauStartIndex;
                plateaus[currentPlateau].end = z + 1;
            }
            break;
        default:
            // Normal
            blockMatrix[indexBlockMatrix].y = prevY;
        }
    }

    currentPlateau++;
    plateaus.resize(currentPlateau + 1);
    plateaus[currentPlateau].start = MAP_HEIGHT + 1;
    plateaus[currentPlateau].end = MAP_HEIGHT + 1;

    if (!smooth)
    {
        // Optimize
        int pullDownPrev = 256;
        int pullDownNext = 256;

        for (size_t i = 0; i < currentPlateau; i++)
        {
            int pullDownHeight = 256;
            for (size_t z = plateaus[i].end; z < plateaus[i + 1].start; z++)
            {
                pullDownHeight = min(blockMatrix[z * MAP_WIDTH + x].y, pullDownHeight);
            }
            for (size_t z = plateaus[i].end; z < plateaus[i + 1].start; z++)
            {
                blockMatrix[z * MAP_WIDTH + x].y -= pullDownHeight;
            }

            pullDownNext = pullDownHeight;

            int plateauPulldownHeight = min(pullDownPrev, pullDownNext);
            for (size_t z = plateaus[i].start; z < plateaus[i].end; z++)
            {
                blockMatrix[z * MAP_WIDTH + x].y -= plateauPulldownHeight;
            }

            pullDownPrev = pullDownNext;
        }
    }
    else
    {
        // Set down to 1, so 0 is the min for base blocks
        for (size_t z = 0; z < (MAP_HEIGHT + 1); z++)
        {
            size_t indexBlockMatrix = (z)*MAP_WIDTH + x;
            blockMatrix[indexBlockMatrix].y -= lowestY;
        }
    }
}

