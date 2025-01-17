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
#include "../tools/text_file.h"
#include <wx/sizer.h>
#include <sstream>

#define MIN_ID_CHECHBOX (10200)
#define MIN_ID_PANEL (10300)
#define MIN_ID_LABEL (10400)
#define MIN_ID_COMBO (10500)
#define MIN_ID_COUNT_LABEL (10600)

using namespace std;
using namespace colors;
using namespace minecraft;
using namespace mapart;

enum Identifiers
{
    ID_File_Load = 1,
    ID_File_Save = 2,
    ID_BlackList_ON = 3,
    ID_BlackList_OFF = 4,
    ID_Timer = 5,

    ID_Preset_start = 800,
    ID_Preset_everything = 801,
    ID_Preset_bw = 802,
    ID_Preset_gray = 803,
    ID_Preset_wool = 804,
    ID_Preset_concrete = 805,
    ID_Preset_carpets = 806,
    ID_Preset_no_minerals = 807,
};

BEGIN_EVENT_TABLE(MaterialsWindow, wxFrame)
EVT_SIZE(MaterialsWindow::OnSize)
EVT_CLOSE(MaterialsWindow::OnClose)
EVT_MENU(ID_File_Load, MaterialsWindow::onLoadFile)
EVT_MENU(ID_File_Save, MaterialsWindow::onSaveFile)
EVT_MENU(ID_BlackList_ON, MaterialsWindow::SetBlackList)
EVT_MENU(ID_BlackList_OFF, MaterialsWindow::SetWhiteList)
EVT_MENU_RANGE(ID_Preset_start, ID_Preset_start + 99, MaterialsWindow::usePreset)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MaterialsPanel, wxScrolledWindow)
EVT_TIMER(ID_Timer, MaterialsPanel::onCountRefreshTimer)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ColorDisplayPanel, wxPanel)
EVT_PAINT(ColorDisplayPanel::paintEvent)
END_EVENT_TABLE()

ColorDisplayPanel::ColorDisplayPanel(wxWindow *parent, unsigned int index, colors::Color baseColor, mapart::MapBuildMethod buildMethod) : wxPanel(parent, MIN_ID_PANEL + index, wxPoint(5, 5), wxSize(48, 48))
{
    this->buildMethod = buildMethod;
    this->colorLight = minecraft::getMinecraftColorFromBase(baseColor, McColorType::LIGHT);
    this->colorNormal = minecraft::getMinecraftColorFromBase(baseColor, McColorType::NORMAL);
    this->colorDark = minecraft::getMinecraftColorFromBase(baseColor, McColorType::DARK);
    this->colorDarker = minecraft::getMinecraftColorFromBase(baseColor, McColorType::DARKER);
}

ColorDisplayPanel::~ColorDisplayPanel()
{
}

void ColorDisplayPanel::setBuildMethod(mapart::MapBuildMethod buildMethod)
{
    this->mu.Lock();
    this->buildMethod = buildMethod;
    this->mu.Unlock();

    this->Refresh();
}

void ColorDisplayPanel::paintEvent(wxPaintEvent &evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

void ColorDisplayPanel::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}

void ColorDisplayPanel::render(wxDC &dc)
{
    this->mu.Lock();

    dc.Clear();

    switch (this->buildMethod)
    {
    case MapBuildMethod::Chaos:
    case MapBuildMethod::Staircased:
        dc.SetBrush(wxBrush(wxColour(this->colorLight.red, this->colorLight.green, this->colorLight.blue)));
        dc.DrawRectangle(wxPoint(0, 0), wxSize(48, 16));
        dc.SetBrush(wxBrush(wxColour(this->colorNormal.red, this->colorNormal.green, this->colorNormal.blue)));
        dc.DrawRectangle(wxPoint(0, 16), wxSize(48, 16));
        dc.SetBrush(wxBrush(wxColour(this->colorDark.red, this->colorDark.green, this->colorDark.blue)));
        dc.DrawRectangle(wxPoint(0, 32), wxSize(48, 16));
        break;
    case MapBuildMethod::None:
        dc.SetBrush(wxBrush(wxColour(this->colorLight.red, this->colorLight.green, this->colorLight.blue)));
        dc.DrawRectangle(wxPoint(0, 0), wxSize(48, 12));
        dc.SetBrush(wxBrush(wxColour(this->colorNormal.red, this->colorNormal.green, this->colorNormal.blue)));
        dc.DrawRectangle(wxPoint(0, 12), wxSize(48, 12));
        dc.SetBrush(wxBrush(wxColour(this->colorDark.red, this->colorDark.green, this->colorDark.blue)));
        dc.DrawRectangle(wxPoint(0, 24), wxSize(48, 12));
        dc.SetBrush(wxBrush(wxColour(this->colorDarker.red, this->colorDarker.green, this->colorDarker.blue)));
        dc.DrawRectangle(wxPoint(0, 36), wxSize(48, 12));
        break;
    case MapBuildMethod::Flat:
        dc.SetBrush(wxBrush(wxColour(this->colorNormal.red, this->colorNormal.green, this->colorNormal.blue)));
        dc.DrawRectangle(wxPoint(0, 0), wxSize(48, 48));
        break;
    default:
        dc.SetBrush(wxBrush(wxColour(this->colorLight.red, this->colorLight.green, this->colorLight.blue)));
        dc.DrawRectangle(wxPoint(0, 0), wxSize(48, 48));
    }

    this->mu.Unlock();
}

