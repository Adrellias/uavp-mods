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

int16	GyroMidRoll, GyroMidPitch, GyroMidYaw;
int16	RollRate, PitchRate, YawRate;

void GetRollPitchGyroValues(void)
{
	static int16 NewRollRate, NewPitchRate;

	if ( P[GyroType] == IDG300 ) // 500 Deg/Sec
	{
		NewRollRate = (int16)ADC(IDGADCRollChan, ADCVREF3V3);
		NewPitchRate = (int16)ADC(IDGADCPitchChan, ADCVREF3V3);
	}
	else
	{
		NewRollRate = (int16)ADC(NonIDGADCRollChan, ADCVREF5V);
		NewPitchRate = (int16)ADC(NonIDGADCPitchChan, ADCVREF5V);
	}

	RollRate += NewRollRate;
	PitchRate += NewPitchRate;

	#ifdef STATS_INC_GYRO_ACC
	if ( State == InFlight )
	{
		if ( NewRollRate > Stats[RollRateS].i24 ) Stats[RollRateS].i24 = NewRollRate;
		if ( NewPitchRate > Stats[PitchRateS].i24 ) Stats[PitchRateS].i24 = NewPitchRate;
	}
	#endif // STATS_INC_GYRO_ACC
} // GetRollPitchGyroValues

void GetYawGyroValue(void)
{
	YawRate = ADC(ADCYawChan, ADCVREF5V);
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

		if ( P[GyroType] == IDG300 )
		{
			RollAv += ADC(IDGADCRollChan, ADCVREF3V3);
			PitchAv += ADC(IDGADCPitchChan, ADCVREF3V3);	
		}
		else
		{
			RollAv += ADC(NonIDGADCRollChan, ADCVREF5V);
			PitchAv += ADC(NonIDGADCPitchChan, ADCVREF5V);
		}
		YawAv += ADC(ADCYawChan, ADCVREF5V);
	}
	
	if( !F.AccelerationsValid )
	{
		RollAv += (int16)P[MiddleLR] * 2;
		PitchAv += (int16)P[MiddleFB] * 2;
	}
	
	GyroMidRoll = (int16)((RollAv + 16) >> 5);	
	GyroMidPitch = (int16)((PitchAv + 16) >> 5);
	GyroMidYaw = (int16)((YawAv + 64) >> 7);

	#ifdef STATS_INC_GYRO_ACC
	Stats[RollRateS].i16 = Stats[GyroMidRollS].i16 = GyroMidRoll;
	Stats[PitchRateS].i16 = Stats[GyroMidPitchS].i16 = GyroMidPitch;
	Stats[YawRateS].i16 = Stats[GyroMidYawS].i16 = GyroMidYaw;
	#endif // STATS_INC_GYRO_ACC

	RollSum = PitchSum = YawSum = REp = PEp = YEp = 0;
	LEDRed_OFF;

	F.GyrosErected = true;

} // ErectGyros

#define AccFilter NoFilter

void CompensateRollPitchGyros(void)
{
	static int16 Temp;
	static int16 LRGrav, LRDyn, FBGrav, FBDyn;
	static int16 NewLRAcc, NewDUAcc, NewFBAcc;

	if( F.AccelerationsValid )
	{
		ReadAccelerations();

		NewLRAcc = Ax.i16;
		NewDUAcc = Ay.i16;
		NewFBAcc = Az.i16;
		
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
			
		#ifdef STATS_INC_GYRO_ACC
		if ( State == InFlight )
		{
			if ( LRAcc > Stats[LRAccS].i24 ) Stats[LRAccS].i24 = LRAcc; 
			if ( FBAcc > Stats[FBAccS].i24 ) Stats[FBAccS].i24 = FBAcc; 
			if ( DUAcc > Stats[DUAccS].i24 ) Stats[DUAccS].i24 = DUAcc;
		}
		#endif // STATS_INC_GYRO_ACC

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

		#ifdef DEBUG_SENSORS
		Trace[TAx]= LRAcc;
		Trace[TAz] = FBAcc;
		Trace[TAy] = DUAcc;

		Trace[TLRIntCorr] = LRIntCorr * 8; // scale up for UAVPSet
		Trace[TFBIntCorr] = FBIntCorr * 8;	
		#endif // DEBUG_SENSORS	
	}	
	else
		LRIntCorr = FBIntCorr = DUAcc = 0;

} // CompensateRollPitchGyros

void CalcGyroRates(void)
{
	static int16 Temp;

	// RollRate & PitchRate hold the sum of 2 consecutive conversions
	// 300 Deg/Sec is the "reference" gyro full scale rate

	if ( P[GyroType] == IDG300 )
	{ 	// 500 Deg/Sec or 1.66 ~= 2 so do not average readings 
		RollRate -= GyroMidRoll * 2;
		PitchRate -= GyroMidPitch * 2;
		RollRate = -RollRate;			// adjust for reversed roll gyro sense
 	}
	else
	{ 	// 1.0
		// Average of two readings
		RollRate = ( RollRate >> 1 ) - GyroMidRoll;	
		PitchRate = ( PitchRate >> 1 ) - GyroMidPitch;

		if ( P[GyroType] == ADXRS150 )	// 150 deg/sec (0.5)
		{ // 150 Deg/Sec or 0.5
			RollRate = SRS16(RollRate, 1); 
			PitchRate = SRS16(PitchRate, 1);
		}
	}

	if ( F.UsingXMode )
	{
		// "Real" Roll = 0.707 * (P + R), Pitch = 0.707 * (P - R)
		Temp = RollRate + PitchRate;	
		PitchRate -= RollRate;	
		RollRate = (Temp * 7L)/10L;
		PitchRate = (PitchRate * 7L)/10L; 
	}
	
	// Yaw is sampled only once every frame, 8 bit A/D resolution
	GetYawGyroValue();
	YawRate >>= 2;
	if (( State == InFlight ) && (YawRate > Stats[YawRateS].i16) ) Stats[YawRateS].i16 = YawRate;	
	YawRate -= GyroMidYaw;

} // CalcGyroRates



