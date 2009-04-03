@echo off
rem ---------------------------------------------
rem Delete all working files but not hex files
rem ---------------------------------------------

set CSRC=accel adc c-ufo irq lisl prog pu-test sensor serial menu-test menu pid utils outputs
set ASRC=bootl18f

rem compiler working files...
if exist *.mcs del *.mcs
if exist *.cof del *.cof

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
if exist general.err del general.err

rem linker working files...
if exist *.cod del *.cod
rem if exist *.lst del *.lst
if exist *.map del *.map



