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

#include "uavx.h"

void DoAltitudeHold(int24, int16);
void UpdateAltitudeSource(void);
void AltitudeHold(void);
void InertialDamping(void);
void LimitRollAngle(void);
void LimitPitchAngle(void);
void LimitYawAngle(void);
void DoOrientationTransform(void);
void DoControl(void);

void CheckThrottleMoved(void);
void LightsAndSirens(void);
void InitControl(void);

int16 RC[CONTROLS];

int16 Angle[3], Anglep[3], Rate[3], Ratep[3]; // Milliradians	
int16 Rl, Pl, Yl, Ylp;	

int16 YawFilterA;
int16 HoldYaw;
int16 RollIntLimit256, PitchIntLimit256, YawIntLimit256;

int16 CruiseThrottle, DesiredThrottle, IdleThrottle, InitialThrottle, StickThrottle;
int16 DesiredRoll, DesiredPitch, DesiredYaw, DesiredHeading, DesiredCamPitchTrim, Heading;
int16 ControlRoll, ControlPitch, ControlRollP, ControlPitchP;
int16 CurrMaxRollPitch;

int16 ThrLow, ThrHigh, ThrNeutral;

int16 AttitudeHoldResetCount;
int16 AltDiffSum, AltD, AltDSum;
int24 DesiredAltitude, Altitude;
int16 ROC;

boolean	FirstPass;
int8 BeepTick = 0;

void DoAltitudeHold(int24 Altitude, int16 ROC)
{ // Syncronised to baro intervals independant of active altitude source
	
	static int16 NewAltComp, LimBE, AltP, AltI, AltD;
	static int24 Temp, BE;

	#ifdef ALT_SCRATCHY_BEEPER
	if ( (--BeepTick <= 0) && !F.BeeperInUse ) 
		Beeper_TOG;
	#endif
			
	BE = DesiredAltitude - Altitude;
	LimBE = Limit(BE, -ALT_BAND_DM, ALT_BAND_DM);
		
	AltP = SRS16(LimBE * (int16)P[AltKp], 4);
	AltP = Limit(AltP, -ALT_MAX_THR_COMP, ALT_MAX_THR_COMP);
		
	AltDiffSum += LimBE;
	AltDiffSum = Limit(AltDiffSum, -ALT_INT_WINDUP_LIMIT, ALT_INT_WINDUP_LIMIT);
	AltI = SRS16(AltDiffSum * (int16)P[AltKi], 3);
	AltI = Limit(AltDiffSum, -(int16)P[AltIntLimit], (int16)P[AltIntLimit]);
				 
	AltD = SRS16(ROC * (int16)P[AltKd], 2);
	AltD = Limit(AltD, -ALT_MAX_THR_COMP, ALT_MAX_THR_COMP); 
			 
	if ( ROC < (int16)P[MaxDescentRateDmpS] )
	{
		AltDSum += 1;
		AltDSum = Limit(AltDSum, 0, ALT_MAX_THR_COMP * 2); 
	}
	else
		AltDSum = DecayX(AltDSum, 1);
			
	NewAltComp = AltP + AltI + AltD + AltDSum;
	NewAltComp = Limit(NewAltComp, -ALT_MAX_THR_COMP, ALT_MAX_THR_COMP);
	
	Comp[Alt] = SlewLimit(Comp[Alt], NewAltComp, 1);
		
	#ifdef ALT_SCRATCHY_BEEPER
	if ( (BeepTick <= 0) && !F.BeeperInUse) 
	{
		Beeper_TOG;
		BeepTick = 5;
	}
	#endif

} // DoAltitudeHold	

void UpdateAltitudeSource(void)
{
	if ( F.UsingRangefinderAlt )
	{
		Altitude = RangefinderAltitude / 10; 	// Decimetres for now
		ROC = RangefinderROC / 10; 				// Decimetres/Sec.
	}
	else
		if ( F.UsingGPSAlt && F.NavValid )
		{
			Altitude = GPSRelAltitude;
			ROC = GPSROC;
		}
		else
		{
			Altitude = BaroRelAltitude;
			ROC = BaroROC;
		}
} // UpdateAltitudeSource

