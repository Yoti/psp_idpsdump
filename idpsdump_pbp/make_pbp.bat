@echo off
path=C:\pspsdk\bin
make
if exist EBOOT.PBP copy /b /y EBOOT.PBP ..\