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
#include <filesystem>
#include "../minecraft/structure.h"

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
EVT_MENU(ID_File_Open, MainWindow::openProject)
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
    previewGenerationThread = NULL;
    originalImagePanel = NULL;
    previewPanel = NULL;
    requiresPreviewGneration = false;
    previewInProgress = false;

    threadNum = max((unsigned int)1, std::thread::hardware_concurrency());

    // Materials window
    materialsWindow = new MaterialsWindow(this);

    colorSetConf = "MODE(BLACKLIST)\n";

    /* Menu Bar */
    menuBar = new wxMenuBar();

    // File
    wxMenu *menuFile = new wxMenu();

    menuFile->Append(ID_File_New, "&New Project\tCtrl+N", "");

    menuFile->AppendSeparator();

    menuFile->Append(ID_File_Open, "&Open Project\tCtrl+O", "");

    menuFile->AppendSeparator();
    menuFile->Append(ID_File_Save, "&Save Project\tCtrl+S", "");
    menuFile->Append(ID_File_Save_As, "&Save Project As...\tCtrl+Shift+S", "");
    menuFile->AppendSeparator();

    wxMenu *exportMenu = new wxMenu();
    exportMenu->Append(ID_Export_Map, "&Export as map files\tCtrl+E", "");
    exportMenu->Append(ID_Export_Structure, "&Export as structures\tCtrl+T", "");
    menuFile->AppendSubMenu(exportMenu, "&Export", "");

    menuFile->AppendSeparator();

    menuFile->Append(wxID_EXIT);

    menuBar->Append(menuFile, "&File");

    // Image
    wxMenu *menuImage = new wxMenu();
    menuImage->Append(ID_Load_Image, "&Load Image\tCtrl+L", "");
    menuImage->AppendSeparator();
    menuImage->Append(ID_Resize_Image, "&Resize Image\tCtrl+Shift+R", "");
    menuImage->Append(ID_Edit_Image, "&Brightness, Saturation, Contrast\tCtrl+Shift+B", "");
    menuBar->Append(menuImage, "&Image");

    // Materials
    wxMenu *menuMaterials = new wxMenu();
    menuMaterials->Append(ID_Blocks_Custom, "&Customize materials\tCtrl+M", "");
    menuMaterials->AppendSeparator();
    menuMaterials->Append(ID_Materials_Save, "&Export materials list\tCtrl+Shift+M", "");
    menuBar->Append(menuMaterials, "&Materials");

    // Color distance
    colorDistanceAlgorithm = ColorDistanceAlgorithm::Euclidean;
    wxMenu *menuColorDistance = new wxMenu();
    menuColorDistance->AppendRadioItem(getIdForColorAlgoMenu(ColorDistanceAlgorithm::Euclidean), "&Euclidean", "")->Check(true);
    menuColorDistance->AppendRadioItem(getIdForColorAlgoMenu(ColorDistanceAlgorithm::DeltaE), "&Delta E", "");
    menuBar->Append(menuColorDistance, "&Color aproximation");

    // Dithering
    ditheringMethod = DitheringMethod::None;
    wxMenu *menuDithering = new wxMenu();
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::None), "&None", "")->Check(true);
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::FloydSteinberg), "&Floyd Steinberg", "");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::MinAvgErr), "&Min Average Error", "");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Atkinson), "&Atkinson", "");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Stucki), "&Stucki", "");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::SierraLite), "&Sierra Lite", "");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Burkes), "&Burkes", "");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Bayer22), "&Bayer (2x2)", "");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Bayer44), "&Bayer (4x4)", "");
    menuDithering->AppendRadioItem(getIdForDitheringMenu(DitheringMethod::Ordered33), "&Ordered (3x3)", "");
    menuBar->Append(menuDithering, "&Dithering");

    // Build method
    buildMethod = MapBuildMethod::Chaos;
    wxMenu *menuBuildMethod = new wxMenu();
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::Chaos), "&3D (Complex)", "")->Check(true);
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::Staircased), "&Staircased", "");
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::Flat), "&Flat", "");
    menuBuildMethod->AppendRadioItem(getIdForBuildMethodMenu(MapBuildMethod::None), "&None", "");
    menuBar->Append(menuBuildMethod, "&Build Method");

    // Version
    version = MC_LAST_VERSION;
    wxMenu *menuVersion = new wxMenu();
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_17), "&1.17", "")->Check(true);
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_16), "&1.16", "");
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_15), "&1.15", "");
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_14), "&1.14", "");
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_13), "&1.13", "");
    menuVersion->AppendRadioItem(getIdForVersionMenu(McVersion::MC_1_12), "&1.12", "");
    menuBar->Append(menuVersion, "&Version");

    SetIcon(wxIcon(_ICON_ICO_XPM));

    wxStatusBar *statusBar = CreateStatusBar();
    statusBar->SetFieldsCount(3);
    statusBar->SetStatusText("Status: Ready", 0);
    statusBar->SetStatusText("Size: 1 x 1", 1);
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

    DragAcceptFiles(true);

    materialsWindow->setMaterialsConf(version, colorSetConf);
}

