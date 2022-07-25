
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

#include "worker_thread.h"

#include <sstream>
#include <filesystem>

#if defined(_WIN32)
#include <shellapi.h>
#endif

#include <zip.h>

#include "../minecraft/structure.h"
#include "../minecraft/mcfunction.h"

using namespace std;
using namespace colors;
using namespace minecraft;
using namespace mapart;

DEFINE_EVENT_TYPE(wxEVT_WorkerThreadPreviewData)
DEFINE_EVENT_TYPE(wxEVT_WorkerThreadMaterials)
DEFINE_EVENT_TYPE(wxEVT_WorkerThreadError)

/* Constructor */

WorkerThread::WorkerThread(wxEvtHandler *pParent, int threadNum) : wxThread(wxTHREAD_DETACHED), m_pParent(pParent), sem(0, 1)
{
    this->threadNum = threadNum;
    taskType = TaskType::None;
    cancellable = false;
    progress.reset();
    progress.setEnded();
}

/* Get progress */

std::string WorkerThread::GetStatus()
{
    pair<string, unsigned int> p;
    std::string progressLine;

    stateMutex.Lock();

    if (progress.hasEnded())
    {
        progressLine = "Status: Ready";
    }
    else
    {
        p = progress.getProgress();

        // Print progress
        if (p.second == NO_PROGRESS)
        {
            progressLine = p.first;
        }
        else
        {
            stringstream ss;
            ss << "Status: " << p.first << " (" << p.second << "%)";
            progressLine = ss.str();
        }
    }

    stateMutex.Unlock();

    return progressLine;
}

bool WorkerThread::isBusy()
{
    bool result;
    stateMutex.Lock();
    result = !progress.hasEnded();
    stateMutex.Unlock();

    return result;
}

/* Get Data */

mapart::MapArtPreviewData WorkerThread::GetPreviewData()
{
    mapart::MapArtPreviewData copy;

    returnDataMutex.Lock();

    copy = previewData;

    returnDataMutex.Unlock();

    return copy;
}

std::vector<size_t> WorkerThread::GetMaterialsCount()
{
    std::vector<size_t> copy;

    returnDataMutex.Lock();

    copy = countMaterials;

    returnDataMutex.Unlock();

    return copy;
}

/* Error */

void WorkerThread::OnError(std::string msg)
{
    wxCommandEvent *errorEvent = new wxCommandEvent(wxEVT_WorkerThreadError);
    errorEvent->SetString(msg);
    wxQueueEvent(m_pParent, errorEvent);
}

/* Request tasks */

void WorkerThread::requestGeneratePreview(mapart::MapArtProject &project)
{
    stateMutex.Lock();

    // Cancel prev task
    if (cancellable)
    {
        progress.terminate();
    }

    // Set new task
    taskType = TaskType::Preview;
    cancellable = true;

    // Copy params
    this->project = project;

    stateMutex.Unlock();

    sem.Post();
}

void WorkerThread::requestExportMaterials(mapart::MapArtProject &project, std::string outPath)
{
    stateMutex.Lock();

    // Cancel prev task
    if (cancellable)
    {
        progress.terminate();
    }

    // Set new task
    taskType = TaskType::Export_Materials;
    cancellable = false;

    // Copy params
    this->project = project;
    this->outPath = outPath;

    stateMutex.Unlock();

    sem.Post();
}

void WorkerThread::requestExportMaterialsSplit(mapart::MapArtProject &project, std::string outPath)
{
    stateMutex.Lock();

    // Cancel prev task
    if (cancellable)
    {
        progress.terminate();
    }

    // Set new task
    taskType = TaskType::Export_MaterialsSplit;
    cancellable = false;

    // Copy params
    this->project = project;
    this->outPath = outPath;

    stateMutex.Unlock();

    sem.Post();
}

void WorkerThread::requestExportMaps(mapart::MapArtProject &project, std::string outPath, int mapNumber)
{
    stateMutex.Lock();

    // Cancel prev task
    if (cancellable)
    {
        progress.terminate();
    }

    // Set new task
    taskType = TaskType::Export_Maps;
    cancellable = false;

    // Copy params
    this->project = project;
    this->outPath = outPath;
    this->mapNumber = mapNumber;

    stateMutex.Unlock();

    sem.Post();
}

