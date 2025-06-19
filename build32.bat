@echo off

call cmake -DCMAKE_BUILD_TYPE=Release -DWIN_RELEASE_BUILD=1 -A Win32 -Ssrc -Brelease32

call cmake --build release32 --config Release

call zip -r release32/ImageToMapMC-1.4.0-Windows-Win32.zip release32/Release
