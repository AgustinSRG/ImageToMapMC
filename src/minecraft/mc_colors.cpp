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

#define COLOR_DARKESS_NORMAL (220)
#define COLOR_DARKESS_DARK (180)
#define COLOR_DARKESS_DARKER (135)

#include "mc_colors.h"
#include "../colors/cielab.h"
#include <cmath>
#include <algorithm>

using namespace colors;
using namespace minecraft;

unsigned char applyColorDarkness(unsigned char component, unsigned char mult);
Color rgb(unsigned char r, unsigned char g, unsigned char b);

// Implementation

unsigned char applyColorDarkness(unsigned char component, unsigned char mult)
{
    return static_cast<unsigned char>(round((static_cast<double>(component) * static_cast<double>(mult)) / 255));
}

Color rgb(unsigned char r, unsigned char g, unsigned char b)
{
    Color c;
    c.red = r;
    c.green = g;
    c.blue = b;
    return c;
}

Color minecraft::getMinecraftColor(std::vector<Color> &baseColors, size_t index, McColorType colorType)
{
    Color color = baseColors[index];
    switch (colorType)
    {
    case McColorType::NORMAL:
        color.red = applyColorDarkness(color.red, COLOR_DARKESS_NORMAL);
        color.green = applyColorDarkness(color.green, COLOR_DARKESS_NORMAL);
        color.blue = applyColorDarkness(color.blue, COLOR_DARKESS_NORMAL);
        break;
    case McColorType::DARK:
        color.red = applyColorDarkness(color.red, COLOR_DARKESS_DARK);
        color.green = applyColorDarkness(color.green, COLOR_DARKESS_DARK);
        color.blue = applyColorDarkness(color.blue, COLOR_DARKESS_DARK);
        break;
    case McColorType::DARKER:
        color.red = applyColorDarkness(color.red, COLOR_DARKESS_DARKER);
        color.green = applyColorDarkness(color.green, COLOR_DARKESS_DARKER);
        color.blue = applyColorDarkness(color.blue, COLOR_DARKESS_DARKER);
        break;
    }
    return color;
}

