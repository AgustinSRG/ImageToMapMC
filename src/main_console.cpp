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

#include "main_console.h"
#include <iostream>
#include <string>
#include "mapart/map_art.h"

using namespace std;
using namespace colors;
using namespace minecraft;
using namespace mapart;

/**
 * @brief  Main (Console)
 * @note   
 * @param  argc: 
 * @param  argv: 
 * @retval 
 */
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        return printVersion();
    }

    string firstArg(argv[1]);

    // Check first arg
    if (firstArg.compare(string("-b")) == 0 || firstArg.compare(string("--build")) == 0)
    {
    }
    else if (firstArg.compare(string("-r")) == 0 || firstArg.compare(string("--render")) == 0)
    {
    }
    else if (firstArg.compare(string("-h")) == 0 || firstArg.compare(string("--help")) == 0)
    {
        return printHelp();
    }
    else
    {
        return printVersion();
    }

    return 0;
}

int printVersion()
{
    cout << "ImageToMapMC Version 1.0.0, by @asanrom" << endl;
    cout << "Repository: https://github.com/AgustinSRG/ImageToMapMC" << endl;
    cout << "-------------------------------------------------------------" << endl;
    cout << "Use 'mcmap --help' for command usage" << endl;

    return 0;
}

/**
 * @brief  Prints help to console
 * @note   
 * @retval 
 */
int printHelp()
{
    cout << "Usage: mcmap [ACTION] [OPTIONS]..." << endl;
    cout << "Converts images into minecraft maps." << endl;
    cout << "This tool can be used to speed up map art design process." << endl;

    cout << endl;

    cout << "Available options:" << endl;
    cout << "    -h, --help        Displays help" << endl;
    cout << "    -b, --build       Builds a map from input image" << endl;
    cout << "    -r, --render      Renders image from input map file in '.dat' format" << endl;

    cout << endl;

    cout << "Build map usage: mcmap --build [input-image] [OPTIONS]" << endl;
    cout << "Note: You have to manually resize the image first if it's too large" << endl;
    cout << "This tool will assume a 1:1 scale unless you use the --resize option" << endl;
    cout << "Available options:" << endl;
    cout << "    -o, --output [path]            Specifies the output folder (REQUIRED)" << endl;
    cout << "    -f, --format [format]          Specifies the output format. By defaulty is 'map'" << endl;
    cout << "                                     'map' format creates '.dat' files for the maps" << endl;
    cout << "                                     'world' format creates a minecraft world with the maps" << endl;
    cout << "                                     'structure' format creates nbt structure files" << endl;
    cout << "    -v, --version [version]        Specifies the minecraft version in A.B format (eg, 1.12)." << endl;
    cout << "                                     Set to 'last' to use the most recent minecraft version available" << endl;
    cout << "    -rs, --resize [WxH]            Resizes the image before building the map" << endl;
    cout << "    -cm, --color-method [method]   Specifies the method to aproximate the color. By default, 'euclidean'" << endl;
    cout << "                                     'euclidean' - Simple RGB 3D squared distance" << endl;
    cout << "                                     'delta-e' - More complex method but more accurate." << endl;
    cout << "    -d, --dithering [method]       Applies dithering to the image (https://en.wikipedia.org/wiki/Dither)" << endl;
    cout << "                                     'none' - No dithering" << endl;
    cout << "                                     'floyd-steinberg'" << endl;
    cout << "                                     'min-average-error'" << endl;
    cout << "                                     'burkes'" << endl;
    cout << "                                     'sierra-lite'" << endl;
    cout << "                                     'stucki'" << endl;
    cout << "                                     'atkinson'" << endl;
    cout << "                                     'bayer-44' (Bayer 4x4)" << endl;
    cout << "                                     'bayer-22' (Bayer 2x2)" << endl;
    cout << "                                     'ordered-33' (Ordered 3x3)" << endl;
    cout << "    -mn, --map-number [num]        Sets the last map ID or total number of maps in your world." << endl;
    cout << "                                     This applies only when --format is set to 'map'" << endl;
    cout << "    -bm, --build-method [method]   Sets the build method. By default '3d'" << endl;
    cout << "                                     This applies only when --format is set to 'world' or 'structure'" << endl;
    cout << "                                     -bm '3d' Build 3 dimensional map with arbitrary height jumps" << endl;
    cout << "                                     -bm '2d' Build 2 dimensional map at the same level" << endl;
    cout << "                                     -bm 'stair' Build 3 dimensional map with 1 block height jumps max" << endl;
    cout << "    -ln, --level-name [name]       Sets the name for the minecraft world." << endl;
    cout << "                                     This applies only when --format is set to 'world'" << endl;
    cout << "    -cs, --color-set [name/file]   Specifies the color set to use." << endl;
    cout << "                                     Color sets can enable or disable colors and" << endl;
    cout << "                                     set the blocks to build each color." << endl;
    cout << "                                     By default all available colors are used" << endl;

    cout << endl;

    cout << "Render map usage: mcmap --render [input-map.dat] [output-image.png]" << endl;
    cout << "Loads NBT map file and converts it to PNG image." << endl;

    cout << endl;

    cout << "Color set files documentation: https://github.com/AgustinSRG/ImageToMapMC" << endl;

    cout << endl;

    return 0;
}
