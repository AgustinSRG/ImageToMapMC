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
#include "map_export_dialog.h"
#include "structure_export_dialog.h"
#include "image_resize_dialog.h"
#include <filesystem>
#include "../minecraft/structure.h"
#include "../minecraft/mcfunction.h"

#include <fstream>

#include <io/stream_reader.h>
#include <io/stream_writer.h>
#include <io/izlibstream.h>
#include <io/ozlibstream.h>

#include <nbt_tags.h>

#if defined(_WIN32)
#include <shellapi.h>
#endif

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
    ID_Export_Function = 8,

    ID_Export_Map_Zip = 9,
    ID_Export_Structure_Zip = 10,

    ID_Export = 16,
    ID_Resize_Image = 17,
    ID_Edit_Image = 18,
    ID_Blocks_Custom = 19,

    ID_Materials_Show = 21,
    ID_Materials_Save = 22,
    ID_Materials_Save_Split = 23,

    ID_Save_Image = 31,
    ID_Save_Preview = 32,

    ID_Timer = 50,
};

#define VERSION_ID_PREFIX (1500)
#define COLOR_METHOD_ID_PREFIX (1600)
#define BUILD_METHOD_ID_PREFIX (1700)
#define DITHERING_ID_PREFIX (1800)

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
EVT_MENU(ID_File_New, MainWindow::newProject)
EVT_MENU(ID_File_Open, MainWindow::openProject)
EVT_MENU(ID_File_Save, MainWindow::saveProject)
EVT_MENU(ID_File_Save_As, MainWindow::saveProjectAs)
EVT_MENU(ID_Load_Image, MainWindow::onLoadImage)
EVT_MENU(ID_Save_Image, MainWindow::saveImageAs)
EVT_MENU(ID_Save_Preview, MainWindow::savePreviewAs)
EVT_MENU(ID_Materials_Save, MainWindow::OnSaveMaterialsList)
EVT_MENU(ID_Materials_Save_Split, MainWindow::OnSaveMaterialsListSplit)
EVT_MENU_RANGE(VERSION_ID_PREFIX, VERSION_ID_PREFIX + 99, MainWindow::onChangeVersion)
EVT_MENU_RANGE(COLOR_METHOD_ID_PREFIX, COLOR_METHOD_ID_PREFIX + 99, MainWindow::onChangeColorAlgo)
EVT_MENU_RANGE(BUILD_METHOD_ID_PREFIX, BUILD_METHOD_ID_PREFIX + 99, MainWindow::onChangeBuildMethod)
EVT_MENU_RANGE(DITHERING_ID_PREFIX, DITHERING_ID_PREFIX + 99, MainWindow::onChangeDithering)
EVT_MENU(wxID_EXIT, MainWindow::onExit)
EVT_MENU(ID_Blocks_Custom, MainWindow::onCustomBlocks)
EVT_SIZE(MainWindow::OnSize)
EVT_DROP_FILES(MainWindow::handleDropFile)
EVT_MENU(ID_Export_Map, MainWindow::onExportToMaps)
EVT_MENU(ID_Export_Map_Zip, MainWindow::onExportToMapsZip)
EVT_MENU(ID_Export_Structure, MainWindow::onExportToStructure)
EVT_MENU(ID_Export_Structure_Zip, MainWindow::onExportToStructureZip)
EVT_MENU(ID_Export_Function, MainWindow::onExportToFunctions)
EVT_MENU(ID_Resize_Image, MainWindow::onImageResize)
EVT_MENU(ID_Edit_Image, MainWindow::onImageEdit)
EVT_TIMER(ID_Timer, MainWindow::OnProgressTimer)
EVT_CLOSE(MainWindow::OnClose)
EVT_COMMAND(wxID_ANY, wxEVT_WorkerThreadPreviewData, MainWindow::onWorkerPreviewDone)
EVT_COMMAND(wxID_ANY, wxEVT_WorkerThreadMaterials, MainWindow::onWorkerMaterialsGiven)
EVT_COMMAND(wxID_ANY, wxEVT_WorkerThreadError, MainWindow::onWorkerError)
END_EVENT_TABLE()

int getIdForVersionMenu(McVersion version)
{
    return VERSION_ID_PREFIX + (short)version;
}

