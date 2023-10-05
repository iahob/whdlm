@echo off
set PARAM=%1

:ZipFile
echo *********************file copy*********************
RD /S /Q ..\client\ciphercode\client
RD /S /Q ..\client\ciphercode\game
RD /S /Q ..\client\ciphercode\base

md ..\client\ciphercode\game
md ..\client\ciphercode\client\res
md ..\client\ciphercode\base\res
xcopy /y /e ..\client\game ..\client\ciphercode\game
xcopy /y /e ..\client\client ..\client\ciphercode\client
xcopy /y /e ..\client\base ..\client\ciphercode\base
xcopy /y /e ..\client\command ..\client\ciphercode\command

xcopy /y /e  ..\client\client\src\privatemode ..\client\ciphercode\client\src\privatemode
xcopy /y /e  ..\client\client\src\plaza\models\match ..\client\ciphercode\client\src\plaza\models\match
xcopy /y /e  ..\client\client\src\gamematch ..\client\ciphercode\client\src\gamematch

PUSHD ..\info

echo *********************make FileCryTo*********************
call make_filecry.bat

echo *********************make md5*********************
call make_md5.bat
POPD

echo *********************file zip*********************
PUSHD ..\client\ciphercode
call WinRAR a -k -r -m1  base\res\client.zip client
if errorlevel 1 goto ZipError
for /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a in (..\..\info\game_list.txt) do (
	if %%j equ 1 (
		echo rar %%b
		call WinRAR a -k -r -m1  base\res\game.zip game\%%h
		if  errorlevel 1 goto ZipError
	)	
)
POPD
if errorlevel 0 goto FINISH
pause
exit

:ZipError
echo file zip error
pause
exit

:FINISH 
echo *********************zip finish*********************
if defined PARAM (echo finish) else pause
