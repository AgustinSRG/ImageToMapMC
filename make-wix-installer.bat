@echo on

call candle -ext WixUIExtension -ext WixUtilExtension -arch x64 Product.wxs -o release\Product.wixobj

call light release\Product.wixobj -o release\ImageToMapMC-1.3.1-x64.msi -ext WixUIExtension -ext WixUtilExtension