int getIdForColorAlgoMenu(ColorDistanceAlgorithm a)
{
    return COLOR_METHOD_ID_PREFIX + (short)a;
}

int getIdForBuildMethodMenu(MapBuildMethod a)
{
    return BUILD_METHOD_ID_PREFIX + (short)a;
}

int getIdForDitheringMenu(DitheringMethod a)
{
    return DITHERING_ID_PREFIX + (short)a;
}

MainWindow::MainWindow() : wxFrame(NULL, wxID_ANY, string("Minecraft Map Art Tool"), wxPoint(50, 50), wxSize(800, 600))
{
    materialsWindow = NULL;
    originalImagePanel = NULL;
    previewPanel = NULL;

    project = MapArtProject();

    dirty = false;
    projectFile = "";

    imageEditDialog = NULL;

    std::vector<size_t> cmats(MAX_COLOR_GROUPS);
    for (size_t i = 0; i < MAX_COLOR_GROUPS; i++)
    {
        cmats[i] = 0;
    }
    countsMats = cmats;

    // Worker thread
    threadNum = max((unsigned int)1, std::thread::hardware_concurrency());
    this->workerThread = new WorkerThread(this, threadNum);
    this->workerThread->Run();

    /* Menu Bar */
    menuBar = new wxMenuBar();

    // File
    wxMenu *menuFile = new wxMenu();

    menuFile->Append(ID_File_New, "&New Project\tCtrl+N", "Creates a new project");

    menuFile->AppendSeparator();

    menuFile->Append(ID_File_Open, "&Open Project\tCtrl+O", "Opens an existing project");

    menuFile->AppendSeparator();
    menuFile->Append(ID_File_Save, "&Save Project\tCtrl+S", "Saves the project");
    menuFile->Append(ID_File_Save_As, "&Save Project As...\tCtrl+Shift+S", "Saves the project to another file (creates a copy)");
    menuFile->AppendSeparator();

    wxMenu *exportMenu = new wxMenu();
    exportMenu->Append(ID_Export_Map, "&Export as map files\tCtrl+E", "Exports the map to Minecraft map files (for server admins)");
    exportMenu->Append(ID_Export_Map_Zip, "&Export as map files (Zip file)\tCtrl+Shift+E", "Exports the map to Minecraft map files inside a zip file (for server admins)");
    exportMenu->Append(ID_Export_Structure, "&Export as structures\tCtrl+T", "Exports the map to nbt structure files (for survival)");
    exportMenu->Append(ID_Export_Structure_Zip, "&Export as structures (Zip file)\tCtrl+Shift+T", "Exports the map to nbt structure files inside a zip file (for survival)");
    exportMenu->Append(ID_Export_Function, "&Export as functions\tCtrl+F", "Exports the map to minecraft function file (for flat maps)");
    menuFile->AppendSubMenu(exportMenu, "&Export", "Exports the map, so you can use it in Minecraft");

    menuFile->AppendSeparator();

    menuFile->Append(wxID_EXIT);

    menuBar->Append(menuFile, "&File");

    // Image
    wxMenu *menuImage = new wxMenu();
    menuImage->Append(ID_Load_Image, "&Load Image\tCtrl+L", "Loads a new image");
    menuImage->AppendSeparator();
    menuImage->Append(ID_Resize_Image, "&Resize Image\tCtrl+Shift+R", "Resizes image");
    menuImage->Append(ID_Edit_Image, "&Image Settings\tCtrl+Shift+B", "Image settings: brightness, saturation, constrast and background color");
    menuImage->AppendSeparator();
    menuImage->Append(ID_Save_Image, "&Save Image As...", "Saves image to a file");
    menuImage->Append(ID_Save_Preview, "&Save Preview As...", "Saves preview image to a file");
    menuBar->Append(menuImage, "&Image");

    // Materials
    wxMenu *menuMaterials = new wxMenu();
    menuMaterials->Append(ID_Blocks_Custom, "&Customize materials\tCtrl+M", "Customizes materials to use");
    menuMaterials->AppendSeparator();
    menuMaterials->Append(ID_Materials_Save, "&Export materials list\tCtrl+Shift+M", "Exports list of materials to a text file");
    menuMaterials->Append(ID_Materials_Save_Split, "&Export materials list (128x128 sections)", "Exports list of materials to a text file. The materials are split in 128x128 blocks sections.");
    menuBar->Append(menuMaterials, "&Materials");

    // Color distance
    wxMenu *menuColorDistance = new wxMenu();
    menuColorDistance->AppendRadioItem(getIdForColorAlgoMenu(ColorDistanceAlgorithm::Euclidean), "&Euclidean", "Simple squared euclidean distance in the RGB color space")->Check(true);
    menuColorDistance->AppendRadioItem(getIdForColorAlgoMenu(ColorDistanceAlgorithm::DeltaE), "&Delta E", "Distance in the Lab color space");
    menuBar->Append(menuColorDistance, "&Color aproximation");

    // Dithering
    wxMenu *menuDithering = new wxMenu();
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::None), "&None", "No dithering")->Check(true);
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::FloydSteinberg), "&Floyd Steinberg", "Diffusses the error to create the illusion of more colors");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::MinAvgErr), "&Min Average Error", "Diffusses the error to create the illusion of more colors");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Atkinson), "&Atkinson", "Diffusses the error to create the illusion of more colors");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Stucki), "&Stucki", "Diffusses the error to create the illusion of more colors");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::SierraLite), "&Sierra Lite", "Diffusses the error to create the illusion of more colors");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Burkes), "&Burkes", "Diffusses the error to create the illusion of more colors");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Bayer22), "&Bayer (2x2)", "Applies filter to create the illusion of more colors");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Bayer44), "&Bayer (4x4)", "Applies filter to create the illusion of more colors");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Ordered33), "&Ordered (3x3)", "Applies filter to create the illusion of more colors");
    menuBar->Append(menuDithering, "&Dithering");

    // Build method
    wxMenu *menuBuildMethod = new wxMenu();
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::Staircased), "&Staircased", "Staircased map with jumps of only a single block")->Check(true);
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::Chaos), "&3D (Complex)", "Staircased map with jumps of arbitrary size");
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::Flat), "&Flat", "Flat map (only 1/3 of the colors)");
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::None), "&None", "Use all the colors (even the unobtainable with blocks)");
    menuBar->Append(menuBuildMethod, "&Build Method");

    // Version
    wxMenu *menuVersion = new wxMenu();
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_19), "&1.19", "Version: 1.19")->Check(true);
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_18), "&1.18.2", "Version: 1.18.2");
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_17), "&1.17.1", "Version: 1.17.1");
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_16), "&1.16.5", "Version: 1.16.5");
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_15), "&1.15.2", "Version: 1.15.2");
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_14), "&1.14.4", "Version: 1.14.4");
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_13), "&1.13.2", "Version: 1.13.2");
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_12), "&1.12.2", "Version: 1.12.2");
    menuBar->Append(menuVersion, "&Version");

    SetIcon(wxIcon(_ICON_ICO_XPM));

    wxStatusBar *statusBar = CreateStatusBar();
    statusBar->SetFieldsCount(4);
    statusBar->SetStatusText("Map art tool for minecraft", 0);
    statusBar->SetStatusText("Status: Ready", 1);
    statusBar->SetStatusText("Size: 1 x 1", 2);
    statusBar->SetStatusText("Project: (not saved yet)", 3);

    wxTimer *progressTimer = new wxTimer(this, ID_Timer);
    progressTimer->Start(40); // 25 FPS

    SetMenuBar(menuBar);

    // Add sub windows

    originalImagePanel = new wxImagePanel(this);
    originalImagePanel->SetPosition(wxPoint(0, 0));
    originalImagePanel->SetSize(GetClientSize().GetWidth() / 2 - FRAME_PAD_PIXELS, GetClientSize().GetHeight());
    originalImagePanel->setColors(project.getColors(), project.width, project.height);

    previewPanel = new wxImagePanel(this);
    previewPanel->SetPosition(wxPoint(GetClientSize().GetWidth() / 2 + FRAME_PAD_PIXELS, 0));
    previewPanel->SetSize(GetClientSize().GetWidth() / 2 - FRAME_PAD_PIXELS, GetClientSize().GetHeight());
    previewPanel->setColors(project.getColors(), project.width, project.height);

    Maximize();

    DragAcceptFiles(true);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onLoadImage(wxCommandEvent &evt)
{
    wxFileDialog
        openFileDialog(this, _("Load image file"), "", "",
                       "Image files (*.png, *.jpg)|*.png;*.jpg|All files|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...
    loadImage(openFileDialog.GetPath().ToStdString());
    dirty = true;
}

void MainWindow::loadImage(std::string file)
{
    wxImage image;
    wxLogNull logNo;
    if (!image.LoadFile(file))
    {
        wxMessageBox(string("Invalid image file: ") + file, wxT("Error"), wxICON_ERROR);
        return;
    }

    project.loadImage(image);

    project.saturation = 1;
    project.contrast = 1;
    project.brightness = 1;

    if (imageEditDialog != NULL)
    {
        imageEditDialog->SetParams(project.saturation, project.contrast, project.brightness, project.background);
    }

    updateOriginalImage();
}

void MainWindow::updateOriginalImage()
{
    wxImage imageCopy = project.toImage();

    if (project.resize_width > 0 && project.resize_height > 0)
    {
        imageCopy.Rescale(project.resize_width, project.resize_height);
    }

    int matrixW;
    int matrixH;
    vector<Color> originalImageColors = loadColorMatrixFromImageAndPad(imageCopy, project.background, &matrixW, &matrixH);

    tools::editImage(originalImageColors, matrixW, matrixH, project.saturation, project.contrast, project.brightness);

    originalImagePanel->setColors(originalImageColors, matrixW, matrixH);
    originalImagePanel->Refresh();

    stringstream ss;
    ss << "Size: " << (matrixW / MAP_WIDTH) << " x " << (matrixH / MAP_HEIGHT);
    GetStatusBar()->SetStatusText(ss.str(), 2);

    RequestPreviewGeneration();
}

void MainWindow::onExit(wxCommandEvent &evt)
{
    Close();
}

void MainWindow::onCustomBlocks(wxCommandEvent &evt)
{
    // Materials window
    if (materialsWindow == NULL)
    {
        materialsWindow = new MaterialsWindow(this);
        materialsWindow->Show();
        materialsWindow->displayCountMaterials(countsMats);
        materialsWindow->setMaterialsConf(project.version, project.colorSetConf);
    }
    else
    {
        materialsWindow->Show();
        materialsWindow->Raise();
    }
}

void MainWindow::onChangeVersion(wxCommandEvent &evt)
{
    project.version = static_cast<McVersion>(evt.GetId() - VERSION_ID_PREFIX);
    if (materialsWindow != NULL)
    {
        materialsWindow->setMaterialsConf(project.version, project.colorSetConf);
    }
    dirty = true;
    RequestPreviewGeneration();
}

void MainWindow::onChangeBuildMethod(wxCommandEvent &evt)
{
    project.buildMethod = static_cast<MapBuildMethod>(evt.GetId() - BUILD_METHOD_ID_PREFIX);
    dirty = true;
    RequestPreviewGeneration();
}

void MainWindow::onChangeColorAlgo(wxCommandEvent &evt)
{
    project.colorDistanceAlgorithm = static_cast<ColorDistanceAlgorithm>(evt.GetId() - COLOR_METHOD_ID_PREFIX);
    dirty = true;
    RequestPreviewGeneration();
}

void MainWindow::onChangeDithering(wxCommandEvent &evt)
{
    project.ditheringMethod = static_cast<DitheringMethod>(evt.GetId() - DITHERING_ID_PREFIX);
    dirty = true;
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
    this->workerThread->requestGeneratePreview(project);
}

void MainWindow::OnSaveMaterialsList(wxCommandEvent &evt)
{
    if (project.buildMethod == MapBuildMethod::None)
    {
        wxMessageBox(wxString("You must choose a build method to create the materials list"), wxT("Error"), wxICON_ERROR);
        return;
    }

    if (this->workerThread->isBusy())
    {
        wxMessageBox(wxString("There is already a task in progress. Wait for it to end to continue."), wxT("Error"), wxICON_ERROR);
        return;
    }

    wxFileDialog saveFileDialog(this, _("Save materials list"), "", "", "Text file (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() != wxID_CANCEL)
    {
        this->workerThread->requestExportMaterials(project, saveFileDialog.GetPath().ToStdString());
    }
}

void MainWindow::OnSaveMaterialsListSplit(wxCommandEvent &evt)
{
    if (project.buildMethod == MapBuildMethod::None)
    {
        wxMessageBox(wxString("You must choose a build method to create the materials list"), wxT("Error"), wxICON_ERROR);
        return;
    }

    if (this->workerThread->isBusy())
    {
        wxMessageBox(wxString("There is already a task in progress. Wait for it to end to continue."), wxT("Error"), wxICON_ERROR);
        return;
    }

    wxFileDialog saveFileDialog(this, _("Save materials list"), "", "", "Text file (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() != wxID_CANCEL)
    {
        this->workerThread->requestExportMaterialsSplit(project, saveFileDialog.GetPath().ToStdString());
    }
}

void MainWindow::handleDropFile(wxDropFilesEvent &event)
{
    if (event.GetNumberOfFiles() > 0)
    {
        string filename = string(event.GetFiles()[0]);

        MapArtProject tryProject;

        if (tryProject.loadFromFile(filename))
        {
            // The file is a project

            if (dirty)
            {
                int r = wxMessageBox("Do you want to save the changes before closing the project?", "Save changes?", wxICON_QUESTION | wxCANCEL | wxYES_NO | wxICON_WARNING);

                if (r == wxCANCEL)
                {
                    return;
                }
                else if (r == wxYES)
                {
                    wxCommandEvent evt;
                    if (projectFile.length() > 0)
                    {
                        saveProject(evt);
                    }
                    else
                    {
                        saveProjectAs(evt);
                        if (projectFile.length() == 0)
                        {
                            return;
                        }
                    }
                }
            }

            project = tryProject;

            if (imageEditDialog != NULL)
            {
                imageEditDialog->SetParams(project.saturation, project.contrast, project.brightness, project.background);
            }

            if (materialsWindow != NULL)
            {
                materialsWindow->setMaterialsConf(project.version, project.colorSetConf);
            }

            // Not dirty
            dirty = false;

            // Not saved yet
            projectFile = filename;

            GetStatusBar()->SetStatusText("Project: " + projectFile, 3);

            updateMenuBarRadios();

            updateOriginalImage();
        }
        else
        {
            loadImage(filename);
            dirty = true;
        }
    }
}

void MainWindow::changeColorSetConf(std::string conf)
{
    project.colorSetConf = conf;
    dirty = true;
    RequestPreviewGeneration();
}

void widgets::displayMainWindow(wxApp &app)
{
    wxInitAllImageHandlers();
    MainWindow *frame = new MainWindow();
    frame->Show(true);

    if (app.argc > 1)
    {
        frame->loadProject(app.argv[1].ToStdString());
    }
}

void MainWindow::onExportToMaps(wxCommandEvent &evt)
{

    if (this->workerThread->isBusy())
    {
        wxMessageBox(wxString("There is already a task in progress. Wait for it to end to continue."), wxT("Error"), wxICON_ERROR);
        return;
    }

    MapExportDialog dialog;
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    this->workerThread->requestExportMaps(project, dialog.getPath(), dialog.getMapNumber());
}

void MainWindow::onExportToMapsZip(wxCommandEvent &evt)
{
    if (this->workerThread->isBusy())
    {
        wxMessageBox(wxString("There is already a task in progress. Wait for it to end to continue."), wxT("Error"), wxICON_ERROR);
        return;
    }

    wxFileDialog saveFileDialog(this, _("Export as map files"), "", "", "Compressed zip files (*.zip)|*.zip", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() != wxID_CANCEL)
    {
        this->workerThread->requestExportMapsZip(project, saveFileDialog.GetPath().ToStdString());
    }
}

void MainWindow::onExportToStructureZip(wxCommandEvent &evt)
{
    if (project.buildMethod == MapBuildMethod::None)
    {
        wxMessageBox(wxString("You must choose a build method to be able to export to structures."), wxT("Cannot export"), wxICON_INFORMATION);
        return;
    }

    if (this->workerThread->isBusy())
    {
        wxMessageBox(wxString("There is already a task in progress. Wait for it to end to continue."), wxT("Error"), wxICON_ERROR);
        return;
    }

    wxFileDialog saveFileDialog(this, _("Export as structure files"), "", "", "Compressed zip files (*.zip)|*.zip", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() != wxID_CANCEL)
    {
        this->workerThread->requestExportStructZip(project, saveFileDialog.GetPath().ToStdString());
    }
}

void MainWindow::onExportToStructure(wxCommandEvent &evt)
{
    if (project.buildMethod == MapBuildMethod::None)
    {
        wxMessageBox(wxString("You must choose a build method to be able to export to structures."), wxT("Cannot export"), wxICON_INFORMATION);
        return;
    }

    if (this->workerThread->isBusy())
    {
        wxMessageBox(wxString("There is already a task in progress. Wait for it to end to continue."), wxT("Error"), wxICON_ERROR);
        return;
    }

    StructureExportDialog dialog(project.version, ExportDialogMode::Structure);
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    this->workerThread->requestExportStruct(project, dialog.getPath());
}

void MainWindow::onExportToFunctions(wxCommandEvent &evt)
{
    if (project.buildMethod != MapBuildMethod::Flat)
    {
        wxMessageBox(wxString("Only flat maps can be exported to minecraft functions."), wxT("Cannot export"), wxICON_INFORMATION);
        return;
    }

    if (this->workerThread->isBusy())
    {
        wxMessageBox(wxString("There is already a task in progress. Wait for it to end to continue."), wxT("Error"), wxICON_ERROR);
        return;
    }

    StructureExportDialog dialog(project.version, ExportDialogMode::Function);
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    this->workerThread->requestExportFunc(project, dialog.getPath());
}

void MainWindow::onImageResize(wxCommandEvent &evt)
{
    ImageResizeDialog dialog(project.resize_width, project.resize_height);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    project.resize_width = dialog.getWidth();
    project.resize_height = dialog.getHeight();

    dirty = true;

    updateOriginalImage();
}

void MainWindow::onImageEdit(wxCommandEvent &evt)
{
    if (imageEditDialog == NULL)
    {
        imageEditDialog = new ImageEditDialog(this);
        imageEditDialog->Show();
        imageEditDialog->SetParams(project.saturation, project.contrast, project.brightness, project.background);
    }
    else
    {
        imageEditDialog->Show();
        imageEditDialog->Raise();
    }
}

void MainWindow::onImageEditParamsChanged(float saturation, float contrast, float brightness, colors::Color background)
{
    project.saturation = saturation;
    project.contrast = contrast;
    project.brightness = brightness;
    project.background = background;
    dirty = true;
    updateOriginalImage();
}

void MainWindow::resetProject()
{
    project = MapArtProject();

    if (imageEditDialog != NULL)
    {
        imageEditDialog->SetParams(project.saturation, project.contrast, project.brightness, project.background);
    }

    // Not dirty
    dirty = false;

    // Not saved yet
    projectFile = "";

    if (materialsWindow != NULL)
    {
        materialsWindow->setMaterialsConf(project.version, project.colorSetConf);
    }

    // Map params

    GetStatusBar()->SetStatusText("Project: (not saved yet)", 3);

    updateMenuBarRadios();

    updateOriginalImage();
}

void MainWindow::loadProject(std::string path)
{

    if (project.loadFromFile(path))
    {
        if (imageEditDialog != NULL)
        {
            imageEditDialog->SetParams(project.saturation, project.contrast, project.brightness, project.background);
        }

        if (materialsWindow != NULL)
        {
            materialsWindow->setMaterialsConf(project.version, project.colorSetConf);
        }

        // Not dirty
        dirty = false;

        // Not saved yet
        projectFile = path;

        GetStatusBar()->SetStatusText("Project: " + projectFile, 3);

        updateMenuBarRadios();

        updateOriginalImage();
    }
    else
    {
        wxMessageBox(string("Could not load project file: ") + path, wxT("Error"), wxICON_ERROR);
        resetProject();
    }
}

void MainWindow::saveProject(std::string path)
{
    if (project.saveToFile(path))
    {
        dirty = false;
        projectFile = path;
        GetStatusBar()->SetStatusText("Project: " + projectFile, 3);
    }
    else
    {
        wxMessageBox(string("Could not save project file: ") + path, wxT("Error"), wxICON_ERROR);
    }
}

void MainWindow::OnClose(wxCloseEvent &event)
{
    if (event.CanVeto() && workerThread->isBusy())
    {
        int r = wxMessageBox("There is a task running. Do you want to terminate it?", "Terminate task", wxICON_QUESTION | wxYES_NO | wxICON_WARNING);

        if (r != wxYES)
        {
            event.Veto();
            return;
        }
    }

    if (event.CanVeto() && dirty)
    {
        int r = wxMessageBox("Do you want to save the changes before closing the project?", "Save changes?", wxICON_QUESTION | wxCANCEL | wxYES_NO | wxICON_WARNING);

        if (r == wxCANCEL)
        {
            event.Veto();
            return;
        }
        else if (r == wxYES)
        {
            wxCommandEvent evt;
            if (projectFile.length() > 0)
            {
                saveProject(evt);
            }
            else
            {
                saveProjectAs(evt);
                if (projectFile.length() == 0)
                {
                    event.Veto();
                    return;
                }
            }
        }
    }

    event.Skip();
}

void MainWindow::openProject(wxCommandEvent &evt)
{
    wxFileDialog
        openFileDialog(this, _("Load project"), "", "",
                       "Map art projects (*.mapart)|*.mapart|All files|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...

    if (dirty)
    {
        int r = wxMessageBox("Do you want to save the changes before closing the project?", "Save changes?", wxICON_QUESTION | wxCANCEL | wxYES_NO | wxICON_WARNING);

        if (r == wxCANCEL)
        {
            return;
        }
        else if (r == wxYES)
        {
            wxCommandEvent evt;
            if (projectFile.length() > 0)
            {
                saveProject(evt);
            }
            else
            {
                saveProjectAs(evt);
                if (projectFile.length() == 0)
                {
                    return;
                }
            }
        }
    }

    loadProject(openFileDialog.GetPath().ToStdString());
}

void MainWindow::newProject(wxCommandEvent &evt)
{
    if (dirty)
    {
        int r = wxMessageBox("Do you want to save the changes before closing the project?", "Save changes?", wxICON_QUESTION | wxCANCEL | wxYES_NO | wxICON_WARNING);

        if (r == wxCANCEL)
        {
            return;
        }
        else if (r == wxYES)
        {
            wxCommandEvent evt;
            if (projectFile.length() > 0)
            {
                saveProject(evt);
            }
            else
            {
                saveProjectAs(evt);
                if (projectFile.length() == 0)
                {
                    return;
                }
            }
        }
    }

    resetProject();
}

void MainWindow::saveProject(wxCommandEvent &evt)
{
    if (projectFile.length() == 0)
    {
        saveProjectAs(evt);
        return;
    }

    saveProject(projectFile);
}

void MainWindow::saveProjectAs(wxCommandEvent &evt)
{
    wxFileDialog saveFileDialog(this, _("Save project"), "", "", "Map art projects (*.mapart)|*.mapart", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    saveProject(saveFileDialog.GetPath().ToStdString());
}

void MainWindow::updateMenuBarRadios()
{
    switch (project.colorDistanceAlgorithm)
    {
    case ColorDistanceAlgorithm::DeltaE:
        GetMenuBar()->GetMenu(3)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(3)->GetMenuItems()[1]->Check(true);
        break;
    default:
        GetMenuBar()->GetMenu(3)->GetMenuItems()[0]->Check(true);
        GetMenuBar()->GetMenu(3)->GetMenuItems()[1]->Check(false);
    }

    switch (project.ditheringMethod)
    {
    case DitheringMethod::FloydSteinberg:
        GetMenuBar()->GetMenu(4)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[1]->Check(true);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[5]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[6]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[7]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[8]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[9]->Check(false);
        break;
    case DitheringMethod::MinAvgErr:
        GetMenuBar()->GetMenu(4)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[2]->Check(true);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[5]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[6]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[7]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[8]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[9]->Check(false);
        break;
    case DitheringMethod::Atkinson:
        GetMenuBar()->GetMenu(4)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[3]->Check(true);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[5]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[6]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[7]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[8]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[9]->Check(false);
        break;
    case DitheringMethod::Stucki:
        GetMenuBar()->GetMenu(4)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[4]->Check(true);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[5]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[6]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[7]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[8]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[9]->Check(false);
        break;
    case DitheringMethod::SierraLite:
        GetMenuBar()->GetMenu(4)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[5]->Check(true);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[6]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[7]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[8]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[9]->Check(false);
        break;
    case DitheringMethod::Burkes:
        GetMenuBar()->GetMenu(4)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[5]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[6]->Check(true);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[7]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[8]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[9]->Check(false);
        break;
    case DitheringMethod::Bayer22:
        GetMenuBar()->GetMenu(4)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[5]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[6]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[7]->Check(true);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[8]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[9]->Check(false);
        break;
    case DitheringMethod::Bayer44:
        GetMenuBar()->GetMenu(4)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[5]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[6]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[7]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[8]->Check(true);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[9]->Check(false);
        break;
    case DitheringMethod::Ordered33:
        GetMenuBar()->GetMenu(4)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[5]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[6]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[7]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[8]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[9]->Check(true);
        break;
    default:
        GetMenuBar()->GetMenu(4)->GetMenuItems()[0]->Check(true);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[5]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[6]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[7]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[8]->Check(false);
        GetMenuBar()->GetMenu(4)->GetMenuItems()[9]->Check(false);
    }

    switch (project.buildMethod)
    {
    case MapBuildMethod::Chaos:
        GetMenuBar()->GetMenu(5)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[1]->Check(true);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[3]->Check(false);
        break;
    case MapBuildMethod::Staircased:
        GetMenuBar()->GetMenu(5)->GetMenuItems()[0]->Check(true);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[3]->Check(false);
        break;
    case MapBuildMethod::Flat:
        GetMenuBar()->GetMenu(5)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[2]->Check(true);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[3]->Check(false);
        break;
    default:
        GetMenuBar()->GetMenu(5)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[3]->Check(true);
    }

    GetMenuBar()->GetMenu(6)->GetMenuItems()[0]->Check(false);
    GetMenuBar()->GetMenu(6)->GetMenuItems()[1]->Check(false);
    GetMenuBar()->GetMenu(6)->GetMenuItems()[2]->Check(false);
    GetMenuBar()->GetMenu(6)->GetMenuItems()[3]->Check(false);
    GetMenuBar()->GetMenu(6)->GetMenuItems()[4]->Check(false);
    GetMenuBar()->GetMenu(6)->GetMenuItems()[5]->Check(false);
    GetMenuBar()->GetMenu(6)->GetMenuItems()[6]->Check(false);
    GetMenuBar()->GetMenu(6)->GetMenuItems()[7]->Check(false);

    switch (project.version)
    {
    case McVersion::MC_1_19:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[0]->Check(true);
        break;
    case McVersion::MC_1_18:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[1]->Check(true);
        break;
    case McVersion::MC_1_17:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[2]->Check(true);
        break;
    case McVersion::MC_1_16:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[3]->Check(true);
        break;
    case McVersion::MC_1_15:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[4]->Check(true);
        break;
    case McVersion::MC_1_14:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[5]->Check(true);
        break;
    case McVersion::MC_1_13:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[6]->Check(true);
        break;
    default:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[7]->Check(true);
        break;
    }
}

void MainWindow::OnProgressTimer(wxTimerEvent &event)
{
    GetStatusBar()->SetStatusText(wxString(this->workerThread->GetStatus()), 1);
}

void MainWindow::onWorkerError(wxCommandEvent &event)
{
    wxMessageBox(event.GetString(), wxT("Error"), wxICON_ERROR);
}

void MainWindow::onWorkerPreviewDone(wxCommandEvent &event)
{
    MapArtPreviewData data = this->workerThread->GetPreviewData();
    previewPanel->setColors(data.colors, data.width, data.height);
}
void MainWindow::onWorkerMaterialsGiven(wxCommandEvent &event)
{
    countsMats = this->workerThread->GetMaterialsCount();

    if (materialsWindow != NULL)
    {
        materialsWindow->displayCountMaterials(countsMats);
    }
}

void MainWindow::saveImageAs(wxCommandEvent &evt)
{
    wxFileDialog saveFileDialog(this, _("Save image"), "", "", "PNG Image file (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    originalImagePanel->bitmap->ConvertToImage().SaveFile(saveFileDialog.GetPath());
}

void MainWindow::savePreviewAs(wxCommandEvent &evt)
{
    wxFileDialog saveFileDialog(this, _("Save preview"), "", "", "PNG Image file (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    previewPanel->bitmap->ConvertToImage().SaveFile(saveFileDialog.GetPath());
}
