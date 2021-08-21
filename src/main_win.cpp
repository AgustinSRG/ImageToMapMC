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

#include "main_win.h"
#include "wx/img_display_window.h"
#include <wx/msgdlg.h>

using namespace std;

int mainEntryPoint(App &app, int argc, char *argv[]);
void displayError();

wxIMPLEMENT_APP(App);
// clang-format on

bool App::OnInit()
{
    if (mainEntryPoint(*this, this->argc, this->argv) != 0) {
        this->Exit();
    }

    return true;
}

void displayError(string str)
{
    wxMessageBox(wxString(str), wxT("Error"), wxICON_ERROR);
}

int mainEntryPoint(App &app, int argc, char *argv[])
{
    if (argc < 2)
    {
        displayError(string("You must specify a map file to open"));
        return 1;
    }

    // Load colors
    std::vector<colors::Color> baseColors = minecraft::loadBaseColors(MC_LAST_VERSION);
    std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);

    try
    {
        std::vector<map_color_t> fileData = mapart::readMapNBTFile(argv[1]);
        std::vector<const minecraft::FinalColor *> testVector = mapart::mapColorsToRGB(colorSet, fileData);

        widgets::displayMapImage(testVector, app);
    }
    catch (int code)
    {
        switch (code)
        {
        case -1:
            displayError(string("Cannot open file: ") + string(argv[1]));
            break;
        case -2:
            displayError(string("Invalid file: ") + string(argv[1]) + string(" is not a valid map NBT file."));
            break;
        }

        return 1;
    }
    catch (const std::exception &ex)
    {
        displayError(string(ex.what()));
        return 1;
    }
    catch (const std::string &ex)
    {
        displayError(ex);
        return 1;
    }
    catch (...)
    {
        displayError(string("Oops, an error ocurred."));
        return 1;
    }

    return 0;
}
