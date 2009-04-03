@echo off
rem ---------------------------------------------
rem Explanation of options. Modify these to fit
rem your UAVP.
rem ---------------------------------------------

rem Version of the board. May be 3_0 or 3_1.
set BOARD=3_1
rem Type of gyros in use. May be OPT_ADXRS300, OPT_ADXRS150, or...
set GYRO=OPT_ADXRS300
rem Type of ESC in use. May be ESC_PPM,  ESC_YGE, ESC_HOLGER, etc...
set ESC=ESC_PPM
rem Type of Rx. RX_DSM2 for Spektrum Rx, RX_PPM for serial PPM frame,
rem or RX_DEFAULT for default PPM Graupner/JR etc Rx
set RX=RX_DEFAULT
rem Type of debugging to use. May be DEBUG_MOTORS or DEBUG_SENSORS or NO_DEBUG.
set DBG=NO_DEBUG

rem ----------------------------------------------
rem Don't modify anything below this line.
rem ----------------------------------------------

rem To see why we do the setlocal, see:
rem http://www.robvanderwoude.com/variableexpansion.html
rem http://www.robvanderwoude.com/ntset.html
SETLOCAL ENABLEDELAYEDEXPANSION

rem Batch compiliert diverse Möglichkeiten der Ufo-Software
rem =======================================================
rem Schalter:
rem NOLEDGAME immer
rem USE_ACCSENS immer
rem OPT_ADXRS150 + OPT_ADXRS300 + OPT_IDG
rem ESC_PPM + ESC_HOLGER
rem BOARD_3_0 + BOARD_3_1
rem DEBUG_MOTORS (nur bei BOARD_3_1)
set CSRC=accel c-ufo irq lisl mathlib matrix pid pid2 prog sensor serial utils utils2
set ASRC=bootloader

set CEXE="%ProgramFiles%\microchip\cc5x\cc5x.exe"
set CCMD=-CC -p16F876 -I"%ProgramFiles%\microchip\cc5x" -a -L -Q -V -FM +reloc.inc -DMATHBANK_VARS=0 -DMATHBANK_PROG=2 -DBATCHMODE -DNOLEDGAME -DUSE_ACCSENS -X
set ACMD=/o+ /e+ /l+ /x- /p16F876 /c+ /q
set AEXE="%ProgramFiles%\microchip\MPASM Suite\MPASMwin.exe"
set LCMD=16f876i.lkr /aINHX8M
set LEXE="%ProgramFiles%\microchip\MPASM Suite\mplink.exe"

set ZIP="%ProgramFiles%\IZarc\IZarcC.exe" -a

rem We add mX to our firmware to indicate that it has been modified.
rem The X represents the version of the firmware.
set VS=3.15m3
set VG=3.09m3

rem Als erstes Testen ob cmd mit /v aufgerufen wurde
set F=x
for %%i in (a b) do set F=!F! %%i
if  "%F%" == "x b"  goto CMDERR

rem Fuer die Liste der erzeugten Pakete
set OF=

rem Das folgende wird 2x durchlaufen!

rem C_NEXT is used to say where to go after going to the cleanup step
set C_NEXT=STEP01

rem Remove the previously generated firmware. The CLEANUP label takes care
rem of the rest of the cleanup for us.
if exist profi-ufo*.hex del profi-ufo*.hex
goto CLEANUP

:STEP01
set NEXT=CLEANUP
set C_NEXT=ENDE
goto DOIT

:DOIT
echo.
echo.
echo.
echo.
echo NEXT= %NEXT%
echo.
for %%i in ( %CSRC% ) do call helper.bat CC5X %%i.c   -DBOARD_%BOARD% -D%GYRO% -D%ESC% -D%RX% -D%DBG% -D%RX%
for %%i in ( %ASRC% ) do call helper.bat ASM  %%i.asm /dBOARD_%BOARD%
set F=
for %%i in ( %CSRC% ) do set F=!F! %%i.o
for %%i in ( %ASRC% ) do set F=!F! %%i.o

rem =============================================
rem = set all the name tokens for the HEX files
rem =============================================
set G=
set E=
set V=
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
if "%DBG%"   == "DEBUG_SENSORS"     set D=SEN-
if "%RX%"    == "RX_PPM"            set R=RXCOM-
if "%RX%"    == "RX_DSM2"            set R=DSM2-

echo Linke Profi-Ufo-V%V%-%D%%G%%R%%E%
%LEXE% %LCMD% %F% /o Profi-Ufo-V%V%-%D%%G%%R%%E%.hex
if %ERRORLEVEL% == 1 goto ERROR
del Profi-Ufo-V%V%-%D%%T%%G%%R%%E%.cod
set OF=!OF! Profi-Ufo-V%V%-%D%%G%%R%%E%

goto %NEXT%

:SYNTAX
echo Fehler!
echo Aufruf mit MAKEUFO BBB GGG
echo wobei BBB die Versionsnummer der schwarzen und GGG der gruenen Platine ist!
goto ENDE

:CMDERR
echo Fehler!
echo CMD wurde nicht mit /V aufgerufen!

:CLEANUP
echo Deleting the C build related intemediate files...
for %%i in ( %CSRC% ) do if exist %%i.asm del %%i.asm
for %%i in ( %CSRC% ) do if exist %%i.err del %%i.err
for %%i in ( %CSRC% ) do if exist %%i.fcs del %%i.fcs
for %%i in ( %CSRC% ) do if exist %%i.lst del %%i.lst
for %%i in ( %CSRC% ) do if exist %%i.o   del %%i.o  
for %%i in ( %CSRC% ) do if exist %%i.occ del %%i.occ
for %%i in ( %CSRC% ) do if exist %%i.var del %%i.var

echo Deleting the ASM build related intermediate files...
for %%i in ( %ASRC% ) do if exist %%i.err del %%i.err
for %%i in ( %ASRC% ) do if exist %%i.lst del %%i.lst
for %%i in ( %ASRC% ) do if exist %%i.o   del %%i.o

echo Deleting the HEX build related intermediate files...
if exist profi-ufo*.cod del profi-ufo*.cod
if exist profi-ufo*.lst del profi-ufo*.lst
if exist profi-ufo*.map del profi-ufo*.map
goto %C_NEXT%

:ENDE
echo .
echo .
echo You have built the following firmware:
echo .
echo .
dir *.hex

:ERROR
rem Delete any partial hex files on error...
if %ERRORLEVEL% == 1 echo Error detected, deleting hex files.
if %ERRORLEVEL% == 1 if exist profi-ufo*.hex del profi-ufo*.hex
set C_NEXT=EOF
goto CLEANUP
:EOF
