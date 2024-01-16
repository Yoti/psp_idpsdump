@echo off
path=C:\pspsdk\bin
psp-build-exports -s regedit_exp.exp
make
if exist regedit.prx copy /b /y regedit.prx ..\