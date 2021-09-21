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

    ID_Export = 16,
    ID_Resize_Image = 17,
    ID_Edit_Image = 18,
    ID_Blocks_Custom = 19,

    ID_Materials_Show = 21,
    ID_Materials_Save = 22
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
EVT_MENU(ID_Materials_Save, MainWindow::OnSaveMaterialsList)
EVT_MENU_RANGE(VERSION_ID_PREFIX, VERSION_ID_PREFIX + 99, MainWindow::onChangeVersion)
EVT_MENU_RANGE(COLOR_METHOD_ID_PREFIX, COLOR_METHOD_ID_PREFIX + 99, MainWindow::onChangeColorAlgo)
EVT_MENU_RANGE(BUILD_METHOD_ID_PREFIX, BUILD_METHOD_ID_PREFIX + 99, MainWindow::onChangeBuildMethod)
EVT_MENU_RANGE(DITHERING_ID_PREFIX, DITHERING_ID_PREFIX + 99, MainWindow::onChangeDithering)
EVT_MENU(wxID_EXIT, MainWindow::onExit)
EVT_MENU(ID_Blocks_Custom, MainWindow::onCustomBlocks)
EVT_SIZE(MainWindow::OnSize)
EVT_DROP_FILES(MainWindow::handleDropFile)
EVT_MENU(ID_Export_Map, MainWindow::onExportToMaps)
EVT_MENU(ID_Export_Structure, MainWindow::onExportToStructure)
EVT_MENU(ID_Export_Function, MainWindow::onExportToFunctions)
EVT_MENU(ID_Resize_Image, MainWindow::onImageResize)
EVT_MENU(ID_Edit_Image, MainWindow::onImageEdit)
EVT_CLOSE(MainWindow::OnClose)
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
    requiresPreviewGneration = false;
    previewInProgress = false;

    imageResizeWidth = 128;
    imageResizeHeight = 128;

    saturation = 1;
    contrast = 1;
    brightness = 1;

    dirty = false;
    projectFile = "";

    imageEditDialog = NULL;

    std::vector<size_t> cmats(MAX_COLOR_GROUPS);
    for (size_t i = 0; i < MAX_COLOR_GROUPS; i++)
    {
        cmats[i] = 0;
    }
    countsMats = cmats;

    threadNum = max((unsigned int)1, std::thread::hardware_concurrency());

    colorSetConf = "MODE(BLACKLIST)\n";

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
    exportMenu->Append(ID_Export_Structure, "&Export as structures\tCtrl+T", "Exports the map to nbt structure files (for survival)");
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
    menuImage->Append(ID_Edit_Image, "&Brightness, Saturation, Contrast\tCtrl+Shift+B", "Modifies image, using filters for brightness, saturation and constrast");
    menuBar->Append(menuImage, "&Image");

    // Materials
    wxMenu *menuMaterials = new wxMenu();
    menuMaterials->Append(ID_Blocks_Custom, "&Customize materials\tCtrl+M", "Customizes materials to use");
    menuMaterials->AppendSeparator();
    menuMaterials->Append(ID_Materials_Save, "&Export materials list\tCtrl+Shift+M", "Exports list of materials to a text file");
    menuBar->Append(menuMaterials, "&Materials");

    // Color distance
    colorDistanceAlgorithm = ColorDistanceAlgorithm::Euclidean;
    wxMenu *menuColorDistance = new wxMenu();
    menuColorDistance->AppendRadioItem(getIdForColorAlgoMenu(ColorDistanceAlgorithm::Euclidean), "&Euclidean", "Simple squared euclidean distance in the RGB color space")->Check(true);
    menuColorDistance->AppendRadioItem(getIdForColorAlgoMenu(ColorDistanceAlgorithm::DeltaE), "&Delta E", "Distance in the Lab color space");
    menuBar->Append(menuColorDistance, "&Color aproximation");

    // Dithering
    ditheringMethod = DitheringMethod::None;
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
    buildMethod = MapBuildMethod::Chaos;
    wxMenu *menuBuildMethod = new wxMenu();
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::Chaos), "&3D (Complex)", "Staircased map with jumps of arbitrary size")->Check(true);
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::Staircased), "&Staircased", "Staircased map with jumps of only a single block");
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::Flat), "&Flat", "Flat map (only 1/3 of the colors)");
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::None), "&None", "Use all the colors (even the unobtainable with blocks)");
    menuBar->Append(menuBuildMethod, "&Build Method");

    // Version
    version = MC_LAST_VERSION;
    wxMenu *menuVersion = new wxMenu();
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_17), "&1.17.1", "Version: 1.17.1")->Check(true);
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

    SetMenuBar(menuBar);

    // Add sub windows

    originalImageColors = std::vector<colors::Color>(MAP_WIDTH * MAP_HEIGHT);
    originalImageWidth = MAP_WIDTH;
    originalImageHeight = MAP_HEIGHT;

    for (size_t i = 0; i < originalImageColors.size(); i++)
    {
        originalImageColors[i].red = 255;
        originalImageColors[i].green = 255;
        originalImageColors[i].blue = 255;
    }

    wxImage blankImage(MAP_WIDTH, MAP_HEIGHT);
    unsigned char *rawData = blankImage.GetData();
    size_t size = MAP_WIDTH * MAP_HEIGHT;

    size_t j = 0;
    for (size_t i = 0; i < size; i++)
    {
        rawData[j++] = 255;
        rawData[j++] = 255;
        rawData[j++] = 255;
    }
    originalImage = blankImage;

    originalImagePanel = new wxImagePanel(this);
    originalImagePanel->SetPosition(wxPoint(0, 0));
    originalImagePanel->SetSize(GetClientSize().GetWidth() / 2 - FRAME_PAD_PIXELS, GetClientSize().GetHeight());
    originalImagePanel->setColors(originalImageColors, originalImageWidth, originalImageHeight);

    previewPanel = new wxImagePanel(this);
    previewPanel->SetPosition(wxPoint(GetClientSize().GetWidth() / 2 + FRAME_PAD_PIXELS, 0));
    previewPanel->SetSize(GetClientSize().GetWidth() / 2 - FRAME_PAD_PIXELS, GetClientSize().GetHeight());
    previewPanel->setColors(originalImageColors, originalImageWidth, originalImageHeight);

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
    if (!originalImage.LoadFile(file))
    {
        wxMessageBox(string("Invalid image file: ") + file, wxT("Error"), wxICON_ERROR);
        return;
    }

    imageResizeWidth = originalImage.GetWidth();
    imageResizeHeight = originalImage.GetHeight();

    saturation = 1;
    contrast = 1;
    brightness = 1;

    if (imageEditDialog != NULL)
    {
        imageEditDialog->SetParams(saturation, contrast, brightness);
    }

    updateOriginalImage();
}

