@echo off
REM 派生项目拷贝脚本

set /p DST=派生目录:

rem 拷贝路径
if not exist "%DST%" ( mkdir %DST%)
if errorlevel 1 goto Finish

rem 派生游戏
set /p CONFIG=配置目录:
if errorlevel 1 goto Finish

rem cocostudio编辑目录:
set COCOSTUDIO=F:\ry_cocostudio\cocostudio
if not exist "%COCOSTUDIO%" (
	echo.
	echo.
	echo "请配置cocostudio编辑器工程目录!"
	echo.
	echo.
	pause
	exit
)
if errorlevel 1 goto Finish

echo *********************file copy start*********************
rem copy cocostudio
if not exist "%DST%\cocostudio" (
	mkdir %DST%\cocostudio
	if not exist "%DST%\cocostudio\Ry_plazz" (
		mkdir %DST%\cocostudio\Ry_plazz
		xcopy /y /e /EXCLUDE:ignore.txt %COCOSTUDIO%\Ry_plazz %DST%\cocostudio\Ry_plazz
	)
	
	for /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a in (%CONFIG%\game_list.txt) do (
		if exist "%COCOSTUDIO%\Ry_game\%%h" (
			echo copy %%b
			if not exist "%DST%\cocostudio\Ry_game\%%h" ( 
				mkdir %DST%\cocostudio\Ry_game\%%h
				xcopy /y /e /EXCLUDE:ignore.txt %COCOSTUDIO%\Ry_game\%%h %DST%\cocostudio\Ry_game\%%h
				if  errorlevel 1 goto Error	
			)
		)
	)
)

rem copy client
if not exist "%DST%\client\base" ( 
	mkdir %DST%\client\base
	xcopy /y /e /EXCLUDE:ignore.txt ..\client\base %DST%\client\base
)

if not exist "%DST%\client\client" ( 
	mkdir %DST%\client\client
	xcopy /y /e /EXCLUDE:ignore.txt ..\client\client %DST%\client\client
)

if not exist "%DST%\client\command" ( 
	mkdir %DST%\client\command
	xcopy /y /e /EXCLUDE:ignore.txt ..\client\command %DST%\client\command
)

if not exist "%DST%\client\game" ( mkdir %DST%\client\game)

for /f "skip=1 tokens=1,2,3,4,5,6,7,8,9,10 delims==," %%a in (%CONFIG%\game_list.txt) do (
	echo copy %%b
	if not exist "%DST%\%%f" ( 
		mkdir %DST%\%%f		
		xcopy /y /e /EXCLUDE:ignore.txt ..\%%f %DST%\%%f
		if  errorlevel 1 goto Error	
	)	
)

rem copy framework
if not exist "%DST%\frameworks\runtime-src" ( 
	mkdir %DST%\frameworks\runtime-src
	xcopy /y /e /EXCLUDE:ignore.txt ..\frameworks\runtime-src %DST%\frameworks\runtime-src
)

if not exist "%DST%\frameworks\cocos2d-x" ( 
	mkdir %DST%\frameworks\cocos2d-x
	xcopy /y /e /EXCLUDE:ignore.txt ..\frameworks\cocos2d-x %DST%\frameworks\cocos2d-x
)

rem copy proj
copy ..\frameworks\.cocos-project.json %DST%\frameworks
copy ..\frameworks\.project %DST%\frameworks

rem copy info
if not exist "%DST%\info" ( mkdir %DST%\info)
xcopy /y /e /EXCLUDE:ignore.txt ..\info %DST%\info

rem copy game_list
copy %CONFIG%\game_list.txt %DST%\info

rem copy android project.properties
copy project.properties %DST%\frameworks\runtime-src\proj.android\

rem copy icon
xcopy /y /e %CONFIG%\icon\ios %DST%\frameworks\runtime-src\proj.ios_mac\ios
xcopy /y /e %CONFIG%\icon\android %DST%\frameworks\runtime-src\proj.android\res

rem copy logo
copy %CONFIG%\logon_logo.png %DST%\client\client\res\Logon
copy %CONFIG%\logo_name_00.png %DST%\client\base\res
copy %CONFIG%\logo_text_00.png %DST%\client\base\res

rem info replace
call python replace.py %CONFIG% %DST%

if errorlevel 1 goto Error
if errorlevel 0 goto Finish

:Error
echo copy error
pause

:Finish
echo *********************file copy finish*********************
pause