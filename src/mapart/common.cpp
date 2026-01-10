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

#include "common.h"

#include <algorithm>

using namespace std;
using namespace mapart;

std::string mapart::ditheringMethodToString(mapart::DitheringMethod method)
{
    switch (method)
    {
    case DitheringMethod::FloydSteinberg:
        return string("Floyd-Steinberg");
    case DitheringMethod::MinAvgErr:
        return string("Min-Average-Error");
    case DitheringMethod::Burkes:
        return string("Burkes");
    case DitheringMethod::SierraLite:
        return string("Sierra-Lite");
    case DitheringMethod::Stucki:
        return string("Stucki");
    case DitheringMethod::Atkinson:
        return string("Atkinson");
    case DitheringMethod::Bayer44:
        return string("Bayer-44");
    case DitheringMethod::Bayer22:
        return string("Bayer-22");
    case DitheringMethod::Ordered33:
        return string("Ordered-33");
    default:
        return string("None");
    }
}

mapart::DitheringMethod mapart::parseDitheringMethodFromString(std::string str)
{
    std::string nameLower(str);
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    if (nameLower.compare(string("floyd-steinberg")) == 0 || nameLower.compare(string("floyd")) == 0 || nameLower.compare(string("fs")) == 0)
    {
        return DitheringMethod::FloydSteinberg;
    }
    else if (nameLower.compare(string("min-average-error")) == 0 || nameLower.compare(string("mae")) == 0)
    {
        return DitheringMethod::MinAvgErr;
    }
    else if (nameLower.compare(string("burkes")) == 0)
    {
        return DitheringMethod::Burkes;
    }
    else if (nameLower.compare(string("sierra-lite")) == 0 || nameLower.compare(string("sierra")) == 0)
    {
        return DitheringMethod::SierraLite;
    }
    else if (nameLower.compare(string("stucki")) == 0)
    {
        return DitheringMethod::Stucki;
    }
    else if (nameLower.compare(string("atkinson")) == 0)
    {
        return DitheringMethod::Atkinson;
    }
    else if (nameLower.compare(string("bayer-44")) == 0)
    {
        return DitheringMethod::Bayer44;
    }
    else if (nameLower.compare(string("bayer-22")) == 0)
    {
        return DitheringMethod::Bayer22;
    }
    else if (nameLower.compare(string("ordered-33")) == 0)
    {
        return DitheringMethod::Ordered33;
    }
    else if (nameLower.compare(string("none")) == 0)
    {
        return DitheringMethod::None;
    }
    else
    {
        return DitheringMethod::Unknown;
    }
}

std::vector<map_color_t> mapart::getMapDataFromColorMatrix(const std::vector<const minecraft::FinalColor *> &matrix, size_t matrixW, size_t matrixH, size_t mapX, size_t mapZ)
{
    size_t offsetX = mapX * MAP_WIDTH;
    size_t offsetZ = mapZ * MAP_HEIGHT;

    std::vector<map_color_t> result(MAP_WIDTH * MAP_HEIGHT);

    for (size_t z = 0; z < MAP_HEIGHT; z++)
    {
        for (size_t x = 0; x < MAP_WIDTH; x++)
        {
            size_t indexInMatrix = (z + offsetZ) * matrixW + (x + offsetX);
            size_t indexInResult = z * MAP_WIDTH + x;

            result[indexInResult] = matrix[indexInMatrix]->getMapColor();
        }
    }

    return result;
}

std::vector<const minecraft::FinalColor *> mapart::mapColorsToRGB(const std::vector<minecraft::FinalColor> &colorSet, const std::vector<map_color_t> &mapColors)
{
    size_t colorset_size = colorSet.size();
    size_t colors_size = mapColors.size();

    std::vector<const minecraft::FinalColor *> result(colors_size);

    for (size_t i = 0; i < colors_size; i++)
    {
        map_color_t color = mapColors[i];
        if (color >= 0 && color < colorset_size)
        {
            result[i] = &(colorSet[color]);
        }
        else
        {
            result[i] = &(colorSet[0]);
        }
    }

    return result;
}

MapBuildingSupportBlock mapart::getSupportBlockOptions(const minecraft::BlockList &supportBlockList, minecraft::McVersion version, std::string supportBlockMaterial, bool supportBlocksAlways) {
    MapBuildingSupportBlock result;
    result.block_ptr = supportBlockList.findBlockDescription(version, supportBlockMaterial);
    result.placeAlways = supportBlocksAlways;
    return result;
}
