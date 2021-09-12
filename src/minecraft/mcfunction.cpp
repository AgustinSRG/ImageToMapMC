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

#include "mcfunction.h"

#include <sstream>

#include "../tools/text_file.h"

using namespace std;
using namespace minecraft;

void minecraft::writeMcFunctionFile(std::string fileName, std::vector<mapart::MapBuildingBlock> &buildData, minecraft::McVersion version)
{
    stringstream fileSS;
    for (size_t i = 0; i < buildData.size(); i++)
    {
        if (buildData[i].z > 0 && buildData[i].block_ptr != NULL)
        { // Ignore first line
            int x = buildData[i].x;
            int z = buildData[i].z - 1;

            stringstream ss;

            switch (version)
            {
            case McVersion::MC_1_12:
                ss << "setblock "
                   << "~" << x << " ~ ~" << z << " minecraft:" << buildData[i].block_ptr->nbtName << " " << buildData[i].block_ptr->dataValue;
                break;
            default:
                // Afetr 1.12, they removed the data values
                ss << "setblock "
                   << "~" << x << " ~ ~" << z << " minecraft:" << buildData[i].block_ptr->nbtName;
            }

            fileSS << ss.str() << endl;
        }
    }

    if (!tools::writeTextFile(fileName, fileSS.str()))
    {
        throw -1;
    }
}
