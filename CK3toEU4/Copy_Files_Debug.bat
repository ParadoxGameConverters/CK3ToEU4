echo on
rem Copy converter data files
copy "Data_Files\log.txt" "..\Debug\CK3toEU4\"
copy "Data_Files\configuration-example.txt" "..\Debug\CK3toEU4\"
copy "Resources\msvcp140_codecvt_ids.dll" "..\Debug\CK3toEU4\"
copy "Resources\vcruntime140_1.dll" "..\Debug\CK3toEU4\"
copy "Resources\rakaly.dll" "..\Debug\CK3toEU4\"

mkdir "..\Debug\Configuration"
copy "Data_Files\fronter-configuration.txt" "..\Debug\Configuration\"
copy "Data_Files\fronter-options.txt" "..\Debug\Configuration\"
copy "Data_Files\*.yml" "..\Debug\Configuration\"

mkdir "..\Debug\Docs"
copy "Data_Files\ReadMe.txt" "..\Debug\Docs\"
copy "Data_Files\FAQ.txt" "..\Debug\Docs\"
copy "Data_Files\license.txt" "..\Debug\Docs\"
copy "Data_Files\after_converting.txt" "..\Debug\Docs\"

rem Imagemagick dlls
xcopy "..\imageMagick\dll-windows-required" "..\Debug\CK3toEU4" /Y /E /I

rem Create Configurables
del "..\Debug\CK3toEU4\configurables" /Q
rmdir "..\Debug\CK3toEU4\configurables" /S /Q
xcopy "Data_Files\configurables" "..\Debug\CK3toEU4\configurables" /Y /E /I

rem Create Blank Mod
del "..\Debug\CK3toEU4\blankMod" /Q
rmdir "..\Debug\CK3toEU4\blankMod" /S /Q
xcopy "Data_Files\blankMod" "..\Debug\CK3toEU4\blankMod" /Y /E /I
