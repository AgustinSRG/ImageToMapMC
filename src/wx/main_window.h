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

#include <mutex>
#include <thread>

#include "../mapart/map_art.h"
#include "../mapart/map_image.h"
#include "../mapart/project.h"

#include "../tools/text_file.h"
#include "../tools/image_edit.h"

#include "img_display_window.h"
#include "worker_thread.h"

class MaterialsWindow;

class ImageEditDialog;

class MainWindow : public wxFrame
{
public:
    MainWindow();
    ~MainWindow();

    void openProject(wxCommandEvent &evt);
    void newProject(wxCommandEvent &evt);
    void saveProject(wxCommandEvent &evt);
    void saveProjectAs(wxCommandEvent &evt);

    void saveImageAs(wxCommandEvent &evt);
    void savePreviewAs(wxCommandEvent &evt);

    void onChangeVersion(wxCommandEvent &evt);
    void onChangeColorAlgo(wxCommandEvent &evt);
    void onChangeDithering(wxCommandEvent &evt);
    void onChangeBuildMethod(wxCommandEvent &evt);
    void onCustomBlocks(wxCommandEvent &evt);
    void onExportToMaps(wxCommandEvent &evt);
    void onExportToStructure(wxCommandEvent &evt);
    void onExportToFunctions(wxCommandEvent &evt);
    void onImageResize(wxCommandEvent &evt);
    void onImageEdit(wxCommandEvent &evt);

    void OnProgressTimer(wxTimerEvent& event);

    void onWorkerError(wxCommandEvent& event);
    void onWorkerPreviewDone(wxCommandEvent& event);
    void onWorkerMaterialsGiven(wxCommandEvent& event);

    void onExit(wxCommandEvent &evt);
    void onLoadImage(wxCommandEvent &evt);
    void loadImage(std::string file);

    void updateOriginalImage();

    void OnSize( wxSizeEvent& event );

    void RequestPreviewGeneration();

    void OnSaveMaterialsList(wxCommandEvent &evt);

    void handleDropFile(wxDropFilesEvent& event);

    void changeColorSetConf(std::string conf);

    void onImageEditParamsChanged(float saturation, float contrast, float brightness, colors::Color background);

    void loadProject(std::string path);
    void saveProject(std::string path);
    void resetProject();

    void updateMenuBarRadios();

    void OnClose(wxCloseEvent &event);
private:
    wxImagePanel * originalImagePanel;
    wxImagePanel * previewPanel;

    WorkerThread * workerThread;

    int threadNum;

    wxMenuBar * menuBar;

    MaterialsWindow * materialsWindow;
    ImageEditDialog * imageEditDialog;

    std::vector<size_t> countsMats;

    mapart::MapArtProject project;
    std::string projectFile;
    bool dirty;

    wxDECLARE_EVENT_TABLE();
};

namespace widgets {
    void displayMainWindow(wxApp &app);
}

#define FRAME_PAD_PIXELS (5)
#define REPORT_THREAD_DELAY (33)

#include "materials_window.h"
#include "image_edit_dialog.h"
