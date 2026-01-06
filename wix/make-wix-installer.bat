@echo off

call powershell < "configure.ps1"

call heat dir "ImageToMapMC" -gg -cg PComponentGroup -dr ProgramFiles64Folder -directoryid -out "HeatFile.wxs"

call candle -ext WixUIExtension -ext WixUtilExtension -arch x64 "HeatFile.wxs" -o "HeatFile.wixobj"

call candle -ext WixUIExtension -ext WixUtilExtension -arch x64 "Product.wxs" -o "Product.wixobj"

call light "Product.wixobj" -spdb "HeatFile.wixobj" -b ImageToMapMC -o "ImageToMapMC-1.8.0-x64.msi" -ext WixUIExtension -ext WixUtilExtension
