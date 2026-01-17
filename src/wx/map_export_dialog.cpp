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

#include "map_export_dialog.h"
#include "../tools/fs.h"
#include "../tools/value_remember.h"
#include <sstream>

using namespace std;

enum Identifiers
{
    ID_OK = 1,
    ID_Cancel = 2,
    ID_Browse = 3,
    ID_Map_Id_Input = 4,
    ID_Checkbox_Open_Folder = 5
};

BEGIN_EVENT_TABLE(MapExportDialog, wxDialog)
EVT_BUTTON(ID_OK, MapExportDialog::OnOk)
EVT_BUTTON(ID_Cancel, MapExportDialog::OnCancel)
EVT_BUTTON(ID_Browse, MapExportDialog::OnBrowse)
EVT_TEXT(ID_Map_Id_Input, MapExportDialog::OnMapIdChanged)
EVT_CHECKBOX(ID_Checkbox_Open_Folder, MapExportDialog::OnCheckboxChanged)
EVT_CHAR_HOOK(MapExportDialog::OnKeyPress)
EVT_SHOW(MapExportDialog::OnShow)
END_EVENT_TABLE()

MapExportDialog::MapExportDialog(int projectMapCount) : wxDialog(NULL, -1, wxString("Export to map files"), wxDefaultPosition, wxDefaultSize)
{
    this->projectMapCount = projectMapCount;

    // Create sizers and wx components

    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    const int buttonWidth = 100;
    const int buttonHeight = 30;

    const int textWidth = 310;

    const int spacing = 5;

    // Folder select

    wxStaticText *label1 = new wxStaticText(this, wxID_ANY, wxString("Choose a folder:"), wxDefaultPosition, wxDefaultSize);

    sizerTop->Add(label1, 0, wxALL, spacing);

    textFolder = new wxTextCtrl(this, wxID_ANY, wxString("mapart"), wxDefaultPosition, wxSize(textWidth, -1));

    sizerTop->Add(textFolder, 0, wxALL, spacing);

    wxButton *browseButton = new wxButton(this, ID_Browse, wxString("Browse..."), wxDefaultPosition, wxSize(buttonWidth, buttonHeight));

    sizerTop->Add(browseButton, 0, wxALL, spacing);

    // Map number

    wxStaticText *label2 = new wxStaticText(this, wxID_ANY, wxString("Start with map number:"), wxDefaultPosition, wxDefaultSize);

    sizerTop->Add(label2, 0, wxALL, spacing);

    textMapNumber = new wxTextCtrl(this, ID_Map_Id_Input, wxString("0"), wxDefaultPosition, wxSize(textWidth, -1));

    sizerTop->Add(textMapNumber, 0, wxALL, spacing);

    // Checkbox

    checkboxOpenFolder = new wxCheckBox(
        this, ID_Checkbox_Open_Folder,
        wxString("Open folder after exporting the maps."),
        wxDefaultPosition,
        wxDefaultSize);
    
    checkboxOpenFolder->SetValue(tools::getRememberedValue(tools::VALUE_PURPOSE_OPEN_FOLDER_EXPORT).compare("false") != 0);

    sizerTop->Add(checkboxOpenFolder, 0, wxALL | wxALIGN_CENTER, spacing);

    // Buttons

    wxBoxSizer *sizerGroupButtons = new wxBoxSizer(wxHORIZONTAL);

    wxButton *cancelButton = new wxButton(
        this, ID_Cancel,
        wxString("Cancel"),
        wxDefaultPosition,
        wxSize(buttonWidth, buttonHeight));

    sizerGroupButtons->Add(cancelButton, 0, wxALL | wxALIGN_CENTER, spacing);

    sizerGroupButtons->AddSpacer(spacing * 2);

    wxButton *okButton = new wxButton(
        this, ID_OK,
        wxString("Export"),
        wxDefaultPosition,
        wxSize(buttonWidth, buttonHeight));

    sizerGroupButtons->Add(okButton, 0, wxALL | wxALIGN_CENTER, spacing);

    sizerTop->Add(sizerGroupButtons, 0, wxALL | wxALIGN_CENTER);

    // End of wx composition

    wxBoxSizer *sizerMain = new wxBoxSizer(wxVERTICAL);
    sizerMain->Add(sizerTop, 0, wxALL | wxALIGN_CENTER, spacing);
    sizerMain->SetSizeHints(this);
    SetSizerAndFit(sizerMain);

    Centre();
}

void MapExportDialog::OnShow(wxShowEvent &event)
{
    if (event.IsShown() && textFolder != NULL)
    {
        std::string rememberPath = tools::getRememberedValue(tools::VALUE_PURPOSE_EXPORT_MAPS);

        if (rememberPath.length() > 0)
        {
            textFolder->ChangeValue(wxString::FromUTF8(rememberPath));
            figureOutMapNumber(rememberPath);
        }
    }
}

