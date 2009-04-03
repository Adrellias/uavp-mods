@echo off
rem ---------------------------------------------
rem Delete all working files but not hex files
rem ---------------------------------------------

set CSRC=accel adc UAVX irq compass_altimeter serial utils i2c spi my_math control autonomous outputs gps
set ASRC=bootl18f2520

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


