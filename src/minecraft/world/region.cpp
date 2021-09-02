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

#include "region.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "../../tools/byte_stream.h"

using namespace std;
using namespace minecraft;

uint32_t minecraft::getChunkLocation(uint32_t pageOffset, uint32_t pageCount)
{
    // Merge the 2 values into one
    return (pageOffset << 8) | pageCount;
}

MinecraftRegion::MinecraftRegion()
{
    x = 0;
    z = 0;
    offsetY = 0;
    version = MC_LAST_VERSION;
    offsetX = 0;
    offsetZ = 0;
}

MinecraftRegion::MinecraftRegion(int x, int z, minecraft::McVersion version, int offsetY)
{
    this->x = x;
    this->z = z;
    this->offsetY = offsetY;
    this->version = version;

    chunkMatrix.resize(REGION_WIDTH * REGION_HEIGHT);

    for (int i = 0; i < chunkMatrix.size(); i++)
    {
        chunkMatrix[i] = NULL;
    }

    offsetX = REGION_WIDTH * CHUNK_WIDTH * x;
    offsetZ = REGION_HEIGHT * CHUNK_HEIGHT * z;
}

minecraft::MinecraftChunk *MinecraftRegion::getChunk(int x, int z)
{
    minecraft::MinecraftChunk *chunk;

    int chunkX = min(max(0, x / CHUNK_WIDTH), REGION_WIDTH - 1);
    int chunkZ = min(max(0, z / CHUNK_HEIGHT), REGION_HEIGHT - 1);

    int indexInMatrix = chunkZ * REGION_WIDTH + chunkX;

    if (chunkMatrix[indexInMatrix] == NULL)
    {
        chunks.resize(chunks.size() + 1);

        chunks[chunks.size() - 1] = MinecraftChunk(version, chunkX, chunkZ);
        chunks[chunks.size() - 1].addBasePlatform(offsetY);
        chunkMatrix[indexInMatrix] = &(chunks[chunks.size() - 1]);
    }

    return chunkMatrix[indexInMatrix];
}

void MinecraftRegion::addBlock(const minecraft::BlockDescription *block, int x, int y, int z)
{
    int xRel = x - offsetX;
    int zRel = z - offsetZ;

    minecraft::MinecraftChunk *chunk = getChunk(xRel, zRel);

    chunk->addBlock(block, xRel % CHUNK_WIDTH, y + offsetY + 1, zRel % CHUNK_HEIGHT);
}

bool MinecraftRegion::writeToFile(std::string regionsPath)
{
    uint32_t offset = REGION_CHUNKS_START / REGION_PAGE_LENGTH;
    vector<uint32_t> offsets(chunks.size());
    vector<uint32_t> counts(chunks.size());
    vector<vector<char>> data(chunks.size());

    // Get chunks data
    for (int i = 0; i < chunks.size(); i++) {
        string dataStr = chunks[i].getBytes();
        std::vector<char> chunkData(dataStr.begin(), dataStr.end());
        data[i] = chunkData;

        offsets[i] = offset;

        size_t chunkSize = chunkData.size();
        uint32_t count = (chunkSize / REGION_PAGE_LENGTH) + 1;

        counts[i] = count;

        offset += count;
    }

    byte_stream stream(byte_stream::SWAP_ENDIAN);

    // insert offsets
    for (int i = 0; i < chunks.size(); i++) {
        stream << (int) getChunkLocation(offsets[i], counts[i]);
    }

    // insert timestamps
    for (int i = 0; i < chunks.size(); i++) {
        stream << (int) 0;
    }

    // Write chunks
	for (int i = 0; i < chunks.size(); i++) {
        // Add size
        stream << (int) data[i].size();

        // Add compression type
        stream << (char) 2;

        // Add chunk data
        stream << data[i];

        // Add padding
        size_t remaining = REGION_PAGE_LENGTH - (data[i].size() % REGION_PAGE_LENGTH) - (sizeof(int) + sizeof(char));
		for(unsigned int j = 0; j < remaining; ++j) {
            stream << (char) 0;
        }
    }

    // Path
    filesystem::path regionFilePath(regionsPath);
    stringstream ss;

    ss << "r." << x << "." << z << ".mca";

    regionFilePath /= ss.str();


    // Save
    std::ofstream file(regionFilePath, std::ios::binary);

    if (!file)
    {
        return false;
    }

    file.write(stream.rdbuf(), stream.size());

    return true;
}
