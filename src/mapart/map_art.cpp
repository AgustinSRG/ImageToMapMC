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

#include "map_art.h"

#include <io/stream_reader.h>
#include <io/izlibstream.h>

#include <nbt_tags.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace nbt;

std::vector<map_color_t> mapart::readMapNBTFile(std::string fileName)
{
    std::vector<map_color_t> result(MAP_WIDTH * MAP_HEIGH);

    std::ifstream file(fileName, std::ios::binary);

    if (!file)
    {
        throw -1;
    }

    try
    {
        zlib::izlibstream igzs(file);

        auto pair = nbt::io::read_compound(igzs);

        nbt::tag_compound comp = *pair.second;
        nbt::value *colorsArray = &comp.at(std::string("data")).at(std::string("colors"));

        nbt::tag_byte_array colorsBytes = colorsArray->as<nbt::tag_byte_array>();

        size_t map_size = MAP_WIDTH * MAP_HEIGH;

        for (int i = 0; i < map_size; i++)
        {
            result[i] = int8_t(colorsBytes.at(i));
        }
    }
    catch (...)
    {
        throw -2;
    }

    return result;
}

std::vector<minecraft::FinalColor *> mapart::mapColorsToRGB(std::vector<minecraft::FinalColor> &colorSet, std::vector<map_color_t> &mapColors)
{
    size_t colorset_size = colorSet.size();
    size_t colors_size = mapColors.size();

    std::vector<minecraft::FinalColor *> result(colors_size);

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