std::vector<Color> minecraft::loadBaseColors(McVersion version)
{
    std::vector<Color> baseColors(52);

    baseColors[(short)McColors::NONE] = rgb(0, 0, 0);

    baseColors[(short)McColors::GRASS] = rgb(127, 178, 56);
    baseColors[(short)McColors::SAND] = rgb(247, 233, 163);
    baseColors[(short)McColors::WOOL] = rgb(199, 199, 199);
    baseColors[(short)McColors::FIRE] = rgb(255, 0, 0);
    baseColors[(short)McColors::ICE] = rgb(160, 160, 255);
    baseColors[(short)McColors::METAL] = rgb(167, 167, 167);
    baseColors[(short)McColors::PLANT] = rgb(0, 124, 0);
    baseColors[(short)McColors::SNOW] = rgb(255, 255, 255);
    baseColors[(short)McColors::CLAY] = rgb(164, 168, 184);
    baseColors[(short)McColors::DIRT] = rgb(151, 109, 77);
    baseColors[(short)McColors::STONE] = rgb(112, 112, 112);
    baseColors[(short)McColors::WATER] = rgb(64, 64, 255);
    baseColors[(short)McColors::WOOD] = rgb(143, 119, 72);
    baseColors[(short)McColors::QUARTZ] = rgb(255, 252, 245);

    baseColors[(short)McColors::COLOR_ORANGE] = rgb(216, 127, 51);
    baseColors[(short)McColors::COLOR_MAGENTA] = rgb(178, 76, 216);
    baseColors[(short)McColors::COLOR_LIGHT_BLUE] = rgb(102, 153, 216);
    baseColors[(short)McColors::COLOR_YELLOW] = rgb(229, 229, 51);
    baseColors[(short)McColors::COLOR_LIGHT_GREEN] = rgb(127, 204, 25);
    baseColors[(short)McColors::COLOR_PINK] = rgb(242, 127, 165);
    baseColors[(short)McColors::COLOR_GRAY] = rgb(76, 76, 76);
    baseColors[(short)McColors::COLOR_LIGHT_GRAY] = rgb(153, 153, 153);
    baseColors[(short)McColors::COLOR_CYAN] = rgb(76, 127, 153);
    baseColors[(short)McColors::COLOR_PURPLE] = rgb(127, 63, 178);
    baseColors[(short)McColors::COLOR_BLUE] = rgb(51, 76, 178);
    baseColors[(short)McColors::COLOR_BROWN] = rgb(102, 76, 51);
    baseColors[(short)McColors::COLOR_GREEN] = rgb(102, 127, 51);
    baseColors[(short)McColors::COLOR_RED] = rgb(153, 51, 51);
    baseColors[(short)McColors::COLOR_BLACK] = rgb(25, 25, 25);

    baseColors[(short)McColors::GOLD] = rgb(250, 238, 77);
    baseColors[(short)McColors::DIAMOND] = rgb(92, 219, 213);
    baseColors[(short)McColors::LAPIS] = rgb(74, 128, 255);
    baseColors[(short)McColors::EMERALD] = rgb(0, 217, 58);

    baseColors[(short)McColors::PODZOL] = rgb(129, 86, 49);
    baseColors[(short)McColors::NETHER] = rgb(112, 2, 0);

    baseColors[(short)McColors::TERRACOTTA_WHITE] = rgb(209, 177, 161);
    baseColors[(short)McColors::TERRACOTTA_ORANGE] = rgb(159, 82, 36);
    baseColors[(short)McColors::TERRACOTTA_MAGENTA] = rgb(149, 87, 108);
    baseColors[(short)McColors::TERRACOTTA_LIGHT_BLUE] = rgb(112, 108, 138);
    baseColors[(short)McColors::TERRACOTTA_YELLOW] = rgb(186, 133, 36);
    baseColors[(short)McColors::TERRACOTTA_LIGHT_GREEN] = rgb(103, 117, 53);
    baseColors[(short)McColors::TERRACOTTA_PINK] = rgb(160, 77, 78);
    baseColors[(short)McColors::TERRACOTTA_GRAY] = rgb(57, 41, 35);
    baseColors[(short)McColors::TERRACOTTA_LIGHT_GRAY] = rgb(135, 107, 98);
    baseColors[(short)McColors::TERRACOTTA_CYAN] = rgb(87, 92, 92);
    baseColors[(short)McColors::TERRACOTTA_PURPLE] = rgb(122, 73, 88);
    baseColors[(short)McColors::TERRACOTTA_BLUE] = rgb(76, 62, 92);
    baseColors[(short)McColors::TERRACOTTA_BROWN] = rgb(76, 50, 35);
    baseColors[(short)McColors::TERRACOTTA_GREEN] = rgb(76, 82, 42);
    baseColors[(short)McColors::TERRACOTTA_RED] = rgb(142, 60, 46);
    baseColors[(short)McColors::TERRACOTTA_BLACK] = rgb(37, 22, 16);

    if (version >= McVersion::MC_1_16)
    {
        baseColors.resize(59);

        baseColors[(short)McColors::CRIMSON_NYLIUM] = rgb(189, 48, 49);
        baseColors[(short)McColors::CRIMSON_STEM] = rgb(148, 63, 97);
        baseColors[(short)McColors::CRIMSON_HYPHAE] = rgb(92, 25, 29);
        baseColors[(short)McColors::WARPED_NYLIUM] = rgb(22, 126, 134);
        baseColors[(short)McColors::WARPED_STEM] = rgb(58, 142, 140);
        baseColors[(short)McColors::WARPED_HYPHAE] = rgb(86, 44, 62);
        baseColors[(short)McColors::WARPED_WART_BLOCK] = rgb(20, 180, 133);

        if (version >= McVersion::MC_1_17)
        {
            baseColors.resize(62);

            baseColors[(short)McColors::DEEPSLATE] = rgb(100, 100, 100);
            baseColors[(short)McColors::RAW_IRON] = rgb(216, 175, 147);
            baseColors[(short)McColors::GLOW_LICHEN] = rgb(127, 167, 150);
        }
    }

    return baseColors;
}