void WorkerThread::requestExportMapsZip(mapart::MapArtProject &project, std::string outPath)
{
    stateMutex.Lock();

    // Cancel prev task
    if (cancellable)
    {
        progress.terminate();
    }

    // Set new task
    taskType = TaskType::Export_Maps_Zip;
    cancellable = false;

    // Copy params
    this->project = project;
    this->outPath = outPath;
    this->mapNumber = 0;

    stateMutex.Unlock();

    sem.Post();
}

void WorkerThread::requestExportStruct(mapart::MapArtProject &project, std::string outPath)
{
    stateMutex.Lock();

    // Cancel prev task
    if (cancellable)
    {
        progress.terminate();
    }

    // Set new task
    taskType = TaskType::Export_Structure;
    cancellable = false;

    // Copy params
    this->project = project;
    this->outPath = outPath;

    stateMutex.Unlock();

    sem.Post();
}

void WorkerThread::requestExportStructZip(mapart::MapArtProject &project, std::string outPath)
{
    stateMutex.Lock();

    // Cancel prev task
    if (cancellable)
    {
        progress.terminate();
    }

    // Set new task
    taskType = TaskType::Export_Structure_Zip;
    cancellable = false;

    // Copy params
    this->project = project;
    this->outPath = outPath;

    stateMutex.Unlock();

    sem.Post();
}

void WorkerThread::requestExportFunc(mapart::MapArtProject &project, std::string outPath)
{
    stateMutex.Lock();

    // Cancel prev task
    if (cancellable)
    {
        progress.terminate();
    }

    // Set new task
    taskType = TaskType::Export_Func;
    cancellable = false;

    // Copy params
    this->project = project;
    this->outPath = outPath;

    stateMutex.Unlock();

    sem.Post();
}

/* Workers */

void WorkerThread::GeneratePreview(mapart::MapArtProject &copyProject)
{
    try
    {
        progress.startTask("Preparing image...", 0, 0);
        wxImage imageCopy = copyProject.toImage();

        if (copyProject.resize_width > 0 && copyProject.resize_height > 0)
        {
            imageCopy.Rescale(copyProject.resize_width, copyProject.resize_height);
        }

        int originalImageWidth;
        int originalImageHeight;
        vector<Color> originalImageColors = loadColorMatrixFromImageAndPad(imageCopy, copyProject.background, &originalImageWidth, &originalImageHeight);

        tools::editImage(originalImageColors, originalImageWidth, originalImageHeight, copyProject.saturation, copyProject.contrast, copyProject.brightness);

        progress.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(copyProject.version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        bool blacklist = true;

        progress.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(copyProject.colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, copyProject.buildMethod);

        progress.startTask("Generating preview...", originalImageHeight, threadNum);

        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);

        for (size_t i = 0; i < MAX_COLOR_GROUPS; i++)
        {
            countsMats[i] = 0;
        }

        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, copyProject.colorDistanceAlgorithm, copyProject.ditheringMethod, threadNum, progress, countsMats);

        returnDataMutex.Lock();
        previewData = MapArtPreviewData(mapArtColorMatrix, originalImageWidth, originalImageHeight);
        countMaterials = countsMats;
        returnDataMutex.Unlock();

        wxCommandEvent *completeEvent = new wxCommandEvent(wxEVT_WorkerThreadPreviewData);
        wxQueueEvent(m_pParent, completeEvent);

        wxCommandEvent *materialsEvent = new wxCommandEvent(wxEVT_WorkerThreadMaterials);
        wxQueueEvent(m_pParent, materialsEvent);
    }
    catch (int)
    {
        progress.reset();
    }

    progress.setEnded();
}

void WorkerThread::ExportMaterials(mapart::MapArtProject &copyProject, std::string &copyOutPath)
{
    try
    {
        progress.startTask("Preparing image...", 0, 0);
        wxImage imageCopy = copyProject.toImage();

        if (copyProject.resize_width > 0 && copyProject.resize_height > 0)
        {
            imageCopy.Rescale(copyProject.resize_width, copyProject.resize_height);
        }

        int originalImageWidth;
        int originalImageHeight;
        vector<Color> originalImageColors = loadColorMatrixFromImageAndPad(imageCopy, copyProject.background, &originalImageWidth, &originalImageHeight);

        tools::editImage(originalImageColors, originalImageWidth, originalImageHeight, copyProject.saturation, copyProject.contrast, copyProject.brightness);

        progress.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(copyProject.version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        progress.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(copyProject.colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, copyProject.buildMethod);

        progress.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, copyProject.colorDistanceAlgorithm, copyProject.ditheringMethod, threadNum, progress, countsMats);

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
                progress.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(copyProject.version, blockSet, mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ, copyProject.buildMethod, threadNum, progress);

                // Add to materials list
                materials.addBlocks(buildingBlocks);

                total++;
            }
        }

        progress.startTask("Saving...", 0, 0);

        if (!tools::writeTextFile(copyOutPath, materials.toString()))
        {
            OnError(string("Could not save the materials due to a file system error."));
        }
    }
    catch (int)
    {
        progress.reset();
    }

    progress.setEnded();