MaterialsPanel::MaterialsPanel(MaterialsWindow *matWin) : wxScrolledWindow(matWin, wxID_ANY, wxPoint(0, 0), matWin->GetClientSize())
{
    materialsWindow = matWin;

    wxFlexGridSizer *sizer = new wxFlexGridSizer(MAX_COLOR_GROUPS, 5, wxSize(5, 5));

    vector<Color> colors = minecraft::loadBaseColors(MC_LAST_VERSION);
    vector<string> colorNames = minecraft::loadBaseColorNames(colors);

    groups.resize(MAX_COLOR_GROUPS);
    enabledConf.resize(MAX_COLOR_GROUPS);
    blacklist = true;

    for (unsigned int i = 1; i < MAX_COLOR_GROUPS; i++)
    {
        groups[i].checkBox = new wxCheckBox(this, MIN_ID_CHECHBOX + i, wxString(""), wxPoint(5, 5), wxSize(24, 24));
        groups[i].checkBox->Bind(wxEVT_CHECKBOX, &MaterialsPanel::onCheckBoxChanged, this);
        sizer->Add(groups[i].checkBox, wxSizerFlags(0).Border(wxALL, 10).Align(wxALIGN_CENTER_VERTICAL));

        if (i < colors.size())
        {
            groups[i].colorPanel = new ColorDisplayPanel(this, i, colors[i], this->buildMethod);
        }
        else
        {
            groups[i].colorPanel = new ColorDisplayPanel(this, i, colors::Color{0, 0, 0}, this->buildMethod);
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

        groups[i].countLabel = new wxStaticText(this, MIN_ID_COUNT_LABEL + i, wxString("0 Blocks"));
        sizer->Add(groups[i].countLabel, wxSizerFlags(0).Border(wxALL, 10).Align(wxALIGN_CENTER_VERTICAL));
    }

    SetSizer(sizer);

    // this part makes the scrollbars show up
    this->FitInside();
    this->SetScrollRate(5, 10);

    requiresCountRefresh = false;

    wxTimer *checkRefreshTimer = new wxTimer(this, ID_Timer);
    checkRefreshTimer->Start(200);
}

void MaterialsPanel::onCountRefreshTimer(wxTimerEvent &event)
{
    countMutex.Lock();
    if (requiresCountRefresh)
    {
        requiresCountRefresh = false;
        for (unsigned int i = 1; i < counts.size() && i < MAX_COLOR_GROUPS; i++)
        {
            size_t count = counts[i];
            stringstream ss;

            if (count >= SHULKER_BOX_AMOUNT)
            {
                double shulkers = (double)count / SHULKER_BOX_AMOUNT;
                shulkers = round(shulkers * 10) / 10;
                ss << shulkers << " Shulkers";
            }
            else if (count >= STACK_AMOUNT)
            {
                double stacks = (double)count / STACK_AMOUNT;
                stacks = round(stacks * 10) / 10;
                ss << stacks << " Stacks";
            }
            else
            {
                ss << count << " Blocks";
            }

            groups[i].countLabel->SetLabel(ss.str());
        }
    }
    countMutex.Unlock();
}

MaterialsWindow::MaterialsWindow(MainWindow *mainWindow) : wxFrame(mainWindow, wxID_ANY, string("Customize materials"), wxPoint(100, 100), wxSize(600, 600))
{
    this->mainWindow = mainWindow;
    panel = new MaterialsPanel(this);

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

    // Presets
    wxMenu *menuPresets = new wxMenu();
    menuPresets->AppendRadioItem(ID_Preset_everything, "&Everything", "");
    menuPresets->AppendRadioItem(ID_Preset_bw, "&Black and white", "");
    menuPresets->AppendRadioItem(ID_Preset_gray, "&Gray scale", "");
    menuPresets->AppendRadioItem(ID_Preset_carpets, "&Only carpets", "");
    menuPresets->AppendRadioItem(ID_Preset_wool, "&Only wool", "");
    menuPresets->AppendRadioItem(ID_Preset_concrete, "&Only concrete", "");
    menuPresets->AppendRadioItem(ID_Preset_no_minerals, "&No mineral blocks", "");
    menuBar->Append(menuPresets, "&Presets");

    SetMenuBar(menuBar);
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
    int index = evt.GetId() - MIN_ID_CHECHBOX;

    if (index >= 0 && index < enabledConf.size())
    {
        enabledConf[index] = groups[index].checkBox->IsChecked();
    }
    onConfigChanged();
}

void MaterialsPanel::onComboBoxChanged(wxCommandEvent &evt)
{
    int index = evt.GetId() - MIN_ID_COMBO;
    if (index >= 0 && index < blockSet.size())
    {
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

std::string MaterialsPanel::getMaterialsConf()
{
    return buildColorSetString(blockSet, blacklist, enabledConf, baseColorNames, version);
}

void MaterialsPanel::onConfigChanged()
{
    this->materialsWindow->mainWindow->changeColorSetConf(getMaterialsConf());
}

void MaterialsPanel::setBlacklistMode(bool blacklist)
{
    for (unsigned int i = 1; i < MAX_COLOR_GROUPS; i++)
    {
        enabledConf[i] = blacklist;
        groups[i].checkBox->SetValue(blacklist);
    }
}

void MaterialsPanel::setMaterialsConf(minecraft::McVersion version, mapart::MapBuildMethod buildMethod, std::string conf)
{
    this->version = version;
    this->buildMethod = buildMethod;
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

    for (unsigned int i = 1; i < MAX_COLOR_GROUPS; i++)
    {

        if (i < baseColors.size())
        {
            groups[i].checkBox->Show();
            groups[i].colorLabel->Show();
            groups[i].colorPanel->setBuildMethod(this->buildMethod);
            groups[i].colorPanel->Show();
            groups[i].combo->Show();
            groups[i].countLabel->Show();

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
            groups[i].countLabel->Hide();
            groups[i].combo->Hide();
        }
    }
}

void MaterialsWindow::setMaterialsConf(minecraft::McVersion version, mapart::MapBuildMethod buildMethod, std::string conf)
{
    panel->setMaterialsConf(version, buildMethod, conf);
}

void MaterialsWindow::onLoadFile(wxCommandEvent &evt)
{
    wxFileDialog
        openFileDialog(this, _("Load configuration"), "", "",
                       "Configuration files (*.txt, *.conf)|*.txt;*.conf|All files|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...

    panel->setMaterialsConf(panel->version, panel->buildMethod, tools::readTextFile(openFileDialog.GetPath().ToStdString()));
    mainWindow->changeColorSetConf(panel->getMaterialsConf());
}

void MaterialsWindow::onSaveFile(wxCommandEvent &evt)
{
    wxFileDialog saveFileDialog(this, _("Save Configuration"), "", "", "Configuration file (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }
    if (!tools::writeTextFile(saveFileDialog.GetPath().ToStdString(), panel->getMaterialsConf()))
    {
        wxMessageBox(wxString("Could not save the configuration due to a file system error."), wxT("Error"), wxICON_ERROR);
    }
}

void MaterialsWindow::SetBlackList(wxCommandEvent &evt)
{
    panel->blacklist = true;
    panel->setBlacklistMode(true);
    panel->onConfigChanged();
}

void MaterialsWindow::SetWhiteList(wxCommandEvent &evt)
{
    panel->blacklist = false;
    panel->setBlacklistMode(false);
    panel->onConfigChanged();
}

void MaterialsWindow::usePreset(wxCommandEvent &evt)
{
    switch (evt.GetId())
    {
    case ID_Preset_everything:
        panel->setMaterialsConf(panel->version, panel->buildMethod, "MODE(BLACKLIST)");
        break;
    case ID_Preset_bw:
        panel->setMaterialsConf(panel->version, panel->buildMethod, "MODE(WHITELIST)\nADD(SNOW,white_concrete)\nADD(COLOR_BLACK,black_concrete)");
        break;
    case ID_Preset_gray:
        panel->setMaterialsConf(panel->version, panel->buildMethod, "MODE(WHITELIST)\nADD(WOOL)\nADD(METAL)\nADD(SNOW)\nADD(STONE)\nADD(COLOR_GRAY)\nADD(COLOR_LIGHT_GRAY)\nADD(COLOR_BLACK)\nADD(DEEPSLATE)");
        break;
    case ID_Preset_carpets:
        panel->setMaterialsConf(panel->version, panel->buildMethod, "MODE(WHITELIST)\nADD(SNOW,white_carpet)\nADD(COLOR_MAGENTA,magenta_carpet)\nADD(COLOR_LIGHT_BLUE,light_blue_carpet)\nADD(COLOR_YELLOW,yellow_carpet)\nADD(COLOR_LIGHT_GREEN,lime_carpet)\nADD(COLOR_PINK,pink_carpet)\nADD(COLOR_GRAY,gray_carpet)\nADD(COLOR_LIGHT_GRAY,light_gray_carpet)\nADD(COLOR_CYAN,cyan_carpet)\nADD(COLOR_PURPLE,purple_carpet)\nADD(COLOR_BLUE,blue_carpet)\nADD(COLOR_BROWN,brown_carpet)\nADD(COLOR_GREEN,green_carpet)\nADD(COLOR_RED,red_carpet)\nADD(COLOR_BLACK,black_carpet)");
        break;
    case ID_Preset_concrete:
        panel->setMaterialsConf(panel->version, panel->buildMethod, "MODE(WHITELIST)\nADD(SNOW,white_concrete)\nADD(COLOR_MAGENTA,magenta_concrete)\nADD(COLOR_LIGHT_BLUE,light_blue_concrete)\nADD(COLOR_YELLOW,yellow_concrete)\nADD(COLOR_LIGHT_GREEN,lime_concrete)\nADD(COLOR_PINK,pink_concrete)\nADD(COLOR_GRAY,gray_concrete)\nADD(COLOR_LIGHT_GRAY,light_gray_concrete)\nADD(COLOR_CYAN,cyan_concrete)\nADD(COLOR_PURPLE,purple_concrete)\nADD(COLOR_BLUE,blue_concrete)\nADD(COLOR_BROWN,brown_concrete)\nADD(COLOR_GREEN,green_concrete)\nADD(COLOR_RED,red_concrete)\nADD(COLOR_BLACK,black_concrete)");
        break;
    case ID_Preset_wool:
        panel->setMaterialsConf(panel->version, panel->buildMethod, "MODE(WHITELIST)\nADD(SNOW,white_wool)\nADD(COLOR_MAGENTA)\nADD(COLOR_LIGHT_BLUE)\nADD(COLOR_YELLOW)\nADD(COLOR_LIGHT_GREEN)\nADD(COLOR_PINK)\nADD(COLOR_GRAY)\nADD(COLOR_LIGHT_GRAY)\nADD(COLOR_CYAN)\nADD(COLOR_PURPLE)\nADD(COLOR_BLUE)\nADD(COLOR_BROWN)\nADD(COLOR_GREEN)\nADD(COLOR_RED)\nADD(COLOR_BLACK)");
        break;
    case ID_Preset_no_minerals:
        panel->setMaterialsConf(panel->version, panel->buildMethod, "MODE(BLACKLIST)\nADD(FIRE,tnt)\nREMOVE(METAL)\nREMOVE(GOLD)\nREMOVE(LAPIS)\nREMOVE(EMERALD)\nREMOVE(RAW_IRON)");
        break;
    }
    panel->onConfigChanged();
}

void MaterialsWindow::displayCountMaterials(std::vector<size_t> &counts)
{
    panel->displayCountMaterials(counts);
}

void MaterialsPanel::displayCountMaterials(std::vector<size_t> &counts)
{
    countMutex.Lock();
    this->counts.resize(counts.size());
    for (size_t i = 0; i < counts.size(); i++)
    {
        this->counts[i] = counts[i];
    }
    requiresCountRefresh = true;
    countMutex.Unlock();
}
