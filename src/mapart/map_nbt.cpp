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

#include "map_nbt.h"

#include <fstream>

#include <io/stream_reader.h>
#include <io/stream_writer.h>
#include <io/izlibstream.h>
#include <io/ozlibstream.h>

#include <nbt_tags.h>

using namespace std;
using namespace nbt;
using namespace colors;
using namespace mapart;
using namespace minecraft;

std::vector<map_color_t> mapart::readMapNBTFile(std::string fileName)
{
    std::vector<map_color_t> result(MAP_WIDTH * MAP_HEIGHT);

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

        size_t map_size = MAP_WIDTH * MAP_HEIGHT;

        for (int i = 0; i < map_size; i++)
        {
            result[i] = uint8_t(colorsBytes.at(i));
        }
    }
    catch (...)
    {
        throw -2;
    }

    return result;
}

void mapart::writeMapNBTFile(std::string fileName, const std::vector<map_color_t> &mapColors, minecraft::McVersion version)
{
    nbt::tag_compound root;
    nbt::tag_compound data;

    // Set meta data
    data.insert("width", nbt::tag_int(MAP_WIDTH));
    data.insert("height", nbt::tag_int(MAP_HEIGHT));
    data.insert("dimension", nbt::tag_int(0));
    data.insert("scale", nbt::tag_int(0));
    data.insert("trackingPosition:", nbt::tag_int(0));
    data.insert("unlimitedTracking", nbt::tag_int(0));

    if (version >= McVersion::MC_1_14)
    {
        // If we can, prevent the map from being modified
        data.insert("locked", nbt::tag_int(1));
    }

    // Set the center far away to prevent issues (20M)
    data.insert("xCenter", nbt::tag_int(20000000));
    data.insert("zCenter", nbt::tag_int(20000000));

    // Set colors array
    nbt::tag_byte_array byteArray;
    size_t size = MAP_WIDTH * MAP_HEIGHT;
    for (size_t i = 0; i < size; i++)
    {
        short val = mapColors[i];
        int8_t ip = static_cast<int8_t>((val > 127) ? (val - 256) : val);
        byteArray.push_back(ip);
    }
    data.insert("colors", byteArray.clone());

    // Insert tags to root
    root.insert("data", data.clone());
    root.insert("DataVersion", minecraft::versionToDataVersion(version));

    std::ofstream file(fileName, std::ios::binary);

    if (!file)
    {
        throw -1;
    }

    try
    {
        zlib::ozlibstream ogzs(file, -1, true);
        nbt::io::write_tag("", root, ogzs);
    }
    catch (...)
    {
        throw -2;
    }
}
