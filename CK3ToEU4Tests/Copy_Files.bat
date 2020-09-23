echo on
rem Imagemagick dlls
xcopy "..\imageMagick\dll-windows-required" "Release" /Y /E /I
mkdir "Release\TestFiles"
xcopy "TestFiles" "Release\TestFiles" /Y /E /I
