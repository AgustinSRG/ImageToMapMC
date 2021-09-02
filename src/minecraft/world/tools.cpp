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

#include "tools.h"

#include <cmath>

using namespace minecraft;
using namespace std;

bool getBit(uint8_t byte, uint8_t bit) {
    return ((byte >> (7 - bit))  & 0x01) == 1;
}

BlockStatesArrayMaker::BlockStatesArrayMaker(size_t paletteSize, bool padded) {
    this->bitsPerState = bitsPerState;
    array.resize(1);
    array[0] = 0x0000000000000000;
    currentLong = 0;
    currentBit = 0;

    bitsPerState = 4;
    while (pow(2, bitsPerState) < paletteSize) {
        bitsPerState++;
    }

    if (padded) {
        bitsPerLong = 64 - (64 % bitsPerState);
    } else {
        bitsPerState = 64;
    }
}

void BlockStatesArrayMaker::insertBit(bool bit) {
    if (bit) {
        // Set bit
        array[currentLong] |= 1UL << (63 - currentBit);
    }

    // Next bit
    currentBit++;

    if (currentBit >= bitsPerState) {
        // Next long
        currentLong++;
        array.resize(currentLong + 1);
        array[currentLong] = 0x0000000000000000;;
        currentBit = 0;
    }
}

void BlockStatesArrayMaker::insertState(uint8_t state) {
    for (uint8_t i = 0; i < bitsPerState; i++) {
        insertBit(getBit(state, 8 - bitsPerState + i));
    }
}

nbt::tag_compound minecraft::blockDescriptionToTag(const minecraft::BlockDescription *desc)
{
    nbt::tag_compound tag;

    tag.insert("Name", desc->nbtName);
    if (desc->nbtTags.size() > 0)
    {
        nbt::tag_compound props;
        for (int i = 0; i < desc->nbtTags.size(); i++)
        {
            props.insert(desc->nbtTags[i].name, desc->nbtTags[i].value);
        }
        tag.insert("Properties", props.clone());
    }

    return tag;
}
