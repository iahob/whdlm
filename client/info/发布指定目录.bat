@echo off

echo.
echo ��ָ��Ŀ¼�Ͻ���������,��: ������λ��client/clientĿ¼, ��Ϸ��λ��game/qipai/landĿ¼
echo.

set /p SRC=Ŀ¼:
if not exist "%SRC%" (
	echo ������Ϸ���Ŀ¼!
	pause.
	exit
)
rem ��ȡ��ǰĿ¼��
set curdir=""
for %%i in ("%SRC%") do (
	set curdir=%%~ni
)
set h=%time:~0,2%
set h=%h: =0%
set folder=..\client_publish\%date:~0,4%-%date:~5,2%-%date:~8,2%-%h%%time:~3,2%\%curdir%
if not exist "%folder%" (
	mkdir %folder%
)

:MakeMD5
echo ����MD5�����ļ�
xcopy /y /e %SRC% %folder%
FileCryTo %folder%
MakeMD5List -dst %temp% -src %folder%
copy %temp%\filemd5List.json %folder%\res\filemd5List.json
del %temp%\filemd5List.json

echo �����ļ��Ѹ��Ƶ�%folder%Ŀ¼
pause