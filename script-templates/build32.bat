@echo on

call cmake -DCMAKE_BUILD_TYPE=Release -A Win32 -Ssrc -Brelease32

call cmake --build release32 --config Release

call zip -r release32/ImageToMapMC-{VERSION}-Windows-Win32.zip release32/Release
