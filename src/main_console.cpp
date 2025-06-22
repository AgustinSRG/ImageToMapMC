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
#include "version.h"

using namespace std;
using namespace colors;
using namespace minecraft;
using namespace mapart;
using namespace tools;

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
    else if (firstArg.compare(string("-v")) == 0 || firstArg.compare(string("--version")) == 0)
    {
        return printVersion();
    }
    else if (firstArg.compare(string("--print-blocks")) == 0)
    {
        return printBlocks(argc, argv);
    }
    else
    {
        return printVersion();
    }

    return 0;
}

int printVersion()
{
    cout << "ImageToMapMC console utility, by @asanrom" << endl;
    cout << "Version: " << (APP_VERSION) << endl;
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
    cout << "    -h, --help              Displays help" << endl;
    cout << "    -v, --version           Displays version" << endl;
    cout << "    --print-blocks [ver]    Prints all available base colors with its associated blocks" << endl;
    cout << "    -b, --build             Builds a map from input image" << endl;
    cout << "    -r, --render            Renders image from input map file in '.dat' format" << endl;

    cout << endl;

    cout << "Build map usage: mcmap --build [input-image] [OPTIONS]..." << endl;
    cout << "Note: You have to manually resize the image first if it's too large" << endl;
    cout << "This tool will assume a 1:1 scale unless you use the --resize option" << endl;
    cout << "Available options:" << endl;
    cout << "    -o, --output [path]            Specifies the output folder or file." << endl;
    cout << "                                     By default the folder name is 'mapart'" << endl;
    cout << "    -f, --format [format]          Specifies the output format. By defaulty is 'map'" << endl;
    cout << "                                     'map' format creates '.dat' files for the maps" << endl;
    cout << "                                     'structure' format creates nbt structure files" << endl;
    cout << "                                     'structure-single' format creates a single nbt structure file" << endl;
    cout << "                                     'schematic' format creates schematic files" << endl;
    cout << "                                     'schematic-single' format creates a single schematic file" << endl;
    cout << "                                     'function' format creates .mcfunction files" << endl;
    cout << "    -mv, --mc-version [version]    Specifies the minecraft version in A.B format (eg, 1.12)." << endl;
    cout << "                                     Set to 'last' to use the most recent minecraft version available" << endl;
    cout << "    -bg, --background [#FFFFFF]    Specifies the background color in hex format." << endl;
    cout << "                                     By default, the background color is white." << endl;
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
    cout << "                                     This applies only when --format is set to 'structure'" << endl;
    cout << "                                     -bm '3d' Build 3 dimensional map with arbitrary height jumps" << endl;
    cout << "                                     -bm '2d' Build 2 dimensional map at the same level" << endl;
    cout << "                                     -bm 'stair' Build 3 dimensional map with 1 block height jumps max" << endl;
    cout << "    -cs, --color-set [name/file]   Specifies the color set to use." << endl;
    cout << "                                     Color sets can enable or disable colors and" << endl;
    cout << "                                     set the blocks to build each color." << endl;
    cout << "                                     By default all available colors are used" << endl;
    cout << "    -m, --materials [file]         Specifies a file to print the list of required materials." << endl;
    cout << "                                     This applies only when --format is set to 'structure' or 'function'" << endl;
    cout << "    -t, --threads [num]            Specifies the number of threads to use." << endl;
    cout << "                                     By default all available cores will be used" << endl;
    cout << "    -y, --yes [num]                Prevents asking any user input." << endl;

    cout << endl;

    cout << "Render map usage: mcmap --render [input-map.dat] [output-image.png]" << endl;
    cout << "Loads NBT map file and converts it to PNG image." << endl;

    cout << endl;

    cout << "Color set files documentation: https://github.com/AgustinSRG/ImageToMapMC" << endl;

    cout << endl;

    return 0;
}

