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

#define MC_LAST_VERSION (minecraft::McVersion::MC_1_17)

#include <string>

namespace minecraft
{
    /**
     * @brief  Minecraft versions
     * @note   
     * @retval None
     */
    enum class McVersion : short
    {
        UNKNOWN = 0,
        MC_1_12 = 12,
        MC_1_13 = 13,
        MC_1_14 = 14,
        MC_1_15 = 15,
        MC_1_16 = 16,
        MC_1_17 = 17
    };

    /**
     * @brief  Parses version from string
     * @note   
     * @param  versionStr: Version string
     * @retval Version (enum). Returns UNKNOWN if version str is invalid
     */
    minecraft::McVersion getVersionFromText(std::string versionStr);

    /**
     * @brief  Gets version as string
     * @note   
     * @param  version: Version
     * @retval Version as string
     */
    std::string versionToString(minecraft::McVersion version);

    /**
     * @brief  Gets the data version for the version
     * @note   
     * @param  version: Version
     * @retval 
     */
    int versionToDataVersion(minecraft::McVersion version);
}
