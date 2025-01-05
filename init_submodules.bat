@echo off

echo Initializing project dependencies...

if exist "submodules/" goto :end

:: fetching the dependencies as git submodules 
git submodule update --init --recursive

echo Initialization of dependencies completed.

@echo on