@echo off

call cmake -DCMAKE_BUILD_TYPE=Release -DWIN_RELEASE=1 -A x64 -Ssrc -Brelease

call cmake --build release --config Release

call zip -r release/ImageToMapMC-1.6.2-Windows-x64.zip release/Release
