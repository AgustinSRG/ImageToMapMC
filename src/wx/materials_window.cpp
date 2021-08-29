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

#include "materials_window.h"
#include "../resources/icon.xpm"
#include <wx/sizer.h>

#define MAX_COLOR_GROUPS (64)

#define MIN_ID_CHECHBOX (10200)
#define MIN_ID_PANEL (10300)
#define MIN_ID_LABEL (10400)
#define MIN_ID_COMBO (10500)

using namespace std;
using namespace colors;
using namespace minecraft;
using namespace mapart;

enum Identifiers
{
    ID_File_Load = 1,
    ID_File_Save = 2,
    ID_BlackList_ON = 3,
    ID_BlackList_OFF = 4
};

BEGIN_EVENT_TABLE(MaterialsWindow, wxFrame)
EVT_SIZE(MaterialsWindow::OnSize)
EVT_CLOSE(MaterialsWindow::OnClose)
EVT_MENU(ID_File_Load, MaterialsWindow::onLoadFile)
EVT_MENU(ID_File_Save, MaterialsWindow::onSaveFile)
EVT_MENU(ID_BlackList_ON, MaterialsWindow::SetBlackList)
EVT_MENU(ID_BlackList_OFF, MaterialsWindow::SetWhiteList)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MaterialsPanel, wxScrolledWindow)
END_EVENT_TABLE()

MaterialsPanel::MaterialsPanel(MaterialsWindow *matWin) : wxScrolledWindow(matWin, wxID_ANY, wxPoint(0, 0), matWin->GetClientSize())
{
    materialsWindow = matWin;

    wxFlexGridSizer *sizer = new wxFlexGridSizer(MAX_COLOR_GROUPS, 4, wxSize(5, 5));

    vector<Color> colors = minecraft::loadBaseColors(MC_LAST_VERSION);
    vector<string> colorNames = minecraft::loadBaseColorNames(colors);

    groups.resize(MAX_COLOR_GROUPS);
    enabledConf.resize(MAX_COLOR_GROUPS);
    blacklist = true;

    for (int i = 1; i < MAX_COLOR_GROUPS; i++)
    {
        groups[i].checkBox = new wxCheckBox(this, MIN_ID_CHECHBOX + i, wxString(""), wxPoint(5, 5), wxSize(24, 24));
        groups[i].checkBox->Bind(wxEVT_CHECKBOX, &MaterialsPanel::onCheckBoxChanged, this);
        sizer->Add(groups[i].checkBox, wxSizerFlags(0).Border(wxALL, 10).Align(wxALIGN_CENTER_VERTICAL));

        groups[i].colorPanel = new wxPanel(this, MIN_ID_PANEL + i, wxPoint(5, 5), wxSize(48, 48));
        if (i < colors.size())
        {
            groups[i].colorPanel->SetBackgroundColour(wxColour(colors[i].red, colors[i].green, colors[i].blue));
        }
        else
        {
            groups[i].colorPanel->SetBackgroundColour(wxColour(0, 0, 0));
        }

        sizer->Add(groups[i].colorPanel, wxSizerFlags(0).Border(wxALL, 10).Align(wxALIGN_CENTER_VERTICAL));

        if (i < colorNames.size())
        {
            groups[i].colorLabel = new wxStaticText(this, MIN_ID_LABEL + i, wxString(colorNames[i]));
        }
        else
        {
            groups[i].colorLabel = new wxStaticText(this, MIN_ID_LABEL + i, wxString("COLOR_NAME"));
        }

        sizer->Add(groups[i].colorLabel, wxSizerFlags(0).Border(wxALL, 10).Align(wxALIGN_CENTER_VERTICAL));

        wxArrayString options;
        options.Add("block_id");
        options.Add("test_block");
        groups[i].combo = new wxComboBox(this, MIN_ID_COMBO + i, wxString("block_id"), wxPoint(5, 5), wxSize(160, 40), options);
        groups[i].combo->Bind(wxEVT_COMBOBOX, &MaterialsPanel::onComboBoxChanged, this);
        sizer->Add(groups[i].combo, wxSizerFlags(0).Border(wxALL, 10).Align(wxALIGN_CENTER_VERTICAL));
    }

    SetSizer(sizer);

    // this part makes the scrollbars show up
    this->FitInside();
    this->SetScrollRate(5, 10);
}

MaterialsWindow::MaterialsWindow(MainWindow *mainWindow) : wxFrame(mainWindow, wxID_ANY, string("Customize materials"), wxPoint(100, 100), wxSize(600, 600))
{
    this->mainWindow = mainWindow;
    SetIcon(wxIcon(_ICON_ICO_XPM));

    /* Menu Bar */
    menuBar = new wxMenuBar();

    // File
    wxMenu *menuFile = new wxMenu();

    menuFile->Append(ID_File_Load, "&Load configuration file", "");

    menuFile->AppendSeparator();

    menuFile->Append(ID_File_Save, "&Save configuration file", "");

    menuBar->Append(menuFile, "&File");

    // Mode
    wxMenu *menuMode = new wxMenu();
    menuMode->AppendRadioItem(ID_BlackList_ON, "&BlackList", "");
    menuMode->AppendRadioItem(ID_BlackList_OFF, "&Whitelist", "");
    menuBar->Append(menuMode, "&Mode");

    SetMenuBar(menuBar);

    panel = new MaterialsPanel(this);
}

