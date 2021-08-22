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
#include <algorithm>

using namespace std;
using namespace nbt;
using namespace colors;
using namespace mapart;
using namespace minecraft;

/* Consts */

#define BAYER_44_MATRIX_H 4
#define BAYER_44_MATRIX_W 4

static double BAYER_44_MATRIX[BAYER_44_MATRIX_H][BAYER_44_MATRIX_W] = {
    1, 9, 3, 11,
    13, 5, 15, 7,
    4, 12, 2, 10,
    16, 8, 14, 6};

#define BAYER_22_MATRIX_H 2
#define BAYER_22_MATRIX_W 2

static double BAYER_22_MATRIX[BAYER_22_MATRIX_H][BAYER_22_MATRIX_W] = {
    1, 3,
    4, 2};

#define ORDERED_33_MATRIX_H 3
#define ORDERED_33_MATRIX_W 3

static double ORDERED_33_MATRIX[ORDERED_33_MATRIX_H][ORDERED_33_MATRIX_W] = {
    1, 7, 4,
    5, 8, 3,
    6, 2, 9};

#define ERROR_DIFFUSSION_MATRIX_H 3
#define ERROR_DIFFUSSION_MATRIX_W 5

static double FLOYD_STEINBERG_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 7, 0,
    0, 3, 5, 1, 0,
    0, 0, 0, 0, 0};

static double FLOYD_STEINBERG_DIVISOR = 16;

static double MINAVGERR_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 7, 5,
    3, 5, 7, 5, 3,
    1, 3, 5, 3, 1};

static double MINAVGERR_DIVISOR = 48;

static double BURKES_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 8, 4,
    2, 4, 8, 4, 2,
    0, 0, 0, 0, 0};

static double BURKES_DIVISOR = 32;

static double SIERRA_LITE_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 2, 0,
    0, 1, 1, 0, 0,
    0, 0, 0, 0, 0};

static double SIERRA_LITE_DIVISOR = 4;

static double STUCKI_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 8, 4,
    2, 4, 8, 4, 2,
    1, 2, 4, 2, 1};

static double STUCKI_DIVISOR = 42;

static double ATKINSON_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 1, 1,
    0, 1, 0, 1, 0,
    0, 0, 1, 0, 0};

static double ATKINSON_DIVISOR = 8;

/* Implementation */

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

    if (nameLower.compare(string("floyd-steinberg")))
    {
        return DitheringMethod::FloydSteinberg;
    }
    else if (nameLower.compare(string("min-average-error")))
    {
        return DitheringMethod::MinAvgErr;
    }
    else if (nameLower.compare(string("burkes")))
    {
        return DitheringMethod::Burkes;
    }
    else if (nameLower.compare(string("sierra-lite")))
    {
        return DitheringMethod::SierraLite;
    }
    else if (nameLower.compare(string("stucki")))
    {
        return DitheringMethod::Stucki;
    }
    else if (nameLower.compare(string("atkinson")))
    {
        return DitheringMethod::Atkinson;
    }
    else if (nameLower.compare(string("bayer-44")))
    {
        return DitheringMethod::Bayer44;
    }
    else if (nameLower.compare(string("bayer-22")))
    {
        return DitheringMethod::Bayer22;
    }
    else if (nameLower.compare(string("ordered-33")))
    {
        return DitheringMethod::FloydSteinberg;
    }
    else if (nameLower.compare(string("none")))
    {
        return DitheringMethod::None;
    }
    else
    {
        return DitheringMethod::Unknown;
    }
}