#if defined(_WIN32)

    ShellExecute(NULL, L"open", std::wstring(copyOutPath.begin(), copyOutPath.end()).c_str(), NULL, NULL, SW_SHOWDEFAULT);

#endif
}

void WorkerThread::ExportMaterialsSplit(mapart::MapArtProject &copyProject, std::string &copyOutPath)
{
    try
    {
        progress.startTask("Preparing image...", 0, 0);
        wxImage imageCopy = copyProject.toImage();

        if (copyProject.resize_width > 0 && copyProject.resize_height > 0)
        {
            imageCopy.Rescale(copyProject.resize_width, copyProject.resize_height);
        }

        int originalImageWidth;
        int originalImageHeight;
        vector<Color> originalImageColors = loadColorMatrixFromImageAndPad(imageCopy, copyProject.background, &originalImageWidth, &originalImageHeight);

        tools::editImage(originalImageColors, originalImageWidth, originalImageHeight, copyProject.saturation, copyProject.contrast, copyProject.brightness);

        progress.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(copyProject.version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        progress.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(copyProject.colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, copyProject.buildMethod);

        progress.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, copyProject.colorDistanceAlgorithm, copyProject.ditheringMethod, threadNum, progress, countsMats);

        // Compute total maps
        int mapsCountX = originalImageWidth / MAP_WIDTH;
        int mapsCountZ = originalImageHeight / MAP_HEIGHT;

        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;

        MaterialsList materials(baseColorNames);
        stringstream resultStream;

        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                stringstream ss;
                ss << "Building map (" << (total + 1) << "/" << totalMapsCount << ")...";
                progress.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(copyProject.version, blockSet, mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ, copyProject.buildMethod, threadNum, progress);

                // Add to materials list
                materials.addBlocks(buildingBlocks);

                total++;

                // Print materials
                resultStream << "Map #" << total << " (X: " << (mapX + 1) << ", Z: " << (mapZ + 1) << ")" << endl
                             << endl;
                resultStream << materials.toString();
                resultStream << endl
                             << "-------------------------------------------------------" << endl
                             << endl;

                // Clear
                materials.clear();
            }
        }

        progress.startTask("Saving...", 0, 0);

        if (!tools::writeTextFile(copyOutPath, resultStream.str()))
        {
            OnError(string("Could not save the materials due to a file system error."));
        }
    }
    catch (int)
    {
        progress.reset();
    }

    progress.setEnded();

#if defined(_WIN32)

    ShellExecute(NULL, L"open", std::wstring(copyOutPath.begin(), copyOutPath.end()).c_str(), NULL, NULL, SW_SHOWDEFAULT);

#endif
}

void WorkerThread::ExportMaps(mapart::MapArtProject &copyProject, std::string &copyOutPath, int copyMapNumber)
{
    try
    {
        progress.startTask("Preparing image...", 0, 0);
        wxImage imageCopy = copyProject.toImage();

        if (copyProject.resize_width > 0 && copyProject.resize_height > 0)
        {
            imageCopy.Rescale(copyProject.resize_width, copyProject.resize_height);
        }

        int originalImageWidth;
        int originalImageHeight;
        vector<Color> originalImageColors = loadColorMatrixFromImageAndPad(imageCopy, copyProject.background, &originalImageWidth, &originalImageHeight);

        tools::editImage(originalImageColors, originalImageWidth, originalImageHeight, copyProject.saturation, copyProject.contrast, copyProject.brightness);

        progress.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(copyProject.version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        progress.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(copyProject.colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, copyProject.buildMethod);

        progress.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, copyProject.colorDistanceAlgorithm, copyProject.ditheringMethod, threadNum, progress, countsMats);

        // Compute total maps
        int mapsCountX = originalImageWidth / MAP_WIDTH;
        int mapsCountZ = originalImageHeight / MAP_HEIGHT;

        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;

        progress.startTask("Saving to map files...", mapsCountZ * mapsCountX, 1);
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                // Get map data
                std::vector<map_color_t> mapDataToSave = getMapDataFromColorMatrix(mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ);

                // Save to file
                stringstream ss;
                ss << "map_" << (mapNumber++) << ".dat";
                filesystem::path outFilePath(copyOutPath);

                outFilePath /= ss.str();

                try
                {
                    writeMapNBTFile(outFilePath.string(), mapDataToSave, copyProject.version);
                }
                catch (...)
                {
                    OnError(string("Cannot write file: ") + outFilePath.string());
                    throw -1;
                }

                total++;
                progress.setProgress(0, total);
            }
        }
    }
    catch (int)
    {
        progress.reset();
    }

    progress.setEnded();

