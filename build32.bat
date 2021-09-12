@echo on

call cmake -DCMAKE_BUILD_TYPE=Release -A Win32 -S. -Brelease32

call cmake --build release32 --config Release
