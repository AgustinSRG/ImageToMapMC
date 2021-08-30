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

#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/combobox.h>

#include "main_window.h"

struct MaterialCustomGroup
{
    wxCheckBox *checkBox;
    wxPanel *colorPanel;
    wxStaticText *colorLabel;
    wxComboBox *combo;
};

class MaterialsWindow;

class MaterialsPanel : public wxScrolledWindow
{
public:
    MaterialsPanel(MaterialsWindow *matWin);

    void onCheckBoxChanged(wxCommandEvent &evt);
    void onComboBoxChanged(wxCommandEvent &evt);

    void setMaterialsConf(minecraft::McVersion version, std::string conf);

    std::string getMaterialsConf();

    void onConfigChanged();

    void setBlacklistMode(bool blacklist);

    bool blacklist;
    minecraft::McVersion version;

private:
    std::vector<MaterialCustomGroup> groups;
    MaterialsWindow *materialsWindow;
    
    std::vector<colors::Color> baseColors;
    std::vector<minecraft::BlockList> blockSet;
    std::vector<minecraft::FinalColor> colorSet;
    std::vector<std::string> baseColorNames;
    
    std::vector<bool> enabledConf;

    wxDECLARE_EVENT_TABLE();
};

class MaterialsWindow : public wxFrame
{
public:
    MainWindow *mainWindow;
    wxMenuBar * menuBar;

    MaterialsWindow(MainWindow *mainWindow);
    ~MaterialsWindow();

    void OnSize(wxSizeEvent &event);
    void OnClose(wxCloseEvent &event);

    void onLoadFile(wxCommandEvent &evt);
    void onSaveFile(wxCommandEvent &evt);
    void SetBlackList(wxCommandEvent &evt);
    void SetWhiteList(wxCommandEvent &evt);

    void setMaterialsConf(minecraft::McVersion version, std::string conf);

private:
    MaterialsPanel *panel;
    
    wxDECLARE_EVENT_TABLE();
};
