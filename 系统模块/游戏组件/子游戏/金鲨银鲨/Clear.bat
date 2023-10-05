@echo Off 
del /s /a *.suo *.ncb *.user *.pdb *.netmodule *.aps *.ilk *.opt *.plg *.old Thumbs.db *.bak *.exp *.sdf *.htm 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "%%d\x64" 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "%%d\Debug" 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "%%d\Debug_Unicode" 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "%%d\Release" 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "%%d\Release_Unicode" 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "%%d\Release_Manager" 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "%%d\Bin" 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "%%d\Obj" 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "%%d\_UpgradeReport_Files" 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "Backup" 2>nul 
FOR /R . %%d IN (.) DO rd /s /q "ipch" 2>nul 

rem If the Properties directory is empty, remove it 
FOR /R . %%d in (.) do rd /q "%%d\Properties" 2> nul 
