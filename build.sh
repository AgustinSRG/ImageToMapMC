#!/bin/bash

# Build script for linux

cmake -DCMAKE_BUILD_TYPE=Release -Ssrc -Brelease

cmake --build release --config Release


