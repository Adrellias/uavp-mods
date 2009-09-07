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
void VerticalDamping(void);
void HorizontalDamping(void);
void LimitRollSum(void);
void LimitPitchSum(void);
void LimitYawSum(void);
void GetGyroValues(void);
void ErectGyros(void);
void CalcGyroRates(void);
void DoControl(void);

void CheckThrottleMoved(void);
void UpdateControls(void);
void CaptureTrims(void);
void StopMotors(void);
void LightsAndSirens(void);
void InitControl(void);

void GetGyroValues(void)
{
	static int16 NewRollRate, NewPitchRate;

	if ( P[GyroType] == IDG300 )
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
	
	if( !_AccelerationsValid )
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

	_GyrosErected = true;

} // ErectGyros

void GyroCompensation(void)
{
	static int16 GravComp, AbsRollSum, AbsPitchSum, Temp;
	static int16 LRGrav, LRDyn, FBGrav, FBDyn;

	#define GYRO_COMP_STEP 1

	if( _AccelerationsValid )
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

		LRAcc -= P[MiddleLR];
		FBAcc -= P[MiddleFB];
		DUAcc -= P[MiddleDU];

		DUAcc -= 1024;	// subtract 1g - not corrrect for other than level
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
		LRIntKorr = (LRAcc + LRGrav + LRDyn) / 10;
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
		FBIntKorr = (FBAcc + FBGrav + FBDyn) / 10;
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

void VerticalDamping(void)
{ // Uses vertical accelerometer to damp altitude changes due mainly to turbulence
	static int16 Temp, AbsRollSum, AbsPitchSum;

	AbsRollSum = Abs(RollSum);
	AbsPitchSum = Abs(PitchSum);
	
	// Empirical - vertical acceleration decreases at ~approx Sum/8
	if ( (AbsRollSum < 200) && ( AbsPitchSum < 200) ) // ~ 10deg
		DUVel += DUAcc + SRS16( AbsRollSum + AbsPitchSum, 3);
	
	DUVel = Limit(DUVel , -16384, 16384); 
	DUVel = Decay(DUVel, 10);
		
	Temp = SRS16(SRS16(DUVel, 4) * (int16) P[VertDampKp], 8);	
	if( Temp > DUComp ) 
		DUComp++;
	else
		if( Temp < DUComp )
			DUComp--;
		
	DUComp = Limit(DUComp, -5, 20);  // -20, 20

} // VerticalDamping

void HorizontalDamping(void)
{ // Uses accelerometer to damp lateral disturbances while hovering
	#define HORIZ_DAMPING_LIMIT 8
	static int16 Temp, AbsRollSum, AbsPitchSum;

	AbsRollSum = Abs(RollSum);
	AbsPitchSum = Abs(PitchSum);
	
	if ( AbsRollSum < 200 ) 				// ~ 10deg
	{
		LRVel += LRAcc;
		LRVel = Limit(LRVel, -16384, 16384);
		LRDisp += SRS16(LRVel, 8);
		LRComp = SRS16(LRDisp * P[HorizDampKp], 8);
		LRComp = Limit(LRComp, -HORIZ_DAMPING_LIMIT, HORIZ_DAMPING_LIMIT);
	}
	else
		LRComp = 0;

	LRVel = Decay(LRVel, 2);
	LRDisp = Decay(LRDisp, 2);

	if ( AbsPitchSum < 200 ) // ~ 10deg
	{
		FBVel += FBAcc;
		FBVel = Limit(FBVel, -16384, 16384);
		FBDisp += SRS16(FBVel, 8);
		FBComp = SRS16(FBDisp * P[HorizDampKp], 8);
		FBComp = Limit(FBComp, -HORIZ_DAMPING_LIMIT, HORIZ_DAMPING_LIMIT);
	}
	else
		FBComp = 0;

	FBVel = Decay(FBVel, 2);
	FBDisp = Decay(FBDisp, 2); 

} // HorizontalDamping

void LimitRollSum(void)
{
	static int16 Temp;

	RollSum += SRS16(RollRate, 1);		// use 9 bit res. for I controller
	RollSum = Limit(RollSum, -RollIntLimit256, RollIntLimit256);

//	Temp = Abs(RollSum);
//	if ( Temp > Stats[RollS].i16 ) Stats[RollS].i16 = Temp;

	RollSum = Decay(RollSum, 1);		// damps to zero even if still rolled
	RollSum += LRIntKorr;				// last for accelerometer compensation
} // LimitRollSum

void LimitPitchSum(void)
{
	static int16 Temp;

	PitchSum += SRS16(PitchRate, 1);
	PitchSum = Limit(PitchSum, -PitchIntLimit256, PitchIntLimit256);

//	Temp = Abs(PitchSum);
//	if ( Temp > Stats[PitchS].i16 ) Stats[PitchS].i16 = Temp;

	PitchSum = Decay(PitchSum, 1);
	PitchSum += FBIntKorr;
} // LimitPitchSum

void LimitYawSum(void)
{
	static int16 Temp;

	if ( _CompassValid )
	{
		// + CCW
		HoldYaw = HardFilter(HoldYaw, Abs(DesiredYaw - YawTrim));
		if ( HoldYaw > COMPASS_MIDDLE ) // acquire new heading
		{
			DesiredHeading = Heading;
			HE = HEp = 0; 
		}
		else
		{
			HE = MakePi(DesiredHeading - Heading);
			HE = Limit(HE, -(MILLIPI/6), MILLIPI/6); // 30 deg limit
			HE = SRS16( (HEp * 3 + HE) * (int16)P[CompassKp], 10); // CompassKp < 16 
			YE -= Limit(HE, -COMPASS_MAXDEV, COMPASS_MAXDEV);
		}
	}

	YawSum += (int16)YE;
	YawSum = Limit(YawSum, -YawIntLimit256, YawIntLimit256);

	YawSum = Decay(YawSum, 2); 				// GKE added to kill gyro drift

} // LimitYawSum



void CalcGyroRates(void)
{
	static int16 Temp;

	// RollRate & Pitchsamples hold the sum of 2 consecutive conversions
	// 300 Deg/Sec is the "reference" gyro full scale rate

	if ( P[GyroType] == IDG300 )
	{ // 500 Deg/Sec or 1.66 ~= 2 so do not average readings 
		RollRate -= GyroMidRoll * 2;
		PitchRate -= GyroMidPitch * 2;
		RollRate = -RollRate;			// adjust for reversed roll gyro sense
 	}
	else
	{ // 1.0
		// Average of two readings
		RollRate = ( RollRate >> 1 ) - GyroMidRoll;	
		PitchRate = ( PitchRate >> 1 ) - GyroMidPitch;

		if ( P[GyroType] == ADXRS150 )	// 150 deg/sec (0.5)
		{ // 150 Deg/Sec or 0.5
			RollRate /= 2; 
			PitchRate /= 2;
		}
	}

	if ( P[ConfigBits] & FlyXModeMask )
	{
		// "Real" Roll = 0.707 * (P + R), Pitch = 0.707 * (P - R)
		Temp = RollRate + PitchRate;	
		PitchRate -= RollRate;	
		RollRate = (Temp * 7)/10;
		PitchRate = (PitchRate * 7)/10; 
	}
	
	// Yaw is sampled only once every frame, 8 bit A/D resolution
	YawRate = ADC(ADCYawChan, ADCVREF5V) >> 2;
	if ( YawRate > Stats[YawRateS].i16) Stats[YawRateS].i16 = YawRate;	
	YawRate -= GyroMidYaw;

} // CalcGyroRates

void DoControl(void)
{				
	CalcGyroRates();
	GyroCompensation();	
	VerticalDamping();
	HorizontalDamping();

	// Roll
				
	RE = SRS16(RollRate, 2);
	LimitRollSum();
 
	Rl  = SRS16(RE *(int16)P[RollKp] + (REp-RE) * P[RollKd], 4);
	Rl += SRS16(RollSum * (int16)P[RollKi], 8); 
	Rl -= DesiredRoll;
	Rl -= LRComp;

	// Pitch

	PE = SRS16(PitchRate, 2);
	LimitPitchSum();

	Pl  = SRS16(PE *(int16)P[PitchKp] + (PEp-PE) * P[PitchKd], 4);
	Pl += SRS16(PitchSum * (int16)P[PitchKi], 8);
	Pl -= DesiredPitch;
	Pl -= FBComp;

	// Yaw

	YE = YawRate;
	YE += DesiredYaw;
	LimitYawSum();

	Yl  = SRS16(YE *(int16)P[YawKp] + (YEp-YE) * P[YawKd], 4);
	Yl += SRS16(YawSum * (int16)P[YawKi], 8);
	Yl = Limit(Yl, -P[YawLimit], P[YawLimit]);		// effective slew limit

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
	if ( _NewValues )
	{
		_NewValues = false;

		MapRC();								// remap channel order for specific Tx/Rx

		DesiredThrottle = RC[ThrottleC];
		if ( DesiredThrottle < RC_THRES_STOP )
			DesiredThrottle = 0;

		DesiredRoll = RC[RollC];
		DesiredPitch = RC[PitchC];
		DesiredYaw = RC[YawC];
		NavSensitivity = RC[NavGainC];
		_ReturnHome = RC[RTHC] > RC_NEUTRAL;
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
	static int16 CurrentThrottle;

	LEDYellow_TOG;
	if ( _Signal ) LEDGreen_ON; else LEDGreen_OFF;

	Beeper_OFF; 
	CurrentThrottle = RC_MAXIMUM;
	Timeout = mS[Clock] + 500; 					// mS.
	do
	{
		ProcessCommand();
		if( _Signal )
		{
			LEDGreen_ON;
			if( _NewValues )
			{
				UpdateControls();
				UpdateParamSetChoice();
				CurrentThrottle = DesiredThrottle;
				DesiredThrottle = 0; 			// prevent motors from starting
				OutSignals();
				if( mS[Clock] > Timeout )
				{
					if ( _ReturnHome )
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
	while( (!_Signal) || (Armed && FirstPass) || _ReturnHome || ( CurrentThrottle >= RC_THRES_START) );

	FirstPass = false;

	Beeper_OFF;
	LEDRed_OFF;
	LEDGreen_ON;

	_LostModel = false;
	mS[FailsafeTimeout] = mS[Clock] + FAILSAFE_TIMEOUT_MS;
	mS[UpdateTimeout] = mS[Clock] + P[TimeSlots];
	FailState = Waiting;

} // LightsAndSirens

void InitControl(void)
{
	RollRate = PitchRate = 0;
	RollTrim = PitchTrim = YawTrim = 0;	
	DUComp = LRComp = FBComp = BaroComp = 0;	
	DUVel = LRVel = FBVel = LRDisp, FBDisp = 0;
	AE = AltSum = 0;
} // InitControl