MainWindow::~MainWindow()
{
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
    if (!originalImage.LoadFile(file))
    {
        wxMessageBox(string("Invalid image file: ") + file, wxT("Error"), wxICON_ERROR);
        return;
    }

    int matrixW;
    int matrixH;
    originalImageColors = loadColorMatrixFromImageAndPad(originalImage, &matrixW, &matrixH);
    originalImageWidth = matrixW;
    originalImageHeight = matrixH;

    originalImagePanel->setColors(originalImageColors, originalImageWidth, originalImageHeight);
    originalImagePanel->Refresh();

    stringstream ss;
    ss << "Size: " << (matrixW / MAP_WIDTH) << " x " << (matrixH / MAP_HEIGHT);
    GetStatusBar()->SetStatusText(ss.str(), 1);

    RequestPreviewGeneration();
}

void MainWindow::onExit(wxCommandEvent &evt)
{
    Close(true);
}

void MainWindow::onCustomBlocks(wxCommandEvent &evt)
{
    materialsWindow->Show();
    materialsWindow->Raise();
}

void MainWindow::onChangeVersion(wxCommandEvent &evt)
{
    version = static_cast<McVersion>(evt.GetId() - VERSION_ID_PREFIX);
    materialsWindow->setMaterialsConf(version, colorSetConf);
    RequestPreviewGeneration();
}

void MainWindow::onChangeBuildMethod(wxCommandEvent &evt)
{
    buildMethod = static_cast<MapBuildMethod>(evt.GetId() - BUILD_METHOD_ID_PREFIX);
    RequestPreviewGeneration();
}

void MainWindow::onChangeColorAlgo(wxCommandEvent &evt)
{
    colorDistanceAlgorithm = static_cast<ColorDistanceAlgorithm>(evt.GetId() - COLOR_METHOD_ID_PREFIX);
    RequestPreviewGeneration();
}

void MainWindow::onChangeDithering(wxCommandEvent &evt)
{
    ditheringMethod = static_cast<DitheringMethod>(evt.GetId() - DITHERING_ID_PREFIX);
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
        previewProgress.terminate();
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

        mutexProgress.lock();
        GetStatusBar()->SetStatusText(progressLine, 0);
        mutexProgress.unlock();

        // Check ended
        ended = progress.hasEnded();
    }

    // Erase line
    mutexProgress.lock();
    GetStatusBar()->SetStatusText("Ready", 0);
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
            std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
            bool blacklist = true;

            previewProgress.startTask("Loading custom configuration...", 0, 0);
            mapart::applyColorSet(colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
            // Apply color restructions based on build method
            applyBuildRestrictions(colorSet, buildMethod);

            previewProgress.startTask("Generating preview...", originalImageHeight, threadNum);
            std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, colorDistanceAlgorithm, ditheringMethod, threadNum, previewProgress, countsMats);

            previewPanel->setColors(mapArtColorMatrix, originalImageWidth, originalImageHeight);
            previewPanel->Refresh();

            materialsWindow->displayCountMaterials(countsMats);
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

    std::thread *t = new std::thread(&MainWindow::SaveMaterialsList, this);
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
            if (!tools::writeTextFile(saveFileDialog.GetPath(), materials.toString()))
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

void MainWindow::handleDropFile(wxDropFilesEvent &event)
{
    if (event.GetNumberOfFiles() > 0)
    {
        loadImage(string(event.GetFiles()[0]));
    }
}

void MainWindow::changeColorSetConf(std::string conf)
{
    colorSetConf = conf;
    RequestPreviewGeneration();
}

void widgets::displayMainWindow(wxApp &app)
{
    wxInitAllImageHandlers();
    MainWindow *frame = new MainWindow();
    frame->Show(true);
}

void MainWindow::onExportToMaps(wxCommandEvent &evt)
{
    MapExportDialog dialog;
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }
    std::thread *t = new std::thread(&MainWindow::ExportAsMapFiles, this, dialog.getPath(), dialog.getMapNumber());
}

void MainWindow::onExportToStructure(wxCommandEvent &evt)
{
    StructureExportDialog dialog(version);
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return; // the user changed idea...
    }
    std::thread *t = new std::thread(&MainWindow::ExportAsStructure, this, dialog.getPath());
}
