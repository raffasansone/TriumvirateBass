@echo off

:: store where we are
SET "de_projectdir=%cd%"
SET "de_projectdrive=%~d0"

:: find the folder for visual studio - sets vsdir variable
echo vs version requested: %vs_version%

if exist "%programfiles%\Microsoft Visual Studio\%vs_version%\Community" (
	echo Vs Community detected
	SET "vsdir=%programfiles%\Microsoft Visual Studio\%vs_version%\Community"
) else (
	if exist "%programfiles%\Microsoft Visual Studio\%vs_version%\Professional" (
		echo Vs Professional detected
		SET "vsdir=%programfiles%\Microsoft Visual Studio\%vs_version%\Professional"
	)else (
		if exist "%programfiles%\Microsoft Visual Studio\%vs_version%\Enterprise" (
			echo Vs enterprise detected	
			SET "vsdir=%programfiles%\Microsoft Visual Studio\%vs_version%\Enterprise"
		) else (
			echo "WARNING - No edition of Visual Studio %vs_version% was detected, is it installed?"
		)
	)
)

:: restore the drive letter after VsDevCmd.bat sets it to C:
%de_projectdrive%
CD %de_projectdir%

@echo on
