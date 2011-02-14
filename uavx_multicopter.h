// ===============================================================================================
// =                                UAVX Quadrocopter Controller                                 =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                     http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// ===============================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/
	
	const rom int8 DefaultParams[MAX_PARAMETERS][2] = {
	{-20,0},			// RollKp, 			01
	{-12,0},	 		// RollKi,			02
	{50, 0},			// RollKd,			03
	{-1,true},			// HorizDampKp,		04c 
	{5,0},	 			// RollIntLimit,	05
	{-20,0},	 		// PitchKp,			06
	{-12,0},	 		// PitchKi,			07
	{50,0},	 			// PitchKd,			08
	{8,0},	 			// AltKp,			09
	{5,0},	 			// PitchIntLimit,	10
	
	{-30,0},	 		// YawKp, 			11
	{-25,0},	 		// YawKi,			12
	{0,0},	 			// YawKd,			13
	{25,0},	 			// YawLimit,		14
	{2,0},	 			// YawIntLimit,		15
	{2,true}, 			// ConfigBits,		16c
	{0,true},			// was TimeSlots,	17c
	{48,true}, 			// LowVoltThres,	18c
	{20,true}, 			// CamRollKp,		19c
	{45,true}, 			// PercentCruiseThr,20c 
	
	{-1,true}, 			// VertDampKp,		21c
	{0,true}, 			// MiddleDU,		22c
	{20,true}, 			// PercentIdleThr,	23c
	{0,true}, 			// MiddleLR,		24c
	{0,true}, 			// MiddleFB,		25c
	{20,true}, 			// CamPitchKp,		26c
	{10,0}, 			// CompassKp,		27
	{8,0},				// AltKi,			28
	{0,0}, 				// was NavRadius,	29
	{8,0}, 				// NavKi,			30 

	{0,0}, 				// GSThrottle,	    31
	{0,0},				// Acro,	    	32
	{10,0}, 		    // NavRTHAlt,		33
	{0,true},			// NavMagVar,		34c
	{LY530Gyro,true}, 	// DesGyroType,     35c
	{ESCPPM,true}, 		// ESCType,			36c
	{UnknownTxRx,true}, // TxRxType			37c
	{2,true},			// NeutralRadius	38
	{30,true},			// PercentNavSens6Ch	39
	{1,true},			// CamRollTrim,		40c

	{-16,0},			// NavKd			41
	{1,true},			// VertDampDecay    42c
	{1,true},			// HorizDampDecay	43c
	{56,true},			// BaroScale	    44c
	{UAVXTelemetry,true}, // TelemetryType	45c
	{-12,0},			// MaxDescentRateDmpS 	46
	{30,true},			// DescentDelayS	47c
	{1,0},				// NavIntLimit		48
	{2,0},				// AltIntLimit		49
	{11,true},			// was GravComp		50c

	{0,true},			// was CompSteps	51c
	{0,true},			// ServoSense		52c	
	{3,true},			// CompassOffsetQtr 53c
	{49,true},			// BatteryCapacity	54c	
	{0,0},				// was GyroYawType	55c		
	{-4,0},				// AltKd			56
	#if (defined  TRICOPTER) | (defined VTCOPTER ) | (defined Y6COPTER )
	{24,true},			// Orient			57
	#else	
	{0,true},			// Orient			57
	#endif // TRICOPTER | VTCOPTER | Y6COPTER			
	
	{12,true},			// NavYawLimit		58
	{50,0},				// Balance			59
	{0,0},				// 60 - 64 unused currently	
	{0,0},	

	{0,0},	
	{0,0},	
	{0,0}						
	};




