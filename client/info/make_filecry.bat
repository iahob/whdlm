@echo off
REM http://www.jb51.net/article/17927.htm

rem client
FileCryTo ..\client\ciphercode

if  errorlevel 0 goto Finish
:OnError
echo make FileCryTo error
pause

:Finish
echo.
echo.
echo.
echo.