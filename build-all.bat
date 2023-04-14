@echo off

call build.bat
call build32.bat

cd wix
call "make-wix-installer.bat"
cd ..
