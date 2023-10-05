
REM /////////////////////////////////////////////////////////////
REM	Android_Build_BAT 2016-03-15 By.CP
REM /////////////////////////////////////////////////////////////

:ZipFile

echo *********************file zip*********************

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

echo *********************make FileCryTo*********************
call make_filecry.bat

echo *********************make md5*********************
call make_md5.bat

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
if errorlevel 0 goto Compile
pause
exit

:ZipError
echo file zip error
pause
exit

:Compile
PUSHD ..\
echo *********************cocos compile android*********************

copy /y frameworks\.cocos-project.json .\
copy /y frameworks\.project .\

call  cocos compile -p android --ap android-19 -m debug -j 4 --compile-script 0
POPD
if  errorlevel 1 goto CompileError
if  errorlevel 0 goto FinshiMove
pause
exit

:CompileError
echo *********************cocos compile fail*********************
del /p .cocos-project.json
del /p .project
pause
exit

:FinshiMove
echo *********************file moveto runpath*********************
xcopy /y /e ..\simulator\android\*.apk ..\run\debug\android\
RD /S /Q simulator

if  errorlevel 1 goto FinshiMoveError
if  errorlevel 0 goto FINISH
pause
exit

:FinshiMoveError
echo *********************file moveto fail*********************
del /p .cocos-project.json
del /p .project
pause
exit

:FINISH 
echo *********************FINISH*********************
rem del /f .cocos-project.json
rem del /f .project
