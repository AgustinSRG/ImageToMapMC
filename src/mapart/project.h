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

#include "common.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace mapart {
    class MapArtProject {
        public:
            int width;
            int height;

            int resize_width;
            int resize_height;

            float saturation;
            float brightness;
            float contrast;

            mapart::MapBuildMethod buildMethod;
            colors::ColorDistanceAlgorithm colorDistanceAlgorithm;
            mapart::DitheringMethod ditheringMethod;

            minecraft::McVersion version;

            std::string colorSetConf;

            std::vector<unsigned char> image_data;

            MapArtProject();
            MapArtProject(const MapArtProject &p1);

            bool loadFromFile(std::string path);
            bool saveToFile(std::string path);

            std::vector<colors::Color> getColors();

            wxImage toImage();

            void loadImage(wxImage &image);
    };
}
