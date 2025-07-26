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

#include "image_resize_dialog.h"
#include <filesystem>
#include <sstream>
#include <algorithm>

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

const int DIALOG_WIDTH = 255;
const int DIALOG_HEIGHT = 205;

const int MAP_SIZE = 128;

ImageResizeDialog::ImageResizeDialog(int width, int height) : wxDialog(NULL, -1, wxString("Resize image"), wxDefaultPosition, wxSize(DIALOG_WIDTH, DIALOG_HEIGHT))
{

    this->width = max(1, width);
    this->height = max(1, height);

    this->usingMaps = false;

    resizedW = max(1, width);
    resizedH = max(1, height);

    resizedWMaps = (double)resizedW / (double)MAP_SIZE;
    resizedHMaps = (double)resizedH / (double)MAP_SIZE;

    linked = true;

    const int paddingTopLabels = 2;
    const int paddingTopLabelCombo = 5;

    const int labelWidth = 40;
    const int labelHeight = 20;

    const int labelUnitsWidth = 25;

    const int textWidth = 130;
    const int textHeight = 20;

    const int comboWidth = 160;
    const int comboHeight = 25;

    const int buttonWidth = 100;
    const int buttonHeight = 30;

    const int checkboxWidth = 180;
    const int checkboxHeight = 15;

    const int spaceBetweenButtons = 10;

    const int paddingLeft = 15;

    const int labelTextSeparation = 10;
    const int textUnitLabelSeparation = 5;

    const int rowMarginBottom = 5;

    const int checkBoxMarginTop = 5;

    const int buttonsMarginTop = 10;

    const int paddingTop = 15;

    int rowY = paddingTop;

    int rowX;

    // Width row

    rowX = paddingLeft;

    wxStaticText *label1 = new wxStaticText(
        this, wxID_ANY,
        wxString("Width:"),
        wxPoint(rowX, rowY + paddingTopLabels),
        wxSize(labelWidth, labelHeight));

    rowX += labelWidth + labelTextSeparation;

    stringstream sw;
    sw << width;

    wText = new wxTextCtrl(
        this, ID_Text_W,
        wxString(sw.str()),
        wxPoint(rowX, rowY),
        wxSize(textWidth, textHeight));

    rowX += textWidth + textUnitLabelSeparation;

    labelUnits1 = new wxStaticText(
        this, wxID_ANY, wxString("px"),
        wxPoint(rowX, rowY + paddingTopLabels),
        wxSize(labelUnitsWidth, labelHeight), wxALIGN_RIGHT);

    // Height row

    rowY += textHeight + rowMarginBottom;
    rowX = paddingLeft;

    wxStaticText *label2 = new wxStaticText(
        this, wxID_ANY,
        wxString("Height:"),
        wxPoint(rowX, rowY + paddingTopLabels),
        wxSize(labelWidth, labelHeight));

    rowX += labelWidth + labelTextSeparation;

    stringstream sh;
    sh << height;

    hText = new wxTextCtrl(
        this, ID_Text_H,
        wxString(sh.str()),
        wxPoint(rowX, rowY),
        wxSize(textWidth, textHeight));

    rowX += textWidth + textUnitLabelSeparation;

    labelUnits2 = new wxStaticText(
        this, wxID_ANY,
        wxString("px"),
        wxPoint(rowX, rowY + paddingTopLabels),
        wxSize(labelUnitsWidth, labelHeight), wxALIGN_RIGHT);

    // Size units row

    rowY += textHeight + rowMarginBottom;
    rowX = paddingLeft;

    wxStaticText *label3 = new wxStaticText(
        this, wxID_ANY,
        wxString("Units:"),
        wxPoint(rowX, rowY + paddingTopLabelCombo),
        wxSize(labelWidth, labelHeight));

    rowX += labelWidth + labelTextSeparation;

    wxArrayString options;
    options.Add("Pixels (1 px = 1 block)");
    options.Add("Maps (1 map = 128 blocks)");
    combo = new wxComboBox(
        this, ID_Combo,
        wxString("Pixels (1 px = 1 block)"),
        wxPoint(rowX, rowY),
        wxSize(comboWidth, comboHeight),
        options, wxCB_READONLY);

    // Keep aspect ratio checkbox row

    rowY += comboHeight + rowMarginBottom + checkBoxMarginTop;
    rowX = paddingLeft;

    checkbox = new wxCheckBox(
        this, ID_Checkbox,
        wxString("Keep aspect ratio."),
        wxPoint(rowX, rowY),
        wxSize(checkboxWidth, checkboxHeight));
    checkbox->SetValue(true);

    // Buttons

    rowY += checkboxHeight + rowMarginBottom + buttonsMarginTop;

    wxButton *cancelButton = new wxButton(
        this, ID_Cancel,
        wxString("Cancel"),
        wxPoint(paddingLeft, rowY),
        wxSize(buttonWidth, buttonHeight));

    wxButton *okButton = new wxButton(
        this, ID_OK,
        wxString("Resize"),
        wxPoint(paddingLeft + buttonWidth + spaceBetweenButtons, rowY),
        wxSize(buttonWidth, buttonHeight));

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

        stringstream ss;
        ss << resizedH;

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
