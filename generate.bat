@echo off
echo Updating submodules...
call git submodule update --init --recursive
call premake5 %* vs2022
pause