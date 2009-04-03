@echo off
rem Batch compiliert diverse Möglichkeiten der Ufo-Software
rem =======================================================
rem Schalter:
rem NOLEDGAME immer
rem USE_ACCSENS immer
rem TRICOPTER
rem OPT_ADXRS150 + OPT_ADXRS300 + OPT_IDG
rem ESC_PPM + ESC_HOLGER
rem BOARD_3_1
rem DEBUG_MOTORS (nur bei BOARD_3_1)

set CSRC=accel c-ufo irq lisl mathlib matrix pid pid2 prog sensor serial utils utils2
set ASRC=bootloader

set CEXE="%ProgramFiles%\microchip\cc5x\cc5x.exe"
set CCMD=-CC -p16F876 -I"%ProgramFiles%\microchip\cc5x" -a -L -Q -V -FM +reloc.inc -DMATHBANK_VARS=0 -DMATHBANK_PROG=2 -DBATCHMODE -DNOLEDGAME -DUSE_ACCSENS -DTRICOPTER -X
set ACMD=/o+ /e+ /l+ /x- /p16F876 /c+ /q
set AEXE="%ProgramFiles%\microchip\MPASM Suite\MPASMwin.exe"
set LCMD=16f876i.lkr /aINHX8M
set LEXE="%ProgramFiles%\microchip\MPASM Suite\mplink.exe"

set ZIP="%ProgramFiles%\IZarc\IZarcC.exe" -a

rem Testen ob die Versionsnummern da sind
if "%1" == "" goto SYNTAX
set VS=%1

rem Als erstes Testen ob cmd mit /v aufgerufen wurde
set F=x
for %%i in (a b) do set F=!F! %%i
if  "%F%" == "x b"  goto CMDERR

rem Fuer die Liste der erzeugten Pakete
set OF=

echo Loeschen der C-Ausgabe-Dateien...
for %%i in ( %CSRC% ) do if exist %%i.asm del %%i.asm
for %%i in ( %CSRC% ) do if exist %%i.err del %%i.err
for %%i in ( %CSRC% ) do if exist %%i.fcs del %%i.fcs
for %%i in ( %CSRC% ) do if exist %%i.lst del %%i.lst
for %%i in ( %CSRC% ) do if exist %%i.o   del %%i.o  
for %%i in ( %CSRC% ) do if exist %%i.occ del %%i.occ
for %%i in ( %CSRC% ) do if exist %%i.var del %%i.var

echo Loeschen der ASM-Ausgabe-Dateien...
for %%i in ( %ASRC% ) do if exist %%i.err del %%i.err
for %%i in ( %ASRC% ) do if exist %%i.lst del %%i.lst
for %%i in ( %ASRC% ) do if exist %%i.o   del %%i.o

echo Loeschen von HEX-Files...
if exist tricopter-ufo.cod del tricopter-ufo.cod
if exist tricopter-ufo.hex del tricopter-ufo.hex
if exist tricopter-ufo.lst del tricopter-ufo.lst
if exist tricopter-ufo.map del tricopter-ufo.map

rem Das folgende wird 2x durchlaufen!

SET CAM=CAM_0_DEG
SET RX=
set BOARD=3_1

:STEP01
set NEXT=STEP02
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS300
set ESC=ESC_PPM
set DBG=NODEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP02
set NEXT=STEP03
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS300
set ESC=ESC_PPM
set DBG=NODEBUG_MOTORS
set THC=USE_THROTTLECURVE
goto DOIT

:STEP03
set NEXT=STEP04
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS300
set ESC=ESC_PPM
set DBG=DEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP04
:STEP05
:STEP06
set NEXT=STEP07
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS150
set ESC=ESC_PPM
SET DBG=DEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP07
set NEXT=STEP08
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS150
set ESC=ESC_PPM
SET DBG=NODEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP08
set NEXT=STEP09
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS150
set ESC=ESC_PPM
SET DBG=NODEBUG_MOTORS
set THC=USE_THROTTLECURVE
goto DOIT

:STEP09
:STEP10
:STEP11
set NEXT=STEP12
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_IDG
set ESC=ESC_PPM
SET DBG=NODEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP12
set NEXT=STEP13
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_IDG
set ESC=ESC_PPM
SET DBG=NODEBUG_MOTORS
set THC=USE_THROTTLECURVE
goto DOIT

:STEP13
set NEXT=STEP14
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_IDG
set ESC=ESC_PPM
SET DBG=DEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP14
:STEP15
:STEP16
set NEXT=STEP17
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS300
set ESC=ESC_HOLGER
SET DBG=NODEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP17
set NEXT=STEP18
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS300
set ESC=ESC_HOLGER
SET DBG=NODEBUG_MOTORS
set THC=USE_THROTTLECURVE
goto DOIT

:STEP18
set NEXT=STEP19
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS300
set ESC=ESC_HOLGER
SET DBG=DEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP19
set NEXT=STEP20
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS150
set ESC=ESC_HOLGER
SET DBG=NODEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP20
set NEXT=STEP21
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS150
set ESC=ESC_HOLGER
SET DBG=NODEBUG_MOTORS
set THC=USE_THROTTLECURVE
goto DOIT

