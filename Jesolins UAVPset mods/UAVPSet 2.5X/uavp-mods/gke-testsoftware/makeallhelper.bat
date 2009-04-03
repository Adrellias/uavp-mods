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

set	BOARD=3_1

set CSRC=pu-test an-test irq lisl mathlib rxtest serial text utils utils2 i2c-scan output
set ASRC=bootloader

set CEXE="%ProgramFiles%\microchip\cc5x\cc5x.exe"
set CCMD=-CC -p16F876 -I"%ProgramFiles%\microchip\cc5x" -a -L -Q -V -FM +reloc.inc -DMATHBANK_PROG=2 -DBATCHMODE -DNOLEDGAME -DUSE_ACCSENS -X
set ACMD=/o+ /e+ /l+ /x- /p16F876 /c+ /q
set AEXE="%ProgramFiles%\microchip\MPASM Suite\MPASMwin.exe"
set LCMD=16f876i.lkr /aINHX8M
set LEXE="%ProgramFiles%\microchip\MPASM Suite\mplink.exe"

rem Set all the name tokens for the HEX files
set G=
set E=
set D=
set T=
set R=
set B=
if "%GYRO%"  == "OPT_ADXRS"      set G=ADX-
if "%GYRO%"  == "OPT_IDG"           set G=IDG-
if "%ESC%"   == "ESC_PPM"           set E=PPM
if "%ESC%"   == "ESC_HOLGER"        set E=HOL
if "%ESC%"   == "ESC_YGEI2C"        set E=YGE
if "%RX%"    == "RX_PPM"            set R=RXCOM-
if "%RX%"    == "RX_DSM2"           set R=DSM2-

rem Build the list of expected object files
set F=
for %%i in ( %CSRC% ) do set F=!F! %%i.o
for %%i in ( %ASRC% ) do set F=!F! %%i.o

rem The warnings etc. previously directed to NUL have been reinstated to log.lst. These 
rem include a number associated with argument passing other than by function parameters to
rem the mathematics module.
rem The local variable offset -ro1 is to overcome aliasing of variables caused by cc5x!
rem As a consequence there are several warnings on bank allocation in the compile.

for %%i in ( %CSRC% ) do (
    set OFFSET=
    if %%i == text set OFFSET=-ro3
    if %%i == output set OFFSET=-ro4
    %CEXE% %%i.c  %CCMD% -DBOARD_%BOARD% -D%GYRO% -D%ESC% -D%DBG% -D%RX% !OFFSET! >> log.lst
)

for %%i in ( %ASRC% ) do %AEXE%  %%i.asm %ACMD% /dBOARD_%BOARD% >> log.lst

%LEXE% %LCMD% %F% /o TestSoftware%BOARD%-V%VERSION%-%D%%T%%G%%R%%E%.hex >> log.lst 


if %ERRORLEVEL% == 1 goto FAILED

echo compiled - TestSoftware-B%BOARD%-V%VERSION%-%D%%T%%G%%R%%E%.hex
echo compiled - TestSoftware-B%BOARD%-V%VERSION%-%D%%T%%G%%R%%E%.hex >> gen.lst
call makeclean.bat
goto FINISH

:FAILED
echo failed - TestSoftware-B%BOARD%-V%VERSION%-%D%%T%%G%%R%%E%.hex
echo failed - TestSoftware-B%BOARD%-V%VERSION%-%D%%T%%G%%R%%E%.hex >> gen.lst
rem don't delete working files

:FINISH








