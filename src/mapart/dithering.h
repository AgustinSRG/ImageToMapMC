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

#define BAYER_44_MATRIX_H 4
#define BAYER_44_MATRIX_W 4

static double BAYER_44_MATRIX[BAYER_44_MATRIX_H][BAYER_44_MATRIX_W] = {
    1, 9, 3, 11,
    13, 5, 15, 7,
    4, 12, 2, 10,
    16, 8, 14, 6};

#define BAYER_22_MATRIX_H 2
#define BAYER_22_MATRIX_W 2

static double BAYER_22_MATRIX[BAYER_22_MATRIX_H][BAYER_22_MATRIX_W] = {
    1, 3,
    4, 2};

#define ORDERED_33_MATRIX_H 3
#define ORDERED_33_MATRIX_W 3

static double ORDERED_33_MATRIX[ORDERED_33_MATRIX_H][ORDERED_33_MATRIX_W] = {
    1, 7, 4,
    5, 8, 3,
    6, 2, 9};

#define ERROR_DIFFUSSION_MATRIX_H 3
#define ERROR_DIFFUSSION_MATRIX_W 5

static double FLOYD_STEINBERG_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 7, 0,
    0, 3, 5, 1, 0,
    0, 0, 0, 0, 0};

static double FLOYD_STEINBERG_DIVISOR = 16;

static double MINAVGERR_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 7, 5,
    3, 5, 7, 5, 3,
    1, 3, 5, 3, 1};

static double MINAVGERR_DIVISOR = 48;

static double BURKES_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 8, 4,
    2, 4, 8, 4, 2,
    0, 0, 0, 0, 0};

static double BURKES_DIVISOR = 32;

static double SIERRA_LITE_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 2, 0,
    0, 1, 1, 0, 0,
    0, 0, 0, 0, 0};

static double SIERRA_LITE_DIVISOR = 4;

static double STUCKI_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 8, 4,
    2, 4, 8, 4, 2,
    1, 2, 4, 2, 1};

static double STUCKI_DIVISOR = 42;

static double ATKINSON_MATRIX[ERROR_DIFFUSSION_MATRIX_H][ERROR_DIFFUSSION_MATRIX_W] = {
    0, 0, 0, 1, 1,
    0, 1, 0, 1, 0,
    0, 0, 1, 0, 0};

static double ATKINSON_DIVISOR = 8;
