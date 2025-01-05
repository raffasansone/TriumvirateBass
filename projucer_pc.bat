call "setPcDevEnv.bat"

:: ******************************************
:: PARAMS TO CHANGE!
:: ******************************************

SET /p de_pluginname=<buildargs\plugin_name.txt
SET /p de_vendor=<buildargs\vendor_name.txt

:: ******************************************
:: END PARAMS TO CHANGE
:: ******************************************


:: store where we are
SET "de_projectdir=%cd%"
SET "de_projectdrive=%~d0"

:: this replaces the call "c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
call "%vsdir%\Common7\Tools\VsDevCmd.bat"

:: restore the drive letter after VsDevCmd.bat sets it to C:
%de_projectdrive%
CD %de_projectdir%


pushd juce\extras\Projucer\Builds\VisualStudio%vs_version%

:: build the projucer
msbuild Projucer.sln

:: run projucer with the plugin's jucer file
"x64/Debug/App/Projucer.exe" ..\..\..\..\..\%de_pluginname%.jucer
popd
