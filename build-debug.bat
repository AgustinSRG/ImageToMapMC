@echo on

call cmake -DCMAKE_BUILD_TYPE=Debug -A x64 -Ssrc -Bbuild

call cmake --build build --config Debug