void MainWindow::updateOriginalImage()
{
    wxImage imageCopy(originalImage);

    if (imageResizeWidth > 0 && imageResizeHeight > 0)
    {
        imageCopy.Rescale(imageResizeWidth, imageResizeHeight);
    }

    int matrixW;
    int matrixH;
    originalImageColors = loadColorMatrixFromImageAndPad(imageCopy, &matrixW, &matrixH);
    originalImageWidth = matrixW;
    originalImageHeight = matrixH;

    tools::editImage(originalImageColors, originalImageWidth, originalImageHeight, saturation, contrast, brightness);

    originalImagePanel->setColors(originalImageColors, originalImageWidth, originalImageHeight);
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
        materialsWindow->setMaterialsConf(version, colorSetConf);
    }
    else
    {
        materialsWindow->Show();
        materialsWindow->Raise();
    }
}

void MainWindow::onChangeVersion(wxCommandEvent &evt)
{
    version = static_cast<McVersion>(evt.GetId() - VERSION_ID_PREFIX);
    if (materialsWindow != NULL)
    {
        materialsWindow->setMaterialsConf(version, colorSetConf);
    }
    dirty = true;
    RequestPreviewGeneration();
}

void MainWindow::onChangeBuildMethod(wxCommandEvent &evt)
{
    buildMethod = static_cast<MapBuildMethod>(evt.GetId() - BUILD_METHOD_ID_PREFIX);
    dirty = true;
    RequestPreviewGeneration();
}

void MainWindow::onChangeColorAlgo(wxCommandEvent &evt)
{
    colorDistanceAlgorithm = static_cast<ColorDistanceAlgorithm>(evt.GetId() - COLOR_METHOD_ID_PREFIX);
    dirty = true;
    RequestPreviewGeneration();
}

