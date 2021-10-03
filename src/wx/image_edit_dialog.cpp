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

#include "image_edit_dialog.h"
#include <wx/colordlg.h>
#include <filesystem>
#include <sstream>
#include <algorithm>

using namespace std;

enum Identifiers
{
    ID_OK = 1,
    ID_Cancel = 2,
    ID_Color = 3,

    ID_Label_Saturation = 11,
    ID_Label_Contrast = 12,
    ID_Label_Brightness = 13,
    ID_Label_Background = 14,

    ID_Slider_Saturation = 21,
    ID_Slider_Contrast = 22,
    ID_Slider_Brightness = 23,

    ID_Panel_Background = 31
};

BEGIN_EVENT_TABLE(ImageEditDialog, wxDialog)
EVT_BUTTON(ID_OK, ImageEditDialog::OnOk)
EVT_BUTTON(ID_Cancel, ImageEditDialog::OnCancel)
EVT_BUTTON(ID_Color, ImageEditDialog::OnColorSelect)
EVT_CLOSE(ImageEditDialog::OnClose)
EVT_COMMAND_SCROLL(ID_Slider_Saturation, ImageEditDialog::OnChangeSaturation)
EVT_COMMAND_SCROLL(ID_Slider_Contrast, ImageEditDialog::OnChangeContrast)
EVT_COMMAND_SCROLL(ID_Slider_Brightness, ImageEditDialog::OnChangeBrightness)
END_EVENT_TABLE()

ImageEditDialog::ImageEditDialog(MainWindow *mainWindow) : wxDialog(mainWindow, -1, wxString("Modify image"), wxDefaultPosition, wxSize(275, 310))
{
    this->mainWindow = mainWindow;
    this->saturation = 1;
    this->contrast = 1;
    this->brightness = 1;

    labelSaturation = new wxStaticText(this, ID_Label_Saturation, wxString("Saturation: 100%"), wxPoint(15, 15), wxSize(200, 20));
    sliderSaturation = new wxSlider(this, ID_Slider_Saturation, 100, 0, 200, wxPoint(5, 35), wxSize(250, 25));

    labelContrast = new wxStaticText(this, ID_Label_Contrast, wxString("Contrast: 100%"), wxPoint(15, 65), wxSize(200, 20));
    sliderContrast = new wxSlider(this, ID_Slider_Contrast, 100, 0, 200, wxPoint(5, 85), wxSize(250, 25));

    labelBrightness = new wxStaticText(this, ID_Label_Brightness, wxString("Brightness: 100%"), wxPoint(15, 115), wxSize(200, 20));
    sliderBrightness = new wxSlider(this, ID_Slider_Brightness, 100, 0, 200, wxPoint(5, 130), wxSize(250, 25));

    labelBackground = new wxStaticText(this, ID_Label_Background, wxString("Background color:"), wxPoint(15, 165), wxSize(200, 20));
    panelColor = new wxPanel(this, ID_Panel_Background, wxPoint(25, 190), wxSize(100, 30), wxSIMPLE_BORDER);

    panelColor->SetBackgroundColour(wxColour(255, 255, 255));

    wxButton *colorButton = new wxButton(this, ID_Color, wxString("Change color"), wxPoint(135, 190), wxSize(100, 30));

    wxButton *okButton = new wxButton(this, ID_OK, wxString("Done"), wxPoint(135, 230), wxSize(100, 30));
    wxButton *cancelButton = new wxButton(this, ID_Cancel, wxString("Reset"), wxPoint(25, 230), wxSize(100, 30));

    Centre();
}

void ImageEditDialog::OnOk(wxCommandEvent &event)
{
    Hide();
}

void ImageEditDialog::OnCancel(wxCommandEvent &event)
{
    saturation = 1.0;
    contrast = 1.0;
    brightness = 1.0;
    background.red = 255;
    background.green = 255;
    background.blue = 255;
    UpdateControls();
    OnChangeParams();
}

void ImageEditDialog::OnClose(wxCloseEvent &event)
{
    if (event.CanVeto())
    {
        event.Veto();
    }

    Hide();
}

void ImageEditDialog::OnChangeParams()
{
    mainWindow->onImageEditParamsChanged(saturation, contrast, brightness, background);
}

int ImageEditDialog::getSliderValue(float val) {
    return min(max(0, static_cast<int>(100.0 * val)), 200);
}

void ImageEditDialog::UpdateControls()
{
    sliderSaturation->SetValue(getSliderValue(saturation));
    sliderContrast->SetValue(getSliderValue(contrast));
    sliderBrightness->SetValue(getSliderValue(brightness));

    stringstream ss1;
    ss1 << "Saturation: " << static_cast<int>(saturation * 100) << "%";
    labelSaturation->SetLabel(ss1.str());

    stringstream ss2;
    ss2 << "Contrast: " << static_cast<int>(contrast * 100) << "%";
    labelContrast->SetLabel(ss2.str());

    stringstream ss3;
    ss3 << "Brightness: " << static_cast<int>(brightness * 100) << "%";
    labelBrightness->SetLabel(ss3.str());

    panelColor->SetBackgroundColour(wxColour(background.red, background.green, background.blue));
    panelColor->Refresh();
}

void ImageEditDialog::SetParams(float saturation, float contrast, float brightness, colors::Color background)
{
    this->saturation = saturation;
    this->contrast = contrast;
    this->brightness = brightness;
    this->background = background;
    UpdateControls();
}

void ImageEditDialog::OnChangeSaturation(wxScrollEvent &event)
{
    int val = sliderSaturation->GetValue();
    float vF = (float) val / 100.0;

    saturation = vF;

    stringstream ss;
    ss << "Saturation: " << static_cast<int>(vF * 100) << "%";
    labelSaturation->SetLabel(ss.str());

    OnChangeParams();
}

void ImageEditDialog::OnChangeContrast(wxScrollEvent &event)
{
    int val = sliderContrast->GetValue();
    float vF = (float) val / 100.0;

    contrast = vF;

    stringstream ss;
    ss << "Contrast: " << static_cast<int>(vF * 100) << "%";
    labelContrast->SetLabel(ss.str());

    OnChangeParams();
}

void ImageEditDialog::OnChangeBrightness(wxScrollEvent &event)
{
    int val = sliderBrightness->GetValue();
    float vF = (float) val / 100.0;

    brightness = vF;

    stringstream ss;
    ss << "Brightness: " << static_cast<int>(vF * 100) << "%";
    labelBrightness->SetLabel(ss.str());

    OnChangeParams();
}

void ImageEditDialog::OnColorSelect(wxCommandEvent &event) {
    wxColourData initColor;
    initColor.SetColour(wxColour(background.red, background.green, background.blue));
    wxColourDialog colorDialog(this, &initColor);

    if (colorDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea..

    background.red = colorDialog.GetColourData().GetColour().Red();
    background.green = colorDialog.GetColourData().GetColour().Green();
    background.blue = colorDialog.GetColourData().GetColour().Blue();

    panelColor->SetBackgroundColour(wxColour(background.red, background.green, background.blue));
    panelColor->Refresh();

    OnChangeParams();
}
