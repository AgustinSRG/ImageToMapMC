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

#include "colors.h"
#include "cielab.h"

#include <cmath>

using namespace std;
using namespace colors;

unsigned char parseHexChar(char c);

/* Implementation code */

unsigned char parseHexChar(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        return 10 + (c - 'A');
    }
    else if (c >= 'a' && c <= 'f')
    {
        return 10 + (c - 'a');
    }
    else
    {
        throw -1;
    }
}

char toHexChar(unsigned char val)
{
    if (val < 10)
    {
        return '0' + val;
    }
    else if (val < 16)
    {
        return 'A' + (val - 10);
    }
    else
    {
        return '?';
    }
}

string colors::colorToHex(Color color)
{
    char hex[8];
    unsigned char B;
    unsigned char b;

    hex[0] = '#';

    // Red
    B = (color.red >> 4) & 0xF;
    b = color.red & 0xF;
    hex[1] = toHexChar(B);
    hex[2] = toHexChar(b);

    // Green
    B = (color.green >> 4) & 0xF;
    b = color.green & 0xF;
    hex[3] = toHexChar(B);
    hex[4] = toHexChar(b);

    // Blue
    B = (color.blue >> 4) & 0xF;
    b = color.blue & 0xF;
    hex[5] = toHexChar(B);
    hex[6] = toHexChar(b);

    hex[7] = '\0';

    return string(hex);
}

Color colors::colorFromHex(string hexStr)
{
    if (hexStr.size() < 7 || hexStr[0] != '#')
    {
        throw -1;
    }

    Color color;

    // Red
    color.red = (parseHexChar(hexStr[1]) << 4) + parseHexChar(hexStr[2]);

    // Green
    color.green = (parseHexChar(hexStr[3]) << 4) + parseHexChar(hexStr[4]);

    // Blue
    color.blue = (parseHexChar(hexStr[5]) << 4) + parseHexChar(hexStr[6]);

    return color;
}

double colors::colorDistance(Color colorA, Color colorB, ColorDistanceAlgorithm algo)
{
    switch (algo)
    {
    case ColorDistanceAlgorithm::DeltaE:
    {
        return cielab::deltaE(colorA, colorB);
    }
    case ColorDistanceAlgorithm::Euclidean:
    default:
    {
        double r = colorA.red - colorB.red;
        double g = colorA.green - colorB.green;
        double b = colorA.blue - colorB.blue;
        return (r * r) + (g * g) + (b * b);
    }
    }
}