void MainWindow::onChangeDithering(wxCommandEvent &evt)
{
    ditheringMethod = static_cast<DitheringMethod>(evt.GetId() - DITHERING_ID_PREFIX);
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
#if defined(_WIN32)
    mutexPreviewGeneration.lock();
#endif

    if (previewInProgress)
    {
        requiresPreviewGneration = true;
        previewProgress.terminate();
    }
    else
    {
        previewInProgress = true;
        requiresPreviewGneration = false;
#if defined(_WIN32)
        std::thread(&MainWindow::GeneratePreview, this).detach();
#else
        std::thread(&MainWindow::GeneratePreview, this).join();
#endif
    }

#if defined(_WIN32)
    mutexPreviewGeneration.unlock();
#endif
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

        mutexProgress.lock();
        GetStatusBar()->SetStatusText(progressLine, 1);
        mutexProgress.unlock();

        // Check ended
        ended = progress.hasEnded();
    }

    // Erase line
    mutexProgress.lock();
    GetStatusBar()->SetStatusText("Ready", 1);
    mutexProgress.unlock();
}

void MainWindow::GeneratePreview()
{
    bool finished = false;

    while (!finished)
    {
        previewProgress.reset();

        thread progressReportThread(&MainWindow::ReportProgress, this, std::ref(previewProgress));

        try
        {
            previewProgress.startTask("Loading minecraft colors...", 0, 0);
            std::vector<colors::Color> baseColors = minecraft::loadBaseColors(version);
            std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
            std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
            std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
            std::vector<bool> enabledConf(baseColors.size());
            bool blacklist = true;

            previewProgress.startTask("Loading custom configuration...", 0, 0);
            mapart::applyColorSet(colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
            // Apply color restructions based on build method
            applyBuildRestrictions(colorSet, buildMethod);

            previewProgress.startTask("Generating preview...", originalImageHeight, threadNum);
            std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, colorDistanceAlgorithm, ditheringMethod, threadNum, previewProgress, countsMats);

            previewPanel->setColors(mapArtColorMatrix, originalImageWidth, originalImageHeight);
            previewPanel->Refresh();

            if (materialsWindow != NULL)
            {
                materialsWindow->displayCountMaterials(countsMats);
            }
        }
        catch (int)
        {
            previewProgress.reset();
        }

        previewProgress.setEnded();
        progressReportThread.join();

        mutexPreviewGeneration.lock();

        if (requiresPreviewGneration)
        {
            requiresPreviewGneration = false;
        }
        else
        {
            previewInProgress = false;
            finished = true;
        }

        mutexPreviewGeneration.unlock();
    }
}

void MainWindow::OnSaveMaterialsList(wxCommandEvent &evt)
{
    if (buildMethod == MapBuildMethod::None)
    {
        wxMessageBox(wxString("You must choose a build method to create the materials list"), wxT("Error"), wxICON_ERROR);
        return;
    }

#if defined(_WIN32)
    std::thread(&MainWindow::SaveMaterialsList, this).detach();
#else
    std::thread(&MainWindow::SaveMaterialsList, this).join();
#endif
}

void MainWindow::SaveMaterialsList()
{
    threading::Progress p;

    thread progressReportThread(&MainWindow::ReportProgress, this, std::ref(p));

    try
    {
        p.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        p.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, buildMethod);

        p.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, colorDistanceAlgorithm, ditheringMethod, threadNum, p, countsMats);

        // Compute total maps
        int mapsCountX = originalImageWidth / MAP_WIDTH;
        int mapsCountZ = originalImageHeight / MAP_HEIGHT;

        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;

        MaterialsList materials(baseColorNames);

        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                stringstream ss;
                ss << "Building map (" << (total + 1) << "/" << totalMapsCount << ")...";
                p.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(version, blockSet, mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ, buildMethod, threadNum, p);

                // Add to materials list
                materials.addBlocks(buildingBlocks);

                total++;
            }
        }

        p.startTask("Saving...", 0, 0);

        wxFileDialog saveFileDialog(this, _("Save materials list"), "", "", "Text file (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (saveFileDialog.ShowModal() != wxID_CANCEL)
        {
            if (!tools::writeTextFile(saveFileDialog.GetPath().ToStdString(), materials.toString()))
            {
                wxMessageBox(wxString("Could not save the materials due to a file system error."), wxT("Error"), wxICON_ERROR);
            }
        }
    }
    catch (int)
    {
    }

    p.setEnded();
    progressReportThread.join();
}

