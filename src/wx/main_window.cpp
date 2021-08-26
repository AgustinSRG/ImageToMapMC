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

#include "main_window.h"
#include <wx/rawbmp.h>
#include <sstream>
#include "../resources/icon.xpm"

using namespace std;
using namespace minecraft;
using namespace colors;
using namespace mapart;
using namespace threading;

enum Identifiers
{
    ID_File_Open = 1,
    ID_File_New = 2,
    ID_File_Save = 3,
    ID_File_Save_As = 4,
    ID_Load_Image = 5,

    ID_Export_Map = 6,
    ID_Export_Structure = 7,
    ID_Export_World = 8,

    ID_Preferences = 15,
    ID_Export = 16,

    ID_Window_Original_Img = 21,
    ID_Window_Preview = 22,
    ID_Window_Settings = 23,
    ID_Window_ColorSet = 24,
    ID_Window_Materials_List = 25
};

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
EVT_MENU(ID_File_Open, MainWindow::openProject)
EVT_MENU(ID_Load_Image, MainWindow::onLoadImage)
EVT_MENU_RANGE(1500, 1600, MainWindow::onChangeVersion)
EVT_MENU(wxID_EXIT, MainWindow::onExit)
EVT_SIZE(MainWindow::OnSize)
END_EVENT_TABLE()

#define VERSION_ID_PREFIX (1500)

int getIdForVersionMenu(McVersion version)
{
    return VERSION_ID_PREFIX + (short)version;
}

MainWindow::MainWindow() : wxFrame(NULL, wxID_ANY, string("Minecraft Map Art Tool"), wxPoint(50, 50), wxSize(800, 600))
{
    previewGenerationThread = NULL;
    originalImagePanel = NULL;
    previewPanel = NULL;
    requiresPreviewGneration = false;
    previewInProgress = false;

    /* Menu Bar */
    menuBar = new wxMenuBar();

    // File
    wxMenu *menuFile = new wxMenu();

    menuFile->Append(ID_File_New, "&New Project\tCtrl+N", "");
    menuFile->Append(ID_File_Open, "&Open Project\tCtrl+O", "");
    menuFile->AppendSeparator();
    menuFile->Append(ID_Load_Image, "&Load Image\tCtrl+L", "");
    menuFile->AppendSeparator();
    menuFile->Append(ID_File_Save, "&Save Project\tCtrl+S", "");
    menuFile->Append(ID_File_Save_As, "&Save Project As...\tCtrl+Shift+S", "");
    menuFile->AppendSeparator();

    wxMenu *exportMenu = new wxMenu();
    exportMenu->Append(ID_Export_Map, "&Export as map files", "");
    exportMenu->Append(ID_Export_Structure, "&Export as structures", "");
    exportMenu->Append(ID_Export_World, "&Export as minecraft world", "");
    menuFile->AppendSubMenu(exportMenu, "&Export", "");

    menuFile->AppendSeparator();

    menuFile->Append(ID_Preferences, "&Preferences\tCtrl+P", "");

    menuFile->AppendSeparator();

    menuFile->Append(wxID_EXIT);

    menuBar->Append(menuFile, "&File");

    // View
    wxMenu *menuView = new wxMenu();
    menuView->Append(ID_Window_Original_Img, "&Original Image", "");
    menuView->Append(ID_Window_Preview, "&Preview", "");
    menuView->Append(ID_Window_Settings, "&Settings", "");
    menuView->Append(ID_Window_ColorSet, "&Customize blocks", "");
    menuView->Append(ID_Window_Materials_List, "&List of materials", "");
    menuBar->Append(menuView, "&View");

    // Version
    version = MC_LAST_VERSION;
    wxMenu *menuVersion = new wxMenu();
    wxMenuItem *ptrMenuVer;
    versionMenuItems.resize(6);
    ptrMenuVer = menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_17), "&1.17", "");
    ptrMenuVer->Check(true);
    versionMenuItems[0] = ptrMenuVer;
    ptrMenuVer = menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_16), "&1.16", "");
    versionMenuItems[1] = ptrMenuVer;
    ptrMenuVer = menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_15), "&1.15", "");
    versionMenuItems[2] = ptrMenuVer;
    ptrMenuVer = menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_14), "&1.14", "");
    versionMenuItems[3] = ptrMenuVer;
    ptrMenuVer = menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_13), "&1.13", "");
    versionMenuItems[4] = ptrMenuVer;
    ptrMenuVer = menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_12), "&1.12", "");
    versionMenuItems[5] = ptrMenuVer;
    menuBar->Append(menuVersion, "&Version");

    SetIcon(wxIcon(_ICON_ICO_XPM));

    wxStatusBar *statusBar = CreateStatusBar();
    statusBar->SetFieldsCount(3);
    statusBar->SetStatusText("Status: Ready", 0);
    statusBar->SetStatusText("Version: " + minecraft::versionToString(MC_LAST_VERSION), 1);
    statusBar->SetStatusText("Project: (not saved yet)", 2);

    SetMenuBar(menuBar);

    // Add sub windows

    originalImageColors = std::vector<colors::Color>(MAP_WIDTH * MAP_HEIGHT);
    originalImageWidth = MAP_WIDTH;
    originalImageHeight = MAP_HEIGHT;

    for (int i = 0; i < originalImageColors.size(); i++)
    {
        originalImageColors[i].red = 255;
        originalImageColors[i].green = 255;
        originalImageColors[i].blue = 255;
    }

    originalImagePanel = new wxImagePanel(this);
    originalImagePanel->SetPosition(wxPoint(0, 0));
    originalImagePanel->SetSize(GetClientSize().GetWidth() / 2 - FRAME_PAD_PIXELS, GetClientSize().GetHeight());
    originalImagePanel->setColors(originalImageColors, originalImageWidth, originalImageHeight);

    previewPanel = new wxImagePanel(this);
    previewPanel->SetPosition(wxPoint(GetClientSize().GetWidth() / 2 + FRAME_PAD_PIXELS, 0));
    previewPanel->SetSize(GetClientSize().GetWidth() / 2 - FRAME_PAD_PIXELS, GetClientSize().GetHeight());
    previewPanel->setColors(originalImageColors, originalImageWidth, originalImageHeight);

    Maximize();
}

