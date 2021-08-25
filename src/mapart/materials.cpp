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

#include "materials.h"

#include <algorithm>
#include <sstream>
#include <iostream>

using namespace std;
using namespace mapart;

MaterialsList::MaterialsList(const std::vector<std::string> baseColorNames)
{
    materials.resize(baseColorNames.size());
    for (int i = 0; i < materials.size(); i++)
    {
        materials[i].id = i;
        materials[i].name = "";
        materials[i].count = 0;
    }
    total = 0;
    placeholder = 0;
}

void MaterialsList::addBlocks(std::vector<mapart::MapBuildingBlock> &buildingBlocks)
{
    for (int i = 0; i < buildingBlocks.size(); i++)
    {
        const minecraft::BlockDescription *blockPtr = buildingBlocks[i].block_ptr;

        if (blockPtr == NULL)
        {
            // Placeholder block
            placeholder++;
        }
        else
        {
            short blockIndex = blockPtr->baseColorIndex;
            if (blockIndex >= 0 && blockIndex < materials.size())
            {
                if (materials[blockIndex].name.size() == 0)
                {
                    materials[blockIndex].name = blockPtr->name;
                }
                materials[blockIndex].count++;
                total++;
                placeholder++;
            }
        }
    }
}

bool compareMaterials(MaterialListItem a, MaterialListItem b) {
    if (a.count > b.count) {
        return true;
    } else if (a.count < b.count) {
        return false;
    } else {
        return a.id < b.id;
    }
}

std::string compactAmount(size_t amount) {
    stringstream ss;
    bool requiresComma = false;

    if (amount >= SHULKER_BOX_AMOUNT) {
        size_t shulkers = amount / SHULKER_BOX_AMOUNT;
        amount = amount % SHULKER_BOX_AMOUNT;
        ss << shulkers << " Shulkers";
        requiresComma = true;
    }

    if (amount >= STACK_AMOUNT) {
        size_t stacks = amount / STACK_AMOUNT;
        amount = amount / STACK_AMOUNT;
        if (requiresComma) {
            ss << ", ";
        } else {
            requiresComma = true;
        }
        ss << stacks << " Stacks";
    }

    if (amount > 0) {
        if (requiresComma) {
            ss << ", ";
        }
        ss << amount << " Blocks";
    }

    return ss.str();
}

std::string MaterialsList::toString() {
    std::vector<MaterialListItem> materialsCopy(materials);

    // Sort list
    std::sort (materialsCopy.begin(), materialsCopy.end(), compareMaterials);

    stringstream ss;

    ss << "List of materials:" << endl << endl;
    
    for (int i = 0; i < materialsCopy.size(); i++) {
        if (materialsCopy[i].count <= 0) {
            break;
        }
        size_t count = materialsCopy[i].count;

        ss << "    " << materialsCopy[i].name << ": " << materialsCopy[i].count << " ( " << compactAmount(materialsCopy[i].count) << " )" << endl;
    }

    ss << endl;

    ss << "Total color blocks: " << total << " ( " << compactAmount(total) << " )" << endl;

    ss << endl;

    ss << "Placeholder blocks: " << placeholder << " ( " << compactAmount(placeholder) << " )" << endl;

    return ss.str();
}
