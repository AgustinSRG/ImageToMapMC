// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".

#include "wx/img_display_window.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "You must specify a map file to open" << endl;
        return 1;
    }

    // Load colors
    std::vector<colors::Color> baseColors = minecraft::loadBaseColors(MC_LAST_VERSION);
    std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);

    try
    {
        std::vector<map_color_t> fileData = mapart::readMapNBTFile(argv[1]);
        std::vector<minecraft::FinalColor *> testVector = mapart::mapColorsToRGB(colorSet, fileData);

        widgets::displayMapImage(testVector, argc, argv);
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
        cerr << "Oops, an error ocurred." << ex.what() << endl;
        return 1;
    }
    catch (const std::string &ex)
    {
        cerr << "Oops, an error ocurred." << ex << endl;
        return 1;
    }
    catch (...)
    {
        cerr << "Oops, an error ocurred." << endl;
        return 1;
    }

    return 0;
}