int printBlocks(int argc, char **argv)
{
    McVersion version = MC_LAST_VERSION;

    if (argc > 2)
    {
        version = getVersionFromText(string(argv[2]));
        if (version == McVersion::UNKNOWN)
        {
            std::cerr << "Urecornized version: " << argv[2] << endl;
            std::cerr << "Available versions: last, 1.17, 1.16, 1.15, 1.14, 1.13, 1.12" << endl;
            return 1;
        }
    }

    std::vector<colors::Color> baseColors = minecraft::loadBaseColors(version);
    std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
    std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);

    printf(BLOCKS_PRINT_TEMPLATE, "COLOR", "HEX", "BLOCKS");
    printf(BLOCKS_PRINT_TEMPLATE, "", "", "");

    for (size_t i = 1; i < baseColors.size(); i++)
    {
        string name = baseColorNames[i];
        string hex = colors::colorToHex(baseColors[i]);
        string blocks = "";
        for (int j = 0; j < blockSet[i].blocks.size(); j++)
        {
            if (blockSet[i].blocks[j].getBlockDescription(version) != NULL)
            {
                if (blocks.size() > 0)
                {
                    blocks += ", ";
                }
                blocks += blockSet[i].blocks[j].id;
            }
        }
        printf(BLOCKS_PRINT_TEMPLATE, name.c_str(), hex.c_str(), blocks.c_str());
    }

    return 0;
}

int renderMap(int argc, char **argv)
{
    if (argc < 4)
    {
        std::cerr << "Usage: mcmap --render [input-map.dat] [output-image.png]" << endl;
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
            std::cerr << "Cannot open file: " << argv[1] << endl;
            break;
        case -2:
            std::cerr << "Invalid file: " << argv[1] << " is not a valid map NBT file." << endl;
            break;
        }

        return 1;
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << endl;
        return 1;
    }
    catch (const std::string &ex)
    {
        std::cerr << ex << endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Oops, an error ocurred." << endl;
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
        std::cerr << "Cannot save to file: " << argv[3] << endl;
        return 1;
    }

    return 0;
}

