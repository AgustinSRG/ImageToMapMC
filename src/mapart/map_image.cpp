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

#include "map_image.h"

using namespace std;
using namespace colors;
using namespace mapart;

std::vector<colors::Color> mapart::loadColorMatrixFromImageAndPad(wxImage &image, int *padWidth, int *padHeight)
{
    int width = image.GetSize().GetWidth();
    int height = image.GetSize().GetHeight();

    int finalWidth = width + ((width % MAP_WIDTH > 0) ? (MAP_WIDTH - (width % MAP_WIDTH)) : 0);
    int finalHeight = height + ((height % MAP_HEIGHT > 0) ? (MAP_HEIGHT - (height % MAP_HEIGHT)) : 0 );
    size_t size = finalWidth * finalHeight;

    int imagePosX = (finalWidth - width) / 2;
    int imagePosZ = (finalHeight - height) / 2;

    vector<Color> result(size);

    // Init colors to white
    for (size_t i = 0; i < size; i++)
    {
        result[i].red = 255;
        result[i].green = 255;
        result[i].blue = 255;
    }

    // Iterate
    unsigned char *rawData = image.GetData();
    for (int z = 0; z < height; z++)
    {
        for (int x = 0; x < width; x++)
        {
            int indexImage = (z * width + x) * 3;
            int indexFinal = (imagePosZ + z) * finalWidth + (imagePosX + x);

            result[indexFinal].red = rawData[indexImage];
            result[indexFinal].green = rawData[indexImage + 1];
            result[indexFinal].blue = rawData[indexImage + 2];
        }
    }

    *padWidth = finalWidth;
    *padHeight = finalHeight;

    return result;
}
