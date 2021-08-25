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

namespace mapart {
    /**
     * @brief  Read text file
     * @note   
     * @param  fileName: File name
     * @retval 
     */
    std::string readTextFile(std::string fileName);

    /**
     * @brief  Apply color set
     * @note   
     * @param  &colorSetFileContent: Color set contents
     * @param  blacklist: Blacklist or whitelist?
     * @param  &enabledConf: Enabled colors
     * @param  &finalColors: Colors
     * @param  &blockSet: Blocks
     * @param  baseColorNames: Color names
     * @retval None
     */
    void applyColorSet(const std::string &colorSetFileContent, bool * blacklist, std::vector<bool> &enabledConf, std::vector<minecraft::FinalColor> &finalColors, std::vector<minecraft::BlockList> &blockSet, const std::vector<std::string> baseColorNames);

    /**
     * @brief  Apply single line of color set
     * @note   
     * @param  command: Command
     * @param  line: Line number
     * @param  blacklist: Blacklist or whitelist?
     * @param  &enabledConf: Enabled colors
     * @param  &finalColors: Colors
     * @param  &blockSet: Blocks
     * @param  baseColorNames: Color names
     * @retval None
     */
    void applyColorSetCommand(const std::string command, int line, bool * blacklist, std::vector<bool> &enabledConf, std::vector<minecraft::FinalColor> &finalColors, std::vector<minecraft::BlockList> &blockSet, const std::vector<std::string> baseColorNames);

    /**
     * @brief  Builds color set file
     * @note   
     * @param  &blockSet: Blocks
     * @param  blacklist: Blacklist or Whitelist?
     * @param  &enabledConf: Enabled blocks
     * @param  baseColorNames: Base color names
     * @param  version: Minecraft version
     * @retval 
     */
    std::string buildColorSetString(const std::vector<minecraft::BlockList> &blockSet, bool blacklist, const std::vector<bool> &enabledConf, const std::vector<std::string> baseColorNames, minecraft::McVersion version);

    /**
     * @brief  Writes text file
     * @note   
     * @param  fileName: File name
     * @param  &colorSetFileContent: Content
     * @retval 
     */
    bool writeTextFile(std::string fileName, const std::string &colorSetFileContent);
}
