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

#include <sstream>
#include <algorithm>

#include "image_resize_dialog.h"
#include "../tools/value_remember.h"

using namespace std;

enum Identifiers
{
    ID_OK = 1,
    ID_Cancel = 2,
    ID_Text_W = 3,
    ID_Text_H = 4,
    ID_Checkbox = 5,
    ID_Combo = 6,
};

BEGIN_EVENT_TABLE(ImageResizeDialog, wxDialog)
EVT_BUTTON(ID_OK, ImageResizeDialog::OnOk)
EVT_BUTTON(ID_Cancel, ImageResizeDialog::OnCancel)
EVT_TEXT(ID_Text_W, ImageResizeDialog::OnTextUpdateW)
EVT_TEXT(ID_Text_H, ImageResizeDialog::OnTextUpdateH)
EVT_CHECKBOX(ID_Checkbox, ImageResizeDialog::OnCheckboxChanged)
EVT_COMBOBOX(ID_Combo, ImageResizeDialog::OnUnitsComboBoxChanged)
EVT_CHAR_HOOK(ImageResizeDialog::OnKeyPress)
END_EVENT_TABLE()

const int MAP_SIZE = 128;

ImageResizeDialog::ImageResizeDialog(int width, int height) : wxDialog(NULL, -1, wxString("Resize image"), wxDefaultPosition, wxDefaultSize)
{

    this->width = max(1, width);
    this->height = max(1, height);

    this->usingMaps = tools::getRememberedValue(tools::VALUE_PURPOSE_SIZE_UNITS).compare("map") == 0;

    resizedW = max(1, width);
    resizedH = max(1, height);

    resizedWMaps = (double)resizedW / (double)MAP_SIZE;
    resizedHMaps = (double)resizedH / (double)MAP_SIZE;

    linked = true;

    // Create sizers and wx components

    const int spacing = 10;

    const int textWidth = 130;

    const int buttonWidth = 100;
    const int buttonHeight = 30;

    sizerTop = new wxBoxSizer(wxVERTICAL);

    const int gridRows = 3;
    const int gridCols = 2;
    const int verticalSpacing = 0;
    const int horizontalSpacing = 0;
    sizerGrid = new wxFlexGridSizer(gridRows, gridCols, verticalSpacing, horizontalSpacing);

    // Width group

    wxStaticText *label1 = new wxStaticText(
        this, wxID_ANY,
        wxString("Width: "),
        wxDefaultPosition,
        wxDefaultSize);

    sizerGrid->Add(label1, 0, wxALL | wxALIGN_CENTER_VERTICAL, spacing);

    sizerGroupWidth = new wxBoxSizer(wxHORIZONTAL);

    stringstream sw;

    if (usingMaps)
    {
        sw << resizedWMaps;
    }
    else
    {
        sw << resizedW;
    }

    wText = new wxTextCtrl(
        this, ID_Text_W,
        wxString(sw.str()),
        wxDefaultPosition,
        wxSize(textWidth, -1));

    sizerGroupWidth->Add(wText, 0, wxALL | wxALIGN_CENTER_VERTICAL, spacing);

    labelUnits1 = new wxStaticText(
        this, wxID_ANY,
        usingMaps ? wxString(" map") : wxString("  px"),
        wxDefaultPosition,
        wxDefaultSize);

    sizerGroupWidth->Add(labelUnits1, 0, wxALL | wxALIGN_CENTER_VERTICAL, spacing);

    sizerGrid->Add(sizerGroupWidth, 0, wxALIGN_CENTER_VERTICAL);

    // Height group

    wxStaticText *label2 = new wxStaticText(
        this, wxID_ANY,
        wxString("Height: "),
        wxDefaultPosition,
        wxDefaultSize);

    sizerGrid->Add(label2, 0, wxALL | wxALIGN_CENTER_VERTICAL, spacing);

    sizerGroupHeight = new wxBoxSizer(wxHORIZONTAL);

    stringstream sh;

    if (usingMaps)
    {
        sh << resizedHMaps;
    }
    else
    {
        sh << resizedH;
    }

    hText = new wxTextCtrl(
        this, ID_Text_H,
        wxString(sh.str()),
        wxDefaultPosition,
        wxSize(textWidth, -1));

    sizerGroupHeight->Add(hText, 0, wxALL | wxALIGN_CENTER_VERTICAL, spacing);

    labelUnits2 = new wxStaticText(
        this, wxID_ANY,
        usingMaps ? wxString(" map") : wxString("  px"),
        wxDefaultPosition,
        wxDefaultSize);

    sizerGroupHeight->Add(labelUnits2, 0, wxALL | wxALIGN_CENTER_VERTICAL, spacing);

    sizerGrid->Add(sizerGroupHeight, 0, wxALIGN_CENTER_VERTICAL);

    // Units switch

    wxStaticText *label3 = new wxStaticText(
        this, wxID_ANY,
        wxString("Units:"),
        wxDefaultPosition,
        wxDefaultSize);

    sizerGrid->Add(label3, 0, wxALL | wxALIGN_CENTER_VERTICAL, spacing);
    
    wxArrayString options;
    wxString optionPixels("Pixels (1 px = 1 block)");
    wxString optionMaps("Maps (1 map = 128 blocks)");

    options.Add(optionPixels);
    options.Add(optionMaps);
    combo = new wxComboBox(
        this, ID_Combo,
        usingMaps ? optionMaps : optionPixels,
        wxDefaultPosition,
        wxDefaultSize,
        options, wxCB_READONLY);

    sizerGrid->Add(combo, 0, wxALL | wxALIGN_CENTER_VERTICAL, spacing);

    // Finished width and height

    sizerTop->Add(sizerGrid);

    // Keep aspect ratio checkbox

    checkbox = new wxCheckBox(
        this, ID_Checkbox,
        wxString("Keep aspect ratio."),
        wxDefaultPosition,
        wxDefaultSize);
    checkbox->SetValue(true);

    sizerTop->Add(checkbox, 0, wxALL | wxALIGN_CENTER, spacing);

    // Buttons

    sizerGroupButtons = new wxBoxSizer(wxHORIZONTAL);

    wxButton *cancelButton = new wxButton(
        this, ID_Cancel,
        wxString("Cancel"),
        wxDefaultPosition,
        wxSize(buttonWidth, buttonHeight));

    sizerGroupButtons->Add(cancelButton, 0, wxALL | wxALIGN_CENTER, spacing);

    wxButton *okButton = new wxButton(
        this, ID_OK,
        wxString("Resize"),
        wxDefaultPosition,
        wxSize(buttonWidth, buttonHeight));

    sizerGroupButtons->Add(okButton, 0, wxALL | wxALIGN_CENTER, spacing);

    sizerTop->Add(sizerGroupButtons, 0, wxALL | wxALIGN_CENTER);

    // End of wx composition

    sizerTop->SetSizeHints(this);
    SetSizerAndFit(sizerTop);

    Centre();
}

