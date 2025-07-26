/*
 * This file is part of ImageToMapMC project
 *
 * Copyright (c) 2025 Agustin San Roman
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

#include "folder_remember.h"
#include "text_file.h"

#include <filesystem>

#if defined(_WIN32)
#include <shlobj.h>
#include <combaseapi.h>
#include <stringapiset.h>
#endif

using namespace std;

std::string getRememberFolderConfigFolder()
{
#if defined(_WIN32)

    PWSTR res;

    SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &res);

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

        return resultPath + std::string("\\ImageToMapMC");
    }

#elif defined(__APPLE__)

    return std::string("~/Library/Application Support/ImageToMapMC");

#elif defined(__linux__)

    return std::string("~/.ImageToMapMC");

#else

    return std::string("~/.ImageToMapMC");

#endif
}

std::string getRememberFolderConfigFile(std::string purpose)
{
    std::string folder = getRememberFolderConfigFolder();

#if defined(_WIN32)
    return folder + std::string("\\") + purpose;
#else
    return folder + std::string("/") + purpose;
#endif
}

std::string tools::getRememberedFolder(std::string purpose)
{
    std::string configFilePath = getRememberFolderConfigFile(purpose);

    return tools::readTextFile(configFilePath);
}

void tools::setRememberedFolder(std::string purpose, std::string folder) {
    std::string configFolder = getRememberFolderConfigFolder();

    if (!filesystem::exists(filesystem::path(configFolder))) {
        bool ok = filesystem::create_directories(configFolder);

        if (!ok) {
            return;
        }
    }

    std::string configFilePath = getRememberFolderConfigFile(purpose);

    (void) tools::writeTextFile(configFilePath, folder);
}
