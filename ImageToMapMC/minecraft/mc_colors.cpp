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
#include <cmath>

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

    baseColors[(short)McColors::VOID] = rgb(0, 0, 0);

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

size_t minecraft::findClosestColor(std::vector<minecraft::FinalColor> &colors, colors::Color color, colors::ColorDistanceAlgorithm algo)
{
    double distance;
    size_t size = colors.size();
    size_t result = 0;

    // Start with i = 4 to skip all VOID blocks
    for (size_t i = 4; i < size; i++)
    {
        if (!colors[i].enabled) {
            // If disabled, skip
            continue;
        }
        double d = colorDistance(colors[i].color, color, algo);
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

void minecraft::initializeEnabledColors(std::vector<minecraft::FinalColor> &colors, bool blacklist) {
    size_t size = colors.size();

    for (size_t i = 0; i < size; i++) {
        colors[i].enabled = blacklist;
    }
}

void minecraft::setBaseColorEnabled(std::vector<minecraft::FinalColor> &colors, short color, bool enabled) {
    size_t start = color * 4;
    size_t end = start + 4;
    size_t size = colors.size();

    for (size_t i = start; i < end; i++) {
        if (i >= 0 && i < size) {
            colors[i].enabled = enabled;
        }
    }
}
