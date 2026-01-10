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
    transparencyTolerance = DEFAULT_TRANSPARENCY_TOLERANCE;

    // Background
    background.red = 255;
    background.green = 255;
    background.blue = 255;

    // Colors conf
    colorSetConf = "MODE(BLACKLIST)\n";

    // Support blocks
    supportBlockMaterial = std::string("stone");
    supportBlocksAlways = true;

    // Version
    version = MC_LAST_VERSION;

    // Map params
    colorDistanceAlgorithm = ColorDistanceAlgorithm::Euclidean;
    ditheringMethod = DitheringMethod::None;
    buildMethod = MapBuildMethod::Staircased;

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

    image_alpha.resize(MAP_WIDTH * MAP_HEIGHT);

    for (size_t i = 0; i < image_alpha.size(); i++)
    {
        image_alpha[i] = 255;
    }

    preserveTransparency = false;
}

MapArtProject::MapArtProject(const MapArtProject &p1)
{
    // Image edit
    saturation = p1.saturation;
    contrast = p1.contrast;
    brightness = p1.brightness;
    transparencyTolerance = p1.transparencyTolerance;

    // Background
    background = p1.background;

    // Colors conf
    colorSetConf = p1.colorSetConf;

    // Support blocks
    supportBlockMaterial = p1.supportBlockMaterial;
    supportBlocksAlways = p1.supportBlocksAlways;

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
    image_alpha = p1.image_alpha;

    // Transparency
    preserveTransparency = p1.preserveTransparency;
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
        resize_height = comp.at("resize_height").as<nbt::tag_int>().get();

        // Image edit params
        saturation = comp.at("saturation").as<nbt::tag_float>().get();
        contrast = comp.at("contrast").as<nbt::tag_float>().get();
        brightness = comp.at("brightness").as<nbt::tag_float>().get();

        // Background
        if (comp.has_key("background"))
        {
            background = colors::colorFromHex(comp.at("background").as<nbt::tag_string>().get());
        }
        else
        {
            background.red = 255;
            background.green = 255;
            background.blue = 255;
        }

        if (comp.has_key("support_block_material")) {
            supportBlockMaterial = comp.at("support_block_material").as<nbt::tag_string>().get();
        }

        if (comp.has_key("support_block_always")) {
            supportBlocksAlways = comp.at("support_block_always").as<nbt::tag_int>().get() != 0;
        } else {
            supportBlocksAlways = true;
        }

        if (comp.has_key("transparency_tol")) {
            transparencyTolerance = static_cast<unsigned char>(comp.at("preserve_transparency").as<nbt::tag_int>().get());

            if (transparencyTolerance == 0) {
                transparencyTolerance = DEFAULT_TRANSPARENCY_TOLERANCE;
            }
        } else {
            transparencyTolerance = DEFAULT_TRANSPARENCY_TOLERANCE;
        }

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

        if (comp.has_key("preserve_transparency")) {
            preserveTransparency = comp.at("preserve_transparency").as<nbt::tag_int>().get() != 0;
        } else {
            preserveTransparency = false;
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

        if (comp.has_key("alpha"))
        {
            nbt::tag_byte_array alphacolorsByte = comp.at("alpha").as<nbt::tag_byte_array>();

            image_alpha.resize(width * height);

            for (size_t i = 0; i < image_alpha.size(); i++)
            {
                image_alpha[i] = alphacolorsByte.at(i);
            }
        }
        else
        {
            image_alpha.resize(width * height);

            for (size_t i = 0; i < image_alpha.size(); i++)
            {
                image_alpha[i] = 255;
            }
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

    root.insert("background", nbt::tag_string(colors::colorToHex(background)));

    root.insert("transparency_tol", nbt::tag_int((int) transparencyTolerance));

    root.insert("colors_conf", nbt::tag_string(colorSetConf));

    root.insert("support_block_material", nbt::tag_string(supportBlockMaterial));
    root.insert("support_block_always", nbt::tag_int(supportBlocksAlways ? 1 : 0));

    root.insert("version", nbt::tag_string(versionToString(version)));

    root.insert("preserve_transparency", nbt::tag_int(preserveTransparency ? 1 : 0));

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

    nbt::tag_byte_array alphaByte;

    for (size_t i = 0; i < image_alpha.size(); i++)
    {
        alphaByte.push_back(image_alpha[i]);
    }

    root.insert("alpha", alphaByte.clone());

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

std::vector<bool> MapArtProject::getTransparency()
{
    std::vector<bool> result(width * height);

    for (size_t i = 0; i < result.size(); i++)
    {
        result[i] = image_alpha[i] == 0;
    }

    return result;
}

wxImage MapArtProject::toImage()
{
    wxImage image(width, height);

    unsigned char *rawData = image.GetData();

    for (size_t i = 0; i < image_data.size(); i++)
    {
        rawData[i] = image_data[i];
    }

    image.InitAlpha(); // Create alpha channel

    unsigned char *alphaData = image.GetAlpha();

    for (size_t i = 0; i < image_alpha.size(); i++)
    {
        alphaData[i] = image_alpha[i];
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

    for (size_t i = 0; i < image_data.size(); i++)
    {
        image_data[i] = rawData[i];
    }

    unsigned char *alphaData = image.GetAlpha();
    image_alpha.resize(width * height);

    if (alphaData != NULL)
    {
        for (size_t i = 0; i < image_alpha.size(); i++)
        {
            image_alpha[i] = alphaData[i];
        }
    }
    else
    {
        for (size_t i = 0; i < image_alpha.size(); i++)
        {
            image_alpha[i] = 255;
        }
    }
}

MapArtPreviewData::MapArtPreviewData(std::vector<const minecraft::FinalColor *> colors, std::vector<bool> transparency, int width, int height, bool preserveTransparency)
{
    this->width = width;
    this->height = height;

    this->preserveTransparency = preserveTransparency;

    this->colors.resize(colors.size());

    for (size_t i = 0; i < colors.size(); i++)
    {
        this->colors[i] = colors[i]->color;
    }

    this->transparency.resize(transparency.size());

    for (size_t i = 0; i < transparency.size(); i++)
    {
        this->transparency[i] = transparency[i];
    }
}

MapArtPreviewData::MapArtPreviewData()
{
    this->colors.resize(0);
    this->transparency.resize(0);
    this->width = 0;
    this->height = 0;
    this->preserveTransparency = false;
}
