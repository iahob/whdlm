@echo off
"%XS_INSTDIR%\x-studio.exe" -c -enc -cfg=encryptPNG.xml -ft=*.png;*.csb -i=..\client2 -o=..\clientResEncrypt
pause