void applyErrorDiffussion(std::vector<colors::Color> &colorMatrix, size_t width, size_t height, Color oldPixel, Color newPixel, size_t x, size_t z, double matrix[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W], double divisor)
{
    Color quantError;
    quantError.red = oldPixel.red - newPixel.red;
    quantError.green = oldPixel.green - newPixel.green;
    quantError.blue = oldPixel.blue - newPixel.blue;

    double weight;
    size_t index;

    // 1 right
    if ((x + 1) < width)
    {
        index = (z)*width + (x + 1);
        weight = matrix[0][3] / divisor;
        colorMatrix[index].red += (weight * quantError.red);
        colorMatrix[index].green += (weight * quantError.green);
        colorMatrix[index].blue += (weight * quantError.blue);

        // 2 right
        if ((x + 2) < width)
        {
            index = (z)*width + (x + 2);
            weight = matrix[0][4] / divisor;
            colorMatrix[index].red += (weight * quantError.red);
            colorMatrix[index].green += (weight * quantError.green);
            colorMatrix[index].blue += (weight * quantError.blue);
        }
    }

    // Row below
    if ((z + 1) < height)
    {
        // 1 down, 1 left
        if (x > 0)
        {
            index = (z + 1) * width + (x - 1);
            weight = matrix[1][1] / divisor;
            colorMatrix[index].red += (weight * quantError.red);
            colorMatrix[index].green += (weight * quantError.green);
            colorMatrix[index].blue += (weight * quantError.blue);
        }

        // 1 down, 2 left
        if (x > 1)
        {
            index = (z + 1) * width + (x - 2);
            weight = matrix[1][0] / divisor;
            colorMatrix[index].red += (weight * quantError.red);
            colorMatrix[index].green += (weight * quantError.green);
            colorMatrix[index].blue += (weight * quantError.blue);
        }

        // 1 down
        index = (z + 1) * width + (x);
        weight = matrix[1][2] / divisor;
        colorMatrix[index].red += (weight * quantError.red);
        colorMatrix[index].green += (weight * quantError.green);
        colorMatrix[index].blue += (weight * quantError.blue);

        // 1 down, 1 right
        if ((x + 1) < width)
        {
            index = (z + 1) * width + (x + 1);
            weight = matrix[1][3] / divisor;
            colorMatrix[index].red += (weight * quantError.red);
            colorMatrix[index].green += (weight * quantError.green);
            colorMatrix[index].blue += (weight * quantError.blue);
        }

        // 1 down, 2 right
        if ((x + 1) < width)
        {
            index = (z + 1) * width + (x + 2);
            weight = matrix[1][4] / divisor;
            colorMatrix[index].red += (weight * quantError.red);
            colorMatrix[index].green += (weight * quantError.green);
            colorMatrix[index].blue += (weight * quantError.blue);
        }
    }

    // 2nd Row below
    if ((z + 2) < height)
    {
        // 2 down, 1 left
        if (x > 0)
        {
            index = (z + 2) * width + (x - 1);
            weight = matrix[2][1] / divisor;
            colorMatrix[index].red += (weight * quantError.red);
            colorMatrix[index].green += (weight * quantError.green);
            colorMatrix[index].blue += (weight * quantError.blue);
        }

        // 1 down, 2 left
        if (x > 1)
        {
            index = (z + 2) * width + (x - 2);
            weight = matrix[2][0] / divisor;
            colorMatrix[index].red += (weight * quantError.red);
            colorMatrix[index].green += (weight * quantError.green);
            colorMatrix[index].blue += (weight * quantError.blue);
        }

        // 1 down
        index = (z + 2) * width + (x);
        weight = matrix[2][2] / divisor;
        colorMatrix[index].red += (weight * quantError.red);
        colorMatrix[index].green += (weight * quantError.green);
        colorMatrix[index].blue += (weight * quantError.blue);

        // 1 down, 1 right
        if ((x + 1) < width)
        {
            index = (z + 2) * width + (x + 1);
            weight = matrix[2][3] / divisor;
            colorMatrix[index].red += (weight * quantError.red);
            colorMatrix[index].green += (weight * quantError.green);
            colorMatrix[index].blue += (weight * quantError.blue);
        }

        // 1 down, 2 right
        if ((x + 1) < width)
        {
            index = (z + 2) * width + (x + 2);
            weight = matrix[2][4] / divisor;
            colorMatrix[index].red += (weight * quantError.red);
            colorMatrix[index].green += (weight * quantError.green);
            colorMatrix[index].blue += (weight * quantError.blue);
        }
    }
}

