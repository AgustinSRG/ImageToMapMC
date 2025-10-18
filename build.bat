@echo off

call cmake -DCMAKE_BUILD_TYPE=Release -DWIN_RELEASE=1 -A x64 -Ssrc -Brelease

call cmake --build release --config Release

powershell -command "Compress-Archive release/Release/* release/ImageToMapMC-1.7.1-Windows-x64.zip"
