@echo off
rem To see why we do the setlocal, see:
rem http://www.robvanderwoude.com/variableexpansion.html
rem http://www.robvanderwoude.com/ntset.html
SETLOCAL ENABLEDELAYEDEXPANSION

rem Helper script for makeall.bat
rem =======================================================
rem parameters passed are:
set 	VERSION=%1
set 	GYRO=%2
set 	ESC=%3
set 	DBG=%4
set 	RX=%5
set 	CFG=%6

set CSRC=UAVX adc autonomous compass_altimeter control gps i2c irq outputs serial spi utils
set ASRC=bootl18f2520

set CC="C:\MCC18\bin\mcc18"
set CCMD=  -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-  -DBATCHMODE -DNOLEDGAME 

set ACMD=/o+ /e+ /l+ /x- /p18f2520 /c+ /q
set AEXE="%ProgramFiles%\microchip\MPASM Suite\MPASMwin.exe"

set LCMD=/p18F2520 /l"C:\MCC18\lib" /k"C:\MCC18\lkr"
set LEXE="C:\MCC18\bin\mplink.exe"

rem Set all the name tokens for the HEX files
set G=
set E=
set D=
set T=
set R=
set B=
set C=
if "%GYRO%"  == "OPT_ADXRS300"      set G=ADX300-
if "%GYRO%"  == "OPT_ADXRS150"      set G=ADX150-
if "%GYRO%"  == "OPT_IDG"           set G=IDG-
if "%ESC%"   == "ESC_PPM"           set E=PPM
if "%ESC%"   == "ESC_HOLGER"        set E=HOL
if "%ESC%"   == "ESC_X3D"           set E=X3D
if "%ESC%"   == "ESC_YGEI2C"        set E=YGE
if "%DBG%"   == "DEBUG_MOTORS"      set D=Debug_MOTORS-
if "%DBG%"   == "DEBUG_SENSORS"     set D=Debug_SENSORS-
if "%RX%"    == "RX_PPM"            set R=RXCOM-
if "%RX%"    == "RX_DSM2"           set R=DSM2-
if "%CFG%"    == "TRICOPTER"           set C=TRI-

rem Build the list of expected object files
set F=
for %%i in ( %CSRC% ) do set F=!F! %%i.o
for %%i in ( %ASRC% ) do set F=!F! %%i.o

rem The warnings etc. previously directed to NUL have been reinstated to log.lst. These 
rem include a number associated with argument passing other than by function parameters to
rem the mathematics module.
rem The local variable offset -ro1 is to overcome aliasing of variables caused by cc5x!
rem As a consequence there are several warnings on bank allocation in the compile.

for %%i in ( %CSRC% ) do %CC% -p=18F2520 /i"C:\MCC18\h" %%i.c -fo=%%i.o %CCMD% -D%GYRO% -D%ESC% -D%DBG% -D%RX% -D%CFG% >> log.lst

for %%i in ( %ASRC% ) do %AEXE%  %%i.asm %ACMD% >> log.lst

%LEXE% %LCMD% %F% /u_CRUNTIME /z__MPLAB_BUILD=1 /W /o UAVP-B3_1-V%VERSION%-%C%%D%%T%%G%%R%%E%.hex >> log.lst 


if %ERRORLEVEL% == 1 goto FAILED

echo compiled - UAVX-V%VERSION%-%C%%D%%T%%G%%R%%E%.hex
echo compiled - UAVX-V%VERSION%-%C%%D%%T%%G%%R%%E%.hex >> gen.lst
call makeclean.bat
goto FINISH

:FAILED
echo failed - UAVX-V%VERSION%-%C%%D%%T%%G%%R%%E%.hex
echo failed - UAVX-V%VERSION%-%C%%D%%T%%G%%R%%E%.hex >> gen.lst
rem don't delete working files

:FINISH








