@echo off
rem To see why we do the setlocal, see:
rem http://www.robvanderwoude.com/variableexpansion.html
rem http://www.robvanderwoude.com/ntset.html
SETLOCAL ENABLEDELAYEDEXPANSION

rem Helper script for makeall.bat
rem =======================================================
rem parameters passed are:
set 	VERSION=%1
set	BOARD=%2
set 	GYRO=%3
set 	ESC=%4
set 	DBG=%5
set 	THC=%6
set 	CAM=%7
set 	RX=%8

set CSRC=accel c-ufo irq lisl mathlib matrix pid pid2 prog sensor serial utils utils2
set ASRC=bootloader

set CEXE="%ProgramFiles%\microchip\cc5x\cc5x.exe"
set CCMD=-CC -p16F876 -I"%ProgramFiles%\microchip\cc5x" -a -L -Q -V -FM +reloc.inc -DMATHBANK_VARS=0 -DMATHBANK_PROG=2 -DBATCHMODE -DNOLEDGAME -DUSE_ACCSENS -X
set ACMD=/o+ /e+ /l+ /x- /p16F876 /c+ /q
set AEXE="%ProgramFiles%\microchip\MPASM Suite\MPASMwin.exe"
set LCMD=16f876i.lkr /aINHX8M
set LEXE="%ProgramFiles%\microchip\MPASM Suite\mplink.exe"

echo Clean up all of the intermediate files
call makeclean.bat

rem Set all the name tokens for the HEX files
set G=
set E=
set D=
set T=
set C=CAM0-
set R=
if "%GYRO%"  == "OPT_ADXRS300"      set G=ADX300-
if "%GYRO%"  == "OPT_ADXRS150"      set G=ADX150-
if "%GYRO%"  == "OPT_IDG"           set G=IDG-
if "%ESC%"   == "ESC_PPM"           set E=PPM
if "%ESC%"   == "ESC_HOLGER"        set E=HOL
if "%ESC%"   == "ESC_X3D"           set E=X3D
if "%ESC%"   == "ESC_YGEI2C"        set E=YGE
if "%BOARD%" == "3_1"	            set V=%VS%
if "%BOARD%" == "3_0"	            set V=%VG%
if "%DBG%"   == "DEBUG_MOTORS"      set D=DBG-
if "%THC%"   == "USE_THROTTLECURVE" set T=THC-
if "%CAM%"   == "CAM_45_DEG"        set C=CAM45-
if "%RX%"    == "RX_PPM"            set R=RXCOM-
if "%RX%"    == "RX_AR7000"            set R=AR7000-

rem Build the list of expected object files
set F=
for %%i in ( %CSRC% ) do set F=!F! %%i.o
for %%i in ( %ASRC% ) do set F=!F! %%i.o

echo The warnings etc. previously directed to NUL have been reinstated. These include
echo a number associated with argument passing other than by function parameters to
echo the mathematics module.
echo Wolfgang has used a local variable offset -ro1 for sensor.c only. No idea why?
echo As a consequence there are several warnings on bank allocation in the compile.

for %%i in ( %CSRC% ) do %CEXE% %%i.c  %CCMD% -DBOARD_%BOARD% -D%GYRO% -D%ESC% -D%DBG% -D%THC% -D%CAM% -D%RX%

rem recompiling sensor.c with -r01 to avoid the use of a separate batch file with conditionals.
%CEXE% sensor.c  %CCMD% -DBOARD_%BOARD% -D%GYRO% -D%ESC% -D%DBG% -D%THC% -D%CAM% -D%RX% -ro1

for %%i in ( %ASRC% ) do %AEXE%  %%i.asm %ACMD% /dBOARD_%BOARD% 

%LEXE% %LCMD% %F% /o Profi-Ufo-V%VERSION%-%D%%T%%C%%G%%R%%E%.hex

if %ERRORLEVEL% == 1 goto FINISH

call makeclean.bat

:FINISH








