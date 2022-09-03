@echo on

call cmake -DCMAKE_BUILD_TYPE=Release -A Win32 -Ssrc -Brelease32

call cmake --build release32 --config Release

call zip -r release32/ImageToMapMC-1.3.0-Windows-Win32.zip release32/Release
