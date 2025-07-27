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

ImageColorMatrix mapart::loadColorMatrixFromImageAndPad(wxImage &image, colors::Color background, int *padWidth, int *padHeight)
{
    int width = image.GetSize().GetWidth();
    int height = image.GetSize().GetHeight();

    int finalWidth = width + ((width % MAP_WIDTH > 0) ? (MAP_WIDTH - (width % MAP_WIDTH)) : 0);
    int finalHeight = height + ((height % MAP_HEIGHT > 0) ? (MAP_HEIGHT - (height % MAP_HEIGHT)) : 0 );
    size_t size = finalWidth * finalHeight;

    int imagePosX = (finalWidth - width) / 2;
    int imagePosZ = (finalHeight - height) / 2;

    vector<Color> resultColors(size);
    vector<bool> resultTransparency(size);

    // Init colors to white
    for (size_t i = 0; i < size; i++)
    {
        resultColors[i].red = background.red;
        resultColors[i].green = background.green;
        resultColors[i].blue = background.blue;

        resultTransparency[i] = true;
    }

    // Iterate
    unsigned char *rawData = image.GetData();
    unsigned char *alphaData = image.GetAlpha();
    for (int z = 0; z < height; z++)
    {
        for (int x = 0; x < width; x++)
        {
            int indexImage = (z * width + x) * 3;
            int indexFinal = (imagePosZ + z) * finalWidth + (imagePosX + x);

            resultColors[indexFinal].red = rawData[indexImage];
            resultColors[indexFinal].green = rawData[indexImage + 1];
            resultColors[indexFinal].blue = rawData[indexImage + 2];

            if (alphaData != NULL) {
                resultColors[indexFinal] = colors::bendColor(resultColors[indexFinal], alphaData[z * width + x], background);
                resultTransparency[indexFinal] = alphaData[z * width + x] < 128;
            } else {
                resultTransparency[indexFinal] = false;
            }
        }
    }

    *padWidth = finalWidth;
    *padHeight = finalHeight;

    return ImageColorMatrix{
        resultColors,
        resultTransparency,
    };
}
