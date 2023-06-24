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

#if defined(_WIN32)
#include <shlobj.h>
#include <combaseapi.h>
#include <stringapiset.h>
#endif

minecraft::McVersion minecraft::getVersionFromText(std::string versionStr)
{
    if (versionStr.substr(0, 4).compare("1.12") == 0)
    {
        return McVersion::MC_1_12;
    }

    if (versionStr.substr(0, 4).compare("1.13") == 0)
    {
        return McVersion::MC_1_13;
    }

    if (versionStr.substr(0, 4).compare("1.14") == 0)
    {
        return McVersion::MC_1_14;
    }

    if (versionStr.substr(0, 4).compare("1.15") == 0)
    {
        return McVersion::MC_1_15;
    }

    if (versionStr.substr(0, 4).compare("1.16") == 0)
    {
        return McVersion::MC_1_16;
    }

    if (versionStr.substr(0, 4).compare("1.17") == 0)
    {
        return McVersion::MC_1_17;
    }

    if (versionStr.substr(0, 4).compare("1.18") == 0)
    {
        return McVersion::MC_1_18;
    }

    if (versionStr.substr(0, 4).compare("1.19") == 0)
    {
        return McVersion::MC_1_19;
    }

    if (versionStr.substr(0, 4).compare("1.20") == 0)
    {
        return McVersion::MC_1_20;
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
        return std::string("1.12.2");
    case McVersion::MC_1_13:
        return std::string("1.13.2");
    case McVersion::MC_1_14:
        return std::string("1.14.4");
    case McVersion::MC_1_15:
        return std::string("1.15.2");
    case McVersion::MC_1_16:
        return std::string("1.16.5");
    case McVersion::MC_1_17:
        return std::string("1.17.1");
    case McVersion::MC_1_18:
        return std::string("1.18.2");
    case McVersion::MC_1_19:
        return std::string("1.19.4");
    case McVersion::MC_1_20:
        return std::string("1.20.1");
    default:
        return std::string("???");
    }
}

int minecraft::versionToDataVersion(minecraft::McVersion version)
{
    switch (version)
    {
    case McVersion::MC_1_12:
        return 1343;
    case McVersion::MC_1_13:
        return 1631;
    case McVersion::MC_1_14:
        return 1976;
    case McVersion::MC_1_15:
        return 2230;
    case McVersion::MC_1_16:
        return 2586;
    case McVersion::MC_1_17:
        return 2730;
    case McVersion::MC_1_18:
        return 2975;
    case McVersion::MC_1_19:
        return 3337;
    case McVersion::MC_1_20:
        return 3465;
    default:
        return 1343;
    }
}

std::string minecraft::getMinecraftFolderLocation()
{
#if defined(_WIN32)

    PWSTR res;

    SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &res);

    if (res == NULL)
    {
        return std::string("");
    }
    else
    {
        std::wstring resultStr(res);
        char *resStr = new char[resultStr.length() * 2 + 1];
        int bytesW = WideCharToMultiByte(CP_UTF8, 0, res, resultStr.length(), resStr, resultStr.length() * 2 + 1, NULL, NULL);

        resStr[bytesW] = '\0';

        std::string resultPath(resStr);

        CoTaskMemFree(res);
        delete resStr;

        return resultPath + std::string("\\.minecraft");
    }

#elif defined(__APPLE__)

    return std::string("~/Library/Application Support/minecraft");

#elif defined(__linux__)

    return std::string("~/.minecraft");

#else

    return std::string("");

#endif
}
