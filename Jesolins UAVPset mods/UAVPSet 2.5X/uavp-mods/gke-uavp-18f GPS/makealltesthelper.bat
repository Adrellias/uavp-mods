@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

set PIC=18F2520

rem Helper script for makeall.bat
rem =======================================================
rem parameters passed are:
set 	VERSION=%1
set	CLOCK=%2
set	PROC=%3
set 	GYRO=%4
set 	ESC=%5
set 	RX=%6

set CSRC=adc irq lisl prog pu-test sensor serial menu-test utils gps autonomous
set ASRC=bootl18f

set CC="C:\MCC18\bin\mcc18"
rem set CCMD=   -DBATCHMODE -DTEST_SOFTWARE 
set CCMD=  -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa- -DBATCHMODE -DTEST_SOFTWARE 

set ACMD=/q /d%CLOCK% /p%PROC% %%i.asm /l%%i.lst /e%%i.err /o%%i.o
set AEXE="C:\MCC18\mpasm\mpasmwin.exe"

set LCMD=/p%PROC% /l"C:\MCC18\lib" /k"C:\MCC18\lkr"
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
if "%CLOCK%"    == "CLOCK_16MHZ"           set X=_16
if "%CLOCK%"    == "CLOCK_40MHZ"           set X=_40

rem Build the list of expected object files
set F=
for %%i in ( %CSRC% ) do set F=!F! %%i.o
for %%i in ( %ASRC% ) do set F=!F! %%i.o

rem The warnings etc. previously directed to NUL have been reinstated to log.lst. These 
rem include a number associated with argument passing other than by function parameters to
rem the mathematics module.
rem The local variable offset -ro1 is to overcome aliasing of variables caused by cc5x!
rem As a consequence there are several warnings on bank allocation in the compile.

for %%i in ( %CSRC% ) do %CC% -p=%PROC% /i"C:\MCC18\h" %%i.c -fo=%%i.o %CCMD% -D%CLOCK% -D%GYRO% -D%ESC% -D%DBG% -D%RX% -D%CFG% >> log.lst

for %%i in ( %ASRC% ) do %AEXE%  %ACMD% >> log.lst

%LEXE% %LCMD% %F% /u_CRUNTIME /z__MPLAB_BUILD=1 /W /o UAVPTest-V%VERSION%%PROC%%X%-%T%%G%%R%%E%.hex >> log.lst 


if %ERRORLEVEL% == 1 goto FAILED

echo compiled - UAVPTest-V%VERSION%%PROC%%X%-%D%%T%%G%%R%%E%.hex
echo compiled - UAVPTest-V%VERSION%%PROC%%X%-%D%%T%%G%%R%%E%.hex >> gen.lst
call makeclean.bat
goto FINISH

:FAILED
echo failed - UAVPTest-V%VERSION%%PROC%%X%-%T%%G%%R%%E%.hex
echo failed - UAVPTest-V%VERSION%%PROC%%X%-%T%%G%%R%%E%.hex >> gen.lst
rem don't delete working files

:FINISH