void FinalColor::setColor(std::vector<Color> &baseColors, size_t index, McColorType colorType)
{
    this->baseColorIndex = index;
    this->colorType = colorType;
    this->enabled = true;
    this->color = getMinecraftColor(baseColors, index, colorType);
    cielab::rgbToLab(this->color, &lab);
}

short FinalColor::getMapColor() const
{
    switch (colorType)
    {
    case McColorType::DARK:
        return baseColorIndex * 4;
    case McColorType::LIGHT:
        return baseColorIndex * 4 + 2;
    case McColorType::DARKER:
        return baseColorIndex * 4 + 3;
    default:
        return baseColorIndex * 4 + 1;
    }
}

std::vector<std::string> minecraft::loadBaseColorNames(std::vector<colors::Color> &baseColors)
{
    size_t size = baseColors.size();
    std::vector<std::string> res(size);

    res[(size_t)McColors::NONE] = std::string("NONE");
    res[(size_t)McColors::GRASS] = std::string("GRASS");
    res[(size_t)McColors::SAND] = std::string("SAND");
    res[(size_t)McColors::WOOL] = std::string("WOOL");
    res[(size_t)McColors::FIRE] = std::string("FIRE");
    res[(size_t)McColors::ICE] = std::string("ICE");
    res[(size_t)McColors::METAL] = std::string("METAL");
    res[(size_t)McColors::PLANT] = std::string("PLANT");
    res[(size_t)McColors::SNOW] = std::string("SNOW");
    res[(size_t)McColors::CLAY] = std::string("CLAY");
    res[(size_t)McColors::DIRT] = std::string("DIRT");
    res[(size_t)McColors::STONE] = std::string("STONE");
    res[(size_t)McColors::WATER] = std::string("WATER");
    res[(size_t)McColors::WOOD] = std::string("WOOD");
    res[(size_t)McColors::QUARTZ] = std::string("QUARTZ");

    res[(size_t)McColors::COLOR_ORANGE] = std::string("COLOR_ORANGE");
    res[(size_t)McColors::COLOR_MAGENTA] = std::string("COLOR_MAGENTA");
    res[(size_t)McColors::COLOR_LIGHT_BLUE] = std::string("COLOR_LIGHT_BLUE");
    res[(size_t)McColors::COLOR_YELLOW] = std::string("COLOR_YELLOW");
    res[(size_t)McColors::COLOR_LIGHT_GREEN] = std::string("COLOR_LIGHT_GREEN");
    res[(size_t)McColors::COLOR_PINK] = std::string("COLOR_PINK");
    res[(size_t)McColors::COLOR_GRAY] = std::string("COLOR_GRAY");
    res[(size_t)McColors::COLOR_LIGHT_GRAY] = std::string("COLOR_LIGHT_GRAY");
    res[(size_t)McColors::COLOR_CYAN] = std::string("COLOR_CYAN");
    res[(size_t)McColors::COLOR_PURPLE] = std::string("COLOR_PURPLE");
    res[(size_t)McColors::COLOR_BLUE] = std::string("COLOR_BLUE");
    res[(size_t)McColors::COLOR_BROWN] = std::string("COLOR_BROWN");
    res[(size_t)McColors::COLOR_GREEN] = std::string("COLOR_GREEN");
    res[(size_t)McColors::COLOR_RED] = std::string("COLOR_RED");
    res[(size_t)McColors::COLOR_BLACK] = std::string("COLOR_BLACK");

    res[(size_t)McColors::GOLD] = std::string("GOLD");
    res[(size_t)McColors::DIAMOND] = std::string("DIAMOND");
    res[(size_t)McColors::LAPIS] = std::string("LAPIS");
    res[(size_t)McColors::EMERALD] = std::string("EMERALD");

    res[(size_t)McColors::PODZOL] = std::string("PODZOL");
    res[(size_t)McColors::NETHER] = std::string("NETHER");

    res[(size_t)McColors::TERRACOTTA_WHITE] = std::string("TERRACOTTA_WHITE");
    res[(size_t)McColors::TERRACOTTA_ORANGE] = std::string("TERRACOTTA_ORANGE");
    res[(size_t)McColors::TERRACOTTA_MAGENTA] = std::string("TERRACOTTA_MAGENTA");
    res[(size_t)McColors::TERRACOTTA_LIGHT_BLUE] = std::string("TERRACOTTA_LIGHT_BLUE");
    res[(size_t)McColors::TERRACOTTA_YELLOW] = std::string("TERRACOTTA_YELLOW");
    res[(size_t)McColors::TERRACOTTA_LIGHT_GREEN] = std::string("TERRACOTTA_LIGHT_GREEN");
    res[(size_t)McColors::TERRACOTTA_PINK] = std::string("TERRACOTTA_PINK");
    res[(size_t)McColors::TERRACOTTA_GRAY] = std::string("TERRACOTTA_GRAY");
    res[(size_t)McColors::TERRACOTTA_LIGHT_GRAY] = std::string("TERRACOTTA_LIGHT_GRAY");
    res[(size_t)McColors::TERRACOTTA_CYAN] = std::string("TERRACOTTA_CYAN");
    res[(size_t)McColors::TERRACOTTA_PURPLE] = std::string("TERRACOTTA_PURPLE");
    res[(size_t)McColors::TERRACOTTA_BLUE] = std::string("TERRACOTTA_BLUE");
    res[(size_t)McColors::TERRACOTTA_BROWN] = std::string("TERRACOTTA_BROWN");
    res[(size_t)McColors::TERRACOTTA_GREEN] = std::string("TERRACOTTA_GREEN");
    res[(size_t)McColors::TERRACOTTA_RED] = std::string("TERRACOTTA_RED");
    res[(size_t)McColors::TERRACOTTA_BLACK] = std::string("TERRACOTTA_BLACK");

    if (size > (size_t)McColors::CRIMSON_NYLIUM)
    {
        // 1.16 +
        res[(size_t)McColors::CRIMSON_NYLIUM] = std::string("CRIMSON_NYLIUM");
        res[(size_t)McColors::CRIMSON_STEM] = std::string("CRIMSON_STEM");
        res[(size_t)McColors::CRIMSON_HYPHAE] = std::string("CRIMSON_HYPHAE");
        res[(size_t)McColors::WARPED_NYLIUM] = std::string("WARPED_NYLIUM");
        res[(size_t)McColors::WARPED_STEM] = std::string("WARPED_STEM");
        res[(size_t)McColors::WARPED_HYPHAE] = std::string("WARPED_HYPHAE");
        res[(size_t)McColors::WARPED_WART_BLOCK] = std::string("WARPED_WART_BLOCK");

        if (size > (size_t)McColors::DEEPSLATE)
        {
            res[(size_t)McColors::DEEPSLATE] = std::string("DEEPSLATE");
            res[(size_t)McColors::RAW_IRON] = std::string("RAW_IRON");
            res[(size_t)McColors::GLOW_LICHEN] = std::string("GLOW_LICHEN");
        }
    }

    return res;
}

