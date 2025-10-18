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

#include "img_display_window.h"

#include <wx/sizer.h>
#include <wx/rawbmp.h>
#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include "../resources/icon.xpm"

using namespace std;

/* Define IDS */

enum MenuIDs
{
    MENU_ID_CONTEXT_1 = 1,
    MENU_ID_CONTEXT_2 = 2
};

/* Events */

BEGIN_EVENT_TABLE(DisplayImageFrame, wxFrame)
EVT_SIZE(DisplayImageFrame::OnSize)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)
// some useful events
/*
 EVT_MOTION(wxImagePanel::mouseMoved)
 EVT_LEFT_DOWN(wxImagePanel::mouseDown)
 EVT_LEFT_UP(wxImagePanel::mouseReleased)
 EVT_RIGHT_DOWN(wxImagePanel::rightClick)
 EVT_LEAVE_WINDOW(wxImagePanel::mouseLeftWindow)
 EVT_KEY_DOWN(wxImagePanel::keyPressed)
 EVT_KEY_UP(wxImagePanel::keyReleased)
 EVT_MOUSEWHEEL(wxImagePanel::mouseWheelMoved)
 */

// catch paint events
EVT_PAINT(wxImagePanel::paintEvent)

END_EVENT_TABLE()

/* Implementation */

wxImagePanel::wxImagePanel(wxFrame *parent) : wxPanel(parent)
{
    // load the file... ideally add a check to see if loading was successful

    matrixHeight = 0;
    matrixWidth = 0;
    bitmap = NULL;
    bitmapBg = NULL;

    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

const colors::Color bgColor1{200, 200, 200};
const colors::Color bgColor2{150, 150, 150};

void wxImagePanel::setColors(const std::vector<const minecraft::FinalColor *> &colorsMatrix, const std::vector<bool> &transparencyMatrix, size_t width, size_t height, bool preserveTransparency)
{
    colorsMutex.Lock();

    if (bitmap != NULL)
    {
        delete bitmap;
        bitmap = NULL;
    }

    if (bitmapBg != NULL)
    {
        delete bitmapBg;
        bitmapBg = NULL;
    }

    matrixHeight = height;
    matrixWidth = width;

    wxImage image(width, height);
    wxImage imageBg(width, height);

    image.InitAlpha();

    unsigned char *rawData = image.GetData();
    unsigned char *alphaData = image.GetAlpha();

    unsigned char *bgData = imageBg.GetData();

    size_t checkerSize = 2;

    if (width >= height)
    {
        checkerSize = 2 * (height / MAP_HEIGHT);
    }
    else
    {
        checkerSize = 2 * (width / MAP_WIDTH);
    }

    for (size_t y = 0; y < height; y++)
    {
        size_t checkerY = y / checkerSize;
        for (size_t x = 0; x < width; x++)
        {
            size_t checkerX = x / checkerSize;

            size_t colorIndex = (y * width + x) * 3;

            bool altColor = (checkerY % 2 == 0) ? (checkerX % 2 == 0) : (checkerX % 2 != 0);

            if (altColor)
            {
                bgData[colorIndex] = bgColor1.red;
                bgData[colorIndex + 1] = bgColor1.green;
                bgData[colorIndex + 2] = bgColor1.blue;
            }
            else
            {
                bgData[colorIndex] = bgColor2.red;
                bgData[colorIndex + 1] = bgColor2.green;
                bgData[colorIndex + 2] = bgColor2.blue;
            }
        }
    }

    size_t size = width * height;

    size_t j = 0;
    for (size_t i = 0; i < size; i++)
    {
        colors::Color color = colorsMatrix[i]->color;

        rawData[j++] = color.red;
        rawData[j++] = color.green;
        rawData[j++] = color.blue;

        if (colorsMatrix[i]->baseColorIndex == (short)minecraft::McColors::NONE || (preserveTransparency && transparencyMatrix[i]))
        {
            alphaData[i] = 0;
        }
        else
        {
            alphaData[i] = 255;
        }
    }

    bitmap = new wxBitmap(image);
    bitmapBg = new wxBitmap(imageBg);

    colorsMutex.Unlock();

    this->Refresh();
}

void wxImagePanel::setColors(const std::vector<colors::Color> &colorsMatrix, const std::vector<bool> &transparencyMatrix, size_t width, size_t height, bool preserveTransparency)
{
    colorsMutex.Lock();

    if (bitmap != NULL)
    {
        delete bitmap;
        bitmap = NULL;
    }

    if (bitmapBg != NULL)
    {
        delete bitmapBg;
        bitmapBg = NULL;
    }

    matrixHeight = height;
    matrixWidth = width;

    wxImage image(width, height);
    wxImage imageBg(width, height);

    image.InitAlpha();

    unsigned char *rawData = image.GetData();
    unsigned char *alphaData = image.GetAlpha();

    unsigned char *bgData = imageBg.GetData();

    size_t checkerSize = 2;

    if (width >= height)
    {
        checkerSize = 2 * (height / MAP_HEIGHT);
    }
    else
    {
        checkerSize = 2 * (width / MAP_WIDTH);
    }

    for (size_t y = 0; y < height; y++)
    {
        size_t checkerY = y / checkerSize;
        for (size_t x = 0; x < width; x++)
        {
            size_t checkerX = x / checkerSize;

            size_t colorIndex = (y * width + x) * 3;

            bool altColor = (checkerY % 2 == 0) ? (checkerX % 2 == 0) : (checkerX % 2 != 0);

            if (altColor)
            {
                bgData[colorIndex] = bgColor1.red;
                bgData[colorIndex + 1] = bgColor1.green;
                bgData[colorIndex + 2] = bgColor1.blue;
            }
            else
            {
                bgData[colorIndex] = bgColor2.red;
                bgData[colorIndex + 1] = bgColor2.green;
                bgData[colorIndex + 2] = bgColor2.blue;
            }
        }
    }

    size_t size = colorsMatrix.size();

    size_t j = 0;
    for (size_t i = 0; i < size; i++)
    {
        colors::Color color = colorsMatrix[i];

        rawData[j++] = color.red;
        rawData[j++] = color.green;
        rawData[j++] = color.blue;

        alphaData[i] = preserveTransparency ? (transparencyMatrix[i] ? 0 : 255) : 255;
    }

    bitmap = new wxBitmap(image);
    bitmapBg = new wxBitmap(imageBg);

    colorsMutex.Unlock();

    this->Refresh();
}

wxImagePanel::~wxImagePanel()
{
    if (bitmap != NULL)
    {
        delete bitmap;
    }
}

void wxImagePanel::paintEvent(wxPaintEvent &evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxAutoBufferedPaintDC dc(this);
    render(dc);
}

void wxImagePanel::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}

