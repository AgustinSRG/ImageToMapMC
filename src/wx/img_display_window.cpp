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
}

void wxImagePanel::setColors(std::vector<const minecraft::FinalColor *> &colorsMatrix, size_t width, size_t height)
{
    if (bitmap != NULL)
    {
        delete bitmap;
        bitmap = NULL;
    }

    matrixHeight = height;
    matrixWidth = width;

    wxImage image(width, height);
    unsigned char *rawData = image.GetData();
    size_t size = width * height;

    size_t j = 0;
    for (size_t i = 0; i < size; i++)
    {
        colors::Color color = colorsMatrix[i]->color;

        rawData[j++] = color.red;
        rawData[j++] = color.green;
        rawData[j++] = color.blue;
    }

    bitmap = new wxBitmap(image);

    this->Refresh();
}

void wxImagePanel::setColors(std::vector<colors::Color> &colorsMatrix, size_t width, size_t height)
{
    if (bitmap != NULL)
    {
        delete bitmap;
        bitmap = NULL;
    }

    matrixHeight = height;
    matrixWidth = width;

    wxImage image(width, height);
    unsigned char *rawData = image.GetData();
    size_t size = colorsMatrix.size();

    size_t j = 0;
    for (size_t i = 0; i < size; i++)
    {
        colors::Color color = colorsMatrix[i];

        rawData[j++] = color.red;
        rawData[j++] = color.green;
        rawData[j++] = color.blue;
    }

    bitmap = new wxBitmap(image);

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
    wxPaintDC dc(this);
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
        dc.DrawBitmap(*bitmap, offsetX, offsetY, false);
    }
    else
    {
        dc.Clear();
    }
}

DisplayImageFrame::DisplayImageFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxInitAllImageHandlers();

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

void DisplayImageFrame::OnContextMenuSelected(wxCommandEvent &event)
{
    wxString str;

    switch (event.GetId())
    {
    case MENU_ID_CONTEXT_1:
        // Save to file
        break;
    case MENU_ID_CONTEXT_2:
        // Copy to clipboard
        if (wxTheClipboard->Open())
        {
            // This data objects are held by the clipboard,
            // so do not delete them in the app.
            wxDataObject * data = new wxBitmapDataObject(*drawPane->bitmap);
            wxTheClipboard->SetData(data);
            wxTheClipboard->Flush();
            wxTheClipboard->Close();
        }
        break;
    }
}

DisplayImageFrame::~DisplayImageFrame()
{
    delete drawPane;
    delete menu;
}

void DisplayImageFrame::setColors(std::vector<const minecraft::FinalColor *> &colorsMatrix, size_t width, size_t height)
{
    drawPane->setColors(colorsMatrix, width, height);
}

void DisplayImageFrame::setColors(std::vector<colors::Color> &colorsMatrix, size_t width, size_t height)
{
    drawPane->setColors(colorsMatrix, width, height);
}

void DisplayImageFrame::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void DisplayImageFrame::OnSize(wxSizeEvent &event)
{
    drawPane->SetSize(this->GetClientSize());
    drawPane->Refresh();
}

void widgets::displayMapImage(std::vector<const minecraft::FinalColor *> &colorsMatrix, wxApp &app)
{
    DisplayImageFrame *frame = new DisplayImageFrame((string("Rendering minecraft map: ") + string(app.argv[1])), wxPoint(50, 50), wxSize(800, 600));
    frame->setColors(colorsMatrix, MAP_WIDTH, MAP_HEIGH);
    frame->Show(true);
}
