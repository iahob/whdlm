@echo off
set /p CONFIG=����Ŀ¼:
set /p PROJECT=����Ŀ¼:
call python replace.py %CONFIG% %PROJECT%
pause