void MainWindow::ExportAsMapFiles(std::string path, int mapNumber)
{
    threading::Progress p;

    thread progressReportThread(&MainWindow::ReportProgress, this, std::ref(p));

    try
    {
        p.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        p.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, buildMethod);

        p.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, colorDistanceAlgorithm, ditheringMethod, threadNum, p, countsMats);

        // Compute total maps
        int mapsCountX = originalImageWidth / MAP_WIDTH;
        int mapsCountZ = originalImageHeight / MAP_HEIGHT;

        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;

        p.startTask("Saving to map files...", mapsCountZ * mapsCountX, 1);
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                // Get map data
                std::vector<map_color_t> mapDataToSave = getMapDataFromColorMatrix(mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ);

                // Save to file
                stringstream ss;
                ss << "map_" << (mapNumber++) << ".dat";
                filesystem::path outFilePath(path);

                outFilePath /= ss.str();

                try
                {
                    writeMapNBTFile(outFilePath.string(), mapDataToSave, version);
                }
                catch (...)
                {
                    wxMessageBox(string("Cannot write file: ") + outFilePath.string(), wxT("Error"), wxICON_ERROR);
                    throw -1;
                }

                total++;
                p.setProgress(0, total);
            }
        }
    }
    catch (int)
    {
    }

    p.setEnded();
    progressReportThread.join();

    wxMessageBox(string("Successfully saved map files. They are sorted from left to right, top to bottom."), wxT("Success!"), wxICON_INFORMATION);

#if defined(_WIN32)

    ShellExecute(NULL, L"open", std::wstring(path.begin(), path.end()).c_str(), NULL, NULL, SW_SHOWDEFAULT);

#endif
}

void MainWindow::ExportAsStructure(std::string path)
{
    threading::Progress p;

    thread progressReportThread(&MainWindow::ReportProgress, this, std::ref(p));

    try
    {
        p.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        p.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, buildMethod);

        p.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, colorDistanceAlgorithm, ditheringMethod, threadNum, p, countsMats);

        // Compute total maps
        int mapsCountX = originalImageWidth / MAP_WIDTH;
        int mapsCountZ = originalImageHeight / MAP_HEIGHT;

        p.startTask("Building maps...", 0, 0);
        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                stringstream ss;
                ss << "Building map (" << (total + 1) << "/" << totalMapsCount << ")...";
                p.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(version, blockSet, mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ, buildMethod, threadNum, p);

                // Save as structure file
                stringstream ss2;
                ss2 << "map_" << (total + 1) << ".nbt";
                filesystem::path outFilePath(path);

                outFilePath /= ss2.str();

                try
                {
                    writeStructureNBTFile(outFilePath.string(), buildingBlocks, version);
                }
                catch (...)
                {
                    wxMessageBox(string("Cannot write file: ") + outFilePath.string(), wxT("Error"), wxICON_ERROR);
                    throw -1;
                }

                total++;
            }
        }
    }
    catch (int)
    {
    }

    p.setEnded();
    progressReportThread.join();

    wxMessageBox(string("Successfully saved structures. They are sorted from left to right, top to bottom."), wxT("Success!"), wxICON_INFORMATION);

#if defined(_WIN32)

    ShellExecute(NULL, L"open", std::wstring(path.begin(), path.end()).c_str(), NULL, NULL, SW_SHOWDEFAULT);

#endif
}

void MainWindow::ExportAsFunctions(std::string path)
{
    threading::Progress p;

    thread progressReportThread(&MainWindow::ReportProgress, this, std::ref(p));

    try
    {
        p.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        p.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, buildMethod);

        p.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, colorDistanceAlgorithm, ditheringMethod, threadNum, p, countsMats);

        // Compute total maps
        int mapsCountX = originalImageWidth / MAP_WIDTH;
        int mapsCountZ = originalImageHeight / MAP_HEIGHT;

        p.startTask("Building maps...", 0, 0);
        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                stringstream ss;
                ss << "Building map (" << (total + 1) << "/" << totalMapsCount << ")...";
                p.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(version, blockSet, mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ, buildMethod, threadNum, p);

                // Save as structure file
                stringstream ss2;
                ss2 << "map_" << (total + 1) << ".mcfunction";
                filesystem::path outFilePath(path);

                outFilePath /= ss2.str();

                try
                {
                    writeMcFunctionFile(outFilePath.string(), buildingBlocks, version);
                }
                catch (...)
                {
                    wxMessageBox(string("Cannot write file: ") + outFilePath.string(), wxT("Error"), wxICON_ERROR);
                    throw -1;
                }

                total++;
            }
        }
    }
    catch (int)
    {
    }

    p.setEnded();
    progressReportThread.join();

    wxMessageBox(string("Successfully saved function files. They are sorted from left to right, top to bottom."), wxT("Success!"), wxICON_INFORMATION);

