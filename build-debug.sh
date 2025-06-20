#!/bin/bash

cmake -DCMAKE_BUILD_TYPE=Debug -A x64 -Ssrc -Bbuild

cmake --build build --config Debug
