// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008, 2009 by Prof. Greg Egan           =
// =   Original V3.15 Copyright (c) 2007, 2008 Ing. Wolfgang Mahringer   =
// =           http://code.google.com/p/uavp-mods/ http://uavp.ch        =
// =======================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "uavx.h"

// Prototypes

void GyroCompensation(void);
void InertialDamping(void);
void LimitRollSum(void);
void LimitPitchSum(void);
void LimitYawSum(void);
void GetGyroValues(void);
void ErectGyros(void);
void CalcGyroRates(void);
void DoControl(void);

void UpdateControls(void);
void CaptureTrims(void);
void StopMotors(void);
void LightsAndSirens(void);
void InitControl(void);

void GetGyroValues(void)
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

	if ( State == InFlight )
	{
		if ( NewRollRate > Stats[RollRateS].i16 ) Stats[RollRateS].i16 = NewRollRate;
		if ( NewPitchRate > Stats[PitchRateS].i16 ) Stats[PitchRateS].i16 = NewPitchRate;
	}
} // GetGyroValues

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
		RollAv += P[MiddleLR] * 2;
		PitchAv += P[MiddleFB] * 2;
	}
	
	GyroMidRoll = (int16)((RollAv + 16) >> 5);	
	GyroMidPitch = (int16)((PitchAv + 16) >> 5);
	GyroMidYaw = (int16)((YawAv + 64) >> 7);

	Stats[RollRateS].i16 = Stats[GyroMidRollS].i16 = GyroMidRoll;
	Stats[PitchRateS].i16 = Stats[GyroMidPitchS].i16 = GyroMidPitch;
	Stats[YawRateS].i16 = Stats[GyroMidYawS].i16 = GyroMidYaw;

	RollSum = PitchSum = YawSum = 0;
	REp = PEp = YEp = 0;
	LEDRed_OFF;

	F.GyrosErected = true;

} // ErectGyros

void GyroCompensation(void)
{
	static int16 GravComp, Temp;
	static int16 LRGrav, LRDyn, FBGrav, FBDyn;

	#define GYRO_COMP_STEP 1

	if( F.AccelerationsValid )
	{
		if ( P[GyroType] == IDG300 )
			GravComp = 8; 		// -1/6 of reference
		else
			GravComp = 11; 		// 9..11   

		ReadAccelerations();

		LRAcc = Ax.i16;
		DUAcc = Ay.i16;
		FBAcc = Az.i16;
		
		// NeutralLR, NeutralFB, NeutralDU pass through UAVPSet 
		// and come back as MiddleLR etc.

		LRAcc -= (int16)P[MiddleLR];
		FBAcc -= (int16)P[MiddleFB];
		DUAcc -= (int16)P[MiddleDU];

		DUAcc -= 1024L;	// subtract 1g - not corrrect for other than level
						// ??? could check for large negative Acc => upside down?

		if ( State == InFlight )
		{
			if ( LRAcc > Stats[LRAccS].i16 ) Stats[LRAccS].i16 = LRAcc; 
			if ( FBAcc > Stats[FBAccS].i16 ) Stats[FBAccS].i16 = FBAcc; 
			if ( DUAcc > Stats[DUAccS].i16 ) Stats[DUAccS].i16 = DUAcc;
		}
		
		// Roll

		// static compensation due to Gravity
		LRGrav = -SRS16(RollSum * GravComp, 5); 
	
		// dynamic correction of moved mass
		#ifdef DISABLE_DYNAMIC_MASS_COMP_ROLL
		LRDyn = 0;
		#else
		LRDyn = RollRate;	
		#endif

		// correct DC level of the integral
		LRIntKorr = SRS16(LRAcc + LRGrav + LRDyn, 3); // / 10;
		LRIntKorr = Limit(LRIntKorr, -GYRO_COMP_STEP, GYRO_COMP_STEP); 
	
		// Pitch

		// static compensation due to Gravity
		FBGrav = -SRS16(PitchSum * GravComp, 5); 
	
		// dynamic correction of moved mass		
		#ifdef DISABLE_DYNAMIC_MASS_COMP_PITCH
		FBDyn = 0;
		#else
		FBDyn = PitchRate;
		#endif

		// correct DC level of the integral	
		FBIntKorr = SRS16(FBAcc + FBGrav + FBDyn, 3); // / 10;
		FBIntKorr = Limit(FBIntKorr, -GYRO_COMP_STEP, GYRO_COMP_STEP); 

		#ifdef DEBUG_SENSORS
		Trace[TAx]= LRAcc;
		Trace[TAz] = FBAcc;
		Trace[TAy] = DUAcc;

		Trace[TLRIntKorr] = LRIntKorr * 8; // scale up for UAVPSet
		Trace[TFBIntKorr] = FBIntKorr * 8;	
		#endif // DEBUG_SENSORS	
	}	
	else
		LRIntKorr = FBIntKorr = DUAcc = 0;

} // GyroCompensation