:STEP21
set NEXT=STEP22
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS150
set ESC=ESC_HOLGER
SET DBG=DEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP22
set NEXT=STEP23
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_IDG
set ESC=ESC_HOLGER
SET DBG=NODEBUG_MOTORS
set THC=USE_THROTTLECURVE
goto DOIT

:STEP23
set NEXT=STEP24
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_IDG
set ESC=ESC_HOLGER
SET DBG=NODEBUG_MOTORS
set THC=THROTTLECURVE
goto DOIT

:STEP24
set NEXT=STEP25
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_IDG
set ESC=ESC_HOLGER
SET DBG=DEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP25
set NEXT=STEP26
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS300
set ESC=ESC_YGEI2C
SET DBG=NODEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP26
set NEXT=STEP27
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS300
set ESC=ESC_YGEI2C
SET DBG=NODEBUG_MOTORS
set THC=USE_THROTTLECURVE
goto DOIT

:STEP27
set NEXT=STEP28
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS300
set ESC=ESC_YGEI2C
SET DBG=DEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP28
set NEXT=STEP29
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS150
set ESC=ESC_YGEI2C
SET DBG=NODEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP29
set NEXT=STEP30
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS150
set ESC=ESC_YGEI2C
SET DBG=NODEBUG_MOTORS
set THC=USE_THROTTLECURVE
goto DOIT

:STEP30
set NEXT=STEP31
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_ADXRS150
set ESC=ESC_YGEI2C
SET DBG=DEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP31
set NEXT=STEP32
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_IDG
set ESC=ESC_YGEI2C
SET DBG=NODEBUG_MOTORS
set THC=USE_THROTTLECURVE
goto DOIT

:STEP32
set NEXT=STEP33
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_IDG
set ESC=ESC_YGEI2C
SET DBG=NODEBUG_MOTORS
set THC=THROTTLECURVE
goto DOIT

:STEP33
set NEXT=STEP34
rem =============================================
rem C-Compiling, assembling and linking a version
rem =============================================

set GYRO=OPT_IDG
set ESC=ESC_YGEI2C
SET DBG=DEBUG_MOTORS
set THC=NO_THROTTLECURVE
goto DOIT

:STEP34
:STEP35
if "%RX%" == "RX_PPM" goto LAST
set RX=RX_PPM
SET CAM=CAM_0_DEG
goto STEP01

:DOIT
echo.
echo.
echo.
echo.
echo NEXT= %NEXT%
echo.
for %%i in ( %CSRC% ) do call makeufo2.bat CC5X %%i.c   -DBOARD_%BOARD% -D%GYRO% -D%ESC% -D%DBG% -D%THC% -D%CAM% -D%RX%
for %%i in ( %ASRC% ) do call makeufo2.bat ASM  %%i.asm /dBOARD_%BOARD%
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
rem if "%BOARD%" == "3_0"	            set V=%VG%
if "%DBG%"   == "DEBUG_MOTORS"      set D=DBG-
if "%THC%"   == "USE_THROTTLECURVE" set T=THC-
if "%CAM%"   == "CAM_45_DEG"        set C=CAM45-
if "%RX%"    == "RX_PPM"            set R=RXCOM-
if "%RX%"    == "RX_AR7000"            set R=AR7000-

echo Linke Tricopter-Ufo-V%V%-%D%%T%%C%%G%%R%%E%
%LEXE% %LCMD% %F% /o Tricopter-Ufo-V%V%-%D%%T%%C%%G%%R%%E%.hex
if %ERRORLEVEL% == 1 goto ENDE
del Tricopter-Ufo-V%V%-%D%%T%%C%%G%%R%%E%.cod
set OF=!OF! Tricopter-Ufo-V%V%-%D%%T%%C%%G%%R%%E%

goto %NEXT%

:LAST
echo ALLE PAKETE ERFOLGREICH ERZEUGT!!!!
for %%i in (%OF%) do echo %%i

echo Erzeuge Archive...
set ADDF=Release\_HISTORY.txt Release\_VERSIONS.txt Release\gpl-*.txt
echo %ZIP% .\Tricopter-Ufo-V%VS%.zip Tricopter-Ufo-V%VS%-*.hex %ADDF%
rem %ZIP% .\Profi-Ufo-V%VG%.zip Profi-Ufo-V%VG%-*.hex %ADDF%
%ZIP% .\Tricopter-Ufo-V%VS%.zip Tricopter-Ufo-V%VS%-*.hex %ADDF%

echo Fertig!
dir Tricopter-Ufo-V%VS%.zip

echo Loesche HEX und LST files
rem del Tricopter-Ufo-V%VG%-*.hex
rem del Tricopter-Ufo-V%VG%-*.lst
del Tricopter-Ufo-V%VS%-*.hex
del Tricopter-Ufo-V%VS%-*.lst

goto ENDE

:SYNTAX
echo Fehler!
echo Aufruf mit MAKETRI BBB
echo wobei BBB die Versionsnummer der schwarzen Platine ist!
goto ENDE

:CMDERR
echo Fehler!
echo CMD wurde nicht mit /V aufgerufen!

:ENDE