void AltitudeHold()
{  // relies upon good cross calibration of baro and rangefinder!!!!!!
	static int16 NewCruiseThrottle;

	GetBaroAltitude();
	GetRangefinderAltitude();
	CheckThrottleMoved();

	if ( F.AltHoldEnabled )
	{
		if ( F.NewBaroValue  ) // sync on Baro which MUST be working
		{		
			F.NewBaroValue = false;

			UpdateAltitudeSource();

			if ( ( NavState != HoldingStation ) && F.AllowNavAltitudeHold ) 
			{  // Navigating - using CruiseThrottle
				F.HoldingAlt = true;
				DoAltitudeHold(Altitude, ROC);
			}	
			else
				if ( F.ThrottleMoving )
				{
					F.HoldingAlt = false;
					DesiredAltitude = Altitude;
					Comp[Alt] = Decay1(Comp[Alt]);
				}
				else
				{
					F.HoldingAlt = true;
					if ( Abs(BaroROC) < ALT_HOLD_MAX_ROC_DMPS  ) // Use Baro NOT GPS
					{
						NewCruiseThrottle = DesiredThrottle + Comp[Alt];
						CruiseThrottle = HardFilter(CruiseThrottle, NewCruiseThrottle);
					}
					DoAltitudeHold(Altitude, ROC);
				}	
		}
	}
	else
	{
		Comp[Alt] = Decay1(Comp[Alt]);
		F.HoldingAlt = false;
	}
} // AltitudeHold

void InertialDamping(void)
{ // Uses accelerometer to damp disturbances while holding altitude
	static int16 Temp;
	static uint8 i;

	if ( F.AccelerationsValid ) 
	{
		// Down - Up
		// Empirical - acceleration changes at ~approx Sum/8 for small angles
		Vel[UD] += ( Acc[UD] + GRAVITY ) + SRS16( Abs(Angle[Roll]) + Abs(Angle[Pitch]), 3);		
		Vel[UD] = Limit(Vel[UD] , -16384, 16383); 			
		Temp = SRS32(SRS16(Vel[UD], 4) * (int32)P[VertDampKp], 13);
		if( Temp > Comp[UD] ) 
			Comp[UD]++;
		else
			if( Temp < Comp[UD] )
				Comp[UD]--;			
		Comp[UD] = Limit(Comp[UD], DAMP_VERT_LIMIT_LOW, DAMP_VERT_LIMIT_HIGH); 
		Vel[UD] = DecayX(Vel[UD], (int16)P[VertDampDecay]);

		#ifdef USE_DCM
			Comp[LR] = Comp[BF] = 0;
		#else
	
		// Lateral compensation only when holding altitude?	
		if ( F.HoldingAlt && F.AttitudeHold ) 
		{
	 		if ( F.WayPointCentred )
			{
				// Left - Right
				Vel[LR] += Acc[LR];
				Vel[LR] = Limit(Vel[LR] , -16384, 16383);  	
				Temp = SRS32(SRS16(Vel[LR], 4) * (int32)P[HorizDampKp], 13);
				if( Temp > Comp[LR] ) 
					Comp[LR]++;
				else
					if( Temp < Comp[LR] )
						Comp[LR]--;
				Comp[LR] = Limit(Comp[LR], -DAMP_HORIZ_LIMIT, DAMP_HORIZ_LIMIT);
				Vel[LR] = DecayX(Vel[LR], (int16)P[HorizDampDecay]);
		
				// Front - Back
				Vel[BF] += Acc[BF];
				Vel[BF] = Limit(Vel[BF] , -16384, 16383);  
				Temp = SRS32(SRS16(Vel[BF], 4) * (int32)P[HorizDampKp], 13);
				if( Temp > Comp[BF] ) 
					Comp[BF]++;
				else
					if( Temp < Comp[BF] )
						Comp[BF]--;
				Comp[BF] = Limit(Comp[BF], -DAMP_HORIZ_LIMIT, DAMP_HORIZ_LIMIT);
				Vel[BF] = DecayX(Vel[BF], (int16)P[HorizDampDecay]);
			}
			else
			{
				Vel[LR] = Vel[BF] = 0;
				Comp[LR] = Decay1(Comp[LR]);
				Comp[BF] = Decay1(Comp[BF]);
			}
		}
		else
		{
			Vel[LR] = Vel[BF] = 0;
	
			Comp[LR] = Decay1(Comp[LR]);
			Comp[BF] = Decay1(Comp[BF]);
		}
		#endif // USE_DCM
	}
	else
		for ( i = 0; i < (uint8)3; i++ )
			Comp[i] = Vel[i] = 0;

} // InertialDamping	

#ifndef USE_DCM