void InertialDamping(void)
{ // Uses accelerometer to damp disturbances while hovering

	static int16 Temp;

	// Down - Up
	// Empirical - acceleration changes at ~approx Sum/8 for small angles
	DUVel += DUAcc + SRS16( Abs(RollSum) + Abs(PitchSum), 3);		
	DUVel = Limit(DUVel , -16384, 16383); 			
	Temp = SRS16(SRS16(DUVel, 4) * (int16) P[VertDampKp], 13);
	if( Temp > DUComp ) 
		DUComp++;
	else
		if( Temp < DUComp )
			DUComp--;			
	DUComp = Limit(DUComp, DAMP_VERT_LIMIT_LOW, DAMP_VERT_LIMIT_HIGH); 
	DUVel = DecayX(DUVel, (int16)P[VertDampDecay]);

	// Lateral compensation only when hovering?	
	if ( F.Hovering && F.AttitudeHold ) 
	{
 		if ( F.CloseProximity )
		{
			// Left - Right
			LRVel += LRAcc;
			LRVel = Limit(LRVel , -16384, 16383);  	
			Temp = SRS16(SRS16(LRVel, 4) * (int32)P[HorizDampKp], 13);
			if( Temp > LRComp ) 
				LRComp++;
			else
				if( Temp < LRComp )
					LRComp--;
			LRComp = Limit(LRComp, -DAMP_HORIZ_LIMIT, DAMP_HORIZ_LIMIT);
			LRVel = DecayX(LRVel, P[HorizDampDecay]);
	
			// Front - Back
			FBVel += FBAcc;
			FBVel = Limit(FBVel , -16384, 16383);  
			Temp = SRS16(SRS16(FBVel, 4) * (int32)P[HorizDampKp], 13);
			if( Temp > FBComp ) 
				FBComp++;
			else
				if( Temp < FBComp )
					FBComp--;
			FBComp = Limit(FBComp, -DAMP_HORIZ_LIMIT, DAMP_HORIZ_LIMIT);
			FBVel = DecayX(FBVel, P[HorizDampDecay]);
		}
		else
		{
			LRVel = FBVel = 0;
			LRComp = Decay1(LRComp);
			FBComp = Decay1(FBComp);
		}
	}
	else
	{
		LRVel = FBVel = 0;

		LRComp = Decay1(LRComp);
		FBComp = Decay1(FBComp);
	}
} // InertialDamping	

void LimitRollSum(void)
{
	static int16 Temp;

	RollSum += SRS16(RollRate, 1);		// use 9 bit res. for I controller
	RollSum = Limit(RollSum, -RollIntLimit256, RollIntLimit256);
	RollSum = Decay1(RollSum);		// damps to zero even if still rolled
	RollSum += LRIntKorr;				// last for accelerometer compensation
} // LimitRollSum

