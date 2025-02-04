@echo off
git submodule update --init --recursive
premake5 --file=sln_structure.lua vs2022
pause