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

#include "map_color_set.h"
#include <iostream>
#include <sstream>
#include <streambuf>
#include <algorithm>

#define IS_CHARACTER(x) (((x) == '_') || ((x) >= 'A' && (x) <= 'Z') || ((x) >= 'a' && (x) <= 'z') || ((x) >= '0' && (x) <= '9'))

using namespace std;
using namespace mapart;

void mapart::applyColorSet(const std::string &colorSetFileContent, bool * blacklist, std::vector<bool> &enabledConf, std::vector<minecraft::FinalColor> &finalColors, std::vector<minecraft::BlockList> &blockSet, const std::vector<std::string> baseColorNames)
{
    string current = colorSetFileContent;
    bool ended = false;
    int line = 1;
    while (!ended)
    {
        size_t lineBreakPos = current.find("\n");
        if (lineBreakPos != string::npos)
        {
            string cmd = current.substr(0, lineBreakPos);
            applyColorSetCommand(cmd, line, blacklist, enabledConf, finalColors, blockSet, baseColorNames);
            current = current.substr(lineBreakPos + 1);
        }
        else
        {
            ended = true;
            applyColorSetCommand(current, line, blacklist, enabledConf, finalColors, blockSet, baseColorNames);
        }
        line++;
    }
}

void mapart::applyColorSetCommand(const std::string command, int line, bool * blacklist, std::vector<bool> &enabledConf, std::vector<minecraft::FinalColor> &finalColors, std::vector<minecraft::BlockList> &blockSet, const std::vector<std::string> baseColorNames)
{
    if (command.size() == 0 || command.at(0) == '#')
    {
        return; // Ignore
    }

    string cmd = "";
    string arg1 = "";
    string arg2 = "";

    int status = 0;

    for (size_t i = 0; i < command.size(); i++)
    {
        char x = command.at(i);

        if (status == 0)
        {
            if (x == '(')
            {
                status = 1;
            }
            else if (IS_CHARACTER(x))
            {
                cmd.append(command.substr(i, 1));
            }
        }
        else if (status == 1)
        {
            if (x == ')')
            {
                status = 3;
            }
            else if (x == ',')
            {
                status = 2;
            }
            else if (IS_CHARACTER(x))
            {
                arg1.append(command.substr(i, 1));
            }
        }
        else if (status == 2)
        {
            if (x == ')' || x == ',')
            {
                status = 3;
            }
            else if (IS_CHARACTER(x))
            {
                arg2.append(command.substr(i, 1));
            }
        }
    }

    // all to lower case
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    std::transform(arg1.begin(), arg1.end(), arg1.begin(), ::tolower);
    std::transform(arg2.begin(), arg2.end(), arg2.begin(), ::tolower);

    if (cmd.compare("mode") == 0)
    {
        if (arg1.compare("blacklist") == 0)
        {
            *blacklist = true;
            initializeEnabledColors(finalColors, true);
            for (size_t i = 0; i < enabledConf.size(); i++)
            {
                enabledConf[i] = true;
            }
        }
        else if (arg1.compare("whitelist") == 0)
        {
            *blacklist = false;
            initializeEnabledColors(finalColors, false);
            for (size_t i = 0; i < enabledConf.size(); i++)
            {
                enabledConf[i] = false;
            }
        }
        else
        {
            cerr << endl
                 << "[Color Set] Warning: Invalid mode at line " << line << endl;
        }
    }
    else if (cmd.compare("use") == 0 || cmd.compare("enable") == 0 || cmd.compare("add") == 0 || cmd.compare("whitelist") == 0)
    {
        size_t color = minecraft::findColorByName(baseColorNames, arg1);
        if (color >= 0)
        {
            setBaseColorEnabled(finalColors, static_cast<short>(color), true);
            if (color < enabledConf.size())
            {
                enabledConf[color] = true;
            }
            if (arg2.size() > 0)
            {
                short bIndex = blockSet[color].findBlock(arg2);
                if (bIndex >= 0)
                {
                    blockSet[color].useBlockIndex = bIndex;
                }
                else
                {
                    cerr << endl
                         << "[Color Set] Warning: Block not found '" << arg2 << "' for color '" << arg1 << "' at line " << line << endl;
                }
            }
        }
        else
        {
            cerr << endl
                 << "[Color Set] Warning: Color not found '" << arg1 << "' at line " << line << endl;
        }
    }
    else if (cmd.compare("remove") == 0 || cmd.compare("disable") == 0 || cmd.compare("blacklist") == 0)
    {
        size_t color = minecraft::findColorByName(baseColorNames, arg1);
        if (color >= 0)
        {
            setBaseColorEnabled(finalColors, static_cast<short>(color), false);
            if (color < enabledConf.size())
            {
                enabledConf[color] = false;
            }
        }
        else
        {
            cerr << endl
                 << "[Color Set] Warning: Color not found '" << arg1 << "' at line " << line << endl;
        }
    }
    else
    {
        cerr << endl
             << "[Color Set] Warning: Invalid command '" << cmd << "' at line " << line << endl;
    }
}

std::string mapart::buildColorSetString(const std::vector<minecraft::BlockList> &blockSet, bool blacklist, const std::vector<bool> &enabledConf, const std::vector<std::string> baseColorNames, minecraft::McVersion version)
{
    stringstream ss;

    ss << "# Color Set file" << endl;
    ss << "# Version: " << minecraft::versionToString(version) << endl;

    ss << endl;

    if (blacklist)
    {
        ss << "MODE(BLACKLIST)" << endl;
    }
    else
    {
        ss << "MODE(WHITELIST)" << endl;
    }

    ss << endl;

    for (size_t i = 1; i < enabledConf.size(); i++)
    {
        if (i >= blockSet.size())
        {
            continue;
        }

        if (blockSet[i].useBlockIndex > 0)
        {
            if (blacklist && !enabledConf[i])
            {
                // Blacklisted
                ss << "REMOVE(" << baseColorNames[i] << "," << blockSet[i].blocks[blockSet[i].useBlockIndex].id << ")" << endl;
            }
            else if (enabledConf[i])
            {
                // Whitelisted or modified
                ss << "ADD(" << baseColorNames[i] << "," << blockSet[i].blocks[blockSet[i].useBlockIndex].id << ")" << endl;
            }
        }
        else
        {
            if (blacklist && !enabledConf[i])
            {
                // Blacklisted
                ss << "REMOVE(" << baseColorNames[i] << ")" << endl;
            }
            else if (!blacklist && enabledConf[i])
            {
                // Whitelisted
                ss << "ADD(" << baseColorNames[i] << ")" << endl;
            }
        }
    }

    return ss.str();
}