void LimitPitchSum(void)
{
	static int16 Temp;

	PitchSum += SRS16(PitchRate, 1);
	PitchSum = Limit(PitchSum, -PitchIntLimit256, PitchIntLimit256);
	PitchSum = Decay1(PitchSum);
	PitchSum += FBIntKorr;
} // LimitPitchSum

void LimitYawSum(void)
{
	static int16 Temp;

	if ( F.CompassValid )
	{
		// + CCW
		Temp = DesiredYaw - YawTrim;
		Temp = Abs(Temp);
		HoldYaw = HardFilter(HoldYaw, Temp);
		if ( HoldYaw > COMPASS_MIDDLE ) // acquire new heading
		{
			DesiredHeading = Heading;
			HE = HEp = 0; 
		}
		else
		{
			HE = MakePi(DesiredHeading - Heading);
			HE = Limit(HE, -(MILLIPI/6), MILLIPI/6); // 30 deg limit
			HE = SRS32( (int32)(HEp * 3 + HE) * (int32)P[CompassKp], 12);  
			YE -= Limit(HE, -COMPASS_MAXDEV, COMPASS_MAXDEV);
		}
	}

	YawSum += YE;
	YawSum = Limit(YawSum, -YawIntLimit256, YawIntLimit256);

	YawSum = DecayX(YawSum, 2); 				// GKE added to kill gyro drift

} // LimitYawSum

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
	YawRate = ADC(ADCYawChan, ADCVREF5V) >> 2;
	if (( State == InFlight ) && (YawRate > Stats[YawRateS].i16) ) Stats[YawRateS].i16 = YawRate;	
	YawRate -= GyroMidYaw;

} // CalcGyroRates

void DoControl(void)
{				
	CalcGyroRates();
	GyroCompensation();	
	InertialDamping();		

	// Roll
				
	RE = SRS16(RollRate, 2);
	LimitRollSum();
 
	Rl  = SRS16(RE *(int16)P[RollKp] + (REp-RE) * (int16)P[RollKd], 4);
	Rl += SRS16(RollSum * (int16)P[RollKi], 9); 
	Rl -= DesiredRoll;
	Rl -= LRComp;

	// Pitch

	PE = SRS16(PitchRate, 2);
	LimitPitchSum();

	Pl  = SRS16(PE *(int16)P[PitchKp] + (PEp-PE) * (int16)P[PitchKd], 4);
	Pl += SRS16(PitchSum * (int16)P[PitchKi], 9);
	Pl -= DesiredPitch;
	Pl -= FBComp;

	// Yaw

	YE = YawRate;
	YE += DesiredYaw;
	LimitYawSum();

	Yl  = SRS16(YE *(int16)P[YawKp] + (YEp-YE) * (int16)P[YawKd], 4);
	Yl += SRS16(YawSum * (int16)P[YawKi], 8);
	Yl = Limit(Yl, -(int16)P[YawLimit], (int16)P[YawLimit]);		// effective slew limit

	#ifdef DEBUG_SENSORS
	Trace[THE] = HE;
	Trace[TRollRate] = RollRate;
	Trace[TPitchRate] = PitchRate;
	Trace[TYE] = YE;
	Trace[TRollSum] = RollSum;
	Trace[TPitchSum] = PitchSum;
	Trace[TYawSum] = YawSum;
	#endif

	REp = RE;
	PEp = PE;
	YEp = YE;
	HEp = HE;

	RollRate = PitchRate = 0;

} // DoControl

