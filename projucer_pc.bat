call "setPcDevEnv.bat"

@echo off
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

@echo on

:: this replaces the call "c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
call "%vsdir%\Common7\Tools\VsDevCmd.bat"

echo Step 1 ok

:: restore the drive letter after VsDevCmd.bat sets it to C:
%de_projectdrive%
CD %de_projectdir%

echo step 2 ok

set res="2026"
if %vs_version%=="2017" set res="2017"
if %vs_version%=="2019" set res="2019"
if %vs_version%=="2022" set res="2022"
if %vs_version%=="18"   set res="2026"

echo Step 3 ok

pushd juce\extras\Projucer\Builds\VisualStudio%res%

echo Step 4 ok

:: build the projucer
msbuild Projucer.sln

:: run projucer with the plugin's jucer file
"x64/Debug/App/Projucer.exe" ..\..\..\..\..\%de_pluginname%.jucer
popd
