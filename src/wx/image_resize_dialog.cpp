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
};

BEGIN_EVENT_TABLE(ImageResizeDialog, wxDialog)
EVT_BUTTON(ID_OK, ImageResizeDialog::OnOk)
EVT_BUTTON(ID_Cancel, ImageResizeDialog::OnCancel)
EVT_TEXT(ID_Text_W, ImageResizeDialog::OnTextUpdateW)
EVT_TEXT(ID_Text_H, ImageResizeDialog::OnTextUpdateH)
EVT_CHECKBOX(ID_Checkbox, ImageResizeDialog::OnCheckboxChanged)
END_EVENT_TABLE()

ImageResizeDialog::ImageResizeDialog(int width, int height) : wxDialog(NULL, -1, wxString("Resize image"), wxDefaultPosition, wxSize(255, 180))
{

    this->width = max(1, width);
    this->height = max(1, height);

    resizedW = max(1, width);
    resizedH = max(1, height);

    linked = true;

    wxStaticText *label1 = new wxStaticText(this, wxID_ANY, wxString("Width:"), wxPoint(15, 17), wxSize(40, 20));
    stringstream sw;
    sw << width;
    wText = new wxTextCtrl(this, ID_Text_W, wxString(sw.str()), wxPoint(65, 15), wxSize(50, 20));
    wxStaticText *labelPx1 = new wxStaticText(this, wxID_ANY, wxString("px"), wxPoint(120, 17), wxSize(40, 20));

    wxStaticText *label2 = new wxStaticText(this, wxID_ANY, wxString("Height:"), wxPoint(15, 42), wxSize(40, 20));
    stringstream sh;
    sh << height;
    hText = new wxTextCtrl(this, ID_Text_H, wxString(sh.str()), wxPoint(65, 40), wxSize(50, 20));
    wxStaticText *labelPx2 = new wxStaticText(this, wxID_ANY, wxString("px"), wxPoint(120, 42), wxSize(40, 20));

    checkbox = new wxCheckBox(this, ID_Checkbox, wxString("Keep aspect ratio."), wxPoint(15, 70), wxSize(180, 15));
    checkbox->SetValue(true);

    wxButton *okButton = new wxButton(this, ID_OK, wxString("Resize"), wxPoint(125, 100), wxSize(100, 30));
    wxButton *cancelButton = new wxButton(this, ID_Cancel, wxString("Cancel"), wxPoint(15, 100), wxSize(100, 30));

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
    if (this->linked) {
        resizedH = max(1, static_cast<int>(((double)height / (double)width) * (double)resizedW));

        stringstream ss;
        ss << resizedH;

        hText->ChangeValue(ss.str());
    }
}

void ImageResizeDialog::OnTextUpdateW(wxCommandEvent &event)
{
    int w = atoi(wText->GetValue().c_str());

    if (w <= 0) {
        return; // Invalid input, ignore
    }

    resizedW = w;

    if (this->linked) {
        resizedH = max(1, static_cast<int>(((double)height / (double)width) * (double)resizedW));

        stringstream ss;
        ss << resizedH;

        hText->ChangeValue(ss.str());
    }
}

void ImageResizeDialog::OnTextUpdateH(wxCommandEvent &event)
{
    int h = atoi(hText->GetValue().c_str());

    if (h <= 0) {
        return; // Invalid input, ignore
    }

    resizedH = h;

    if (this->linked) {
        resizedW = max(1, static_cast<int>(((double)width / (double)height) * (double)resizedH));

        stringstream ss;
        ss << resizedW;

        wText->ChangeValue(ss.str());
    }
}

int ImageResizeDialog::getWidth() {
    return resizedW;
}

int ImageResizeDialog::getHeight() {
    return resizedH;
}
