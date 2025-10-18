#!/bin/bash

# Build script for linux

cmake -DCMAKE_BUILD_TYPE=Release -Ssrc -Brelease

cmake --build release --config Release

rm -rf release/bin || true

rm -rf release/ImageToMapMC-1.7.1-linux-x64.tar.gz || true

mkdir release/bin

cp release/mcmap release/bin/mcmap
cp release/mcmap-gui release/bin/mcmap-gui

tar -cvzf release/ImageToMapMC-1.7.1-linux-x64.tar.gz release/bin

echo "DONE: Created release ImageToMapMC-1.7.1-linux-x64.tar.gz"
