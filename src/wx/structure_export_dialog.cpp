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

#include "structure_export_dialog.h"
#include "../tools/fs.h"
#include "../tools/text_file.h"
#include "../tools/value_remember.h"

using namespace std;

enum Identifiers
{
    ID_OK = 1,
    ID_Cancel = 2,
    ID_Browse = 3,
};

BEGIN_EVENT_TABLE(StructureExportDialog, wxDialog)
EVT_BUTTON(ID_OK, StructureExportDialog::OnOk)
EVT_BUTTON(ID_Cancel, StructureExportDialog::OnCancel)
EVT_BUTTON(ID_Browse, StructureExportDialog::OnBrowse)
EVT_CHAR_HOOK(StructureExportDialog::OnKeyPress)
EVT_SHOW(StructureExportDialog::OnShow)
END_EVENT_TABLE()

StructureExportDialog::StructureExportDialog(minecraft::McVersion version, ExportDialogMode mode) : wxDialog(NULL, -1, wxString("Export map art"), wxDefaultPosition, wxDefaultSize)
{

    this->version = version;
    this->mode = mode;

    // Create sizers and wx components

    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    const int buttonWidth = 100;
    const int buttonHeight = 30;

    const int textWidth = 310;

    const int spacing = 5;

    // Folder select

    wxStaticText *label1 = new wxStaticText(this, wxID_ANY, wxString("Choose a minecraft world folder:"), wxDefaultPosition, wxDefaultSize);

    sizerTop->Add(label1, 0, wxALL, spacing);

    fs::path savesPath(minecraft::getMinecraftFolderLocation());
    savesPath /= "saves";
    textFolder = new wxTextCtrl(this, wxID_ANY, wxString(savesPath.string()), wxDefaultPosition, wxSize(textWidth, -1));

    sizerTop->Add(textFolder, 0, wxALL, spacing);

    wxButton *browseButton = new wxButton(this, ID_Browse, wxString("Browse..."), wxDefaultPosition, wxSize(buttonWidth, buttonHeight));

    sizerTop->Add(browseButton, 0, wxALL, spacing);

    // Namespace

    wxStaticText *label2 = new wxStaticText(this, wxID_ANY, wxString("Namespace, or empty for 'minecraft':"), wxDefaultPosition, wxDefaultSize);

    sizerTop->Add(label2, 0, wxALL, spacing);

    textNamespace = new wxTextCtrl(this, wxID_ANY, wxString(""), wxDefaultPosition, wxSize(textWidth, -1));

    sizerTop->Add(textNamespace, 0, wxALL, spacing);

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

    wxBoxSizer * sizerMain = new wxBoxSizer(wxVERTICAL);
    sizerMain->Add(sizerTop, 0, wxALL | wxALIGN_CENTER, spacing);
    sizerMain->SetSizeHints(this);
    SetSizerAndFit(sizerMain);

    Centre();
}

void StructureExportDialog::OnShow(wxShowEvent& event) {
    if (event.IsShown() && textFolder != NULL) {
        std::string rememberPath = tools::getRememberedValue(tools::VALUE_PURPOSE_EXPORT_STRUCTURES);

        if (rememberPath.length() > 0) {
            textFolder->ChangeValue(wxString::FromUTF8(rememberPath));
        }
    }
}

void StructureExportDialog::OnOk(wxCommandEvent &event)
{
    fs::path worldPath(textFolder->GetValue().utf8_string());
    if (!fs::exists(worldPath.string()))
    {
        wxMessageBox(string("Cannot find the folder: ") + worldPath.string(), wxT("Error"), wxICON_ERROR);
        return;
    }
    if (!fs::is_directory(worldPath.string()))
    {
        wxMessageBox(string("Cannot find the folder: ") + worldPath.string(), wxT("Error"), wxICON_ERROR);
        return;
    }

    // Check if it's a minecraft world
    fs::path levelDatPath = worldPath / "level.dat";
    if (!fs::exists(levelDatPath.string()))
    {
        wxMessageBox(string("Not a valid minecraft world: ") + worldPath.string(), wxT("Error"), wxICON_ERROR);
        return;
    }

    // Create structures folder
    fs::create_directories(getPath());
    createDataPackMetadata();

    tools::setRememberedValue(tools::VALUE_PURPOSE_EXPORT_STRUCTURES, textFolder->GetValue().utf8_string());

    EndModal(wxID_OK);
}

void StructureExportDialog::OnCancel(wxCommandEvent &event)
{
    EndModal(wxID_CANCEL);
}

void StructureExportDialog::OnBrowse(wxCommandEvent &event)
{
    std::string defaultPath = textFolder->GetValue().utf8_string();

    fs::path savesPath(minecraft::getMinecraftFolderLocation());

    savesPath /= "saves";

    if (defaultPath.length() == 0) {
        defaultPath = savesPath.string();
    }

    wxDirDialog dialog(this, wxString("Choose the output folder"), wxString::FromUTF8(defaultPath), wxDD_DIR_MUST_EXIST);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    tools::setRememberedValue(tools::VALUE_PURPOSE_EXPORT_STRUCTURES, dialog.GetPath().utf8_string());

    textFolder->ChangeValue(dialog.GetPath());
}

void StructureExportDialog::createDataPackMetadata()
{
    fs::path dataPackPath(std::string(textFolder->GetValue()));

    if (mode == ExportDialogMode::Function && version > minecraft::McVersion::MC_1_12)
    {
        dataPackPath /= "datapacks";

        dataPackPath /= "mcmap";
        dataPackPath /= "pack.mcmeta";

        tools::writeTextFile(dataPackPath.string(), std::string("{\"pack\":{\"pack_format\":9,\"description\":\"ImageToMapMC DataPack\"}}"));
    }
}

std::string StructureExportDialog::getPath()
{
    fs::path structuresPath(textFolder->GetValue().utf8_string());
    std::string namespaceMC(textNamespace->GetValue());

    if (mode == ExportDialogMode::Function)
    {
        if (version > minecraft::McVersion::MC_1_20)
        {
            structuresPath /= "datapacks";

            structuresPath /= "mcmap";

            structuresPath /= "data";

            if (namespaceMC.length() > 0)
            {
                structuresPath /= namespaceMC;
            }
            else
            {
                structuresPath /= "minecraft";
            }

            structuresPath /= "function";
        }
        else if (version > minecraft::McVersion::MC_1_12)
        {
            structuresPath /= "datapacks";

            structuresPath /= "mcmap";

            structuresPath /= "data";

            if (namespaceMC.length() > 0)
            {
                structuresPath /= namespaceMC;
            }
            else
            {
                structuresPath /= "minecraft";
            }

            structuresPath /= "functions";
        }
        else
        {
            structuresPath /= "data";

            structuresPath /= "functions";

            if (namespaceMC.length() > 0)
            {
                structuresPath /= namespaceMC;
            }
            else
            {
                structuresPath /= "minecraft";
            }
        }
    }
    else
    {
        if (version > minecraft::McVersion::MC_1_12)
        {
            structuresPath /= "generated";
            if (namespaceMC.length() > 0)
            {
                structuresPath /= namespaceMC;
            }
            else
            {
                structuresPath /= "minecraft";
            }

            structuresPath /= "structures";
        }
        else
        {
            structuresPath /= "structures";
            if (namespaceMC.length() > 0)
            {
                structuresPath /= namespaceMC;
            }
        }
    }

    return structuresPath.string();
}

void StructureExportDialog::OnKeyPress(wxKeyEvent &event) {
    if (event.GetKeyCode() == WXK_ESCAPE) {
        EndModal(wxID_CANCEL);
        return;
    }

    event.Skip();
}