#if defined(_WIN32)

    ShellExecute(NULL, L"open", std::wstring(path.begin(), path.end()).c_str(), NULL, NULL, SW_SHOWDEFAULT);

#endif
}

void MainWindow::handleDropFile(wxDropFilesEvent &event)
{
    if (event.GetNumberOfFiles() > 0)
    {
        loadImage(string(event.GetFiles()[0]));
        dirty = true;
    }
}

void MainWindow::changeColorSetConf(std::string conf)
{
    colorSetConf = conf;
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
    MapExportDialog dialog;
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }
#if defined(_WIN32)
    std::thread(&MainWindow::ExportAsMapFiles, this, dialog.getPath(), dialog.getMapNumber()).detach();
#else
    std::thread(&MainWindow::ExportAsMapFiles, this, dialog.getPath(), dialog.getMapNumber()).join();
#endif
}

void MainWindow::onExportToStructure(wxCommandEvent &evt)
{
    if (buildMethod == MapBuildMethod::None)
    {
        wxMessageBox(wxString("You must choose a build method to be able to export to structures."), wxT("Cannot export"), wxICON_INFORMATION);
        return;
    }
    StructureExportDialog dialog(version, ExportDialogMode::Structure);
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

#if defined(_WIN32)
    std::thread(&MainWindow::ExportAsStructure, this, dialog.getPath()).detach();
#else
    std::thread(&MainWindow::ExportAsStructure, this, dialog.getPath()).join();
#endif
}

void MainWindow::onExportToFunctions(wxCommandEvent &evt)
{
    if (buildMethod != MapBuildMethod::Flat)
    {
        wxMessageBox(wxString("Only flat maps can be exported to minecraft functions."), wxT("Cannot export"), wxICON_INFORMATION);
        return;
    }
    StructureExportDialog dialog(version, ExportDialogMode::Function);
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

#if defined(_WIN32)
    std::thread(&MainWindow::ExportAsFunctions, this, dialog.getPath()).detach();
#else
    std::thread(&MainWindow::ExportAsFunctions, this, dialog.getPath()).join();
#endif
}

void MainWindow::onImageResize(wxCommandEvent &evt)
{
    ImageResizeDialog dialog(imageResizeWidth, imageResizeHeight);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }

    imageResizeWidth = dialog.getWidth();
    imageResizeHeight = dialog.getHeight();

    dirty = true;

    updateOriginalImage();
}

void MainWindow::onImageEdit(wxCommandEvent &evt)
{
    if (imageEditDialog == NULL)
    {
        imageEditDialog = new ImageEditDialog(this);
        imageEditDialog->Show();
        imageEditDialog->SetParams(saturation, contrast, brightness);
    }
    else
    {
        imageEditDialog->Show();
        imageEditDialog->Raise();
    }
}

void MainWindow::onImageEditParamsChanged(float saturation, float contrast, float brightness)
{
    this->saturation = saturation;
    this->contrast = contrast;
    this->brightness = brightness;
    dirty = true;
    updateOriginalImage();
}

