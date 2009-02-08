@echo off
rem To see why we do the setlocal, see:
rem http://www.robvanderwoude.com/variableexpansion.html
rem http://www.robvanderwoude.com/ntset.html
SETLOCAL ENABLEDELAYEDEXPANSION

rem Batch compiles various possibilities of the UFO software
rem
rem Greg Egan 2008
rem
rem Uses: makeallhelper.bat and makeclean.bat
rem 
rem Type of barometers used. May be BMP085 or SMD500
rem Type of gyros in use. May be OPT_ADXRS300, OPT_ADXRS150, or IDG300.
rem Type of ESC in use. May be ESC_PPM,  ESC_YGEI2C, ESC_HOLGER.
rem Type of Rx. May be RX_DSM2 for for reference DX7/AR7000 combination, 
rem RX_PPM for serial PPM frame, or RX_DEFAULT for default PPM Graupner/JR etc Rx.
rem Type of debugging to use. May be DEBUG_MOTORS (3.1 only) or DEBUG_SENSORS.
rem Motors are disabled for DEBUG_SENSORS as there is not enough program space
rem in the 876 PIC.
rem Prevous switches THC and CAM have been abandoned in favour of Tx based
rem throttle shaping and X-mode to orient the camera forward set under UAVPSet.

set VERSION=3_15m3

rem Add/Delete required combinations to these sets
set BAROS= BMP085 SMD500
set GYROS=OPT_IDG OPT_ADXRS150 OPT_ADXRS300
set ESCS=ESC_PPM ESC_HOLGER
set DBGS=NO_DEBUG DEBUG_SENSORS DEBUG_MOTORS 
set RXS=RX_DEFAULT RX_DSM2 RX_PPM

rem Personal choice
rem set BAROS= SMD500 BMP085
rem set GYROS=OPT_IDG
rem set ESCS=ESC_PPM
rem set DBGS=NO_DEBUG DEBUG_SENSORS
rem set RXS=RX_DEFAULT

rem Delete working files
call makeclean.bat
echo Starting makeall uavp > gen.lst
echo Starting makeall uavp > log.lst

rem Parameters for makeallhelper.bat are VERSION BARO GYRO ESC DBG RX

for %%b in (%BAROS%) do for %%g in (%GYROS%) do for %%e in (%ESCS%) do for %%d in (%DBGS%) do for %%r in (%RXS%) do call makeallhelper.bat %VERSION% %%b %%g %%e %%d %%r 




