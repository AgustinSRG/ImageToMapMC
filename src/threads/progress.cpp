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

#include "progress.h"

using namespace std;
using namespace threading;

Progress::Progress()
{
    task_name = "Initializing...";
    total_threads = 0;
    ended = false;
    terminated = false;
    total_progress = 0;
}

void Progress::reset() {
    task_name = "Initializing...";
    total_threads = 0;
    ended = false;
    terminated = false;
    total_progress = 0;
}

bool Progress::hasEnded()
{
    return ended;
}

void Progress::startTask(std::string name, unsigned int totalP, unsigned int threadsNum)
{
    if (terminated) {
        throw -1;
    }
    mtx.lock();
    task_name = name;
    total_threads = threadsNum;
    progress.resize(total_threads);
    for (size_t i = 0; i < total_threads; i++)
    {
        progress[i] = 0;
    }
    total_progress = totalP;
    mtx.unlock();
}

void Progress::setProgress(unsigned int thread_num, unsigned int p)
{
    if (terminated) {
        throw -1;
    }
    mtx.lock();
    progress[thread_num] = p;
    mtx.unlock();
}

std::pair<std::string, unsigned int> Progress::getProgress()
{
    unsigned int t = 0;
    std::string name;
    std::pair<std::string, unsigned int> result;

    mtx.lock();
    name = task_name;
    if (total_progress > 0)
    {
        for (size_t i = 0; i < total_threads; i++)
        {
            t += progress[i];
        }
        t = (t * 100) / total_progress;
    }
    else
    {
        t = NO_PROGRESS;
    }

    mtx.unlock();

    result.first = name;
    result.second = t;

    return result;
}

void Progress::setEnded() {
    ended = true;
}

void Progress::terminate() {
    terminated = true;
}
