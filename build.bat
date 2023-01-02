@echo on

call cmake -DCMAKE_BUILD_TYPE=Release -A x64 -Ssrc -Brelease

call cmake --build release --config Release

call zip -r release/ImageToMapMC-1.3.1-Windows-x64.zip release/Release
