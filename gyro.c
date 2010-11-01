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

// Gyros

#include "uavx.h"

void ShowGyroType(uint8);
void CompensateRollPitchGyros(void);
void GetGyroValues(void);
void CalculateGyroRates(void);
void CheckGyroFault(uint8, uint8, uint8);
void ErectGyros(void);
void GyroTest(void);
void InitGyros(void);

int16	GyroMidRoll, GyroMidPitch, GyroMidYaw;
int16	RollRate, PitchRate, YawRate;
int16	RollRateADC, PitchRateADC, YawRateADC;
i32u 	YawRateF;

#ifdef GYRO_ITG3200
	#include "gyro_itg3200.h"
#else // GYRO_ITG3200
	#ifdef GYRO_WII
		#include "gyro_wii.h"
	#else
		#include "gyro_analog.h"
	#endif // GYRO_WII
#endif // GYRO_ITG3200

#define AccFilter NoFilter

void ShowGyroType(uint8 T)
{
	switch ( P[T] ) {
		case Gyro300D5V:TxString("ADXRS610/300 or MLX90609\r\n"); break;
		case Gyro150D5V:TxString("ADXRS613/150\r\n"); break;
		case IDG300:TxString("IDG300\r\n"); break;
		case Gyro300D3V:TxString("ST-AY530\r\n"); break;
		case CustomGyro:TxString("Custom\r\n"); break;
	}
} // ShowGyroType

void CompensateRollPitchGyros(void)
{
	#define GRAV_COMP 11L
	#define GYRO_COMP_STEP 2

	static int16 Temp;
	static int24 Temp24;
	static int16 LRGrav, LRDyn, FBGrav, FBDyn;
	static int16 NewLRAcc, NewDUAcc, NewFBAcc;

	if( F.AccelerationsValid )
	{
		ReadAccelerations();

		#ifdef USE_FLAT_ACC // chip up and twisted over
			NewLRAcc = -Ax.i16;	
			NewFBAcc = Ay.i16;
			NewDUAcc = Az.i16;
		#else	
			NewLRAcc = Ax.i16;
			NewDUAcc = Ay.i16;
			NewFBAcc = Az.i16;
		#endif // USE_FLAT_ACC

		// NeutralLR, NeutralFB, NeutralDU pass through UAVPSet 
		// and come back as MiddleLR etc.

		NewLRAcc -= (int16)P[MiddleLR];
		NewFBAcc -= (int16)P[MiddleFB];
		NewDUAcc -= (int16)P[MiddleDU];

		NewDUAcc -= 1024L;	// subtract 1g - not corrrect for other than level
						// ??? could check for large negative Acc => upside down?

		LRAcc = AccFilter((int32)LRAcc, (int32)NewLRAcc);
		DUAcc = AccFilter((int32)DUAcc, (int32)NewDUAcc);
		FBAcc = AccFilter((int32)FBAcc, (int32)NewFBAcc);	
			
		// Roll

		// static compensation due to Gravity
		LRGrav = -SRS16(RollSum * GRAV_COMP, 5); 
	
		// dynamic correction of moved mass
		#ifdef DISABLE_DYNAMIC_MASS_COMP_ROLL
		LRDyn = 0;
		#else
		LRDyn = RollRate;	
		#endif

		// correct DC level of the integral
		LRIntCorr = SRS16(LRAcc + LRGrav + LRDyn, 3); // / 10;
		LRIntCorr = Limit(LRIntCorr, -GYRO_COMP_STEP, GYRO_COMP_STEP); 
	
		// Pitch

		// static compensation due to Gravity
		FBGrav = -SRS16(PitchSum * GRAV_COMP, 5); 
	
		// dynamic correction of moved mass		
		#ifdef DISABLE_DYNAMIC_MASS_COMP_PITCH
		FBDyn = 0;
		#else
		FBDyn = PitchRate;
		#endif

		// correct DC level of the integral	
		FBIntCorr = SRS16(FBAcc + FBGrav + FBDyn, 3); // / 10;
		FBIntCorr = Limit(FBIntCorr, -GYRO_COMP_STEP, GYRO_COMP_STEP); 
	}	
	else
		LRIntCorr = FBIntCorr = LRAcc = FBAcc = DUAcc = 0;

} // CompensateRollPitchGyros