void LimitRollAngle(void)
{
	Angle[Roll] += Rate[Roll];
	Angle[Roll] = Limit(Angle[Roll], -RollIntLimit256, RollIntLimit256);
	#ifdef ATTITUDE_ENABLE_DECAY
	Angle[Roll] = Decay1(Angle[Roll]);		// damps to zero even if still rolled
	#endif // ATTITUDE_ENABLE_DECAY
	Angle[Roll] += IntCorr[LR];				// last for accelerometer compensation
} // LimitRollAngle

void LimitPitchAngle(void)
{
	Angle[Pitch] += Rate[Pitch];
	Angle[Pitch] = Limit(Angle[Pitch], -PitchIntLimit256, PitchIntLimit256);
	#ifdef ATTITUDE_ENABLE_DECAY
	Angle[Pitch] = Decay1(Angle[Pitch]); 
	#endif // ATTITUDE_ENABLE_DECAY
	Angle[Pitch] += IntCorr[BF];
} // LimitPitchAngle

void LimitYawAngle(void)
{
	static int16 Temp, HE;

	if ( F.CompassValid )
	{
		// + CCW
		Temp = DesiredYaw - Trim[Yaw];
		if ( Abs(Temp) > COMPASS_MIDDLE ) // acquire new heading
		{
			DesiredHeading = Heading;
			HE = 0; 
		}
		else
		{
			HE = MakePi(DesiredHeading - Heading);
			HE = Limit(HE, -SIXTHMILLIPI, SIXTHMILLIPI); // 30 deg limit
			HE = SRS32( (int32)HE * (int32)P[CompassKp], 12); 
			Rate[Yaw] -= Limit(HE, -COMPASS_MAXDEV, COMPASS_MAXDEV);
		}
	}

	#ifndef USE_DCM
	Angle[Yaw] += Rate[Yaw];
	Angle[Yaw] = Limit(Angle[Yaw], -YawIntLimit256, YawIntLimit256);
	#endif // !USE_DCM

	Angle[Yaw] = DecayX(Angle[Yaw], 2); 				// GKE added to kill gyro drift

} // LimitYawAngle

#endif // !USE_DCM

void DoOrientationTransform(void)
{
	static i24u Temp;
	static int24 OSO, OCO;

	if ( F.UsingPolar )
	{
		OSO = OSin[PolarOrientation];
		OCO = OCos[PolarOrientation];
	}
	else
	{
		OSO = OSin[Orientation];
		OCO = OCos[Orientation];
	}

	if ( !F.NavigationActive )
		NavRCorr = NavPCorr = NavYCorr = 0;

	// -PS+RC
	Temp.i24 = -DesiredPitch * OSO + DesiredRoll * OCO;
	ControlRoll = Temp.i2_1 + NavRCorr - Comp[LR];
		
	// PC+RS
	Temp.i24 = DesiredPitch * OCO + DesiredRoll * OSO;
	ControlPitch = Temp.i2_1 + NavPCorr - Comp[BF]; 

} // DoOrientationTransform

