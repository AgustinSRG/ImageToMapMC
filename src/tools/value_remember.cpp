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

#include "value_remember.h"
#include "fs.h"
#include "text_file.h"

#if defined(_WIN32)
#include <shlobj.h>
#include <combaseapi.h>
#include <stringapiset.h>
#endif

#if defined(__linux__)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
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

    size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1) {
        bufsize = 16384;
    }

    char *buf = (char*)malloc(bufsize);

    if (buf == NULL) {
        exit(1);
    }

    struct passwd pwd;
    struct passwd *result;

    getpwuid_r(getuid(), &pwd, buf, bufsize, &result);

    if (result == NULL) {
        return std::string("./.config");
    }

    const char *homedir = pwd.pw_dir;

    return std::string(homedir) + std::string("/.config/ImageToMapMC");

#else

    return std::string("./.config");

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

std::string tools::getRememberedValue(std::string purpose)
{
    std::string configFilePath = getRememberFolderConfigFile(purpose);

    return tools::readTextFile(configFilePath);
}

void tools::setRememberedValue(std::string purpose, std::string folder)
{
    std::string configFolder = getRememberFolderConfigFolder();

    if (!fs::exists(fs::path(configFolder)))
    {
        bool ok = fs::create_directories(configFolder);

        if (!ok)
        {
            return;
        }
    }

    std::string configFilePath = getRememberFolderConfigFile(purpose);

    (void)tools::writeTextFile(configFilePath, folder);
}
