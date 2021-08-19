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

#include "mc_common.h"

minecraft::McVersion minecraft::getVersionFromText(std::string versionStr)
{
    if (versionStr.compare("1.12") == 0)
    {
        return McVersion::MC_1_12;
    }

    if (versionStr.compare("1.13") == 0)
    {
        return McVersion::MC_1_13;
    }

    if (versionStr.compare("1.14") == 0)
    {
        return McVersion::MC_1_14;
    }

    if (versionStr.compare("1.15") == 0)
    {
        return McVersion::MC_1_15;
    }

    if (versionStr.compare("1.16") == 0)
    {
        return McVersion::MC_1_16;
    }

    if (versionStr.compare("1.17") == 0)
    {
        return McVersion::MC_1_17;
    }

    if (versionStr.compare("last") == 0)
    {
        return MC_LAST_VERSION;
    }

    return McVersion::UNKNOWN;
}

std::string minecraft::versionToString(minecraft::McVersion version)
{
    switch (version)
    {
    case McVersion::MC_1_12:
        return std::string("1.12");
    case McVersion::MC_1_13:
        return std::string("1.13");
    case McVersion::MC_1_14:
        return std::string("1.14");
    case McVersion::MC_1_15:
        return std::string("1.15");
    case McVersion::MC_1_16:
        return std::string("1.16");
    case McVersion::MC_1_17:
        return std::string("1.17");
    default:
        return std::string("???");
    }
}
