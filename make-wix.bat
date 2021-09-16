@echo on

call candle -ext WixUIExtension -ext WixUtilExtension -arch x64 wix-64.wxs -o build\wix-64.wixobj

call light build\wix-64.wixobj -o release\subprojects\Build\ImageToMapMC_core\Release\ImageToMapMC-x64.msi -ext WixUIExtension -ext WixUtilExtension

%SystemRoot%\explorer.exe "release\subprojects\Build\ImageToMapMC_core\Release"
