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

// Gyros

#include "uavx.h"

void CompensateRollPitchGyros(void);
void GetRollPitchGyroValues(void);
void GetYawGyroValue(void);
void ErectGyros(void);
void CalcGyroRates(void);

int16	GyroMidRoll, GyroMidRollBy2, GyroMidPitch, GyroMidPitchBy2, GyroMidYaw;
int16	RollRate, PitchRate, YawRate;
int16	RollRateADC, PitchRateADC, YawRateADC;

void GetRollPitchGyroValues(void)
{ // invoked TWICE per control cycle so roll/pitch "rates" are double actual
	if ( P[GyroRollPitchType] == IDG300 ) // 500 Deg/Sec
	{
		RollRateADC = (int16)ADC(IDGADCRollChan);
		PitchRateADC = (int16)ADC(IDGADCPitchChan);
	}
	else
	{
		RollRateADC = (int16)ADC(NonIDGADCRollChan);
		PitchRateADC = (int16)ADC(NonIDGADCPitchChan);
	}

	RollRate = RollRate + RollRateADC;
	PitchRate = PitchRate + PitchRateADC;

} // GetRollPitchGyroValues

void GetYawGyroValue(void)
{
	YawRateADC = ADC(ADCYawChan);
	YawRate = YawRateADC - GyroMidYaw; 

	if ( P[GyroYawType] == Gyro150D5V )
		YawRate = SRS16(YawRate, 2);
	else // 
		if ( P[GyroYawType] == Gyro300D5V )
			YawRate = SRS16(YawRate, 1);
	//	else // Gyro300D3V  
	//		YawRate = YawRate;

} // GetYawGyroValue

void ErectGyros(void)
{
	static uint8 i;
	static uint16 RollAv, PitchAv, YawAv;
	
	RollAv = PitchAv = YawAv = 0;	
    for ( i = 32; i ; i-- )
	{
		LEDRed_TOG;
		Delay100mSWithOutput(GYRO_ERECT_DELAY);

		if ( P[GyroRollPitchType] == IDG300 )
		{
			RollAv += ADC(IDGADCRollChan);
			PitchAv += ADC(IDGADCPitchChan);	
		}
		else
		{
			RollAv += ADC(NonIDGADCRollChan);
			PitchAv += ADC(NonIDGADCPitchChan);
		}
		YawAv += ADC(ADCYawChan);
	}
	
	if( !F.AccelerationsValid )
	{
		RollAv += (int16)P[MiddleLR] * 2;
		PitchAv += (int16)P[MiddleFB] * 2;
	}
	
	GyroMidRoll = (int16)((RollAv + 16) >> 5);	
	GyroMidPitch = (int16)((PitchAv + 16) >> 5);
	GyroMidYaw = (int16)((YawAv + 16) >> 5);

	// compute here to remove from main control loop
	GyroMidRollBy2 = GyroMidRoll * 2;
	GyroMidPitchBy2 = GyroMidPitch * 2;
	
	RollSum = PitchSum = YawSum = REp = PEp = YEp = 0;
	LEDRed_OFF;

} // ErectGyros

#define AccFilter NoFilter

