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

void ShowGyroType(void);
void CompensateRollPitchGyros(void);
void GetGyroValues(void);
void CalculateGyroRates(void);
void CheckGyroFault(uint8, uint8, uint8);
void ErectGyros(void);
void GyroTest(void);
void InitGyros(void);

int16	Rate[3], GyroNeutral[3], GyroADC[3];
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

void ShowGyroType(void)
{
    switch ( GyroType ) {
        case MLX90609Gyro:
            TxString("MLX90609\r\n");
            break;
        case ADXRS150Gyro:
            TxString("ADXRS613/150\r\n");
            break;
        case IDG300Gyro:
            TxString("IDG300\r\n");
            break;
        case LY530Gyro:
            TxString("ST-AY530\r\n");
            break;
        case ADXRS300Gyro:
            TxString("ADXRS610/300\r\n");
            break;
        case ITG3200Gyro:
            TxString("ITG3200\r\n");
            break;
        default:
            TxString("UNKNOWN\r\n");
            break;
	}
} // ShowGyroType

void CompensateRollPitchGyros(void)
{
	#define GRAV_COMP 11L
	#define GYRO_COMP_STEP 3

	static int16 Temp;
	static int24 Temp24;
	static int16 Grav[2], Dyn[2];

	if( F.AccelerationsValid )
	{
		ReadAccelerations();

		#ifdef USE_FLAT_ACC // chip up and twisted over
			Acc[LR] = -Ax.i16;	
			Acc[FB] = Ay.i16;
			Acc[DU] = Az.i16;
		#else	
			Acc[LR] = Ax.i16;
			Acc[DU] = Ay.i16;
			Acc[FB] = Az.i16;
		#endif // USE_FLAT_ACC

		// NeutralLR, NeutralFB, NeutralDU pass through UAVPSet 
		// and come back as MiddleLR etc.

		Acc[LR] -= (int16)P[MiddleLR];
		Acc[FB] -= (int16)P[MiddleFB];
		Acc[DU] -= (int16)P[MiddleDU];

		Acc[DU] -= 1024L;	// subtract 1g - not corrrect for other than level
						// ??? could check for large negative Acc => upside down?	
			
		// Roll

		// static compensation due to Gravity
		Grav[LR] = -SRS16(Angle[Roll] * GRAV_COMP, 5); 
	
		// dynamic correction of moved mass
		#ifdef DISABLE_DYNAMIC_MASS_COMP_ROLL
		Dyn[LR] = 0;
		#else
		Dyn[LR] = Rate[Roll];	
		#endif

		// correct DC level of the integral
		IntCorr[LR] = SRS16(Acc[LR] + Grav[LR] + Dyn[LR], 3); // / 10;
		IntCorr[LR] = Limit(IntCorr[LR], -GYRO_COMP_STEP, GYRO_COMP_STEP); 
	
		// Pitch

		// static compensation due to Gravity
		Grav[FB] = -SRS16(Angle[Pitch] * GRAV_COMP, 5); 
	
		// dynamic correction of moved mass		
		#ifdef DISABLE_DYNAMIC_MASS_COMP_PITCH
		Dyn[FB] = 0;
		#else
		Dyn[FB] = Rate[Pitch];
		#endif

		// correct DC level of the integral	
		IntCorr[FB] = SRS16(Acc[FB] + Grav[FB] + Dyn[FB], 3); // / 10;
		IntCorr[FB] = Limit(IntCorr[FB], -GYRO_COMP_STEP, GYRO_COMP_STEP); 
	}	
	else
		IntCorr[LR] = IntCorr[FB] = Acc[LR] = Acc[FB] = Acc[DU] = 0;

} // CompensateRollPitchGyros






