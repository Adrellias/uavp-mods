@echo off
rem ---------------------------------------------
rem Delete all working files but not hex files
rem ---------------------------------------------

set CSRC=accel c-ufo irq lisl mathlib matrix pid pid2 prog sensor serial trig utils utils2
set ASRC=bootloader

rem compiler working files...
for %%i in ( %CSRC% ) do if exist %%i.asm del %%i.asm
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
if exist profi-ufo*.cod del profi-ufo*.cod
if exist profi-ufo*.lst del profi-ufo*.lst
if exist profi-ufo*.map del profi-ufo*.map