std::vector<const minecraft::FinalColor *> mapart::generateMapArt(const std::vector<minecraft::FinalColor> &colorSet, const std::vector<colors::Color> &colorMatrix, size_t width, size_t height, colors::ColorDistanceAlgorithm colorDistanceAlgo, mapart::DitheringMethod ditheringMethod)
{
    std::vector<colors::Color> matrix(colorMatrix); // Make a copy of colorMatrix to work with
    std::vector<const minecraft::FinalColor *> result(width * height);

    size_t closest;
    vector<size_t> closest2;
    double d1;
    double d2;

    for (size_t z = 0; z < height; z++)
    {
        for (size_t x = 0; x < width; x++)
        {
            size_t index = z * width + x;

            switch (ditheringMethod)
            {
            case DitheringMethod::Bayer44:
                closest2 = find2ClosestColors(colorSet, matrix[index], colorDistanceAlgo, &d1, &d2);
                if (((d1 * (BAYER_44_MATRIX_H * BAYER_44_MATRIX_W + 1)) / d2) > BAYER_44_MATRIX[x % BAYER_44_MATRIX_H][z % BAYER_44_MATRIX_W])
                {
                    result[index] = &(colorSet[closest2[1]]);
                }
                else
                {
                    result[index] = &(colorSet[closest2[0]]);
                }
                break;
            case DitheringMethod::Bayer22:
                closest2 = find2ClosestColors(colorSet, matrix[index], colorDistanceAlgo, &d1, &d2);
                if (((d1 * (BAYER_22_MATRIX_H * BAYER_22_MATRIX_W + 1)) / d2) > BAYER_22_MATRIX[x % BAYER_22_MATRIX_H][z % BAYER_22_MATRIX_W])
                {
                    result[index] = &(colorSet[closest2[1]]);
                }
                else
                {
                    result[index] = &(colorSet[closest2[0]]);
                }
                break;
            case DitheringMethod::Ordered33:
                closest2 = find2ClosestColors(colorSet, matrix[index], colorDistanceAlgo, &d1, &d2);
                if (((d1 * (ORDERED_33_MATRIX_H * ORDERED_33_MATRIX_W + 1)) / d2) > ORDERED_33_MATRIX[x % ORDERED_33_MATRIX_H][z % ORDERED_33_MATRIX_W])
                {
                    result[index] = &(colorSet[closest2[1]]);
                }
                else
                {
                    result[index] = &(colorSet[closest2[0]]);
                }
                break;
            case DitheringMethod::FloydSteinberg:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, colorMatrix[index], colorSet[closest].color, x, z, FLOYD_STEINBERG_MATRIX, FLOYD_STEINBERG_DIVISOR);
                break;
            case DitheringMethod::MinAvgErr:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, colorMatrix[index], colorSet[closest].color, x, z, MINAVGERR_MATRIX, MINAVGERR_DIVISOR);
                break;
            case DitheringMethod::Burkes:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, colorMatrix[index], colorSet[closest].color, x, z, BURKES_MATRIX, BURKES_DIVISOR);
                break;
            case DitheringMethod::SierraLite:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, colorMatrix[index], colorSet[closest].color, x, z, SIERRA_LITE_MATRIX, SIERRA_LITE_DIVISOR);
                break;
            case DitheringMethod::Stucki:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, colorMatrix[index], colorSet[closest].color, x, z, STUCKI_MATRIX, STUCKI_DIVISOR);
                break;
            case DitheringMethod::Atkinson:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, colorMatrix[index], colorSet[closest].color, x, z, ATKINSON_MATRIX, ATKINSON_DIVISOR);
                break;
            default:
                // None (No dithering)
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
            }
        }
    }

    return result;
}

void mapart::applyBuildRestrictions(std::vector<minecraft::FinalColor> &colorSet, MapBuildMethod method) {
    if (method == MapBuildMethod::None) {
        return;
    }
    
    // Darker colors are unobtainable with blocks, so cannot be built
    setColorTypesEnabled(colorSet, McColorType::DARKER, false);

    if (method == MapBuildMethod::Flat) {
        // If flat, only normal are obtainable
        setColorTypesEnabled(colorSet, McColorType::LIGHT, false);
        setColorTypesEnabled(colorSet, McColorType::DARK, false);
    } else {
        // If 3d, water is a pain
        setBaseColorEnabled(colorSet, (short) McColors::WATER, false);
    }
}
