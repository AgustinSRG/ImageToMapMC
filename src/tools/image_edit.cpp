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

#include "image_edit.h"
#include <algorithm>
#include <cmath>

using namespace std;

#define Pr .299
#define Pg .587
#define Pb .114

inline unsigned char addAndTruncate(unsigned char c, unsigned char b)
{
    if (((int)c + (int)b) > 255)
    {
        return 255;
    }
    else if (((int)c + (int)b) < 0)
    {
        return 0;
    }
    else
    {
        return c + b;
    }
}

inline unsigned char truncateS(short n)
{
    int i = (int)n;

    if (i > 255)
    {
        return 255;
    }
    else if (i < 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned char>(i);
    }
}

inline unsigned char truncate(float n)
{
    int i = (int)n;

    if (i > 255)
    {
        return 255;
    }
    else if (i < 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned char>(i);
    }
}

inline unsigned char truncateD(double n)
{
    int i = (int)n;

    if (i > 255)
    {
        return 255;
    }
    else if (i < 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned char>(i);
    }
}

colors::Color applySaturation(colors::Color c, double change)
{
    colors::Color color;

    double P = sqrt(
        (double)c.red * (double)c.red * Pr +
        (double)c.green * (double)c.green * Pg +
        (double)c.blue * (double)c.blue * Pb);

    color.red = truncateD(P + ((double)c.red - P) * change);
    color.green = truncateD(P + ((double)c.green - P) * change);
    color.blue = truncateD(P + ((double)c.blue - P) * change);

    return color;
}

void tools::editImage(std::vector<colors::Color> &colors, size_t width, size_t height, float saturation, float contrast, float brightness)
{
    short brightFactor = static_cast<short>(((float)128 * (brightness - 1)));

    short contrastRatio = static_cast<short>(((float)128 * (contrast - 1)));
    float contrastFactor = static_cast<float>((259.0 * ((float)contrastRatio + 255.0)) / (255.0 * (259.0 - (float)contrastRatio)));

    for (size_t i = 0; i < colors.size(); i++)
    {

        // Contrast
        if (contrast != 1.0)
        {
            colors[i].red = truncate(contrastFactor * ((float)colors[i].red - 128) + 128);
            colors[i].green = truncate(contrastFactor * ((float)colors[i].green - 128) + 128);
            colors[i].blue = truncate(contrastFactor * ((float)colors[i].blue - 128) + 128);
        }

        // Brightness
        if (brightness != 1.0)
        {
            colors[i].red = truncateS((short)colors[i].red + brightFactor);
            colors[i].green = truncateS((short)colors[i].green + brightFactor);
            colors[i].blue = truncateS((short)colors[i].blue + brightFactor);
        }

        // Saturation
        if (saturation != 1.0) {
            colors[i] = applySaturation(colors[i], saturation);
        }
    }
}
