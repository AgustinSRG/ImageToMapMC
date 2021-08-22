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
#include <filesystem>
#include <string>
#include "mapart/map_art.h"
#include "mapart/map_image.h"

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
        return buildMap(argc, argv);
    }
    else if (firstArg.compare(string("-r")) == 0 || firstArg.compare(string("--render")) == 0)
    {
        return renderMap(argc, argv);
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

    cout << "Build map usage: mcmap --build [input-image] [OPTIONS]..." << endl;
    cout << "Note: You have to manually resize the image first if it's too large" << endl;
    cout << "This tool will assume a 1:1 scale unless you use the --resize option" << endl;
    cout << "Available options:" << endl;
    cout << "    -o, --output [path]            Specifies the output folder." << endl;
    cout << "                                     By default the folder name is 'mapart'" << endl;
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

int renderMap(int argc, char **argv)
{
    if (argc < 4)
    {
        cerr << "Usage: mcmap --render [input-map.dat] [output-image.png]" << endl;
        return 1;
    }

    // Image library init
    wxInitAllImageHandlers();

    // Load colors
    std::vector<colors::Color> baseColors = minecraft::loadBaseColors(MC_LAST_VERSION);
    std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);

    // Colors
    std::vector<const minecraft::FinalColor *> colorsMatrix;

    // Load map file
    try
    {
        std::vector<map_color_t> fileData = mapart::readMapNBTFile(argv[2]);
        colorsMatrix = mapart::mapColorsToRGB(colorSet, fileData);
    }
    catch (int code)
    {
        switch (code)
        {
        case -1:
            cerr << "Cannot open file: " << argv[1] << endl;
            break;
        case -2:
            cerr << "Invalid file: " << argv[1] << " is not a valid map NBT file." << endl;
            break;
        }

        return 1;
    }
    catch (const std::exception &ex)
    {
        cerr << ex.what() << endl;
        return 1;
    }
    catch (const std::string &ex)
    {
        cerr << ex << endl;
        return 1;
    }
    catch (...)
    {
        cerr << "Oops, an error ocurred." << endl;
        return 1;
    }

    // Convert colors to image
    wxImage image(MAP_WIDTH, MAP_HEIGHT);
    unsigned char *rawData = image.GetData();
    size_t size = colorsMatrix.size();

    size_t j = 0;
    for (size_t i = 0; i < size; i++)
    {
        colors::Color color = colorsMatrix[i]->color;

        rawData[j++] = color.red;
        rawData[j++] = color.green;
        rawData[j++] = color.blue;
    }

    // Save the image to file (PNG)

    bool ok = image.SaveFile(wxString(argv[3]), wxBitmapType::wxBITMAP_TYPE_PNG);

    if (!ok)
    {
        cerr << "Cannot save to file: " << argv[3] << endl;
        return 1;
    }

    return 0;
}