#if defined(_WIN32)

    ShellExecute(NULL, L"open", std::wstring(copyOutPath.begin(), copyOutPath.end()).c_str(), NULL, NULL, SW_SHOWDEFAULT);

#endif
}

void WorkerThread::ExportMapsZip(mapart::MapArtProject &copyProject, std::string &outFilePath)
{
    zip_t *zipper = nullptr;
    try
    {
        progress.startTask("Preparing image...", 0, 0);
        wxImage imageCopy = copyProject.toImage();

        if (copyProject.resize_width > 0 && copyProject.resize_height > 0)
        {
            imageCopy.Rescale(copyProject.resize_width, copyProject.resize_height);
        }

        int originalImageWidth;
        int originalImageHeight;
        vector<Color> originalImageColors = loadColorMatrixFromImageAndPad(imageCopy, copyProject.background, &originalImageWidth, &originalImageHeight);

        tools::editImage(originalImageColors, originalImageWidth, originalImageHeight, copyProject.saturation, copyProject.contrast, copyProject.brightness);

        progress.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(copyProject.version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        progress.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(copyProject.colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, copyProject.buildMethod);

        progress.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, copyProject.colorDistanceAlgorithm, copyProject.ditheringMethod, threadNum, progress, countsMats);

        // Create zip container for the files
        int errorp;
        zipper = zip_open(outFilePath.c_str(), ZIP_CREATE | ZIP_EXCL, &errorp);

        if (zipper == nullptr)
        {
            OnError(string("Cannot write file: ") + outFilePath);
            throw -1;
        }

        // Compute total maps
        int mapsCountX = originalImageWidth / MAP_WIDTH;
        int mapsCountZ = originalImageHeight / MAP_HEIGHT;

        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;

        progress.startTask("Saving to map files...", mapsCountZ * mapsCountX, 1);
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                // Get map data
                std::vector<map_color_t> mapDataToSave = getMapDataFromColorMatrix(mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ);

                // Save to file
                stringstream ss;
                ss << "map_" << (mapNumber++) << ".dat";

                std::string fPath = ss.str();

                try
                {
                    writeMapNBTFileZip(fPath, zipper, mapDataToSave, copyProject.version);
                }
                catch (...)
                {
                    OnError(string("Cannot store map file into zip: ") + fPath);
                    throw -1;
                }

                total++;
                progress.setProgress(0, total);
            }
        }

        zip_close(zipper); // Close zipper
    }
    catch (int)
    {
        if (zipper != nullptr)
        {
            zip_close(zipper); // Close zipper
        }
        progress.reset();
    }

    progress.setEnded();

#if defined(_WIN32)

    ShellExecute(NULL, L"open", std::wstring(outFilePath.begin(), outFilePath.end()).c_str(), NULL, NULL, SW_SHOWDEFAULT);

#endif
}

void WorkerThread::ExportStruct(mapart::MapArtProject &copyProject, std::string &copyOutPath)
{
    try
    {
        progress.startTask("Preparing image...", 0, 0);
        wxImage imageCopy = copyProject.toImage();

        if (copyProject.resize_width > 0 && copyProject.resize_height > 0)
        {
            imageCopy.Rescale(copyProject.resize_width, copyProject.resize_height);
        }

        int originalImageWidth;
        int originalImageHeight;
        vector<Color> originalImageColors = loadColorMatrixFromImageAndPad(imageCopy, copyProject.background, &originalImageWidth, &originalImageHeight);

        tools::editImage(originalImageColors, originalImageWidth, originalImageHeight, copyProject.saturation, copyProject.contrast, copyProject.brightness);

        progress.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(copyProject.version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        progress.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(copyProject.colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, copyProject.buildMethod);

        progress.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, copyProject.colorDistanceAlgorithm, copyProject.ditheringMethod, threadNum, progress, countsMats);

        // Compute total maps
        int mapsCountX = originalImageWidth / MAP_WIDTH;
        int mapsCountZ = originalImageHeight / MAP_HEIGHT;

        progress.startTask("Building maps...", 0, 0);
        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                stringstream ss;
                ss << "Building map (" << (total + 1) << "/" << totalMapsCount << ")...";
                progress.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(copyProject.version, blockSet, mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ, copyProject.buildMethod, threadNum, progress);

                // Save as structure file
                stringstream ss2;
                ss2 << "map_" << (total + 1) << ".nbt";
                filesystem::path outFilePath(copyOutPath);

                outFilePath /= ss2.str();

                try
                {
                    writeStructureNBTFile(outFilePath.string(), buildingBlocks, copyProject.version);
                }
                catch (...)
                {
                    OnError(string("Cannot write file: ") + outFilePath.string());
                    throw -1;
                }

                total++;
            }
        }
    }
    catch (int)
    {
        progress.reset();
    }

    progress.setEnded();

