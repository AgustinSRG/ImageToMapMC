@echo off

call powershell < "wix\configure.ps1"

call heat dir "wix\ImageToMapMC" -gg -cg PComponentGroup -dr ProgramFiles64Folder -out HeatFile.wxs

call candle -ext WixUIExtension -ext WixUtilExtension -arch x64 Product.wxs -o release\Product.wixobj

call light release\Product.wixobj -o release\ImageToMapMC-{VERSION}-x64.msi -ext WixUIExtension -ext WixUtilExtension
