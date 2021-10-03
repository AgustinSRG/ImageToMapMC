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

#pragma once

#include <string>

/* Color distance algorithms definition */

#define COLOR_DISTANCE_EUCLIDEAN (1)
#define COLOR_DISTANCE_DELTA_E_CIE (2)

/**
 * @brief  Colors namespace
 * @note   All code related to colors goes here
 * @retval None
 */
namespace colors
{

    /**
     * @brief  Color structure (RGB)
     * @note   
     * @retval None
     */
    struct Color
    {
        unsigned char red;
        unsigned char blue;
        unsigned char green;
    };

    /**
     * @brief  Struct to store color error
     * @note   
     * @retval None
     */
    struct ColorError
    {
        short red;
        short blue;
        short green;
    };

     /**
     * @brief  L*ab color
     * @note   
     * @retval None
     */
    struct Lab {
        double L;
        double a;
        double b;
    };

    /**
     * @brief  XYZ color
     * @note   
     * @retval None
     */
    struct XYZ {
        double x;
        double y;
        double z;
    };

    /* HEX */

    /**
     * @brief  Prints color to hex format
     * @note   Format is #RRGGBB
     * @param  color: Color to print
     * @retval None
     */
    std::string colorToHex(Color color);

    /**
     * @brief  Parses a color in hex format
     * @note   
     * @param  hexStr: Color in Hex format (#RRGGBB)
     * @retval The color parsed from the hex
     * @throws -1 If invalid hex
     */
    Color colorFromHex(std::string hexStr);

    /* Distance */

    enum class ColorDistanceAlgorithm {
        Euclidean,
        DeltaE,
    };

    /**
     * @brief  Computes distance between 2 colors
     * @note   
     * @param  colorA: Color 1
     * @param  colorB: Color 2
     * @retval The distance
     */
    double colorDistance(colors::Color colorA, colors::Color colorB);

    /**
     * @brief  Computes distance between 2 colors
     * @note   
     * @param  colorA: Color 1
     * @param  colorB: Color 2
     * @retval The distance
     */
    double colorDistance(const colors::Lab * colorA, const colors::Lab * colorB);

    /**
     * @brief  Bend RGBA color with background
     * @note   
     * @param  color: Color
     * @param  alpha: Alpha value
     * @param  background: Background color
     * @retval Resulting color
     */
    colors::Color bendColor(colors::Color color, unsigned char alpha, colors::Color background);
}
