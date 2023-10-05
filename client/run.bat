@echo off
set WORKDIR=%CD%/client
set game=%CD%/run/debug/win32/GloryProject.exe
start %game% -workdir %WORKDIR% -FILE %WORKDIR%/base/src/main.lua
exit
