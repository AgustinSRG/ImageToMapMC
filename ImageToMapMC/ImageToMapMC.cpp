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

#include "ImageToMapMC.h"
#include "colors/colors.h"
#include "minecraft/mc_colors.h"

#include <vector>
#include <string>

using namespace std;
using namespace colors;
using namespace minecraft;

void printColors()
{
    // Load colors
    vector<Color> baseColors = loadBaseColors(McVersion::MC_1_12);
    vector<FinalColor> finalColors = loadFinalColors(baseColors);

    // Iterate
    int size = finalColors.size();
    for (int i = 0; i < size; i++)
    {
        string hex = colorToHex(finalColors[i].color);
        cout << "&" << i << " | Base: " << finalColors[i].baseColorIndex << " | ";
        switch (finalColors[i].colorType)
        {
        case McColorType::DARK:
            cout << "DARK  ";
            break;
        case McColorType::DARKER:
            cout << "DARKER";
            break;
        case McColorType::LIGHT:
            cout << "LIGHT ";
            break;
        case McColorType::NORMAL:
        default:
            cout << "NORMAL";
        }
        cout << " | R: " << ((unsigned int)finalColors[i].color.red) << " | G: " << ((unsigned int)finalColors[i].color.green) << " | B: " << ((unsigned int)finalColors[i].color.blue) << " | HEX: " << hex;
        cout << endl;
    }
}

int main()
{
    printColors();

    return 0;
}