int buildMap(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "Usage: mcmap --build [input-image] [OPTIONS]..." << endl;
        return 1;
    }

    string inputImageFile(argv[2]);
    string outputPath = "mapart";
    string colorSetFilename = "";
    MapOutputFormat outFormat = MapOutputFormat::Map;
    McVersion version = MC_LAST_VERSION;
    int mapNumber = -1;
    ColorDistanceAlgorithm colorAlgo = ColorDistanceAlgorithm::Euclidean;
    DitheringMethod ditheringMethod = DitheringMethod::None;
    string levelName = "Map Art World";
    int rsW = -1;
    int rsH = -1;

    // Load arguments
    for (int i = 3; i < argc; i++)
    {
        string arg(argv[i]);

        if (arg.compare(string("-o")) == 0 || arg.compare(string("--output")) == 0)
        {
            if ((i + 1) < argc)
            {
                outputPath = argv[i + 1];
                i++;
            }
            else
            {
                cerr << "Option " << arg << " requires a parameter." << endl;
                cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-f")) == 0 || arg.compare(string("--format")) == 0)
        {
            if ((i + 1) < argc)
            {
                string outputFormatStr(argv[i + 1]);

                if (outputFormatStr.compare(string("map")) == 0)
                {
                    outFormat = MapOutputFormat::Map;
                }
                else if (outputFormatStr.compare(string("world")) == 0)
                {
                    outFormat = MapOutputFormat::World;
                }
                else if (outputFormatStr.compare(string("structure")) == 0)
                {
                    outFormat = MapOutputFormat::Structure;
                }
                else
                {
                    cerr << "Urecornized outout format: " << argv[i + 1] << endl;
                    cerr << "Available formats: map, world, structure" << endl;
                    return 1;
                }

                i++;
            }
            else
            {
                cerr << "Option " << arg << " requires a parameter." << endl;
                cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-v")) == 0 || arg.compare(string("--version")) == 0)
        {
            if ((i + 1) < argc)
            {
                version = getVersionFromText(string(argv[i + 1]));
                i++;
                if (version == McVersion::UNKNOWN)
                {
                    cerr << "Urecornized version: " << argv[i + 1] << endl;
                    cerr << "Available versions: last, 1.17, 1.16, 1.15, 1.14, 1.13, 1.12" << endl;
                    return 1;
                }
            }
            else
            {
                cerr << "Option " << arg << " requires a parameter." << endl;
                cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-rs")) == 0 || arg.compare(string("--resize")) == 0)
        {
            if ((i + 1) < argc)
            {
                string resizeString(argv[i + 1]);
                int xIndex;

                xIndex = resizeString.find("x");

                if (xIndex < 1)
                {
                    xIndex = resizeString.find("X");
                    if (xIndex < 1)
                    {
                        cerr << "Size must be provided using the WIDTHxHEIGHT format. Example: '128x128'" << endl;
                        cerr << "For help type: mcmap --help" << endl;
                        return 1;
                    }
                }

                string wStr = resizeString.substr(0, xIndex);
                string hStr = resizeString.substr(xIndex + 1);

                rsW = atoi(wStr.c_str());
                rsH = atoi(hStr.c_str());

                i++;
            }
            else
            {
                cerr << "Option " << arg << " requires a parameter." << endl;
                cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-cm")) == 0 || arg.compare(string("--color-method")) == 0)
        {
            if ((i + 1) < argc)
            {
                string colorMethodStr(argv[i + 1]);

                if (colorMethodStr.compare(string("delta-e")) == 0)
                {
                    colorAlgo = ColorDistanceAlgorithm::DeltaE;
                }
                else if (colorMethodStr.compare(string("euclidean")) == 0)
                {
                    colorAlgo = ColorDistanceAlgorithm::Euclidean;
                }
                else
                {
                    cerr << "Urecornized color medthod: " << argv[i + 1] << endl;
                    cerr << "Available methods: euclidean, delta-e" << endl;
                    return 1;
                }

                i++;
            }
            else
            {
                cerr << "Option " << arg << " requires a parameter." << endl;
                cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-d")) == 0 || arg.compare(string("--dithering")) == 0)
        {
            if ((i + 1) < argc)
            {
                ditheringMethod = parseDitheringMethodFromString(string(argv[i + 1]));
                i++;
                if (ditheringMethod == DitheringMethod::Unknown)
                {
                    cerr << "Urecornized dithering: " << argv[i + 1] << endl;
                    cerr << "Available dithering methods: none, floyd-steinberg, min-average-error, burkes, sierra-lite, stucki, atkinson, bayer-44, bayer-22, ordered-33" << endl;
                    return 1;
                }
            }
            else
            {
                cerr << "Option " << arg << " requires a parameter." << endl;
                cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-mn")) == 0 || arg.compare(string("--map-number")) == 0)
        {
            if ((i + 1) < argc)
            {
                mapNumber = atoi(argv[i + 1]);
                i++;
            }
            else
            {
                cerr << "Option " << arg << " requires a parameter." << endl;
                cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-bm")) == 0 || arg.compare(string("--build-method")) == 0)
        {
        }
        else if (arg.compare(string("-ln")) == 0 || arg.compare(string("--level-name")) == 0)
        {
            if ((i + 1) < argc)
            {
                levelName = argv[i + 1];
                i++;
            }
            else
            {
                cerr << "Option " << arg << " requires a parameter." << endl;
                cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-cs")) == 0 || arg.compare(string("--color-set")) == 0)
        {
            if ((i + 1) < argc)
            {
                colorSetFilename = argv[i + 1];
                i++;
            }
            else
            {
                cerr << "Option " << arg << " requires a parameter." << endl;
                cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else
        {
            cerr << "Unrecognized option: " << arg << endl;
            cerr << "For help type: mcmap --help" << endl;
            return 1;
        }
    }

    // Image library init
    wxInitAllImageHandlers();

    // Load input image
    wxImage image;
    if (!image.LoadFile(inputImageFile))
    {
        cerr << "Cannot load image: " << inputImageFile << endl;
        return 1;
    }

    // Resize image if required
    if (rsW > 0 || rsH > 0)
    {
        if (rsW <= 0)
        {
            rsW = static_cast<int>(((double)rsH / image.GetSize().GetHeight()) * image.GetSize().GetWidth());
        }
        else if (rsH <= 0)
        {
            rsH = static_cast<int>(((double)rsW / image.GetSize().GetWidth()) * image.GetSize().GetHeight());
        }

        image.Rescale(rsW, rsH);
    }

    // Convert image to color matrix and pad if needed
    int matrixW;
    int matrixH;
    vector<Color> imageColorsMatrix = loadColorMatrixFromImageAndPad(image, &matrixW, &matrixH);

    // Test
    wxImage imgSave(matrixW, matrixH);
    unsigned char *imgSaveData = imgSave.GetData();
    size_t size = imageColorsMatrix.size();

    size_t j = 0;
    for (size_t i = 0; i < size; i++)
    {
        colors::Color color = imageColorsMatrix[i];

        imgSaveData[j++] = color.red;
        imgSaveData[j++] = color.green;
        imgSaveData[j++] = color.blue;
    }

    imgSave.SaveFile("test.png", wxBITMAP_TYPE_PNG);

    return 0;
}
