@echo on

call cmake -DCMAKE_BUILD_TYPE=Release -A Win32 -Ssrc -Brelease32

call cmake --build release32 --config Release

%SystemRoot%\explorer.exe "release32\Release"