void wxImagePanel::render(wxDC &dc)
{
    colorsMutex.Lock();

    if (bitmap != NULL && matrixWidth > 0 && matrixHeight > 0)
    {
        double scale = 1;
        int offsetX = 0;
        int offsetY = 0;

        int width = this->GetSize().GetWidth();
        int height = this->GetSize().GetHeight();

        if (height > width)
        {
            if (matrixHeight > matrixWidth)
            {
                scale = (double)height / matrixHeight;
                if (width > matrixWidth * scale)
                {
                    double xPad = width - (matrixWidth * scale);
                    xPad = (xPad / 2) / scale;
                    offsetX += static_cast<int>(xPad);
                }
                else
                {
                    scale = (double)width / matrixWidth;
                    double yPad = height - (matrixHeight * scale);
                    yPad = (yPad / 2) / scale;
                    offsetY += static_cast<int>(yPad);
                }
            }
            else
            {
                scale = (double)width / matrixWidth;
                double yPad = height - (matrixHeight * scale);
                yPad = (yPad / 2) / scale;
                offsetY += static_cast<int>(yPad);
            }
        }
        else if (width > height)
        {
            if (matrixWidth > matrixHeight)
            {
                scale = (double)width / matrixWidth;
                if (height > (matrixHeight * scale))
                {
                    double yPad = height - (matrixHeight * scale);
                    yPad = (yPad / 2) / scale;
                    offsetY += static_cast<int>(yPad);
                }
                else
                {
                    scale = (double)height / matrixHeight;
                    double xPad = width - (matrixWidth * scale);
                    xPad = (xPad / 2) / scale;
                    offsetX += static_cast<int>(xPad);
                }
            }
            else
            {
                scale = (double)height / matrixHeight;
                double xPad = width - (matrixWidth * scale);
                xPad = (xPad / 2) / scale;
                offsetX += static_cast<int>(xPad);
            }
        }
        else
        {
            if (matrixWidth > matrixHeight)
            {
                scale = (double)width / matrixWidth;
                double yPad = height - (matrixHeight * scale);
                yPad = (yPad / 2) / scale;
                offsetY += static_cast<int>(yPad);
            }
            else if (matrixHeight > matrixWidth)
            {
                scale = (double)height / matrixHeight;
                double xPad = width - (matrixWidth * scale);
                xPad = (xPad / 2) / scale;
                offsetX += static_cast<int>(xPad);
            }
            else
            {
                scale = (double)width / matrixWidth;
            }
        }

        dc.Clear();

        dc.SetUserScale(scale, scale);

        if (bitmapBg != NULL)
        {
            dc.DrawBitmap(*bitmapBg, offsetX, offsetY, false);
        }

        dc.DrawBitmap(*bitmap, offsetX, offsetY, false);
    }
    else
    {
        dc.Clear();
    }

    colorsMutex.Unlock();
}