void MapExportDialog::OnOk(wxCommandEvent &event)
{
    std::string path = getPath();

    if (!fs::exists(path))
    {
        if (wxMessageBox(wxString::FromUTF8(string("The folder does not exist: ") + getPath() + "\nDo you want to create it?"), wxT("Warning"), wxICON_WARNING | wxYES_NO) == wxYES) {
            fs::create_directories(path);
        } else {
            return;
        }
    }

    if (!fs::exists(path))
    {
        wxMessageBox(wxString::FromUTF8(std::string("Cannot find the folder: ") + path), wxT("Error"), wxICON_ERROR);
        return;
    }
    if (!fs::is_directory(path))
    {
        wxMessageBox(wxString::FromUTF8(std::string("Cannot find the folder: ") + path), wxT("Error"), wxICON_ERROR);
        return;
    }
    if (!fs::is_empty(path))
    {
        int startMapNumber = this->getMapNumber();

        bool foundOverwrite = false;
        stringstream ssOverWriteList;

        for (int i = 0; i < this->projectMapCount; i++)
        {
            int mapNumber = startMapNumber + i;

            stringstream ss;
            ss << "map_" << (mapNumber) << ".dat";

            std::string mapFileName = ss.str();

            fs::path outFilePath(path);
            outFilePath /= mapFileName;

            if (fs::exists(outFilePath))
            {
                if (foundOverwrite)
                {
                    ssOverWriteList << ", ";
                }

                ssOverWriteList << mapFileName;

                foundOverwrite = true;
            }
        }

        if (foundOverwrite && wxMessageBox(wxString::FromUTF8(string("The folder is not empty: ") + getPath() + "\nWant to export anyway? The following files will be overwritten:\n\n" + ssOverWriteList.str()), wxT("Warning"), wxICON_WARNING | wxYES_NO) != wxYES)
        {
            return;
        }
    }

    // Remember values for later

    tools::setRememberedValue(tools::VALUE_PURPOSE_EXPORT_MAPS, getPath());

    int nextMapNumber = this->getMapNumber() + this->projectMapCount;
    stringstream ss;
    ss << nextMapNumber;
    tools::setRememberedValue(tools::VALUE_PURPOSE_EXPORT_MAPS_LAST_ID, ss.str());

    EndModal(wxID_OK);
}

void MapExportDialog::OnCancel(wxCommandEvent &event)
{
    EndModal(wxID_CANCEL);
}

void MapExportDialog::OnBrowse(wxCommandEvent &event)
{
    wxDirDialog dialog(this, wxString("Choose the output folder"), textFolder->GetValue(), wxDD_DIR_MUST_EXIST);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    textFolder->ChangeValue(dialog.GetPath());
    figureOutMapNumber(dialog.GetPath().utf8_string());
    tools::setRememberedValue(tools::VALUE_PURPOSE_EXPORT_MAPS, dialog.GetPath().utf8_string());
}

std::string MapExportDialog::getPath()
{
    return textFolder->GetValue().utf8_string();
}

int MapExportDialog::getMapNumber()
{
    return atoi(textMapNumber->GetValue().c_str());
}

void MapExportDialog::OnMapIdChanged(wxCommandEvent &event)
{
    int mapNumber = this->getMapNumber();

    stringstream ss;

    ss << mapNumber;

    tools::setRememberedValue(tools::VALUE_PURPOSE_EXPORT_MAPS_LAST_ID, ss.str());
}

void MapExportDialog::OnKeyPress(wxKeyEvent &event)
{
    if (event.GetKeyCode() == WXK_ESCAPE)
    {
        EndModal(wxID_CANCEL);
        return;
    }

    event.Skip();
}

void MapExportDialog::figureOutMapNumber(std::string path)
{
    if (textMapNumber != NULL)
    {
        std::string lastRememberedIdString = tools::getRememberedValue(tools::VALUE_PURPOSE_EXPORT_MAPS_LAST_ID);
        int mapIdInt = atoi(lastRememberedIdString.c_str());

        stringstream ss;

        ss << mapIdInt;

        textMapNumber->SetValue(ss.str());
    }

    int lastMap = 0;
    bool foundAtLeastOneMap = false;

    try
    {
        for (const auto &entry : fs::directory_iterator(path))
        {
            std::string fileName = entry.path().filename().string();

            if (fileName.length() > 8 && fileName.substr(0, 4).compare("map_") == 0 && fileName.substr(fileName.length() - 4, 4).compare(".dat") == 0)
            {
                foundAtLeastOneMap = true;
                std::string mapId = fileName.substr(4, fileName.length() - 8);
                int mapIdInt = atoi(mapId.c_str());

                if (mapIdInt > lastMap)
                {
                    lastMap = mapIdInt;
                }
            }
        }
    }
    catch (...)
    {
        return;
    }

    if (textMapNumber != NULL)
    {
        if (foundAtLeastOneMap)
        {
            lastMap++;

            stringstream ss;

            ss << lastMap;

            textMapNumber->SetValue(ss.str());
        }
    }
}

bool MapExportDialog::mustOpenFolderAfterExport() {
    if (!this->checkboxOpenFolder) {
        return tools::getRememberedValue(tools::VALUE_PURPOSE_OPEN_FOLDER_EXPORT).compare("false") != 0;
    }

    return this->checkboxOpenFolder->IsChecked();
}

void MapExportDialog::OnCheckboxChanged(wxCommandEvent &event)
{
    if (this->mustOpenFolderAfterExport()) {
        tools::setRememberedValue(tools::VALUE_PURPOSE_OPEN_FOLDER_EXPORT, "true");
    } else {
        tools::setRememberedValue(tools::VALUE_PURPOSE_OPEN_FOLDER_EXPORT, "false");
    }
}
