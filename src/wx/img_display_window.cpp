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

/* Define IDS */

enum
{
    ID_Hello = 1
};

/* Events */

BEGIN_EVENT_TABLE(DisplayImageFrame, wxFrame)
EVT_MENU(ID_Hello, DisplayImageFrame::OnHello)
EVT_MENU(wxID_EXIT, DisplayImageFrame::OnExit)
EVT_MENU(wxID_ABOUT, DisplayImageFrame::OnAbout)
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

wxImagePanel::wxImagePanel(wxFrame* parent, std::vector<minecraft::FinalColor *> &colorsMatrix) :
wxPanel(parent)
{
    // load the file... ideally add a check to see if loading was successful

    wxImage image(MAP_WIDTH, MAP_HEIGH);
    unsigned char* rawData = image.GetData();
    size_t size = colorsMatrix.size();

    size_t j = 0;
    for (size_t i = 0; i < size; i++) {
        colors::Color color = colorsMatrix[i]->color;

        rawData[j++] = color.red;
        rawData[j++] = color.green;
        rawData[j++] = color.blue;
    }


    bitmap = new wxBitmap(image);
}


void wxImagePanel::paintEvent(wxPaintEvent & evt)
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

void wxImagePanel::render(wxDC&  dc)
{
    dc.DrawBitmap( *bitmap, 0, 0, false );
}

DisplayImageApp::DisplayImageApp(std::vector<minecraft::FinalColor *> &cm) {
    colorsMatrix = &cm;
}

bool DisplayImageApp::OnInit()
{
    frame = new DisplayImageFrame("Hello World", wxPoint(50, 50), wxSize(450, 340), *colorsMatrix);
    frame->Show(true);
    return true;
}
DisplayImageFrame::DisplayImageFrame(const wxString &title, const wxPoint &pos, const wxSize &size, std::vector<minecraft::FinalColor *> &colorsMatrix)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    // make sure to call this first
    wxInitAllImageHandlers();

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

    drawPane = new wxImagePanel( this, colorsMatrix);
    sizer->Add(drawPane, 1, wxEXPAND);
    this->SetSizer(sizer);

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");


}
void DisplayImageFrame::OnExit(wxCommandEvent &event)
{
    Close(true);
}
void DisplayImageFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox("This is a wxWidgets' Hello world sample",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
void DisplayImageFrame::OnHello(wxCommandEvent &event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

void widgets::displayMapImage(std::vector<minecraft::FinalColor *> &colorsMatrix, int argc, char **argv)
{
    wxApp::SetInstance(new DisplayImageApp(colorsMatrix));
    wxEntryStart(argc, argv);
    wxTheApp->CallOnInit();

    wxTheApp->OnRun();
    wxTheApp->OnExit();
    wxEntryCleanup();
}
