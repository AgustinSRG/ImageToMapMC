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

#include "project.h"

#include <fstream>

#include <io/stream_reader.h>
#include <io/stream_writer.h>
#include <io/izlibstream.h>
#include <io/ozlibstream.h>

#include <nbt_tags.h>

using namespace std;
using namespace colors;
using namespace minecraft;
using namespace mapart;

MapArtProject::MapArtProject()
{

    // Image edit
    saturation = 1;
    contrast = 1;
    brightness = 1;

    // Colors conf
    colorSetConf = "MODE(BLACKLIST)\n";

    // Version
    version = MC_LAST_VERSION;

    // Map params
    colorDistanceAlgorithm = ColorDistanceAlgorithm::Euclidean;
    ditheringMethod = DitheringMethod::None;
    buildMethod = MapBuildMethod::Chaos;

    // Image size
    width = MAP_WIDTH;
    height = MAP_HEIGHT;
    resize_width = width;
    resize_height = height;

    // Image data
    image_data.resize(MAP_WIDTH * MAP_HEIGHT * 3);

    for (size_t i = 0; i < image_data.size(); i++)
    {
        image_data[i] = 255;
    }
}

MapArtProject::MapArtProject(const MapArtProject &p1)
{
    // Image edit
    saturation = p1.saturation;
    contrast = p1.contrast;
    brightness = p1.brightness;

    // Colors conf
    colorSetConf = p1.colorSetConf;

    // Version
    version = p1.version;

    // Map params
    colorDistanceAlgorithm = p1.colorDistanceAlgorithm;
    ditheringMethod = p1.ditheringMethod;
    buildMethod = p1.buildMethod;

    // Image size
    width = p1.width;
    height = p1.height;
    resize_width = p1.resize_width;
    resize_height = p1.resize_height;

    // Image data
    image_data = p1.image_data;
}

bool MapArtProject::loadFromFile(std::string path)
{
    try
    {
        std::ifstream file(path, std::ios::binary);

        if (!file)
        {
            throw -1;
        }

        zlib::izlibstream igzs(file);

        auto pair = nbt::io::read_compound(igzs);
        nbt::tag_compound comp = *pair.second;

        // Image resize
        resize_width = comp.at("resize_width").as<nbt::tag_int>().get();
        resize_width = comp.at("resize_height").as<nbt::tag_int>().get();

        // Image edit params
        saturation = comp.at("saturation").as<nbt::tag_float>().get();
        contrast = comp.at("contrast").as<nbt::tag_float>().get();
        brightness = comp.at("brightness").as<nbt::tag_float>().get();

        // Colors conf
        colorSetConf = comp.at("colors_conf").as<nbt::tag_string>().get();

        // Version
        version = minecraft::getVersionFromText(comp.at("version").as<nbt::tag_string>().get());

        if (version == McVersion::UNKNOWN)
        {
            version = MC_LAST_VERSION;
        }

        // Map params
        string paramStr;

        paramStr = comp.at("color_distance").as<nbt::tag_string>().get();
        if (paramStr.compare("delta-e") == 0)
        {
            colorDistanceAlgorithm = ColorDistanceAlgorithm::DeltaE;
        }
        else if (paramStr.compare("euclidean") == 0)
        {
            colorDistanceAlgorithm = ColorDistanceAlgorithm::Euclidean;
        }
        else
        {
            colorDistanceAlgorithm = ColorDistanceAlgorithm::Euclidean;
        }

        ditheringMethod = mapart::parseDitheringMethodFromString(comp.at("dithering").as<nbt::tag_string>().get());

        if (ditheringMethod == DitheringMethod::Unknown)
        {
            ditheringMethod = DitheringMethod::None;
        }

        paramStr = comp.at("build_method").as<nbt::tag_string>().get();
        if (paramStr.compare("flat") == 0)
        {
            buildMethod = MapBuildMethod::Flat;
        }
        else if (paramStr.compare("stair") == 0)
        {
            buildMethod = MapBuildMethod::Staircased;
        }
        else if (paramStr.compare("chaos") == 0)
        {
            buildMethod = MapBuildMethod::Chaos;
        }
        else
        {
            buildMethod = MapBuildMethod::None;
        }

        width = comp.at("width").as<nbt::tag_int>().get();
        height = comp.at("height").as<nbt::tag_int>().get();

        if (width <= 0 || height <= 0 || width > 1000000 || height > 1000000)
        {
            return false;
        }

        nbt::tag_byte_array colorsByte = comp.at("image").as<nbt::tag_byte_array>();

        image_data.resize(width * height * 3);

        for (size_t i = 0; i < image_data.size(); i++)
        {
            image_data[i] = colorsByte.at(i);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool MapArtProject::saveToFile(std::string path)
{
    nbt::tag_compound root;

    root.insert("resize_width", nbt::tag_int(static_cast<int>(resize_width)));
    root.insert("resize_height", nbt::tag_int(static_cast<int>(resize_height)));

    root.insert("saturation", nbt::tag_float(saturation));
    root.insert("contrast", nbt::tag_float(contrast));
    root.insert("brightness", nbt::tag_float(brightness));

    root.insert("colors_conf", nbt::tag_string(colorSetConf));

    root.insert("version", nbt::tag_string(versionToString(version)));

    switch (colorDistanceAlgorithm)
    {
    case ColorDistanceAlgorithm::DeltaE:
        root.insert("color_distance", nbt::tag_string("delta-e"));
        break;
    default:
        root.insert("color_distance", nbt::tag_string("euclidean"));
    }

    root.insert("dithering", nbt::tag_string(ditheringMethodToString(ditheringMethod)));

    switch (buildMethod)
    {
    case MapBuildMethod::Chaos:
        root.insert("build_method", nbt::tag_string("chaos"));
        break;
    case MapBuildMethod::Flat:
        root.insert("build_method", nbt::tag_string("flat"));
        break;
    case MapBuildMethod::Staircased:
        root.insert("build_method", nbt::tag_string("stair"));
        break;
    default:
        root.insert("build_method", nbt::tag_string("none"));
    }

    // Image data
    root.insert("width", nbt::tag_int(static_cast<int>(width)));
    root.insert("height", nbt::tag_int(static_cast<int>(height)));

    nbt::tag_byte_array colorsByte;

    for (size_t i = 0; i < image_data.size(); i++)
    {
        colorsByte.push_back(image_data[i]);
    }

    root.insert("image", colorsByte.clone());

    // Save
    std::ofstream file(path, std::ios::binary);

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

std::vector<colors::Color> MapArtProject::getColors()
{
    std::vector<colors::Color> result(width * height);

    int j = 0;
    for (size_t i = 0; i < result.size(); i++)
    {
        result[i].red = image_data[j++];
        result[i].green = image_data[j++];
        result[i].blue = image_data[j++];
    }

    return result;
}

wxImage MapArtProject::toImage()
{
    wxImage image(width, height);
    unsigned char *rawData = image.GetData();

    for (size_t i = 0; i < image_data.size(); i++) {
        rawData[i] = image_data[i];
    }

    return image;
}

void MapArtProject::loadImage(wxImage &image)
{
    unsigned char *rawData = image.GetData();

    width = image.GetWidth();
    height = image.GetHeight();

    resize_width = width;
    resize_height = height;

    image_data.resize(width * height * 3);

    for (size_t i = 0; i < image_data.size(); i++) {
        image_data[i] = rawData[i];
    }
}