size_t minecraft::findColorByName(const std::vector<std::string> &colorNames, std::string name)
{
    size_t size = colorNames.size();
    std::string nameUpper(name);
    std::transform(nameUpper.begin(), nameUpper.end(), nameUpper.begin(), ::toupper);

    for (size_t i = 0; i < size; i++)
    {
        if (nameUpper.compare(colorNames[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}

std::vector<FinalColor> minecraft::loadFinalColors(std::vector<Color> &baseColors)
{
    size_t size = baseColors.size();
    std::vector<FinalColor> colors(size * 4);

    for (size_t i = 0; i < size; i++)
    {
        colors[i * 4].setColor(baseColors, i, McColorType::DARK);
        colors[i * 4 + 1].setColor(baseColors, i, McColorType::NORMAL);
        colors[i * 4 + 2].setColor(baseColors, i, McColorType::LIGHT);
        colors[i * 4 + 3].setColor(baseColors, i, McColorType::DARKER);
    }

    return colors;
}

size_t minecraft::findClosestColor(const std::vector<minecraft::FinalColor> &colors, colors::Color color, colors::ColorDistanceAlgorithm algo)
{
    double distance;
    size_t size = colors.size();
    size_t result = 0;

    colors::Lab colorALab;

    if (algo == ColorDistanceAlgorithm::DeltaE) {
        cielab::rgbToLab(color, &colorALab);
    }

    // Start with i = 4 to skip all NONE blocks
    for (size_t i = 4; i < size; i++)
    {
        if (!colors[i].enabled)
        {
            // If disabled, skip
            continue;
        }
        double d = (algo == ColorDistanceAlgorithm::DeltaE) ? colorDistance(&colorALab, &(colors[i].lab)) : colorDistance(colors[i].color, color);
        if (result > 1)
        {
            if (distance > d)
            {
                distance = d;
                result = i;
            }
        }
        else
        {
            distance = d;
            result = i;
        }
    }

    return result;
}

std::vector<size_t> minecraft::find2ClosestColors(const std::vector<minecraft::FinalColor> &colors, colors::Color color, colors::ColorDistanceAlgorithm algo, double *distFirst, double *distSecond)
{
    size_t size = colors.size();
    size_t res1 = 0;
    size_t res2 = 0;
    double distance1;
    double distance2;

    colors::Lab colorALab;

    if (algo == ColorDistanceAlgorithm::DeltaE) {
        cielab::rgbToLab(color, &colorALab);
    }

    // Start with i = 4 to skip all NONE blocks
    for (size_t i = 4; i < size; i++)
    {
        if (!colors[i].enabled)
        {
            // If disabled, skip
            continue;
        }
        double d = (algo == ColorDistanceAlgorithm::DeltaE) ? colorDistance(&colorALab, &(colors[i].lab)) : colorDistance(colors[i].color, color);
        if (res1 > 1)
        {
            if (distance1 > d)
            {
                distance1 = d;
                res1 = i;
            }
        }
        else
        {
            distance1 = d;
            res1 = i;
        }

        if (i != res1)
        {
            if (res2 > 0)
            {
                if (distance2 > d)
                {
                    distance2 = d;
                    res2 = i;
                }
            }
            else
            {
                distance2 = d;
                res2 = i;
            }
        }
    }

    if (res2 == 0)
    {
        res2 = res1;
        distance2 = distance1;
    }

    *distFirst = distance1;
    *distSecond = distance2;

    std::vector<size_t> v(2);

    v[0] = res1;
    v[1] = res2;

    return v;
}

void minecraft::initializeEnabledColors(std::vector<minecraft::FinalColor> &colors, bool blacklist)
{
    size_t size = colors.size();

    for (size_t i = 0; i < size; i++)
    {
        colors[i].enabled = blacklist;
    }
}

void minecraft::setColorTypesEnabled(std::vector<minecraft::FinalColor> &colors, McColorType colorType, bool enabled)
{
    size_t size = colors.size();

    for (size_t i = 0; i < size; i++)
    {
        if (colors[i].colorType == colorType)
        {
            colors[i].enabled = enabled;
        }
    }
}

void minecraft::setBaseColorEnabled(std::vector<minecraft::FinalColor> &colors, short color, bool enabled)
{
    size_t start = color * 4;
    size_t end = start + 4;
    size_t size = colors.size();

    for (size_t i = start; i < end; i++)
    {
        if (i >= 0 && i < size)
        {
            colors[i].enabled = enabled;
        }
    }
}
