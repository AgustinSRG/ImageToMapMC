# Remove old folder
Remove-Item -ErrorAction Ignore -Path "ImageToMapMC" -Recurse

# Copy files
Copy-Item -Path "..\release\Release" -Destination "ImageToMapMC" -Recurse -Force