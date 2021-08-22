@echo on

call cmake -DCMAKE_BUILD_TYPE=Release -S. -Brelease

call cmake --build release --config Release
