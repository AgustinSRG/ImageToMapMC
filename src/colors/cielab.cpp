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

#include "cielab.h"
#include <cmath>

using namespace cielab;
using namespace colors;

double xyzParseRGBComponent(char c);
double labParseXYZComponent(double v);

/* Implementation code */

double xyzParseRGBComponent(unsigned char c) {
    double value = static_cast<double>(c) / 255;

    if (value > 0.04045) {
        value = pow(((value + 0.055) / 1.055), 2.4);
    } else {
        value = value / 12.92;
    }

    return value * 100;
}

void cielab::rgbToXYZ(colors::Color color, XYZ * xyz) {
    double r = xyzParseRGBComponent(color.red);
    double g = xyzParseRGBComponent(color.green);
    double b = xyzParseRGBComponent(color.blue);

    xyz->x = (r * 0.4124 + g * 0.3576 + b * 0.1805) / 95.047;
    xyz->y = (r * 0.2126 + g * 0.7152 + b * 0.0722) / 100.0;
    xyz->z  = (r * 0.0193 + g * 0.1192 + b * 0.9505) / 108.883;
}

double labParseXYZComponent(double value) {
    if (value > 0.008856) {
        return pow(value, 0.3333333333333333);
    } else {
        return (7.787 * value) + (0.13793103448275862);
    }
}

void cielab::xyzToLab(const XYZ * xyz, Lab * lab) {
    double x = labParseXYZComponent(xyz->x);
    double y = labParseXYZComponent(xyz->y);
    double z = labParseXYZComponent(xyz->z);

    lab->L = (y * 116) - 16;
    lab->a = 500 * (x - y);
    lab->b = 200 * (y - z);
}

void cielab::rgbToLab(colors::Color color, Lab * lab) {
    XYZ xyz;
    rgbToXYZ(color, &xyz);
    xyzToLab(&xyz, lab);
}

double cielab::deltaE(colors::Color colorA, colors::Color colorB) {
    Lab a;
    Lab b;

    rgbToLab(colorA, &a);
    rgbToLab(colorB, &b);

    return deltaE(&a, &b);
}

double cielab::deltaE(const Lab * colorA, const Lab * colorB) {
    double L = colorA->L - colorB->L;
    double a = colorA->a - colorB->a;
    double b = colorA->b - colorB->b;

    return (L * L) + (a * a) + (b * b);
}