#if defined(_WIN32)

    ShellExecute(NULL, L"open", std::wstring(copyOutPath.begin(), copyOutPath.end()).c_str(), NULL, NULL, SW_SHOWDEFAULT);

#endif
}

void WorkerThread::ExportStructZip(mapart::MapArtProject &copyProject, std::string &outFilePath)
{
    zip_t *zipper = nullptr;
    try
    {
        progress.startTask("Preparing image...", 0, 0);
        wxImage imageCopy = copyProject.toImage();

        if (copyProject.resize_width > 0 && copyProject.resize_height > 0)
        {
            imageCopy.Rescale(copyProject.resize_width, copyProject.resize_height);
        }

        int originalImageWidth;
        int originalImageHeight;
        vector<Color> originalImageColors = loadColorMatrixFromImageAndPad(imageCopy, copyProject.background, &originalImageWidth, &originalImageHeight);

        tools::editImage(originalImageColors, originalImageWidth, originalImageHeight, copyProject.saturation, copyProject.contrast, copyProject.brightness);

        progress.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(copyProject.version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        progress.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(copyProject.colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, copyProject.buildMethod);

        progress.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, copyProject.colorDistanceAlgorithm, copyProject.ditheringMethod, threadNum, progress, countsMats);

        // Create zip container for the files
        int errorp;
        zipper = zip_open(outFilePath.c_str(), ZIP_CREATE | ZIP_EXCL, &errorp);

        if (zipper == nullptr)
        {
            OnError(string("Cannot write file: ") + outFilePath);
            throw -1;
        }

        // Compute total maps
        int mapsCountX = originalImageWidth / MAP_WIDTH;
        int mapsCountZ = originalImageHeight / MAP_HEIGHT;

        progress.startTask("Building maps...", 0, 0);
        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                stringstream ss;
                ss << "Building map (" << (total + 1) << "/" << totalMapsCount << ")...";
                progress.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(copyProject.version, blockSet, mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ, copyProject.buildMethod, threadNum, progress);

                // Save as structure file
                stringstream ss2;
                ss2 << "map_" << (total + 1) << ".nbt";

                std::string fPath = ss2.str();

                try
                {
                    writeStructureNBTFileZip(fPath, zipper, buildingBlocks, copyProject.version);
                }
                catch (...)
                {
                    OnError(string("Cannot add file to zip: ") + fPath);
                    throw -1;
                }

                total++;
            }
        }
        zip_close(zipper); // Close zipper
    }
    catch (int)
    {
        if (zipper != nullptr)
        {
            zip_close(zipper); // Close zipper
        }
        progress.reset();
    }

    progress.setEnded();

#if defined(_WIN32)

    ShellExecute(NULL, L"open", std::wstring(outFilePath.begin(), outFilePath.end()).c_str(), NULL, NULL, SW_SHOWDEFAULT);

#endif
}