DisplayImageFrame::DisplayImageFrame(wxWindow *parent, const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(parent, wxID_ANY, title, pos, size)
{
    defaultFile = "map.png";

    this->SetIcon(wxIcon(_ICON_ICO_XPM));

    menu = new wxMenu();
    menu->Append(MENU_ID_CONTEXT_1, "Save image to file");
    menu->Append(MENU_ID_CONTEXT_2, "Copy to clipboard");

    drawPane = new wxImagePanel(this);
    drawPane->SetPosition(wxPoint(0, 0));
    drawPane->SetSize(this->GetSize());

    drawPane->Bind(wxEVT_CONTEXT_MENU, &DisplayImageFrame::OnShowContextMenu, this);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &DisplayImageFrame::OnContextMenuSelected, this, MENU_ID_CONTEXT_1, MENU_ID_CONTEXT_2);
}

void DisplayImageFrame::OnShowContextMenu(wxContextMenuEvent &event)
{
    PopupMenu(menu);
}

void DisplayImageFrame::OnSaveImage()
{
    wxFileDialog saveFileDialog(this, _("Save Image"), "", defaultFile, "PNG Images (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    bool ok = drawPane->bitmap->ConvertToImage().SaveFile(saveFileDialog.GetPath().utf8_string(), wxBITMAP_TYPE_PNG);

    if (!ok)
    {
        wxMessageBox(wxString("Could not save the image due to a file system error."), wxT("Error"), wxICON_ERROR);
    }
}

void DisplayImageFrame::OnContextMenuSelected(wxCommandEvent &event)
{
    wxString str;

    switch (event.GetId())
    {
    case MENU_ID_CONTEXT_1:
        // Save to file
        this->OnSaveImage();
        break;
    case MENU_ID_CONTEXT_2:
        // Copy to clipboard
        if (wxTheClipboard->Open())
        {
            // This data objects are held by the clipboard,
            // so do not delete them in the app.
            wxDataObject *data = new wxBitmapDataObject(*drawPane->bitmap);
            wxTheClipboard->SetData(data);
            wxTheClipboard->Flush();
            wxTheClipboard->Close();
        }
        break;
    }
}

DisplayImageFrame::~DisplayImageFrame()
{
}

void DisplayImageFrame::setColors(const std::vector<const minecraft::FinalColor *> &colorsMatrix, const std::vector<bool> &transparencyMatrix, size_t width, size_t height, bool preserveTransparency)
{
    drawPane->setColors(colorsMatrix, transparencyMatrix, width, height, preserveTransparency);
}

void DisplayImageFrame::setColors(const std::vector<colors::Color> &colorsMatrix, const std::vector<bool> &transparencyMatrix, size_t width, size_t height, bool preserveTransparency)
{
    drawPane->setColors(colorsMatrix, transparencyMatrix, width, height, preserveTransparency);
}

void DisplayImageFrame::OnSize(wxSizeEvent &event)
{
    drawPane->SetSize(this->GetClientSize());
    drawPane->Refresh();
}

void widgets::displayMapImage(std::vector<const minecraft::FinalColor *> &colorsMatrix, wxApp &app)
{
    wxInitAllImageHandlers();
    DisplayImageFrame *frame = new DisplayImageFrame(NULL, (string("Rendering minecraft map: ") + string(app.argv[1])), wxPoint(50, 50), wxSize(800, 600));
    std::vector<bool> transparency(0);
    frame->setColors(colorsMatrix, transparency, MAP_WIDTH, MAP_HEIGHT, false);
    frame->defaultFile = string(app.argv[1]) + string(".png");
    frame->Show(true);
}
