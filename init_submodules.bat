@echo off

echo Initializing project dependencies...

:: fetching the dependencies as git submodules 
git submodule update --init --recursive

echo Initialization of dependencies completed.

@echo on