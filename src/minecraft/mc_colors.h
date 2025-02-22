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

#include "../colors/colors.h"

#include "mc_common.h"

#include <vector>
#include <map>

#define MAX_COLOR_GROUPS (64)

namespace minecraft
{

    /**
     * @brief  Color types in maps
     * @note   
     * @retval None
     */
    enum class McColorType
    {
        NORMAL,
        LIGHT,
        DARK,
        DARKER, // Darker colors are not obtainable
    };

    /**
     * @brief  Get a minecraft final color from a base color
     * @note   
     * @param  &baseColors: List of base colors
     * @param  index: Index of the color inside the base colors
     * @param  colorType: Colort type
     * @retval The final color
     */
    colors::Color getMinecraftColor(std::vector<colors::Color> &baseColors, size_t index, McColorType colorType);

    /**
     * @brief  Get a minecraft final color from a base color
     * @note   
     * @param  baseColor: The base color
     * @param  colorType: Colort type
     * @retval The final color
     */
    colors::Color getMinecraftColorFromBase(colors::Color baseColor, McColorType colorType);

    /**
     * @brief  Base colors
     * @note   
     * @retval None
     */
    enum class McColors : short
    {
        NONE = 0,

        GRASS = 1,
        SAND = 2,
        WOOL = 3,
        FIRE = 4,
        ICE = 5,
        METAL = 6,
        PLANT = 7,
        SNOW = 8,
        CLAY = 9,
        DIRT = 10,
        STONE = 11,
        WATER = 12,
        WOOD = 13,
        QUARTZ = 14,

        COLOR_ORANGE = 15,
        COLOR_MAGENTA = 16,
        COLOR_LIGHT_BLUE = 17,
        COLOR_YELLOW = 18,
        COLOR_LIGHT_GREEN = 19,
        COLOR_PINK = 20,
        COLOR_GRAY = 21,
        COLOR_LIGHT_GRAY = 22,
        COLOR_CYAN = 23,
        COLOR_PURPLE = 24,
        COLOR_BLUE = 25,
        COLOR_BROWN = 26,
        COLOR_GREEN = 27,
        COLOR_RED = 28,
        COLOR_BLACK = 29,

        GOLD = 30,
        DIAMOND = 31,
        LAPIS = 32,
        EMERALD = 33,

        PODZOL = 34,
        NETHER = 35,

        TERRACOTTA_WHITE = 36,
        TERRACOTTA_ORANGE = 37,
        TERRACOTTA_MAGENTA = 38,
        TERRACOTTA_LIGHT_BLUE = 39,
        TERRACOTTA_YELLOW = 40,
        TERRACOTTA_LIGHT_GREEN = 41,
        TERRACOTTA_PINK = 42,
        TERRACOTTA_GRAY = 43,
        TERRACOTTA_LIGHT_GRAY = 44,
        TERRACOTTA_CYAN = 45,
        TERRACOTTA_PURPLE = 46,
        TERRACOTTA_BLUE = 47,
        TERRACOTTA_BROWN = 48,
        TERRACOTTA_GREEN = 49,
        TERRACOTTA_RED = 50,
        TERRACOTTA_BLACK = 51,

        CRIMSON_NYLIUM = 52,
        CRIMSON_STEM = 53,
        CRIMSON_HYPHAE = 54,
        WARPED_NYLIUM = 55,
        WARPED_STEM = 56,
        WARPED_HYPHAE = 57,
        WARPED_WART_BLOCK = 58,

        DEEPSLATE = 59,
        RAW_IRON = 60,
        GLOW_LICHEN = 61
    };

    /**
     * @brief  Loads base colors
     * @note   
     * @param  version: Minecraft version
     * @retval List of base colors
     */
    std::vector<colors::Color> loadBaseColors(McVersion version);

    /**
     * @brief  Load base color names to display them in GUI or text output
     * @note   
     * @param  &baseColors: List of base colors
     * @retval List of base color names
     */
    std::vector<std::string> loadBaseColorNames(std::vector<colors::Color> &baseColors);

    /**
     * @brief  Finds color by name
     * @note   
     * @param  &colorNames: List of color names
     * @param  name: Color name
     * @retval The index of the color, or -1 if not found
     */
    size_t findColorByName(const std::vector<std::string> &colorNames, std::string name);

    /**
     * @brief  Final color
     * @note   
     * @retval None
     */
    class FinalColor
    {
    public:
        short baseColorIndex;  // Index of base color (for block selection)
        McColorType colorType; // Color type
        bool enabled;          // Enabled for use?
        colors::Color color;   // RGB
        colors::Lab lab;       // LAB

        // Methods

        /**
             * @brief  Sets the color
             * @note   
             * @param  &baseColors: List of base colors
             * @param  index: Index of the color
             * @param  colorType: Color type (light)
             * @retval None
             */
        void setColor(std::vector<colors::Color> &baseColors, size_t index, McColorType colorType);

        short getMapColor()  const;
    };

    /**
     * @brief  Loads full color list
     * @note   
     * @param  &baseColors: Base color list
     * @retval List of colors
     */
    std::vector<minecraft::FinalColor> loadFinalColors(std::vector<colors::Color> &baseColors);

    /**
     * @brief  Finds the best color
     * @note   
     * @param  &colors: List of colors
     * @param  color: Original color (RGB)
     * @param  algo: Algorithm to compute the distance
     * @retval The index inside the list
     */
    size_t findClosestColor(const std::vector<minecraft::FinalColor> &colors, colors::Color color, colors::ColorDistanceAlgorithm algo);

    /**
     * @brief  Finds the 2 closest colors
     * @note   
     * @param  &colors: List of colors
     * @param  color: Original color (RGB)
     * @param  algo: Algorithm to compute the distance
     * @param  distFirst: Variable to store distance
     * @param  distSecond: Variable to store distance
     * @retval Vector. 1st element is the closest color, 2nd is the 2nd closest color
     */
    std::vector<size_t> find2ClosestColors(const std::vector<minecraft::FinalColor> &colors, colors::Color color, colors::ColorDistanceAlgorithm algo, double * distFirst, double * distSecond);

    /**
     * @brief  Initializaes colors list enabled properties
     * @note   
     * @param  &colors: Colors list
     * @param  blacklist: true if blacklist, false if whitelist
     * @retval None
     */
    void initializeEnabledColors(std::vector<minecraft::FinalColor> &colors, bool blacklist);

    /**
     * @brief  Sets enabled property
     * @note   
     * @param  &colors: List of colors
     * @param  color: Base color
     * @param  enabled: Enabled?
     * @retval None
     */
    void setBaseColorEnabled(std::vector<minecraft::FinalColor> &colors, short color, bool enabled);

    /**
     * @brief  Changes enabled for all the colors of a type
     * @note   
     * @param  &colors: List of colors
     * @param  colorType: Type of color
     * @param  enabled: Enabled?
     * @retval None
     */
    void setColorTypesEnabled(std::vector<minecraft::FinalColor> &colors, McColorType colorType, bool enabled);
}
