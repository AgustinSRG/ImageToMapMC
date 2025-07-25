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

#include "../mapart/map_art.h"

#include <mutex>

class wxImagePanel : public wxPanel
{

public:
    wxBitmap *bitmap;
    wxBitmap *bitmapBg;
    size_t matrixWidth;
    size_t matrixHeight;

    wxMutex colorsMutex;

    wxImagePanel(wxFrame *parent);
    ~wxImagePanel();

    void paintEvent(wxPaintEvent &evt);
    void paintNow();

    void render(wxDC &dc);

    void setColors(const std::vector<const minecraft::FinalColor *> &colorsMatrix, const std::vector<bool> &transparencyMatrix, size_t width, size_t height, bool preserveTransparency);
    void setColors(const std::vector<colors::Color> &colorsMatrix, const std::vector<bool> &transparencyMatrix, size_t width, size_t height, bool preserveTransparency);

    DECLARE_EVENT_TABLE()
};

class DisplayImageFrame : public wxFrame
{
public:
    wxImagePanel *drawPane;
    wxMenu *menu;
    std::string defaultFile;
    DisplayImageFrame(wxWindow *parent, const wxString &title, const wxPoint &pos, const wxSize &size);
    ~DisplayImageFrame();

    void setColors(const std::vector<const minecraft::FinalColor *> &colorsMatrix, const std::vector<bool> &transparencyMatrix, size_t width, size_t height, bool preserveTransparency);
    void setColors(const std::vector<colors::Color> &colorsMatrix, const std::vector<bool> &transparencyMatrix, size_t width, size_t height, bool preserveTransparency);

    void OnShowContextMenu(wxContextMenuEvent &event);
    void OnContextMenuSelected(wxCommandEvent &event);
    void OnSaveImage();

private:
    void OnSize(wxSizeEvent &event);
    wxDECLARE_EVENT_TABLE();
};

namespace widgets
{
    void displayMapImage(std::vector<const minecraft::FinalColor *> &colorsMatrix, wxApp &app);
}
