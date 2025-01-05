@echo off

set "vs_version=%~1"
if "%~1"=="" (set "vs_version=2022")

@echo on

call "init_submodules.bat"
call "projucer_pc.bat"