void CompensateRollPitchGyros(void)
{
	static int16 Temp;
	static int24 Temp24;
	static int16 LRGrav, LRDyn, FBGrav, FBDyn;
	static int16 NewLRAcc, NewDUAcc, NewFBAcc;

	if( F.AccelerationsValid )
	{
		ReadAccelerations();

		if ( F.UsingFlatAcc ) // chip up and twisted over
		{
			NewLRAcc = -Ax.i16;	
			NewFBAcc = Ay.i16;
			NewDUAcc = Az.i16;
		}
		else
		{	
			NewLRAcc = Ax.i16;
			NewDUAcc = Ay.i16;
			NewFBAcc = Az.i16;
		}

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

		#if ( defined QUADROCOPTER | defined TRICOPTER )
			#ifdef TRICOPTER
			if ( F.UsingAltOrientation ) // K1 forward
			{
				FBAcc = -FBAcc;	
				LRAcc = -LRAcc;
			}
			#else
			if ( F.UsingAltOrientation )
			{
				// "Real" LR =  0.707 * (LR + FB), FB = 0.707 * ( FB - LR )
				Temp24 = FBAcc + LRAcc;	
				FBAcc -= LRAcc;		
				FBAcc = (FBAcc * 7L)/10L;
				LRAcc = (Temp * 7L)/10L;
			}
			#endif // TRICOPTER
		#endif // QUADROCOPTER | TRICOPTER 
			
		// Roll

		// static compensation due to Gravity
		LRGrav = -SRS16(RollSum * (int16)P[GravComp], 5); 
	
		// dynamic correction of moved mass
		#ifdef DISABLE_DYNAMIC_MASS_COMP_ROLL
		LRDyn = 0;
		#else
		LRDyn = RollRate;	
		#endif

		// correct DC level of the integral
		LRIntCorr = SRS16(LRAcc + LRGrav + LRDyn, 3); // / 10;
		LRIntCorr = Limit(LRIntCorr, -(int16)P[CompSteps], (int16)P[CompSteps]); 
	
		// Pitch

		// static compensation due to Gravity
		FBGrav = -SRS16(PitchSum * (int16)P[GravComp], 5); 
	
		// dynamic correction of moved mass		
		#ifdef DISABLE_DYNAMIC_MASS_COMP_PITCH
		FBDyn = 0;
		#else
		FBDyn = PitchRate;
		#endif

		// correct DC level of the integral	
		FBIntCorr = SRS16(FBAcc + FBGrav + FBDyn, 3); // / 10;
		FBIntCorr = Limit(FBIntCorr, -(int16)P[CompSteps], (int16)P[CompSteps]); 
	}	
	else
		LRIntCorr = FBIntCorr = DUAcc = 0;

} // CompensateRollPitchGyros

void CalcGyroRates(void)
{
	static int16 Temp;

	// RollRate & PitchRate hold the sum of 2 consecutive conversions
	// 300 Deg/Sec is the "reference" gyro full scale rate

	if ( P[GyroRollPitchType] == IDG300 ) // 2.0
	{ 	// 500 Deg/Sec, 5/3.3 FS => ~2.5 so do not average 
		RollRate -= GyroMidRollBy2;
		PitchRate -= GyroMidPitchBy2;
		RollRate = -RollRate;			// adjust for reversed roll gyro sense
 	}
	else
		if ( P[GyroRollPitchType] == Gyro300D3V ) // 2.0
		{ 	// 300 Deg/Sec, 5/2.4 FS => ~2.0 so do not average 
			RollRate -= GyroMidRollBy2;
			PitchRate -= GyroMidPitchBy2;
	 	}
		else
		{ 	// 1.0
			// Average of two readings
			RollRate = ( RollRate >> 1 ) - GyroMidRoll;	
			PitchRate = ( PitchRate >> 1 ) - GyroMidPitch;
	
			if ( P[GyroRollPitchType] == Gyro150D5V )	// 0.5
			{ // 150 Deg/Sec or 0.5
				RollRate = SRS16(RollRate, 1); 
				PitchRate = SRS16(PitchRate, 1);
			}
		}

	#if ( defined QUADROCOPTER | defined TRICOPTER )
		#ifdef TRICOPTER
		if ( F.UsingAltOrientation ) // K1 forward
		{
			RollRate = -RollRate;
			PitchRate = -PitchRate;
		}		
		#else
		if ( F.UsingAltOrientation )
		{
			// "Real" Roll = 0.707 * (P + R), Pitch = 0.707 * (P - R)
			Temp = RollRate + PitchRate;	
			PitchRate -= RollRate;	
			RollRate = (Temp * 7L)/10L;
			PitchRate = (PitchRate * 7L)/10L; 
		}
		#endif // TRICOPTER
	#endif // QUADROCOPTER | TRICOPTER 
	
	// Yaw is sampled only once every frame
	GetYawGyroValue();	

} // CalcGyroRates