void MainWindow::resetProject()
{
    // Image resize
    imageResizeWidth = 128;
    imageResizeHeight = 128;

    // Image edit params
    saturation = 1;
    contrast = 1;
    brightness = 1;

    if (imageEditDialog != NULL)
    {
        imageEditDialog->SetParams(saturation, contrast, brightness);
    }

    // Not dirty
    dirty = false;

    // Not saved yet
    projectFile = "";

    // Colors conf
    colorSetConf = "MODE(BLACKLIST)\n";

    // Version
    version = MC_LAST_VERSION;

    if (materialsWindow != NULL)
    {
        materialsWindow->setMaterialsConf(version, colorSetConf);
    }

    // Map params
    colorDistanceAlgorithm = ColorDistanceAlgorithm::Euclidean;
    ditheringMethod = DitheringMethod::None;
    buildMethod = MapBuildMethod::Chaos;

    // Initial image
    originalImageColors = std::vector<colors::Color>(MAP_WIDTH * MAP_HEIGHT);
    originalImageWidth = MAP_WIDTH;
    originalImageHeight = MAP_HEIGHT;

    for (size_t i = 0; i < originalImageColors.size(); i++)
    {
        originalImageColors[i].red = 255;
        originalImageColors[i].green = 255;
        originalImageColors[i].blue = 255;
    }

    wxImage blankImage(MAP_WIDTH, MAP_HEIGHT);
    unsigned char *rawData = blankImage.GetData();
    size_t size = MAP_WIDTH * MAP_HEIGHT;

    size_t j = 0;
    for (size_t i = 0; i < size; i++)
    {
        rawData[j++] = 255;
        rawData[j++] = 255;
        rawData[j++] = 255;
    }
    originalImage = blankImage;

    GetStatusBar()->SetStatusText("Project: (not saved yet)", 3);

    updateMenuBarRadios();

    updateOriginalImage();
}

void MainWindow::loadProject(std::string path)
{
    try
    {
        std::ifstream file(path, std::ios::binary);

        if (!file)
        {
            throw -1;
        }

        zlib::izlibstream igzs(file);

        auto pair = nbt::io::read_compound(igzs);
        nbt::tag_compound comp = *pair.second;

        // Image resize
        imageResizeWidth = comp.at("resize_width").as<nbt::tag_int>().get();
        imageResizeHeight = comp.at("resize_height").as<nbt::tag_int>().get();

        // Image edit params
        saturation = comp.at("saturation").as<nbt::tag_float>().get();
        contrast = comp.at("contrast").as<nbt::tag_float>().get();
        brightness = comp.at("brightness").as<nbt::tag_float>().get();

        if (imageEditDialog != NULL)
        {
            imageEditDialog->SetParams(saturation, contrast, brightness);
        }

        // Not dirty
        dirty = false;

        // Not saved yet
        projectFile = path;

        // Colors conf
        colorSetConf = comp.at("colors_conf").as<nbt::tag_string>().get();

        // Version
        version = minecraft::getVersionFromText(comp.at("version").as<nbt::tag_string>().get());

        if (version == McVersion::UNKNOWN)
        {
            version = MC_LAST_VERSION;
        }

        if (materialsWindow != NULL)
        {
            materialsWindow->setMaterialsConf(version, colorSetConf);
        }

        // Map params
        string paramStr;

        paramStr = comp.at("color_distance").as<nbt::tag_string>().get();
        if (paramStr.compare("delta-e") == 0)
        {
            colorDistanceAlgorithm = ColorDistanceAlgorithm::DeltaE;
        }
        else if (paramStr.compare("euclidean") == 0)
        {
            colorDistanceAlgorithm = ColorDistanceAlgorithm::Euclidean;
        }
        else
        {
            colorDistanceAlgorithm = ColorDistanceAlgorithm::Euclidean;
        }

        ditheringMethod = mapart::parseDitheringMethodFromString(comp.at("dithering").as<nbt::tag_string>().get());

        if (ditheringMethod == DitheringMethod::Unknown)
        {
            ditheringMethod = DitheringMethod::None;
        }

        paramStr = comp.at("build_method").as<nbt::tag_string>().get();
        if (paramStr.compare("flat") == 0)
        {
            buildMethod = MapBuildMethod::Flat;
        }
        else if (paramStr.compare("stair") == 0)
        {
            buildMethod = MapBuildMethod::Staircased;
        }
        else if (paramStr.compare("chaos") == 0)
        {
            buildMethod = MapBuildMethod::Chaos;
        }
        else
        {
            buildMethod = MapBuildMethod::None;
        }

        size_t w = comp.at("width").as<nbt::tag_int>().get();
        size_t h = comp.at("height").as<nbt::tag_int>().get();

        nbt::tag_byte_array colorsByte = comp.at("image").as<nbt::tag_byte_array>();

        wxImage blankImage(w, h);
        unsigned char *rawData = blankImage.GetData();
        size_t size = w * h * 3;

        for (size_t i = 0; i < size; i++)
        {
            rawData[i] = colorsByte.at(i);
        }
        originalImage = blankImage;

        GetStatusBar()->SetStatusText("Project: " + projectFile, 3);

        updateMenuBarRadios();

        updateOriginalImage();
    }
    catch (...)
    {
        wxMessageBox(string("Could not load project file: ") + path, wxT("Error"), wxICON_ERROR);
        resetProject();
        return;
    }
}