void DoControl(void)
{
	static i24u Temp;
	static uint8 i;

	GetGyroValues();
	CalculateGyroRates();
	CompensateRollPitchGyros();	
    InertialDamping();

	#ifdef SIMULATE

	FakeDesiredRoll = DesiredRoll + NavRCorr;
	FakeDesiredPitch = DesiredPitch + NavPCorr;
	FakeDesiredYaw =  DesiredYaw + NavYCorr;
	Angle[Roll] = SlewLimit(Angle[Roll], FakeDesiredRoll * 16, 4);
	Angle[Pitch] = SlewLimit(Angle[Pitch], FakeDesiredPitch * 16, 4);
	Angle[Yaw] = SlewLimit(Angle[Yaw], FakeDesiredYaw, 4);
	Rl = FakeDesiredRoll;
	Pl = FakeDesiredPitch;
	Yl = DesiredYaw;
	 
    #else

	DoOrientationTransform();

	// Roll
				
	#ifndef USE_DCM
		LimitRollAngle();
	#endif // !USE_DCM
 
	Rl  = -SRS16(Rate[Roll] * (int16)P[RollKp] - (Ratep[Roll]-Rate[Roll]) * (int16)P[RollKd], 5);
	Rl -= SRS16(Angle[Roll] * (int16)P[RollKi], 9); 
	Rl += ControlRoll + SRS16((ControlRoll - ControlRollP) * ATTITUDE_FF_DIFF, 4);
	ControlRollP = ControlRoll;
	Ratep[Roll] = Rate[Roll];

	// Pitch
 
	#ifndef USE_DCM
		LimitPitchAngle();
	#endif // !USE_DCM

	Pl  = -SRS16(Rate[Pitch] * (int16)P[PitchKp] - (Rate[Pitch]-Ratep[Pitch]) * (int16)P[PitchKd], 5);
	Pl -= SRS16(Angle[Pitch] * (int16)P[PitchKi], 9);
	Pl += ControlPitch + SRS16((ControlPitch - ControlPitchP) * ATTITUDE_FF_DIFF, 4);
	ControlPitchP = ControlPitch;
	Ratep[Pitch] = Rate[Pitch];

	// Yaw
	
	#ifdef USE_DCM
		Rate[Yaw] += NavYCorr; 
		if ( Abs(DesiredYaw) > 5 )
			Rate[Yaw] += DesiredYaw;	
	#else
		LimitYawAngle();
		Rate[Yaw] += DesiredYaw + NavYCorr; 
	#endif // !USE_DCM

	Yl  = SRS16(Rate[Yaw] *(int16)P[YawKp] + (Rate[Yaw]-Ratep[Yaw]) * (int16)P[YawKd], 4);
	Yl += SRS16(Angle[Yaw] * (int16)P[YawKi], 8);
	Ratep[Yaw] = Rate[Yaw];

	#ifdef TRICOPTER
		Yl = SlewLimit(Ylp, Yl, 2);
		Ylp = Yl;
		Yl = Limit(Yl, -(int16)P[YawLimit]*4, (int16)P[YawLimit]*4);
	#else
		Yl = Limit(Yl, -(int16)P[YawLimit], (int16)P[YawLimit]);
	#endif // TRICOPTER
		
	#endif // SIMULATE		

	F.NearLevel = Max(Abs(Angle[Roll]), Abs(Angle[Pitch])) < NAV_RTH_LOCKOUT;

} // DoControl

static int8 RCStart = RC_INIT_FRAMES;

void LightsAndSirens(void)
{
	static int24 Ch5Timeout, AccTimeout;

	LEDYellow_TOG;
	if ( F.Signal ) LEDGreen_ON; else LEDGreen_OFF;

	Beeper_OFF;
	Ch5Timeout = AccTimeout = mSClock()+ 500; 					// mS.
	do
	{
		ProcessCommand();
		if( F.Signal )
		{
			LEDGreen_ON;
			if( F.RCNewValues )
			{
				UpdateControls();
				if ( --RCStart == 0 ) // wait until RC filters etc. have settled
				{
					UpdateParamSetChoice();
					MixAndLimitCam();
					RCStart = 1;
				}
				GetBaroAltitude();
				InitialThrottle = StickThrottle;
				StickThrottle = 0; 
				OutSignals();
				if( mSClock() > Ch5Timeout )
				{
					if ( F.Navigate || F.ReturnHome || !F.ParametersValid )
					{
						Beeper_TOG;					
						LEDRed_TOG;
					}
					else
						if ( Armed )
							LEDRed_TOG;
							
					Ch5Timeout += 500;
				}	
			}
		}
		else
		{
			LEDRed_ON;
			LEDGreen_OFF;
		}	
		ReadParametersEE();
		
		if ( !F.AccelerationsValid && ( mSClock() > AccTimeout ) )
		{
			InitAccelerometers();
			LEDYellow_TOG;
			AccTimeout += 400;
		}
	}
	while( (!F.Signal) || (Armed && FirstPass) || F.Ch5Active || 
		F.GyroFailure || (!F.AccelerationsValid) ||
		( InitialThrottle >= RC_THRES_START ) || (!F.ParametersValid) );
				
	FirstPass = false;

	Beeper_OFF;
	LEDRed_OFF;
	LEDGreen_ON;
	LEDYellow_ON;

	mS[LastBattery] = mSClock();
	mS[FailsafeTimeout] = mSClock() + FAILSAFE_TIMEOUT_MS;
	mS[UpdateTimeout] = mSClock() + (uint24)P[TimeSlots];

	F.LostModel = false;
	FailState = MonitoringRx;

} // LightsAndSirens

void InitControl(void)
{
	static uint8 i;

	for ( i = 0; i < (uint8)3; i++ )
		Angle[i] = Anglep[i] = Rate[i] = Trim[i] = Vel[i] = Comp[i] = 0;

	Comp[Alt] = AltSum = Ylp = 	ControlRollP = ControlPitchP = 0;
	YawGyroF.i32 = 0;	

} // InitControl



