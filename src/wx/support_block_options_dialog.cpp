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

#include "support_block_options_dialog.h"
#include "../minecraft/mc_blocks.h"

enum Identifiers
{
    ID_OK = 1,
    ID_Combo_Material = 2,
    ID_CheckBox_Always = 3
};

BEGIN_EVENT_TABLE(SupportBlockOptionsDialog, wxDialog)
EVT_BUTTON(ID_OK, SupportBlockOptionsDialog::OnOk)
EVT_CLOSE(SupportBlockOptionsDialog::OnClose)
EVT_CHAR_HOOK(SupportBlockOptionsDialog::OnKeyPress)
EVT_COMBOBOX(ID_Combo_Material, SupportBlockOptionsDialog::OnComboBoxChanged)
EVT_CHECKBOX(ID_CheckBox_Always, SupportBlockOptionsDialog::OnCheckBoxChanged)
END_EVENT_TABLE()

SupportBlockOptionsDialog::SupportBlockOptionsDialog(MainWindow *mainWindow) : wxDialog(mainWindow, -1, wxString("Support blocks options"), wxDefaultPosition, wxDefaultSize)
{
    this->mainWindow = mainWindow;
    this->version = MC_LAST_VERSION;
    this->supportBlockMaterial = std::string("stone");
    this->supportBlocksAlways = true;
    supportBlockList = minecraft::loadSupportBlocks();

    // Create sizers and wx components

    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    const int buttonWidth = 100;
    const int buttonHeight = 30;

    const int spacing = 5;

    const int comboWidth = 160;
    const int comboHeight = 40;

    wxStaticText *labelMaterial = new wxStaticText(
        this, wxID_ANY,
        wxString("Material for support blocks: "),
        wxDefaultPosition,
        wxDefaultSize);

    sizerTop->Add(labelMaterial, 0, wxALL, spacing);

    wxArrayString options;
    options.Add("block_id");
    options.Add("test_block");
    comboMaterial = new wxComboBox(this, ID_Combo_Material, wxString("block_id"), wxPoint(5, 5), wxSize(comboWidth, comboHeight), options, wxCB_READONLY);

    sizerTop->Add(comboMaterial, 0, wxEXPAND | wxALL, spacing);

    sizerTop->AddSpacer(spacing);

    // Checkbox

    checkboxAlways = new wxCheckBox(
        this, ID_CheckBox_Always,
        wxString("Always place support blocks, even if unnecessary."),
        wxDefaultPosition,
        wxDefaultSize);

    checkboxAlways->SetValue(true);

    sizerTop->Add(checkboxAlways, 0, wxALL | wxALIGN_CENTER, spacing);

    // Action buttons

    wxBoxSizer *sizerGroupButtons = new wxBoxSizer(wxHORIZONTAL);

    wxButton *okButton = new wxButton(
        this, ID_OK,
        wxString("Done"),
        wxDefaultPosition,
        wxSize(buttonWidth, buttonHeight));

    sizerGroupButtons->Add(okButton, 0, wxALL | wxALIGN_CENTER, spacing);

    sizerTop->AddSpacer(spacing);
    sizerTop->Add(sizerGroupButtons, 0, wxALL | wxALIGN_CENTER);

    // End of wx composition

    wxBoxSizer *sizerMain = new wxBoxSizer(wxVERTICAL);
    sizerMain->Add(sizerTop, 0, wxALL | wxALIGN_CENTER, spacing);
    sizerMain->SetSizeHints(this);
    SetSizerAndFit(sizerMain);

    Centre();
}

void SupportBlockOptionsDialog::OnOk(wxCommandEvent &event)
{
    Hide();
}

void SupportBlockOptionsDialog::OnClose(wxCloseEvent &event)
{
    if (event.CanVeto())
    {
        event.Veto();
    }

    Hide();
}

void SupportBlockOptionsDialog::OnKeyPress(wxKeyEvent &event)
{
    if (event.GetKeyCode() == WXK_ESCAPE)
    {
        Hide();
        return;
    }

    event.Skip();
}

void SupportBlockOptionsDialog::OnChangeParams()
{
    mainWindow->onSupportBlockOptionsChanged(supportBlockMaterial, supportBlocksAlways);
}

void SupportBlockOptionsDialog::OnCheckBoxChanged(wxCommandEvent &evt)
{

    this->supportBlocksAlways = this->checkboxAlways->GetValue();

    OnChangeParams();
}

void SupportBlockOptionsDialog::OnComboBoxChanged(wxCommandEvent &evt)
{
    short materialIndex = supportBlockList.findBlockByVersionIndex(version, comboMaterial->GetSelection());

    if (materialIndex >= 0 && materialIndex < supportBlockList.blocks.size())
    {
        this->supportBlockMaterial = supportBlockList.blocks[materialIndex].id;
    }
    else
    {
        this->supportBlockMaterial = std::string("stone");
    }

    OnChangeParams();
}

void SupportBlockOptionsDialog::UpdateControls()
{
    comboMaterial->Clear();

    int i = 0;
    bool foundSelectedMaterial = false;

    for (int j = 0; j < this->supportBlockList.blocks.size(); j++)
    {

        if (this->supportBlockList.blocks[j].getBlockDescription(version) != NULL)
        {
            comboMaterial->Append(wxString::FromUTF8(this->supportBlockList.blocks[j].getBlockDescription(version)->name));

            if (this->supportBlockMaterial.compare(this->supportBlockList.blocks[j].id) == 0)
            {
                comboMaterial->SetSelection(i);
                foundSelectedMaterial = true;
            }

            i++;
        }
    }

    if (!foundSelectedMaterial)
    {
        comboMaterial->SetSelection(0);
    }

    checkboxAlways->SetValue(this->supportBlocksAlways);
}

void SupportBlockOptionsDialog::SetParams(minecraft::McVersion version, std::string supportBlockMaterial, bool supportBlocksAlways)
{
    this->version = version;
    this->supportBlockMaterial = supportBlockMaterial;
    this->supportBlocksAlways = supportBlocksAlways;

    UpdateControls();
}
