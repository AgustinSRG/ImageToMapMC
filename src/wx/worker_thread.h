
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

#include "../mapart/project.h"
#include "../mapart/map_art.h"
#include "../mapart/map_image.h"
#include "../tools/text_file.h"
#include "../tools/image_edit.h"

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_WorkerThreadPreviewData, -1)
DECLARE_EVENT_TYPE(wxEVT_WorkerThreadMaterials, -1)
DECLARE_EVENT_TYPE(wxEVT_WorkerThreadError, -1)
END_DECLARE_EVENT_TYPES()

enum class TaskType
{
    None,
    Preview,
    Export_Maps,
    Export_Maps_Zip,
    Export_Structure,
    Export_Structure_Single,
    Export_Structure_Zip,
    Export_Func,
    Export_Materials,
    Export_MaterialsSplit
};

class WorkerThread : public wxThread
{
public:
    WorkerThread(wxEvtHandler *pParent, int threadNum);

    std::string GetStatus();
    bool isBusy();

    mapart::MapArtPreviewData GetPreviewData();
    std::vector<size_t> GetMaterialsCount();

    void requestGeneratePreview(mapart::MapArtProject &project);

    void requestExportMaterials(mapart::MapArtProject &project, std::string outPath);
    void requestExportMaterialsSplit(mapart::MapArtProject &project, std::string outPath);

    void requestExportMaps(mapart::MapArtProject &project, std::string outPath, int mapNumber);
    void requestExportMapsZip(mapart::MapArtProject &project, std::string outPath);

    void requestExportStruct(mapart::MapArtProject &project, std::string outPath);
    void requestExportStructSingleFile(mapart::MapArtProject &project, std::string outPath);
    void requestExportStructZip(mapart::MapArtProject &project, std::string outPath);

    void requestExportFunc(mapart::MapArtProject &project, std::string outPath);
private:
    int threadNum;
    wxMutex stateMutex;

    wxSemaphore sem;

    TaskType taskType;
    bool cancellable;

    mapart::MapArtProject project;
    std::string outPath;
    int mapNumber;

    threading::Progress progress;

    wxMutex returnDataMutex;
    std::vector<size_t> countMaterials;
    mapart::MapArtPreviewData previewData;

    void OnError(std::string msg);

    // Tasks
    void GeneratePreview(mapart::MapArtProject &copyProject);

    void ExportMaterials(mapart::MapArtProject &copyProject, std::string &copyOutPath);
    void ExportMaterialsSplit(mapart::MapArtProject &copyProject, std::string &copyOutPath);

    void ExportMaps(mapart::MapArtProject &copyProject, std::string &copyOutPath, int copyMapNumber);
    void ExportMapsZip(mapart::MapArtProject &copyProject, std::string &outFilePath);

    void ExportStruct(mapart::MapArtProject &copyProject, std::string &copyOutPath);
    void ExportStructSingleFile(mapart::MapArtProject &copyProject, std::string &copyOutPath);
    void ExportStructZip(mapart::MapArtProject &copyProject, std::string &copyOutPath);

    void ExportFunc(mapart::MapArtProject &copyProject, std::string &copyOutPath);
protected:
    virtual ExitCode Entry();
    wxEvtHandler *m_pParent;
};
