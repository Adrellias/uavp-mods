:echo off
rem Helper batch
rem param1 = program
rem param2..n = params

goto %1

:CC5X
set EP=
if %2 == sensor.c set EP=-ro1
if %2 == accel.c set EP=-ro2
if %2 == c-ufo.c set EP=-ro9
if %2 == mathlib.c set EP=-ro4
if %2 == trig.c set EP=-ro5
if %2 == utils.c set EP=-ro6
echo %CEXE% %2 %CCMD% %3 %4 %5 %6 %7 %8 %9 %EP%
%CEXE% %2 %CCMD% %3 %4 %5 %6 %7 %8 %9 %EP% >NUL:
goto ENDE

:ASM
echo %AEXE% %2 %ACMD% %3 %4 %5 %6 %7 %8 %9
%AEXE% %2 %ACMD% %3 %4 %5 %6 %7 %8 %9 >NUL:
goto ENDE


:ENDE
