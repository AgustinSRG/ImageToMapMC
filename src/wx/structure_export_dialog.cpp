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
#include "../tools/text_file.h"
#include "../tools/value_remember.h"
#include <filesystem>

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

StructureExportDialog::StructureExportDialog(minecraft::McVersion version, ExportDialogMode mode) : wxDialog(NULL, -1, wxString("Export map art"), wxDefaultPosition, wxSize(350, 230))
{

    this->version = version;
    this->mode = mode;

    wxStaticText *label1 = new wxStaticText(this, wxID_ANY, wxString("Choose a minecraft world folder:"), wxPoint(15, 15), wxSize(200, 15));

    filesystem::path savesPath(minecraft::getMinecraftFolderLocation());
    savesPath /= "saves";
    textFolder = new wxTextCtrl(this, wxID_ANY, wxString(savesPath.string()), wxPoint(15, 35), wxSize(305, 20));
    wxButton *browseButton = new wxButton(this, ID_Browse, wxString("Browse..."), wxPoint(15, 60), wxSize(80, 30));

    wxStaticText *label2 = new wxStaticText(this, wxID_ANY, wxString("Namespace, or empty for 'minecraft':"), wxPoint(15, 100), wxSize(300, 15));
    textNamespace = new wxTextCtrl(this, wxID_ANY, wxString(""), wxPoint(15, 120), wxSize(305, 20));

    wxButton *okButton = new wxButton(this, ID_OK, wxString("Export"), wxPoint(220, 150), wxSize(100, 30));
    wxButton *cancelButton = new wxButton(this, ID_Cancel, wxString("Cancel"), wxPoint(110, 150), wxSize(100, 30));

    Centre();
}

void StructureExportDialog::OnShow(wxShowEvent& event) {
    if (event.IsShown() && textFolder != NULL) {
        std::string rememberPath = tools::getRememberedValue(tools::VALUE_PURPOSE_EXPORT_STRUCTURES);

        if (rememberPath.length() > 0) {
            textFolder->ChangeValue(rememberPath);
        }
    }
}

void StructureExportDialog::OnOk(wxCommandEvent &event)
{
    filesystem::path worldPath(std::string(textFolder->GetValue()));
    if (!filesystem::exists(worldPath.string()))
    {
        wxMessageBox(string("Cannot find the folder: ") + worldPath.string(), wxT("Error"), wxICON_ERROR);
        return;
    }
    if (!filesystem::is_directory(worldPath.string()))
    {
        wxMessageBox(string("Cannot find the folder: ") + worldPath.string(), wxT("Error"), wxICON_ERROR);
        return;
    }

    // Check if it's a minecraft world
    filesystem::path levelDatPath = worldPath / "level.dat";
    if (!filesystem::exists(levelDatPath.string()))
    {
        wxMessageBox(string("Not a valid minecraft world: ") + worldPath.string(), wxT("Error"), wxICON_ERROR);
        return;
    }

    // Create structures folder
    filesystem::create_directories(getPath());
    createDataPackMetadata();

    tools::setRememberedValue(tools::VALUE_PURPOSE_EXPORT_STRUCTURES, getPath());

    EndModal(wxID_OK);
}

void StructureExportDialog::OnCancel(wxCommandEvent &event)
{
    EndModal(wxID_CANCEL);
}

void StructureExportDialog::OnBrowse(wxCommandEvent &event)
{

    filesystem::path savesPath(minecraft::getMinecraftFolderLocation());

    savesPath /= "saves";

    wxDirDialog dialog(this, wxString("Choose the output folder"), savesPath.string(), wxDD_DIR_MUST_EXIST);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    tools::setRememberedValue(tools::VALUE_PURPOSE_EXPORT_STRUCTURES, dialog.GetPath().ToStdString());

    textFolder->ChangeValue(dialog.GetPath());
}

void StructureExportDialog::createDataPackMetadata()
{
    filesystem::path dataPackPath(std::string(textFolder->GetValue()));

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
    filesystem::path structuresPath(std::string(textFolder->GetValue()));
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
