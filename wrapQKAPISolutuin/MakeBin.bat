mkdir bin
mkdir bin\Win32
mkdir bin\Win32\Debug
mkdir bin\Win32\Release
mkdir bin\x64
mkdir bin\x64\Debug
mkdir bin\x64\Release

xcopy Externals\QuikAPI\Win32\bin\TRANS2QUIK.dll bin\Win32\Debug /y
xcopy Externals\QuikAPI\Win32\bin\TRANS2QUIK.dll bin\Win32\Release /y

xcopy Externals\QuikAPI\Win64\bin\trans2quik.dll  bin\x64\Debug /y
xcopy Externals\QuikAPI\Win64\bin\trans2quik.dll  bin\x64\Release /y
