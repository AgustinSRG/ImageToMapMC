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

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <iostream>
#include <filesystem>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <cmath>
#include "mapart/map_art.h"
#include "mapart/map_image.h"
#include "threads/progress.h"
#include "minecraft/structure.h"
#include "tools/basedir.h"
#include "tools/text_file.h"

int printHelp();
int printVersion();
int printBlocks(int argc, char **argv);
int renderMap(int argc, char ** argv);
int buildMap(int argc, char ** argv);
void progressReporter(threading::Progress &progress);

enum class MapOutputFormat {
    Map,
    Structure
};

#define REPORT_THREAD_DELAY (33)

#define BLOCKS_PRINT_TEMPLATE ("| %-25s | %-7s | %s\n")