void WorkerThread::ExportFunc(mapart::MapArtProject &copyProject, std::string &copyOutPath)
{
    try
    {
        progress.startTask("Preparing image...", 0, 0);
        wxImage imageCopy = copyProject.toImage();

        if (copyProject.resize_width > 0 && copyProject.resize_height > 0)
        {
            imageCopy.Rescale(copyProject.resize_width, copyProject.resize_height);
        }

        int originalImageWidth;
        int originalImageHeight;
        vector<Color> originalImageColors = loadColorMatrixFromImageAndPad(imageCopy, copyProject.background, &originalImageWidth, &originalImageHeight);

        tools::editImage(originalImageColors, originalImageWidth, originalImageHeight, copyProject.saturation, copyProject.contrast, copyProject.brightness);

        progress.startTask("Loading minecraft colors...", 0, 0);
        std::vector<colors::Color> baseColors = minecraft::loadBaseColors(copyProject.version);
        std::vector<minecraft::FinalColor> colorSet = minecraft::loadFinalColors(baseColors);
        std::vector<minecraft::BlockList> blockSet = loadBlocks(baseColors);
        std::vector<std::string> baseColorNames = loadBaseColorNames(baseColors);
        std::vector<bool> enabledConf(baseColors.size());
        std::vector<size_t> countsMats(MAX_COLOR_GROUPS);
        bool blacklist = true;

        progress.startTask("Loading custom configuration...", 0, 0);
        mapart::applyColorSet(copyProject.colorSetConf, &blacklist, enabledConf, colorSet, blockSet, baseColorNames);
        // Apply color restructions based on build method
        applyBuildRestrictions(colorSet, copyProject.buildMethod);

        progress.startTask("Adjusting colors...", originalImageHeight, threadNum);
        std::vector<const minecraft::FinalColor *> mapArtColorMatrix = generateMapArt(colorSet, originalImageColors, originalImageWidth, originalImageHeight, copyProject.colorDistanceAlgorithm, copyProject.ditheringMethod, threadNum, progress, countsMats);

        // Compute total maps
        int mapsCountX = originalImageWidth / MAP_WIDTH;
        int mapsCountZ = originalImageHeight / MAP_HEIGHT;

        progress.startTask("Building maps...", 0, 0);
        int total = 0;
        int totalMapsCount = mapsCountX * mapsCountZ;
        for (int mapZ = 0; mapZ < mapsCountZ; mapZ++)
        {
            for (int mapX = 0; mapX < mapsCountX; mapX++)
            {
                stringstream ss;
                ss << "Building map (" << (total + 1) << "/" << totalMapsCount << ")...";
                progress.startTask(ss.str(), MAP_WIDTH, threadNum);

                std::vector<mapart::MapBuildingBlock> buildingBlocks = mapart::buildMap(copyProject.version, blockSet, mapArtColorMatrix, originalImageWidth, originalImageHeight, mapX, mapZ, copyProject.buildMethod, threadNum, progress);

                // Save as structure file
                stringstream ss2;
                ss2 << "map_" << (total + 1) << ".mcfunction";
                filesystem::path outFilePath(copyOutPath);

                outFilePath /= ss2.str();

                try
                {
                    writeMcFunctionFile(outFilePath.string(), buildingBlocks, copyProject.version);
                }
                catch (...)
                {
                    OnError(string("Cannot write file: ") + outFilePath.string());
                    throw -1;
                }

                total++;
            }
        }
    }
    catch (int)
    {
        progress.reset();
    }

    progress.setEnded();

#if defined(_WIN32)

    ShellExecute(NULL, L"open", std::wstring(copyOutPath.begin(), copyOutPath.end()).c_str(), NULL, NULL, SW_SHOWDEFAULT);

#endif
}

/* Main Entry */

wxThread::ExitCode WorkerThread::Entry()
{
    TaskType copyTaskType;
    mapart::MapArtProject copyProject;
    std::string copyOutPath;
    int copyMapNumber;

    while (!TestDestroy())
    {
        sem.Wait();

        stateMutex.Lock();
        copyTaskType = taskType;

        if (copyTaskType == TaskType::None)
        {
            stateMutex.Unlock();
            continue;
        }

        copyProject = project;
        copyOutPath = outPath;
        copyMapNumber = mapNumber;

        progress.reset();

        taskType = TaskType::None;

        stateMutex.Unlock();

        switch (copyTaskType)
        {
        case TaskType::Preview:
            GeneratePreview(copyProject);
            break;
        case TaskType::Export_Materials:
            ExportMaterials(copyProject, copyOutPath);
            break;
        case TaskType::Export_MaterialsSplit:
            ExportMaterialsSplit(copyProject, copyOutPath);
            break;
        case TaskType::Export_Maps:
            ExportMaps(copyProject, copyOutPath, copyMapNumber);
            break;
        case TaskType::Export_Maps_Zip:
            ExportMapsZip(copyProject, copyOutPath);
            break;
        case TaskType::Export_Structure:
            ExportStruct(copyProject, copyOutPath);
            break;
        case TaskType::Export_Structure_Zip:
            ExportStructZip(copyProject, copyOutPath);
            break;
        case TaskType::Export_Func:
            ExportFunc(copyProject, copyOutPath);
            break;
        }
    }

    return (wxThread::ExitCode)0; // success
}
