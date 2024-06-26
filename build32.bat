@echo off

call cmake -DCMAKE_BUILD_TYPE=Release -A Win32 -Ssrc -Brelease32

call cmake --build release32 --config Release

call zip -r release32/ImageToMapMC-1.3.4-Windows-Win32.zip release32/Release
