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

#pragma once

#include <string>

namespace tools
{
    const std::string FOLDER_PURPOSE_EXPORT_MAPS("folder-exp-maps");
    const std::string FOLDER_PURPOSE_EXPORT_STRUCTURES("folder-exp-struct");

    /**
     * @brief  Gets the remembered folder
     * @note   
     * @param  purpose: The purpose of the folder. Use the constants (tools::FOLDER_PURPOSE_{})
     * @retval The remembered folder, or an empty string by default
     */
    std::string getRememberedFolder(std::string purpose);


    /**
     * @brief  Sets a folder to remember it
     * @note   
     * @param  purpose: The purpose of the folder. Use the constants (tools::FOLDER_PURPOSE_{})
     * @param  folder: The folder to be remembered
     * @retval None
     */
    void setRememberedFolder(std::string purpose, std::string folder);
}