void MainWindow::saveProject(std::string path)
{
    nbt::tag_compound root;

    root.insert("resize_width", nbt::tag_int(static_cast<int>(imageResizeWidth)));
    root.insert("resize_height", nbt::tag_int(static_cast<int>(imageResizeHeight)));

    root.insert("saturation", nbt::tag_float(saturation));
    root.insert("contrast", nbt::tag_float(contrast));
    root.insert("brightness", nbt::tag_float(brightness));

    root.insert("colors_conf", nbt::tag_string(colorSetConf));

    root.insert("version", nbt::tag_string(versionToString(version)));

    switch (colorDistanceAlgorithm)
    {
    case ColorDistanceAlgorithm::DeltaE:
        root.insert("color_distance", nbt::tag_string("delta-e"));
        break;
    default:
        root.insert("color_distance", nbt::tag_string("euclidean"));
    }

    root.insert("dithering", nbt::tag_string(ditheringMethodToString(ditheringMethod)));

    switch (buildMethod)
    {
    case MapBuildMethod::Chaos:
        root.insert("build_method", nbt::tag_string("chaos"));
        break;
    case MapBuildMethod::Flat:
        root.insert("build_method", nbt::tag_string("flat"));
        break;
    case MapBuildMethod::Staircased:
        root.insert("build_method", nbt::tag_string("stair"));
        break;
    default:
        root.insert("build_method", nbt::tag_string("none"));
    }

    // Image data
    root.insert("width", nbt::tag_int(static_cast<int>(originalImage.GetWidth())));
    root.insert("height", nbt::tag_int(static_cast<int>(originalImage.GetHeight())));

    nbt::tag_byte_array colorsByte;

    size_t size = originalImage.GetWidth() * originalImage.GetHeight() * 3;
    unsigned char *rawData = originalImage.GetData();
    for (size_t i = 0; i < size; i++)
    {
        colorsByte.push_back(rawData[i]);
    }

    root.insert("image", colorsByte.clone());

    // Save
    std::ofstream file(path, std::ios::binary);

    if (!file)
    {
        wxMessageBox(string("Could not save project file: ") + path, wxT("Error"), wxICON_ERROR);
        return;
    }

    try
    {
        zlib::ozlibstream ogzs(file, -1, true);
        nbt::io::write_tag("", root, ogzs);
    }
    catch (...)
    {
        wxMessageBox(string("Could not save project file: ") + path, wxT("Error"), wxICON_ERROR);
        return;
    }

    projectFile = path;
    GetStatusBar()->SetStatusText("Project: " + projectFile, 3);
    dirty = false;
}

void MainWindow::OnClose(wxCloseEvent &event)
{
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
    switch (colorDistanceAlgorithm)
    {
    case ColorDistanceAlgorithm::DeltaE:
        GetMenuBar()->GetMenu(3)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(3)->GetMenuItems()[1]->Check(true);
        break;
    default:
        GetMenuBar()->GetMenu(3)->GetMenuItems()[0]->Check(true);
        GetMenuBar()->GetMenu(3)->GetMenuItems()[1]->Check(false);
    }

    switch (ditheringMethod)
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

    switch (buildMethod)
    {
    case MapBuildMethod::Chaos:
        GetMenuBar()->GetMenu(5)->GetMenuItems()[0]->Check(true);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[3]->Check(false);
        break;
    case MapBuildMethod::Staircased:
        GetMenuBar()->GetMenu(5)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(5)->GetMenuItems()[1]->Check(true);
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

    switch (version)
    {
    case McVersion::MC_1_17:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[0]->Check(true);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[5]->Check(false);
        break;
    case McVersion::MC_1_16:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[1]->Check(true);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[5]->Check(false);
        break;
    case McVersion::MC_1_15:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[2]->Check(true);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[5]->Check(false);
        break;
    case McVersion::MC_1_14:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[3]->Check(true);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[5]->Check(false);
        break;
    case McVersion::MC_1_13:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[4]->Check(true);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[5]->Check(false);
        break;
    default:
        GetMenuBar()->GetMenu(6)->GetMenuItems()[0]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[1]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[2]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[3]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[4]->Check(false);
        GetMenuBar()->GetMenu(6)->GetMenuItems()[5]->Check(true);
        break;
    }
}