int buildMap(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: mcmap --build [input-image] [OPTIONS]..." << endl;
        return 1;
    }

    string inputImageFile(argv[2]);
    string outputPath = "mapart";
    bool outputPathSet = false;
    string colorSetFilename = "";
    MapOutputFormat outFormat = MapOutputFormat::Map;
    McVersion version = MC_LAST_VERSION;
    int mapNumber = 0;
    ColorDistanceAlgorithm colorAlgo = ColorDistanceAlgorithm::Euclidean;
    DitheringMethod ditheringMethod = DitheringMethod::None;
    MapBuildMethod buildMethod = MapBuildMethod::None;
    int rsW = -1;
    int rsH = -1;
    bool yesForced = false;
    unsigned int threadNum = max((unsigned int)1, std::thread::hardware_concurrency());
    string materialsOutFile = "";
    Color background = {255, 255, 255};

    // Load arguments
    for (int i = 3; i < argc; i++)
    {
        string arg(argv[i]);

        if (arg.compare(string("-o")) == 0 || arg.compare(string("--output")) == 0)
        {
            if ((i + 1) < argc)
            {
                outputPath = argv[i + 1];
                outputPathSet = true;
                i++;
            }
            else
            {
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
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
                else if (outputFormatStr.compare(string("structure")) == 0)
                {
                    outFormat = MapOutputFormat::Structure;
                    if (buildMethod == MapBuildMethod::None)
                    {
                        buildMethod = MapBuildMethod::Chaos;
                    }
                }
                else if (outputFormatStr.compare(string("structure-single")) == 0)
                {
                    outFormat = MapOutputFormat::StructureSingle;
                    if (buildMethod == MapBuildMethod::None)
                    {
                        buildMethod = MapBuildMethod::Chaos;
                    }

                    if (!outputPathSet)
                    {
                        outputPath = "mapart.nbt";
                    }
                }
                else if (outputFormatStr.compare(string("schematic")) == 0)
                {
                    outFormat = MapOutputFormat::Schematic;
                    if (buildMethod == MapBuildMethod::None)
                    {
                        buildMethod = MapBuildMethod::Chaos;
                    }
                }
                else if (outputFormatStr.compare(string("schematic-single")) == 0)
                {
                    outFormat = MapOutputFormat::SchematicSingle;
                    if (buildMethod == MapBuildMethod::None)
                    {
                        buildMethod = MapBuildMethod::Chaos;
                    }

                    if (!outputPathSet)
                    {
                        outputPath = "mapart.schem";
                    }
                }
                else if (outputFormatStr.compare(string("function")) == 0)
                {
                    outFormat = MapOutputFormat::Function;
                    if (buildMethod == MapBuildMethod::None)
                    {
                        buildMethod = MapBuildMethod::Flat;
                    }
                }
                else
                {
                    std::cerr << "Unrecognized outout format: " << argv[i + 1] << endl;
                    std::cerr << "Available formats: map, structure, structure-single, schematic, schematic-single, function" << endl;
                    return 1;
                }

                i++;
            }
            else
            {
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-mv")) == 0 || arg.compare(string("--mc-version")) == 0)
        {
            if ((i + 1) < argc)
            {
                version = getVersionFromText(string(argv[i + 1]));
                i++;
                if (version == McVersion::UNKNOWN)
                {
                    std::cerr << "Unrecognized version: " << argv[i + 1] << endl;
                    std::cerr << "Available versions: last, 1.17, 1.16, 1.15, 1.14, 1.13, 1.12" << endl;
                    return 1;
                }
            }
            else
            {
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-bg")) == 0 || arg.compare(string("--background")) == 0)
        {
            if ((i + 1) < argc)
            {
                background = colors::colorFromHex(string(argv[i + 1]));
                i++;
            }
            else
            {
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-rs")) == 0 || arg.compare(string("--resize")) == 0)
        {
            if ((i + 1) < argc)
            {
                string resizeString(argv[i + 1]);
                size_t xIndex;

                xIndex = resizeString.find("x");

                if (xIndex == string::npos || xIndex < 1)
                {
                    xIndex = resizeString.find("X");
                    if (xIndex == string::npos || xIndex < 1)
                    {
                        std::cerr << "Size must be provided using the WIDTHxHEIGHT format. Example: '128x128'" << endl;
                        std::cerr << "For help type: mcmap --help" << endl;
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
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
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
                    std::cerr << "Unrecognized color method: " << argv[i + 1] << endl;
                    std::cerr << "Available methods: euclidean, delta-e" << endl;
                    return 1;
                }

                i++;
            }
            else
            {
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-d")) == 0 || arg.compare(string("--dithering")) == 0)
        {
            if ((i + 1) < argc)
            {
                ditheringMethod = parseDitheringMethodFromString(string(argv[i + 1]));
                if (ditheringMethod == DitheringMethod::Unknown)
                {
                    std::cerr << "Unrecognized dithering: " << argv[i + 1] << endl;
                    std::cerr << "Available dithering methods: none, floyd-steinberg, min-average-error, burkes, sierra-lite, stucki, atkinson, bayer-44, bayer-22, ordered-33" << endl;
                    return 1;
                }
                i++;
            }
            else
            {
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
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
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-t")) == 0 || arg.compare(string("--threads")) == 0)
        {
            if ((i + 1) < argc)
            {
                threadNum = atoi(argv[i + 1]);
                if (threadNum == 0)
                {
                    threadNum = max((unsigned int)1, std::thread::hardware_concurrency());
                }
                i++;
            }
            else
            {
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-bm")) == 0 || arg.compare(string("--build-method")) == 0)
        {
            string buildMethodStr(argv[i + 1]);

            if (buildMethodStr.compare(string("3d")) == 0 || buildMethodStr.compare(string("chaos")) == 0)
            {
                buildMethod = MapBuildMethod::Chaos;
            }
            else if (buildMethodStr.compare(string("2d")) == 0 || buildMethodStr.compare(string("flat")) == 0)
            {
                buildMethod = MapBuildMethod::Flat;
            }
            else if (buildMethodStr.compare(string("stair")) == 0 || buildMethodStr.compare(string("staircase")) == 0 || buildMethodStr.compare(string("staircased")) == 0)
            {
                buildMethod = MapBuildMethod::Staircased;
            }
            else
            {
                std::cerr << "Unrecognized building method: " << argv[i + 1] << endl;
                std::cerr << "Available methods: 3d, 2d, stair" << endl;
                return 1;
            }

            i++;
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
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else if (arg.compare(string("-y")) == 0 || arg.compare(string("--yes")) == 0)
        {
            yesForced = true;
        }
        else if (arg.compare(string("-m")) == 0 || arg.compare(string("--materials")) == 0)
        {
            if ((i + 1) < argc)
            {
                materialsOutFile = argv[i + 1];
                i++;
            }
            else
            {
                std::cerr << "Option " << arg << " requires a parameter." << endl;
                std::cerr << "For help type: mcmap --help" << endl;
                return 1;
            }
        }
        else
        {
            std::cerr << "Unrecognized option: " << arg << endl;
            std::cerr << "For help type: mcmap --help" << endl;
            return 1;
        }
    }

    if (!yesForced && filesystem::exists(filesystem::path(outputPath)))
    {
        std::cerr << "The folder '" << outputPath << "' already exists. May contain another map art." << endl;
        std::cerr << "Do you want to overwrite? (Y/N): ";
        string line;
        getline(cin, line);
        if (line.at(0) != 'Y' && line.at(0) != 'y')
        {
            // Cancel
            return 0;
        }
    }

    if (outFormat != MapOutputFormat::StructureSingle && !filesystem::exists(filesystem::path(outputPath)))
    {
        // Create dir if not found
        filesystem::create_directory(filesystem::path(outputPath));
    }

    // Initializae progress report thread
    threading::Progress p;
    thread progressReportThread(progressReporter, std::ref(p));

    // Image library init
    wxInitAllImageHandlers();

    // Load input image
    p.startTask("Loading image...", 0, 0);
    wxImage image;
    wxLogNull logNo;
    if (!image.LoadFile(inputImageFile))
    {
        p.setEnded();
        progressReportThread.join();
        std::cerr << endl
                  << "Cannot load image: " << inputImageFile << endl;
        return 1;
    }

    // Resize image if required
    if (rsW > 0 || rsH > 0)
    {
        p.startTask("Resizing image...", 0, 0);
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
    p.startTask("Adjusting image size...", 0, 0);
    int matrixW;
    int matrixH;
    vector<Color> imageColorsMatrix = loadColorMatrixFromImageAndPad(image, background, &matrixW, &matrixH);

    // Load colors
    p.startTask("Loading minecraft colors...", 0, 0);
    std::vector<colors::Color> baseColors = minecraft::loadBaseColors(version);
    std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
    std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
    std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
    std::vector<bool> enabledConf(baseColors.size());
    MaterialsList materials(baseColorNames);

    for (size_t i = 0; i < enabledConf.size(); i++)
    {
        enabledConf[i] = true;
    }

    // Apply color set
    p.startTask("Loading custom configuration...", 0, 0);
    bool isBlacklist = true;
    if (colorSetFilename.size() > 0)
    {
        filesystem::path setsPath(getExecutableDir());
        string setFn = colorSetFilename + ".txt";

        setsPath /= "presets";
        setsPath /= setFn;

        string bsStr = readTextFile(setsPath.string());

        if (bsStr.size() > 0)
        {
            applyColorSet(bsStr, &isBlacklist, enabledConf, colorSet, blockSet, baseColorNames);
        }
        else
        {
            bsStr = readTextFile(colorSetFilename);
            if (bsStr.size() > 0)
            {
                applyColorSet(bsStr, &isBlacklist, enabledConf, colorSet, blockSet, baseColorNames);
            }
            else
            {
                p.setEnded();
                progressReportThread.join();
                std::cerr << endl
                          << "Custom configuration file not found or empty: " << colorSetFilename << endl;
                return 1;
            }
        }
    }

    // Apply color restrictions based on build method
    applyBuildRestrictions(colorSet, buildMethod);

    // Generate map art
    p.startTask("Adjusting image colors...", matrixH, threadNum);
    std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
    std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, imageColorsMatrix, matrixW, matrixH, colorAlgo, ditheringMethod, threadNum, p, countsMats);

    // Compute total maps
    int mapsCountX = matrixW / MAP_WIDTH;
    int mapsCountZ = matrixH / MAP_HEIGHT;

    // Do something different depending on the outout format
    if (outFormat == MapOutputFormat::Map)
    {
        // No need to build, just export to nbt map files
        p.startTask("Saving to map files...", mapsCountZ * mapsCountX, 1);
        int total = 0;
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                // Get map data
                std::vector<map_color_t> mapDataToSave = getMapDataFromColorMatrix(mapArtColorMatrix, matrixW, matrixH, mapX, mapZ);

                // Save to file
                stringstream ss;
                ss << "map_" << (mapNumber++) << ".dat";
                filesystem::path outFilePath(outputPath);

                outFilePath /= ss.str();

                try
                {
                    writeMapNBTFile(outFilePath.string(), mapDataToSave, version);
                }
                catch (...)
                {
                    p.setEnded();
                    progressReportThread.join();
                    std::cerr << endl
                              << "Cannot write file: " << outFilePath.string() << endl;
                    return 1;
                }

                total++;
                p.setProgress(0, total);
            }
        }

        // Finish
        p.setEnded();
        progressReportThread.join();
        std::cerr << endl
                  << "Successfully saved as map files to: " << outputPath << endl;
        std::cerr << "Note: The map numbers are sorted up to down, left to right" << endl;
    }
    else if (outFormat == MapOutputFormat::StructureSingle)
    {
        std::vector<std::vector<mapart::MapBuildingBlock>> chunks;

        p.startTask("Building maps...", 0, 0);
        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                stringstream ss;
                ss << "Building map (" << (total + 1) << "/" << totalMapsCount << ")...";
                p.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(version, blockSet, mapArtColorMatrix, matrixW, matrixH, mapX, mapZ, buildMethod, threadNum, p);

                // Add to materials list
                materials.addBlocks(buildingBlocks);

                // Add chunk
                chunks.push_back(buildingBlocks);

                total++;
            }
        }

        p.startTask("Building maps...", static_cast<unsigned int>(totalMapsCount), 1);

        try
        {
            if (buildMethod == MapBuildMethod::Flat)
            {
                writeStructureNBTFileCompactFlat(outputPath, chunks, mapsCountX, version, p);
            }
            else
            {
                writeStructureNBTFileCompact(outputPath, chunks, version, p);
            }
        }
        catch (...)
        {
            std::cerr << endl
                      << "Cannot write file: " << outputPath << endl;
            return 1;
        }

        p.setEnded();
        progressReportThread.join();

        if (materialsOutFile.size() > 0)
        {
            // Save materials
            if (!tools::writeTextFile(materialsOutFile, materials.toString()))
            {
                std::cerr << "Cannot write file: " << materialsOutFile << endl;
                return 1;
            }
        }
    }
    else if (outFormat == MapOutputFormat::SchematicSingle)
    {
        std::vector<std::vector<mapart::MapBuildingBlock>> chunks;

        p.startTask("Building maps...", 0, 0);
        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                stringstream ss;
                ss << "Building map (" << (total + 1) << "/" << totalMapsCount << ")...";
                p.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(version, blockSet, mapArtColorMatrix, matrixW, matrixH, mapX, mapZ, buildMethod, threadNum, p);

                // Add to materials list
                materials.addBlocks(buildingBlocks);

                // Add chunk
                chunks.push_back(buildingBlocks);

                total++;
            }
        }

        p.startTask("Building maps...", static_cast<unsigned int>(totalMapsCount), 1);

        try
        {
            if (buildMethod == MapBuildMethod::Flat)
            {
                writeSchematicNBTFileCompactFlat(outputPath, chunks, mapsCountX, version, p);
            }
            else
            {
                writeSchematicNBTFileCompact(outputPath, chunks, version, p);
            }
        }
        catch (...)
        {
            std::cerr << endl
                      << "Cannot write file: " << outputPath << endl;
            return 1;
        }

        p.setEnded();
        progressReportThread.join();

        if (materialsOutFile.size() > 0)
        {
            // Save materials
            if (!tools::writeTextFile(materialsOutFile, materials.toString()))
            {
                std::cerr << "Cannot write file: " << materialsOutFile << endl;
                return 1;
            }
        }
    }
    else
    {
        p.startTask("Building maps...", 0, 0);
        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                stringstream ss;
                ss << "Building map (" << (total + 1) << "/" << totalMapsCount << ")...";
                p.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(version, blockSet, mapArtColorMatrix, matrixW, matrixH, mapX, mapZ, buildMethod, threadNum, p);

                // Add to materials list
                materials.addBlocks(buildingBlocks);

                // Save
                if (outFormat == MapOutputFormat::Structure)
                {
                    // Save as structure file
                    stringstream ss2;
                    ss2 << "map_" << (total + 1) << ".nbt";
                    filesystem::path outFilePath(outputPath);

                    outFilePath /= ss2.str();

                    try
                    {
                        writeStructureNBTFile(outFilePath.string(), buildingBlocks, version, false);
                    }
                    catch (...)
                    {
                        p.setEnded();
                        progressReportThread.join();
                        std::cerr << endl
                                  << "Cannot write file: " << outFilePath.string() << endl;
                        return 1;
                    }
                }
                else if (outFormat == MapOutputFormat::Schematic)
                {
                    // Save as schematic file
                    stringstream ss2;
                    ss2 << "map_" << (total + 1) << ".schem";
                    filesystem::path outFilePath(outputPath);

                    outFilePath /= ss2.str();

                    try
                    {
                        writeSchematicNBTFile(outFilePath.string(), buildingBlocks, version, false);
                    }
                    catch (...)
                    {
                        p.setEnded();
                        progressReportThread.join();
                        std::cerr << endl
                                  << "Cannot write file: " << outFilePath.string() << endl;
                        return 1;
                    }
                }
                else if (outFormat == MapOutputFormat::Function)
                {
                    // Save as structure file
                    stringstream ss2;
                    ss2 << "map_" << (total + 1) << ".mcfunction";
                    filesystem::path outFilePath(outputPath);

                    outFilePath /= ss2.str();

                    try
                    {
                        writeMcFunctionFile(outFilePath.string(), buildingBlocks, version);
                    }
                    catch (...)
                    {
                        p.setEnded();
                        progressReportThread.join();
                        std::cerr << endl
                                  << "Cannot write file: " << outFilePath.string() << endl;
                        return 1;
                    }
                }

                total++;
            }
        }

        p.setEnded();
        progressReportThread.join();

        if (materialsOutFile.size() > 0)
        {
            // Save materials
            if (!tools::writeTextFile(materialsOutFile, materials.toString()))
            {
                std::cerr << endl
                          << "Cannot write file: " << materialsOutFile << endl;
                return 1;
            }
        }

        if (outFormat == MapOutputFormat::Structure)
        {
            std::cerr << endl
                      << "Successfully saved as structure files to: " << outputPath << endl;
            if (materialsOutFile.size() > 0)
            {
                std::cerr << "Materials list saved to: " << materialsOutFile << endl;
            }
            std::cerr << "Note: The map numbers are sorted up to down, left to right" << endl;
        }
        else if (outFormat == MapOutputFormat::Function)
        {
            std::cerr << endl
                      << "Successfully saved as function files to: " << outputPath << endl;
            if (materialsOutFile.size() > 0)
            {
                std::cerr << "Materials list saved to: " << materialsOutFile << endl;
            }
            std::cerr << "Note: The map numbers are sorted up to down, left to right" << endl;
        }
        else
        {
            std::cerr << endl
                      << "Saved!" << endl;
        }
    }

    return 0;
}

void progressReporter(threading::Progress &progress)
{
    bool ended = false;
    pair<string, unsigned int> p;
    string progressLine = "";
    size_t old_line_size = 0;

    while (!ended)
    {
        // Sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(REPORT_THREAD_DELAY));

        // Get progress
        p = progress.getProgress();

        // Print progress
        if (p.second == NO_PROGRESS)
        {
            progressLine = p.first;
        }
        else
        {
            stringstream ss;
            ss << p.first << "(" << p.second << "%)";
            progressLine = ss.str();
        }

        while (progressLine.length() < old_line_size)
        {
            progressLine.append(" ");
        }

        std::cerr << "\r" << progressLine;

        old_line_size = progressLine.length();

        // Check ended
        ended = progress.hasEnded();
    }

    // Erase line
    string eraser = "\r";
    for (size_t i = 0; i < progressLine.length(); i++)
    {
        eraser.append(" ");
    }
    std::cerr << eraser;
}
