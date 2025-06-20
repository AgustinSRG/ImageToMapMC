@echo off

call build.bat

cd wix
call "make-wix-installer.bat"
cd ..
