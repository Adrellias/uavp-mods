@echo off
rem To see why we do the setlocal, see:
rem http://www.robvanderwoude.com/variableexpansion.html
rem http://www.robvanderwoude.com/ntset.html
SETLOCAL ENABLEDELAYEDEXPANSION

rem Batch compiles various possibilities of the UFO software for the Microchip 18F2xxx series
rem
rem Greg Egan 2008-2009
rem
rem Uses: makeallhelper.bat and makeclean.bat
rem
rem Clock rate CLOCK_16MHZ (Only 16MHZ available for UAVP version)
rem Type of PIC processor 18F2620 and 18F2520
rem Type of gyros in use. May be OPT_ADXRS300, OPT_ADXRS150, or OPT_IDG
rem Type of ESC in use. May be ESC_PPM,  ESC_YGEI2C, ESC_HOLGER.
rem Type of Rx. May be RX_DSM2 for for reference DX7/AR7000 combination, 
rem RX_DSM2 for AR7000, RX_PPM for serial PPM frame, or RX_DEFAULT for default PPM Graupner/JR etc Rx.
rem DEBUG_SENSORS to generate trace files of all main program and sensor values which can be plotted
rem using UAVPSet (blank option in menu below testsoftware).
rem Configuration TRICOPTER for three motor and QUAD for 4.
rem Motors are disabled for DEBUG_SENSORS for safety reasons.
rem Prevous switches THC and CAM have been abandoned in favour of Tx based
rem throttle shaping and X-mode to orient the camera forward set under UAVPSet.

set VERSION=3_15m3_

rem Add/Delete required combinations to these sets
set CLOCK=CLOCK_16MHZ
set PROC=18F2520
set GYRO=OPT_IDG OPT_ADXRS150 OPT_ADXRS300
set ESC=ESC_PPM ESC_HOLGER
set DBG=NO_DEBUG DEBUG_SENSORS 
set RX=RX_DEFAULT RX_DSM2 RX_PPM
set CFG=QUAD TRICOPTER

rem Personal choice
rem set CLOCK=CLOCK_16MHZ
rem set PROC=18F2620 18F2520
rem set GYRO=OPT_IDG OPT_ADXRS300
rem set ESC=ESC_PPM
rem set DBG=NO_DEBUG DEBUG_SENSORS
rem set RX=RX_DEFAULT RX_DSM2
rem set CFG=QUAD

rem Delete working files
call makeclean.bat
echo Starting makeall uavp > gen.lst
echo Starting makeall uavp > log.lst

rem Parameters for makeallhelper.bat are VERSION BARO GYRO ESC DBG RX

for %%x in (%CLOCK%) do for %%p in (%PROC%) do for %%g in (%GYRO%) do for %%e in (%ESC%) do for %%d in (%DBG%) do for %%r in (%RX%) do for %%c in (%CFG%) do call makeallhelper.bat %VERSION% %%x %%p %%g %%e %%d %%r %%c

set RX=RX_DEFAULT RX_PPM

rem Delete working files
call makeclean.bat
echo Starting makeall uavptest > gen.lst
echo Starting makeall uavptest > log.lst

rem Parameters for makealltesthelper.bat are VERSION BARO GYRO ESC DBG RX

for %%x in (%CLOCK%) do for %%p in (%PROC%) do for %%g in (%GYRO%) do for %%e in (%ESC%) do for %%r in (%RX%) do call makealltesthelper.bat %VERSION% %%x %%p %%g %%e %%r



