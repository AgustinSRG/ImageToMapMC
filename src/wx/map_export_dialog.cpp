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
#include <filesystem>

using namespace std;

enum Identifiers
{
    ID_OK = 1,
    ID_Cancel = 2,
    ID_Browse = 3,
};

BEGIN_EVENT_TABLE(MapExportDialog, wxDialog)
EVT_BUTTON(ID_OK, MapExportDialog::OnOk)
EVT_BUTTON(ID_Cancel, MapExportDialog::OnCancel)
EVT_BUTTON(ID_Browse, MapExportDialog::OnBrowse)
END_EVENT_TABLE()

MapExportDialog::MapExportDialog() : wxDialog(NULL, -1, wxString("Export to map files"), wxDefaultPosition, wxSize(350, 230))
{
    wxStaticText * label1 = new wxStaticText(this, wxID_ANY, wxString("Choose a folder:"), wxPoint(15, 15), wxSize(200, 15));

    textFolder = new wxTextCtrl(this, wxID_ANY, wxString("mapart"), wxPoint(15, 35), wxSize(305, 20)); 
    wxButton * browseButton = new wxButton(this, ID_Browse, wxString("Browse..."), wxPoint(15, 60), wxSize(80, 30));

    wxStaticText * label2 = new wxStaticText(this, wxID_ANY, wxString("Start with map number:"), wxPoint(15, 100), wxSize(200, 15));
    textMapNumber = new wxTextCtrl(this, wxID_ANY, wxString("0"), wxPoint(15, 120), wxSize(305, 20)); 
    
    wxButton * okButton = new wxButton(this, ID_OK, wxString("Export"), wxPoint(220, 150), wxSize(100, 30));
    wxButton * cancelButton = new wxButton(this, ID_Cancel, wxString("Cancel"), wxPoint(110, 150), wxSize(100, 30));
   
    Centre();
}

void MapExportDialog::OnOk(wxCommandEvent &event)
{
    if (!filesystem::exists(getPath())) {
        wxMessageBox(string("Cannot find the folder: ") + getPath(), wxT("Error"), wxICON_ERROR);
        return;
    }
    if (!filesystem::is_directory(getPath())) {
        wxMessageBox(string("Cannot find the folder: ") + getPath(), wxT("Error"), wxICON_ERROR);
        return;
    }
    if (!filesystem::is_empty(getPath())) {
        if (wxMessageBox(string("The folder is not empty ") + getPath() + "\nWant to export anyway? This may overwrite map files if you have them in that folder.", wxT("Warning"), wxICON_WARNING | wxYES_NO) == wxID_CANCEL) {
            return;
        }
    }
    EndModal(wxID_OK);
}

void MapExportDialog::OnCancel(wxCommandEvent &event)
{
    EndModal(wxID_CANCEL);
}

void MapExportDialog::OnBrowse(wxCommandEvent &event) {
    wxDirDialog dialog(this, wxString("Choose the output folder"), wxEmptyString, wxDD_DIR_MUST_EXIST);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    textFolder->ChangeValue(dialog.GetPath());
}

std::string MapExportDialog::getPath()
{
    return textFolder->GetValue();
}

int MapExportDialog::getMapNumber()
{
    return atoi(textMapNumber->GetValue().c_str());
}