void UpdateControls(void)
{
	static int16 HoldRoll, HoldPitch;

	if ( F.RCNewValues )
	{
		F.RCNewValues = false;

		MapRC();								// remap channel order for specific Tx/Rx

		DesiredThrottle = RC[ThrottleC];
		if ( DesiredThrottle < RC_THRES_STOP )	// to deal with usual non-zero EPA
			DesiredThrottle = 0;

		DesiredRoll = RC[RollC] - RC_NEUTRAL;
		DesiredPitch = RC[PitchC] - RC_NEUTRAL;
		DesiredYaw = RC[YawC] - RC_NEUTRAL;

		#ifdef RX6CH
			DesiredCamPitchTrim = RC_NEUTRAL;
			// NavSensitivity set in ReadParametersEE
		#else
			DesiredCamPitchTrim = RC[CamPitchC] - RC_NEUTRAL;
			NavSensitivity = RC[NavGainC];
			NavSensitivity = Limit(NavSensitivity, 0, RC_MAXIMUM);
		#endif // !RX6CH

		F.ReturnHome = RC[RTHC] > RC_NEUTRAL;

		HoldRoll = DesiredRoll - RollTrim;
		HoldRoll = Abs(HoldRoll);
		HoldPitch = DesiredPitch - PitchTrim;
		HoldPitch = Abs(HoldPitch);
		CurrMaxRollPitch = Max(HoldRoll, HoldPitch);

		if ( CurrMaxRollPitch > ATTITUDE_HOLD_LIMIT )
			if ( AttitudeHoldResetCount > ATTITUDE_HOLD_RESET_INTERVAL )
				F.AttitudeHold = false;
			else
			{
				AttitudeHoldResetCount++;
				F.AttitudeHold = true;
			}
		else
		{
			F.AttitudeHold = true;	
			if ( AttitudeHoldResetCount > 1 )
				AttitudeHoldResetCount -= 2;		// Faster decay
		}

		F.NewCommands = true;
	}
} // UpdateControls

void CaptureTrims(void)
{ // only used in detecting movement from neutral in hold GPS position
	RollTrim = Limit(DesiredRoll, -NAV_MAX_TRIM, NAV_MAX_TRIM);
	PitchTrim = Limit(DesiredPitch, -NAV_MAX_TRIM, NAV_MAX_TRIM);
	YawTrim = Limit(DesiredYaw, -NAV_MAX_TRIM, NAV_MAX_TRIM);

	HoldYaw = 0;
} // CaptureTrims

void StopMotors(void)
{
	static uint8 m;

	for (m = 0; m < NoOfMotors; m++)
		Motor[m] = ESCMin;

	MCamPitch = MCamRoll = OUT_NEUTRAL;
} // StopMotors

void LightsAndSirens(void)
{
	static uint24 Timeout;

	LEDYellow_TOG;
	if ( F.Signal ) LEDGreen_ON; else LEDGreen_OFF;

	Beeper_OFF; 
	Timeout = mS[Clock] + 500; 					// mS.
	do
	{
		ProcessCommand();
		if( F.Signal )
		{
			LEDGreen_ON;
			if( F.RCNewValues )
			{
				UpdateControls();
				UpdateParamSetChoice();
				InitialThrottle = DesiredThrottle;
				DesiredThrottle = 0; 			// prevent motors from starting
				OutSignals();
				if( mS[Clock] > Timeout )
				{
					if ( F.ReturnHome )
					{
						Beeper_TOG;					
						LEDRed_TOG;
					}
					else
						if ( Armed )
							LEDRed_TOG;
						
					Timeout += 500;
				}
			}
		}
		else
		{
			LEDRed_ON;
			LEDGreen_OFF;
		}	
		ReadParametersEE();	
	}
	while( (!F.Signal) || (Armed && FirstPass) || F.ReturnHome || ( InitialThrottle >= RC_THRES_START) );

	FirstPass = false;

	Beeper_OFF;
	LEDRed_OFF;
	LEDGreen_ON;

	F.LostModel = false;
	mS[FailsafeTimeout] = mS[Clock] + FAILSAFE_TIMEOUT_MS;
	mS[UpdateTimeout] = mS[Clock] + P[TimeSlots];
	FailState = Waiting;

} // LightsAndSirens

void InitControl(void)
{
	RollRate = PitchRate = 0;
	RollTrim = PitchTrim = YawTrim = 0;	
	BaroComp = 0;
	DUComp = DUVel = LRVel = LRComp = FBVel = FBComp = 0;	
	AE = AltSum = 0;
} // InitControl



