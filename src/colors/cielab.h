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

#include "colors.h"

/**
 * @brief  CIE L*ab colors utils
 * @note   
 * @retval None
 */
namespace cielab {
    /**
     * @brief  RGB color to XYZ coordinates
     * @note   
     * @param  color: RGB color
     * @param  xyz: Reference to XYZ struct to store the result
     * @retval None
     */
    void rgbToXYZ(colors::Color color, colors::XYZ * xyz);

    /**
     * @brief  XYZ coordinates to L*ab color
     * @note   
     * @param  xyz: Color as XYZ
     * @param  lab: Reference to L*ab struct to store the result
     * @retval None
     */
    void xyzToLab(const colors::XYZ * xyz, colors::Lab * lab);

    /**
     * @brief  RGB color to L*ab
     * @note   
     * @param  color: RGB color
     * @param  lab: Reference to L*ab struct to store the result
     * @retval None
     */
    void rgbToLab(colors::Color color, colors::Lab * lab);

    /**
     * @brief  Delta*E color distance algorithm
     * @note   
     * @param  colorA: Color 1
     * @param  colorB: Color 2
     * @retval Distance
     */
    double deltaE(colors::Color colorA, colors::Color colorB);

    /**
     * @brief  Delta*E color distance algorithm
     * @note   
     * @param  colorA: Color 1
     * @param  colorB: Color 2
     * @retval Distance
     */
    double deltaE(const colors::Lab * colorA, const colors::Lab * colorB);
}