MainWindow::~MainWindow()
{
    delete menuBar;
}

void MainWindow::openProject(wxCommandEvent &evt)
{
}

void MainWindow::onLoadImage(wxCommandEvent &evt)
{
    wxFileDialog
        openFileDialog(this, _("Load image file"), "", "",
                       "Image files (*.png, *.jpg)|*.png;*.jpg|All files|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...
    loadImage(openFileDialog.GetPath());
}

void MainWindow::loadImage(std::string file)
{
    wxImage image;
    if (!image.LoadFile(file))
    {
        wxMessageBox(string("Invalid image file: ") + file, wxT("Error"), wxICON_ERROR);
        return;
    }

    int matrixW;
    int matrixH;
    originalImageColors = loadColorMatrixFromImageAndPad(image, &matrixW, &matrixH);
    originalImageWidth = matrixW;
    originalImageHeight = matrixH;

    originalImagePanel->setColors(originalImageColors, originalImageWidth, originalImageHeight);
    originalImagePanel->Refresh();

    RequestPreviewGeneration();
}

void MainWindow::onExit(wxCommandEvent &evt)
{
    Close(true);
}

void MainWindow::onChangeVersion(wxCommandEvent &evt)
{
    version = static_cast<McVersion>(evt.GetId() - VERSION_ID_PREFIX);
    GetStatusBar()->SetStatusText("Version: " + minecraft::versionToString(version), 1);
    RequestPreviewGeneration();
}

void MainWindow::OnSize(wxSizeEvent &event)
{
    if (originalImagePanel != NULL)
    {
        originalImagePanel->SetPosition(wxPoint(0, 0));
        originalImagePanel->SetSize(GetClientSize().GetWidth() / 2 - FRAME_PAD_PIXELS, GetClientSize().GetHeight());
        originalImagePanel->Refresh();
    }

    if (previewPanel != NULL)
    {
        previewPanel->SetPosition(wxPoint(GetClientSize().GetWidth() / 2 + FRAME_PAD_PIXELS, 0));
        previewPanel->SetSize(GetClientSize().GetWidth() / 2 - FRAME_PAD_PIXELS, GetClientSize().GetHeight());
        previewPanel->Refresh();
    }
}

void MainWindow::RequestPreviewGeneration()
{
    mutexPreviewGeneration.lock();

    if (previewInProgress)
    {
        requiresPreviewGneration = true;
    }
    else
    {
        previewInProgress = true;
        requiresPreviewGneration = false;
        previewGenerationThread = new std::thread(&MainWindow::GeneratePreview, this);
    }

    mutexPreviewGeneration.unlock();
}

void MainWindow::ReportProgress(Progress &progress)
{
    bool ended = false;
    pair<string, unsigned int> p;
    string progressLine = "";
    size_t old_line_size = 0;

    while (!ended)
    {
        // Sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(REPORT_THREAD_DELAY));

        // Get progress
        p = progress.getProgress();

        // Print progress
        if (p.second == NO_PROGRESS)
        {
            progressLine = p.first;
        }
        else
        {
            stringstream ss;
            ss << p.first << "(" << p.second << "%)";
            progressLine = ss.str();
        }

        GetStatusBar()->SetStatusText(progressLine, 0);

        // Check ended
        ended = progress.hasEnded();
    }

    // Erase line
    GetStatusBar()->SetStatusText("Ready", 0);
}

void MainWindow::GeneratePreview()
{
    bool finished = false;

    while (!finished)
    {
        Progress p;
        int threadNum = 1;

        thread progressReportThread(&MainWindow::ReportProgress, this, std::ref(p));

        p.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        bool blacklist = true;

        p.startTask("Loading custom configuration...", 0, 0);
        // TODO

        p.startTask("Adjusting image colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, ColorDistanceAlgorithm::Euclidean, DitheringMethod::None, threadNum, p);

        p.setEnded();
        progressReportThread.join();

        previewPanel->setColors(mapArtColorMatrix, originalImageWidth, originalImageHeight);
        previewPanel->Refresh();

        mutexPreviewGeneration.lock();

        if (requiresPreviewGneration) {
            requiresPreviewGneration = false;
        } else {
            previewInProgress = false;
            finished = true;
        }

        mutexPreviewGeneration.unlock();
    }


}

void widgets::displayMainWindow(wxApp &app)
{
    wxInitAllImageHandlers();
    MainWindow *frame = new MainWindow();
    frame->Show(true);
}
