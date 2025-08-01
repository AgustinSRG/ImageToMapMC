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

#include "common.h"
#include "../threads/progress.h"

namespace mapart
{
    /**
     * @brief  Generates map art
     * @note
     * @param  &colorSet: Color set
     * @param  &colorMatrix: Original color matrix
     * @param  &transparency: Transparency matrix
     * @param  width: Image width
     * @param  height: Image height
     * @param  preserveTransparency: True to preserve transparency
     * @param  colorDistanceAlgo: Color distance algorithm
     * @param  ditheringMethod: Dithering method
     * @retval Array of final colors
     */
    std::vector<const minecraft::FinalColor *> generateMapArt(const std::vector<minecraft::FinalColor> &colorSet, const std::vector<colors::Color> &colorMatrix, const std::vector<bool> &transparency, size_t width, size_t height, bool preserveTransparency, colors::ColorDistanceAlgorithm colorDistanceAlgo, mapart::DitheringMethod ditheringMethod, size_t threadNum, threading::Progress &progress, std::vector<size_t> &counts);
}
