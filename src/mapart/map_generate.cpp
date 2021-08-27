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

#include "map_generate.h"
#include "dithering.h"
#include <algorithm>

using namespace std;
using namespace colors;
using namespace mapart;

inline Color applyErrorSimple(Color color, ColorError quantError, double weight)
{
    Color result;
    result.red = static_cast<unsigned char>(
        min(
            max(
                (double)color.red + (weight * quantError.red),
                (double)0),
            (double)255));
    result.green = static_cast<unsigned char>(
        min(
            max(
                (double)color.green + (weight * quantError.green),
                (double)0),
            (double)255));

    result.blue = static_cast<unsigned char>(
        min(
            max(
                (double)color.blue + (weight * quantError.blue),
                (double)0),
            (double)255));

    return result;
}

void applyErrorDiffussion(std::vector<colors::Color> &colorMatrix, size_t width, size_t height, Color oldPixel, Color newPixel, size_t x, size_t z, double matrix[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W], double divisor)
{
    ColorError quantError;
    quantError.red = (short)oldPixel.red - (short)newPixel.red;
    quantError.green = (short)oldPixel.green - (short)newPixel.green;
    quantError.blue = (short)oldPixel.blue - (short)newPixel.blue;

    double weight;
    size_t index;

    // 1 right
    if ((x + 1) < width)
    {
        index = (z)*width + (x + 1);
        weight = matrix[0][3] / divisor;
        colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);

        // 2 right
        if ((x + 2) < width)
        {
            index = (z)*width + (x + 2);
            weight = matrix[0][4] / divisor;
            colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);
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
            colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);
        }

        // 1 down, 2 left
        if (x > 1)
        {
            index = (z + 1) * width + (x - 2);
            weight = matrix[1][0] / divisor;
            colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);
        }

        // 1 down
        index = (z + 1) * width + (x);
        weight = matrix[1][2] / divisor;
        colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);

        // 1 down, 1 right
        if ((x + 1) < width)
        {
            index = (z + 1) * width + (x + 1);
            weight = matrix[1][3] / divisor;
            colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);
        }

        // 1 down, 2 right
        if ((x + 1) < width)
        {
            index = (z + 1) * width + (x + 2);
            weight = matrix[1][4] / divisor;
            colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);
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
            colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);
        }

        // 2 down, 2 left
        if (x > 1)
        {
            index = (z + 2) * width + (x - 2);
            weight = matrix[2][0] / divisor;
            colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);
        }

        // 2 down
        index = (z + 2) * width + (x);
        weight = matrix[2][2] / divisor;
        colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);

        // 2 down, 1 right
        if ((x + 1) < width)
        {
            index = (z + 2) * width + (x + 1);
            weight = matrix[2][3] / divisor;
            colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);
        }

        // 2 down, 2 right
        if ((x + 1) < width)
        {
            index = (z + 2) * width + (x + 2);
            weight = matrix[2][4] / divisor;
            colorMatrix[index] = applyErrorSimple(colorMatrix[index], quantError, weight);
        }
    }
}

void threadGenerateMapFunc(int id, size_t fromZ, size_t toZ, std::vector<const minecraft::FinalColor *> &result, const std::vector<minecraft::FinalColor> &colorSet, std::vector<colors::Color> &matrix, size_t width, size_t height, colors::ColorDistanceAlgorithm colorDistanceAlgo, mapart::DitheringMethod ditheringMethod, threading::Progress &progress)
{
    size_t closest;
    vector<size_t> closest2;
    double d1;
    double d2;

    // Compute data
    for (size_t z = fromZ; z < toZ; z++)
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
                applyErrorDiffussion(matrix, width, height, matrix[index], colorSet[closest].color, x, z, FLOYD_STEINBERG_MATRIX, FLOYD_STEINBERG_DIVISOR);
                break;
            case DitheringMethod::MinAvgErr:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, matrix[index], colorSet[closest].color, x, z, MINAVGERR_MATRIX, MINAVGERR_DIVISOR);
                break;
            case DitheringMethod::Burkes:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, matrix[index], colorSet[closest].color, x, z, BURKES_MATRIX, BURKES_DIVISOR);
                break;
            case DitheringMethod::SierraLite:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, matrix[index], colorSet[closest].color, x, z, SIERRA_LITE_MATRIX, SIERRA_LITE_DIVISOR);
                break;
            case DitheringMethod::Stucki:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, matrix[index], colorSet[closest].color, x, z, STUCKI_MATRIX, STUCKI_DIVISOR);
                break;
            case DitheringMethod::Atkinson:
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
                applyErrorDiffussion(matrix, width, height, matrix[index], colorSet[closest].color, x, z, ATKINSON_MATRIX, ATKINSON_DIVISOR);
                break;
            default:
                // None (No dithering)
                closest = findClosestColor(colorSet, matrix[index], colorDistanceAlgo);
                result[index] = &(colorSet[closest]);
            }
        }
        try {
            progress.setProgress(static_cast<unsigned int>(id), static_cast<unsigned int>(z - fromZ + 1));
        } catch (int e) {
            return;
        }
    }
}

std::vector<const minecraft::FinalColor *> mapart::generateMapArt(const std::vector<minecraft::FinalColor> &colorSet, const std::vector<colors::Color> &colorMatrix, size_t width, size_t height, colors::ColorDistanceAlgorithm colorDistanceAlgo, mapart::DitheringMethod ditheringMethod, size_t threadNum, threading::Progress &progress)
{
    std::vector<colors::Color> matrix(colorMatrix); // Make a copy of colorMatrix to work with
    std::vector<const minecraft::FinalColor *> result(width * height);

    switch (ditheringMethod)
    {
    // These dithering methods won't support muti-threading due to race conditions
    case DitheringMethod::FloydSteinberg:
    case DitheringMethod::MinAvgErr:
    case DitheringMethod::Burkes:
    case DitheringMethod::SierraLite:
    case DitheringMethod::Stucki:
    case DitheringMethod::Atkinson:
        threadNum = 1;
        break;
    }

    std::vector<std::thread> threads(threadNum);

    size_t amountPerThread = height / threadNum;

    // Create threads
    for (int i = 0; i < threadNum; i++)
    {
        size_t startZ = i * amountPerThread;
        size_t endZ = startZ + amountPerThread;

        if (i == threadNum - 1)
        {
            // Last thread, get the rest
            endZ = height;
        }
        threads[i] = std::thread(threadGenerateMapFunc, i, startZ, endZ, std::ref(result), std::ref(colorSet), std::ref(matrix), width, height, colorDistanceAlgo, ditheringMethod, std::ref(progress));
    }

    // Wait for the threads
    for (int i = 0; i < threadNum; i++)
    {
        threads[i].join();
    }

    if (progress.isTerminated()) {
        throw -1;
    }

    return result;
}
