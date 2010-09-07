// =================================================================================================
// =                                  UAVX Quadrocopter Controller                                 =
// =                             Copyright (c) 2008 by Prof. Greg Egan                             =
// =                   Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                       http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// =================================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even 
//    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//    General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/
	
	const rom int8 DefaultParams[] = {
	-20, 			// RollKp, 			01
	-12, 			// RollKi,			02
	50, 			// RollKd,			03
	-1, 			// HorizDampKp,		04c 
	3, 				// RollIntLimit,	05
	-20, 			// PitchKp,			06
	-12, 			// PitchKi,			07
	50, 			// PitchKd,			08
	8, 				// AltKp,			09
	3, 				// PitchIntLimit,	10
	
	-30, 			// YawKp, 			11
	-25, 			// YawKi,			12
	0, 				// YawKd,			13
	25, 			// YawLimit,		14
	2, 				// YawIntLimit,		15
	0, 				// ConfigBits,		16c
#ifdef CLOCK_16MHZ
	8, 				// TimeSlots,		17c
#else // CLOCK_40MHZ
	5, 				// TimeSlots,		17c
#endif // CLOCK_16MHZ
	48, 			// LowVoltThres,	18c
	20, 			// CamRollKp,		19
	45, 			// PercentCruiseThr,20c 
	
	-1, 			// VertDampKp,		21c
	0, 				// MiddleDU,		22c
	20, 			// PercentIdleThr,	23c
	0, 				// MiddleLR,		24c
	0, 				// MiddleFB,		25c
	20, 			// CamPitchKp,		26
	10, 			// CompassKp,		27
	8, 				// AltKi,			28
	30, 			// NavRadius,		29
	8, 				// NavKi,			30 

	0, 				// unused1,			31
	0, 				// unused2,			32
	10, 			// NavRTHAlt,		33
	0, 				// NavMagVar,		34c
	Gyro300D5V, 	// GyroRollPitchType,	35c
	ESCPPM, 		// ESCType,			36c
	DX7AR7000, 		// TxRxType			37c
	2,				// NeutralRadius	38
	30,				// PercentNavSens6Ch	39
	1,				// CamRollTrim,		40c

	-16,			// NavKd			41
	1,				// VertDampDecay    42c
	1,				// HorizDampDecay	43c
	56,				// BaroScale		44c
	NoTelemetry,	// TelemetryType	45c
	-8,				// MaxDescentRateDmpS 	46
	30,				// DescentDelayS	47c
	1,				// NavIntLimit		48
	2,				// AltIntLimit		49
	11,				// was GravComp		50c
	1,				// CompSteps		51c
	0,				// ServoSense		52c	
	3,				// CompassOffsetQtr 53c
	49,				// BatteryCapacity	54c	
	Gyro300D5V,		// GyroYawType		55c		
	-4,				// AltKd			56
	#ifdef TRICOPTER
	24,				// Orient			57
	#else	
	0,				// Orient			57
	#endif // TRICOPTER				
	
	12,				// NavYawLimit		58
	0,				// 58 - 64 unused currently	
	0,

	0,
	0,
	0,
	0					
	};

