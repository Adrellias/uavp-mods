@echo off
rem ---------------------------------------------
rem Update SVN revision include and batch files
rem ---------------------------------------------

if exist UAVXRevision.h del UAVXRevision.h
SubWCRev . UAVXRevisionSVN.h UAVXRevision.h

if exist makeallhelper.bat del makeallhelper.bat
SubWCRev . makeallhelperSVN.bat makeallhelper.bat