MaterialsWindow::~MaterialsWindow()
{
}

void MaterialsWindow::OnClose(wxCloseEvent &event)
{
    if (event.CanVeto())
    {
        event.Veto();
    }

    Hide();
}

void MaterialsPanel::onCheckBoxChanged(wxCommandEvent &evt)
{
    int index  = evt.GetId() - MIN_ID_CHECHBOX;

    if (index >= 0 && index < enabledConf.size()) {
        enabledConf[index] = groups[index].checkBox->IsChecked();
    }
    onConfigChanged();
}

void MaterialsPanel::onComboBoxChanged(wxCommandEvent &evt)
{
    int index  = evt.GetId() - MIN_ID_COMBO;
    if (index >= 0 && index < blockSet.size()) {
        blockSet[index].useBlockIndex = groups[index].combo->GetSelection();
    }
    onConfigChanged();
}

void MaterialsWindow::OnSize(wxSizeEvent &event)
{
    if (panel == NULL)
        return;
    panel->SetSize(this->GetClientSize());
}

std::string MaterialsPanel::getMaterialsConf() {
    return buildColorSetString(blockSet, blacklist, enabledConf, baseColorNames, version);
}

void MaterialsPanel::onConfigChanged() {
    this->materialsWindow->mainWindow->changeColorSetConf(getMaterialsConf());
}

void MaterialsPanel::setMaterialsConf(minecraft::McVersion version, std::string conf)
{
    this->version = version;
    baseColors = minecraft::loadBaseColors(version);
    colorSet = minecraft::loadFinalColors(baseColors);
    blockSet = loadBlocks(baseColors);
    baseColorNames = loadBaseColorNames(baseColors);
    mapart::applyColorSet(conf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);

    if (blacklist)
    {
        materialsWindow->menuBar->GetMenu(1)->FindItemByPosition(1)->Check(false);
        materialsWindow->menuBar->GetMenu(1)->FindItemByPosition(0)->Check(true);
    }
    else
    {
        materialsWindow->menuBar->GetMenu(1)->FindItemByPosition(0)->Check(false);
        materialsWindow->menuBar->GetMenu(1)->FindItemByPosition(1)->Check(true);
    }

    for (int i = 1; i < MAX_COLOR_GROUPS; i++)
    {

        if (i < baseColors.size())
        {
            groups[i].checkBox->Show();
            groups[i].colorLabel->Show();
            groups[i].colorPanel->Show();
            groups[i].combo->Show();

            groups[i].checkBox->SetValue(enabledConf[i]);

            wxArrayString options;

            int count = 0;

            for (int j = 0; j < blockSet[i].blocks.size(); j++)
            {
                if (blockSet[i].blocks[j].getBlockDescription(version) != NULL)
                {
                    count++;
                }
            }

            if (count > 0)
            {
                groups[i].combo->Clear();

                for (int j = 0; j < blockSet[i].blocks.size(); j++)
                {

                    if (blockSet[i].blocks[j].getBlockDescription(version) != NULL)
                    {
                        groups[i].combo->Append(blockSet[i].blocks[j].getBlockDescription(version)->name);
                    }
                }

                if (blockSet[i].useBlockIndex >= 0 && blockSet[i].useBlockIndex < blockSet[i].blocks.size())
                {
                    groups[i].combo->SetSelection(blockSet[i].useBlockIndex);
                }
                else
                {
                    groups[i].combo->SetSelection(0);
                }
            }
            else
            {
                groups[i].combo->Clear();
                groups[i].combo->Append("Nothing");
                groups[i].combo->SetSelection(0);
            }
        }
        else
        {
            groups[i].checkBox->Hide();
            groups[i].colorLabel->Hide();
            groups[i].colorPanel->Hide();
            groups[i].combo->Hide();
        }
    }
}

void MaterialsWindow::setMaterialsConf(minecraft::McVersion version, std::string conf)
{
    panel->setMaterialsConf(version, conf);
}

void MaterialsWindow::onLoadFile(wxCommandEvent &evt)
{
}

void MaterialsWindow::onSaveFile(wxCommandEvent &evt)
{
}

void MaterialsWindow::SetBlackList(wxCommandEvent &evt)
{
    panel->blacklist = true;
    panel->onConfigChanged();
}

void MaterialsWindow::SetWhiteList(wxCommandEvent &evt)
{
    panel->blacklist = false;
    panel->onConfigChanged();
}