void ImageResizeDialog::OnOk(wxCommandEvent &event)
{
    EndModal(wxID_OK);
}

void ImageResizeDialog::OnCancel(wxCommandEvent &event)
{
    EndModal(wxID_CANCEL);
}

void ImageResizeDialog::OnCheckboxChanged(wxCommandEvent &event)
{
    this->linked = checkbox->GetValue();
    if (this->linked)
    {
        resizedH = max(1, static_cast<int>(((double)height / (double)width) * (double)resizedW));
        resizedHMaps = (double)resizedH / (double)MAP_SIZE;

        stringstream ss;

        if (usingMaps)
        {
            ss << resizedHMaps;
        }
        else
        {
            ss << resizedH;
        }

        hText->ChangeValue(ss.str());
    }
}

void ImageResizeDialog::OnTextUpdateW(wxCommandEvent &event)
{
    if (usingMaps)
    {
        int wMaps = atof(wText->GetValue().c_str());
        int w = static_cast<int>(wMaps * (double)MAP_SIZE);

        if (w <= 0)
        {
            return; // Invalid input, ignore
        }

        resizedWMaps = wMaps;
        resizedW = w;
    }
    else
    {
        int w = atoi(wText->GetValue().c_str());

        if (w <= 0)
        {
            return; // Invalid input, ignore
        }

        resizedW = w;
        resizedWMaps = (double)resizedW / (double)MAP_SIZE;
    }

    if (this->linked)
    {
        resizedH = max(1, static_cast<int>(((double)height / (double)width) * (double)resizedW));
        resizedHMaps = (double)resizedH / (double)MAP_SIZE;

        stringstream ss;

        if (usingMaps)
        {
            ss << resizedHMaps;
        }
        else
        {
            ss << resizedH;
        }

        hText->ChangeValue(ss.str());
    }
}

void ImageResizeDialog::OnTextUpdateH(wxCommandEvent &event)
{
    if (usingMaps)
    {
        double hMaps = atof(hText->GetValue().c_str());
        int h = static_cast<int>(hMaps * (double)MAP_SIZE);

        if (h <= 0)
        {
            return; // Invalid input, ignore
        }

        resizedHMaps = hMaps;
        resizedH = h;
    }
    else
    {
        int h = atoi(hText->GetValue().c_str());

        if (h <= 0)
        {
            return; // Invalid input, ignore
        }

        resizedH = h;
        resizedHMaps = (double)resizedH / (double)MAP_SIZE;
    }

    if (this->linked)
    {
        resizedW = max(1, static_cast<int>(((double)width / (double)height) * (double)resizedH));
        resizedWMaps = (double)resizedW / (double)MAP_SIZE;

        stringstream ss;

        if (usingMaps)
        {
            ss << resizedWMaps;
        }
        else
        {
            ss << resizedW;
        }

        wText->ChangeValue(ss.str());
    }
}

int ImageResizeDialog::getWidth()
{
    return resizedW;
}

int ImageResizeDialog::getHeight()
{
    return resizedH;
}

void ImageResizeDialog::OnUnitsComboBoxChanged(wxCommandEvent &evt)
{
    usingMaps = combo->GetSelection() > 0;

    if (usingMaps)
    {
        stringstream ssW;
        ssW << resizedWMaps;
        wText->ChangeValue(ssW.str());

        stringstream ssH;
        ssH << resizedHMaps;
        hText->ChangeValue(ssH.str());

        if (labelUnits1 != NULL)
        {
            labelUnits1->SetLabelText(wxString("map"));
        }

        if (labelUnits2 != NULL)
        {
            labelUnits2->SetLabelText(wxString("map"));
        }
    }
    else
    {
        stringstream ssW;
        ssW << resizedW;
        wText->ChangeValue(ssW.str());

        stringstream ssH;
        ssH << resizedH;
        hText->ChangeValue(ssH.str());

        if (labelUnits1 != NULL)
        {
            labelUnits1->SetLabelText(wxString("px"));
        }

        if (labelUnits2 != NULL)
        {
            labelUnits2->SetLabelText(wxString("px"));
        }
    }

    tools::setRememberedValue(tools::VALUE_PURPOSE_SIZE_UNITS, usingMaps ? std::string("map") : std::string("px"));
}

void ImageResizeDialog::OnKeyPress(wxKeyEvent &event)
{
    if (event.GetKeyCode() == WXK_ESCAPE)
    {
        Hide();
        return;
    }

    event.Skip();
}
