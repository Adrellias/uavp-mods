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

void AdaptiveYawFilterA(void);
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

void AdaptiveYawFilterA(void)
{ // ~600uS @ 16MHz - use approximation 4.5uS

/*
	static int16 TwoPiF, r; // 2.0*Pi*F

	TwoPiF = ( Abs(DesiredYaw) * YAW_MAX_FREQ * 6L ) / RC_NEUTRAL;
	TwoPiF = Limit(TwoPiF, 3, YAW_MAX_FREQ * 6L );

	YawFilterA = ( (int24) PID_CYCLE_MS * 256L) / ( 1000L / TwoPiF + (int16) PID_CYCLE_MS );
*/

	YawFilterA = 5 + Abs(DesiredYaw);

} // AdaptiveYawFilterA

#define AccFilter NoFilter

void GetGyroValues(void)
{
	if ( GyroType == ITG3200Gyro )
		BlockReadITG3200();
	else
		GetAnalogGyroValues();

    LPFilter16(&Rate[Yaw], &YawRateF, YawFilterA);

} // GetGyroValues

void CalculateGyroRates(void)
{
	static i24u RollT, PitchT, YawT;

	Rate[Roll] = GyroADC[Roll] - GyroNeutral[Roll];
	Rate[Pitch] = GyroADC[Pitch] - GyroNeutral[Pitch];
	Rate[Yaw] = (int16)GyroADC[Yaw] - GyroNeutral[Yaw];

	switch ( GyroType ) {
	case IDG300Gyro:// 500 Deg/Sec 
		RollT.i24 = -(int24)Rate[Roll] * 422; // reversed roll gyro sense
		PitchT.i24 = (int24)Rate[Pitch] * 422;
		YawT.i24 = (int24)Rate[Yaw] * 34; // ADXRS150 assumed
		break;
 	case LY530Gyro:// REFERENCE generically 300deg/S 3.3V
		RollT.i24 = (int24)Rate[Roll] * 256;
		PitchT.i24 = (int24)Rate[Pitch] * 256;
		YawT.i24 = (int24)Rate[Yaw] * 128;
		break;
	case MLX90609Gyro:// generically 300deg/S 5V
		RollT.i24 = (int24)Rate[Roll] * 127;
		PitchT.i24 = (int24)Rate[Pitch] * 127;
		YawT.i24 = (int24)Rate[Yaw] * 63;
		break;
	case ADXRS300Gyro:// ADXRS610/300 300deg/S 5V
		RollT.i24 = (int24)Rate[Roll] * 169;
		PitchT.i24 = (int24)Rate[Pitch] * 169;
		YawT.i24 = (int24)Rate[Yaw] * 84;
		break;
	case ITG3200Gyro:// ITG3200
		RollT.i24 = (int24)Rate[Roll] * 18;
		PitchT.i24 = (int24)Rate[Pitch] * 18;
		YawT.i24 = (int24)Rate[Yaw] * 9;
		break;
	case IRSensors:// IR Sensors - NOT IMPLEMENTED IN PIC VERSION
		RollT.i24 = PitchT.i24 = YawT.i24 = 0;
		break;
	case ADXRS150Gyro:// ADXRS613/150 or generically 150deg/S 5V
		RollT.i24 = (int24)Rate[Roll] * 68;
		PitchT.i24 = (int24)Rate[Pitch] * 68;
		YawT.i24 = (int24)Rate[Yaw] * 34;
		break;
	default:;
	} // GyroType

	Rate[Roll] = RollT.i2_1;
	Rate[Pitch] = PitchT.i2_1;
	Rate[Yaw] = YawT.i2_1;

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
	
	for ( g = 0; g < (int8)3; g++ )
	{
		GyroNeutral[g] = (int16)SRS32( Av[g], 5); // ITG3200 is signed
		Rate[g] =  Ratep[g] = Angle[g] = RawAngle[g] = 0;
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

	GyroType = P[DesGyroType];
	if ( GyroType == ITG3200Gyro )
	{
		if ( ITG3200GyroActive() )
			InitITG3200();	
	}
	else
		InitAnalogGyros();

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
	static i32u Temp;

	// RESCALE_TO_ACC is dependent on cycle time and is defined in uavx.h

	#define ANGLE_COMP_STEP 25

	static int16 Grav[2], Dyn[2];
	static int16 AccMag;

	if( F.AccelerationsValid ) // zzz check falling/climbing
	{
		ReadAccelerations();
	
		Acc[LR] = Ax.i16;
		Acc[DU] = Ay.i16;
		Acc[FB] = Az.i16;

		// NeutralLR, NeutralFB, NeutralDU pass through UAVPSet 
		// and come back as MiddleLR etc.

		AccMag = int32sqrt( Sqr(Acc[LR]) + Sqr(Acc[FB]) + Sqr(Acc[DU]) );
		F.UsingAccComp = AccMag > 850;

		Acc[LR] -= (int16)P[MiddleLR];
		Acc[FB] -= (int16)P[MiddleFB];
		Acc[DU] -= (int16)P[MiddleDU];

		LPFilter16(&Acc[DU], &AccDUF, AccDUFilterA);	
		
		// Roll
	
		Temp.i32 = -(int32)Angle[Roll] * RESCALE_TO_ACC; // avoid shift
		Grav[LR] = Temp.i3_1;
		Dyn[LR] = 0; //Rate[Roll];
	
		IntCorr[LR] = SRS32(Acc[LR] + Grav[LR] + Dyn[LR], 3); 
		IntCorr[LR] = Limit1(IntCorr[LR], ANGLE_COMP_STEP); 
		
		// Pitch
	
		Temp.i32 = -(int32)Angle[Pitch] * RESCALE_TO_ACC; // avoid shift
		Grav[FB] = Temp.i3_1;
		Dyn[FB] = 0; // Rate[Pitch];
	
		IntCorr[FB] = SRS16(Acc[FB] + Grav[FB] + Dyn[FB], 3); 
		IntCorr[FB] = Limit1(IntCorr[FB], ANGLE_COMP_STEP);
	}	
	else
	{
		ROC = IntCorr[LR] = IntCorr[FB] = Acc[LR] = Acc[FB] = Acc[DU] = 0;
		F.UsingAccComp = false;
	}

} // CompensateRollPitchGyros




