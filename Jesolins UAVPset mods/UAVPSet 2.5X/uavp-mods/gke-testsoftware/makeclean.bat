@echo off
rem ---------------------------------------------
rem Delete all working files but not hex files
rem ---------------------------------------------

set CSRC=pu-test an-test irq lisl mathlib rxtest serial text utils utils2 i2c-scan output
set ASRC=bootloader

rem compiler working files...
for %%i in ( %CSRC% ) do if exist %%i.asm del %%i.asm
for %%i in ( %CSRC% ) do if exist %%i.asm del %%i.cod
for %%i in ( %CSRC% ) do if exist %%i.err del %%i.err
for %%i in ( %CSRC% ) do if exist %%i.fcs del %%i.fcs
for %%i in ( %CSRC% ) do if exist %%i.lst del %%i.lst
for %%i in ( %CSRC% ) do if exist %%i.o   del %%i.o  
for %%i in ( %CSRC% ) do if exist %%i.occ del %%i.occ
for %%i in ( %CSRC% ) do if exist %%i.var del %%i.var

rem assembler working files...
for %%i in ( %ASRC% ) do if exist %%i.err del %%i.err
for %%i in ( %ASRC% ) do if exist %%i.lst del %%i.lst
for %%i in ( %ASRC% ) do if exist %%i.o   del %%i.o

rem linker working files...
if exist TestSoftware*.cod del TestSoftware*.cod
if exist TestSoftware*.lst del TestSoftware*.lst
if exist TestSoftware*.map del TestSoftware*.map


