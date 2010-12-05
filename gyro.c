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
int8 	GyroType;

#include "gyro_itg3200.h"
#include "gyro_analog.h"

#define AccFilter NoFilter

void GetGyroValues(void)
{
	if ( GyroType == ITG3200Gyro )
		BlockReadITG3200();
	else
		GetAnalogGyroValues();
} // GetGyroValues

void CalculateGyroRates(void)
{
	static i24u Temp;

	Rate[Roll] = GyroADC[Roll] - GyroNeutral[Roll];
	Rate[Pitch] = GyroADC[Pitch] - GyroNeutral[Pitch];
	Rate[Yaw] = (int16)GyroADC[Yaw] - GyroNeutral[Yaw];

	switch ( GyroType ) {
	case IDG300Gyro:// 500 Deg/Sec 
		Rate[Roll] = -Rate[Roll] * 2; // adjust for reversed roll gyro sense
		Rate[Pitch] *= 2;
		Rate[Yaw] = SRS16(Rate[Yaw], 1); // ADXRS150 assumed
		break;
 	case LY530Gyro:// generically 300deg/S 3.3V
		Rate[Yaw] = SRS16(Rate[Yaw], 1);
		break;
	case MLX90609Gyro:// generically 300deg/S 5V
		Rate[Roll] = SRS16(Rate[Roll], 1);	
		Rate[Pitch] = SRS16(Rate[Pitch], 1);
		Rate[Yaw] = SRS16(Rate[Yaw], 2);
		break;
	case ADXRS300Gyro:// ADXRS610/300 300deg/S 5V
		Rate[Roll] = SRS16(Rate[Roll], 1);	// scaling is not correct
		Rate[Pitch] = SRS16(Rate[Pitch], 1);
		Rate[Yaw] = SRS16(Rate[Yaw], 2);
		break;
	case ITG3200Gyro:// ITG3200
		Rate[Roll] = SRS16(Rate[Roll], 8);	
		Rate[Pitch] = SRS16(Rate[Pitch], 8);
		Rate[Yaw] = SRS16(Rate[Yaw], 9);
		break;
	case IRSensors:// IR Sensors - NOT IMPLEMENTED IN PIC VERSION
		Rate[Roll] = 0;	
		Rate[Pitch] = 0;
		Rate[Yaw] = 0;
		break;
	case ADXRS150Gyro:	 // ADXRS613/150 or generically 150deg/S 5V
		Rate[Roll] = SRS16(Rate[Roll], 2); 
		Rate[Pitch] = SRS16(Rate[Pitch], 2);
		Rate[Yaw] = SRS16(Rate[Yaw], 3);
		break;
	default:;
	} // GyroType

//	LPFilter16(&Rate[Yaw], &YawRateF, YawFilterA);

} // CalculateGyroRates

void ErectGyros(void)
{
	static int8 i, g;
	static int32 Av[3];

	for ( g = 0; g < (int8)3; g++ )	
		Av[g] = 0;

    for ( i = 32; i ; i-- )
	{
		LEDRed_TOG;
		Delay100mSWithOutput(1);

		GetGyroValues();

		Av[Roll] += GyroADC[Roll];
		Av[Pitch] += GyroADC[Pitch];	
		Av[Yaw] += GyroADC[Yaw];
	}
	
	if( !F.AccelerationsValid )
	{
		Av[Roll] += (int16)P[MiddleLR] * 2;
		Av[Pitch] += (int16)P[MiddleFB] * 2;
	}
	
	for ( g = 0; g < (int8)3; g++ )
	{
		GyroNeutral[g] = (int16)(Av[g]/32);	
		Rate[g] =  Ratep[g] = Angle[g] = 0;
	}
 
	LEDRed_OFF;

} // ErectGyros

void ShowGyroType(void)
{
    switch ( GyroType ) {
        case MLX90609Gyro:
            TxString("MLX90609");
            break;
        case ADXRS150Gyro:
            TxString("ADXRS613/150");
            break;
        case IDG300Gyro:
            TxString("IDG300");
            break;
        case LY530Gyro:
            TxString("ST-AY530");
            break;
        case ADXRS300Gyro:
            TxString("ADXRS610/300");
            break;
        case ITG3200Gyro:
            TxString("ITG3200");
            break;
        case IRSensors:
            TxString("IR Sensors");
            break;
        default:
            TxString("UNKNOWN");
            break;
	}
} // ShowGyroType

void InitGyros(void)
{
	if ( ITG3200GyroActive() )
	{
		InitITG3200();
		GyroType = ITG3200Gyro;
	}
	else
	{
		GyroType = P[DesGyroType];
		InitAnalogGyros();
	}
} // InitGyros

#ifdef TESTING
void GyroTest(void)
{
	if ( GyroType == ITG3200Gyro )
		GyroITG3200Test();
	else
		GyroAnalogTest();
} // GyroTest
#endif // TESTING

void CompensateRollPitchGyros(void)
{
	#define GRAV_COMP 11L
	#define GYRO_COMP_STEP 3

	static int16 Grav[2], Dyn[2];

	if( F.AccelerationsValid )
	{
		ReadAccelerations();
	
		Acc[LR] = Ax.i16;
		Acc[DU] = Ay.i16;
		Acc[FB] = Az.i16;

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






