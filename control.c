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

void DoAltitudeHold(void);
int24 AltitudeCF(int24);
void AltitudeHold(void);
void DoAttitudeAngle(uint8, uint8);
void DoYawRate(void);
void DoOrientationTransform(void);
void GainSchedule(void);
void DoControl(void);

void CheckThrottleMoved(void);
void LightsAndSirens(void);
void InitControl(void);

#pragma udata access motorvars	
near int16 Rl, Pl, Yl;
#pragma udata
			
int16 Angle[3], RawAngle[3];		
int16 CameraRollAngle, CameraPitchAngle, CameraRollAnglep, CameraPitchAnglep;

int16 Ylp;				
int24 OSO, OCO;

int16 YawRateIntE;
int16 HoldYaw;
int16 YawIntLimit256;

int16 ControlRoll, ControlPitch, CurrMaxRollPitch;

int16 AttitudeHoldResetCount;
int24 DesiredAltitude, Altitude, Altitudep; 
int16 AccAltComp, AltComp, BaroROC, BaroROCp, RangefinderROC, ROC, ROCIntE, MinROCCmpS;

int32 GS;

boolean	FirstPass;
int8 BeepTick = 0;

void DoAltitudeHold(void)
{ 	// Syncronised to baro intervals independant of active altitude source
	
	static int24 AltE;
	static int16 ROCE, pROC, iROC, DesiredROC;
	static int16 NewAltComp;
	static i24u Temp;

	#ifdef ALT_SCRATCHY_BEEPER
	if ( (--BeepTick <= 0) && !F.BeeperInUse ) 
		Beeper_TOG;
	#endif
					
	AltE = DesiredAltitude - Altitude;
		
	DesiredROC = Limit(AltE, MinROCCmpS, ALT_MAX_ROC_CMPS);
		
	ROCE = DesiredROC - ROC;		
	pROC = ROCE * (int16)P[AltKp]; 
		
	ROCIntE += ROCE  * (int16)P[AltKi];
	ROCIntE = Limit1(ROCIntE, (int16)P[AltIntLimit]);
	iROC = ROCIntE;
				
	NewAltComp = SRS16(pROC + iROC, 6);
	AltComp = Limit1(NewAltComp, ALT_MAX_THR_COMP);
					
	#ifdef ALT_SCRATCHY_BEEPER
	if ( (BeepTick <= 0) && !F.BeeperInUse) 
	{
		Beeper_TOG;
		BeepTick = 5;
	}
	#endif

} // DoAltitudeHold	

void AltitudeHold()
{  // relies upon good cross calibration of baro and rangefinder!!!!!!

	static int16 NewCruiseThrottle;

	GetBaroAltitude();
	GetRangefinderAltitude();
	CheckThrottleMoved();

	if ( F.UsingRangefinderAlt )
	{
		Altitude = RangefinderAltitude;	 
		ROC = RangefinderROC;
	}
	else
		if ( F.NewBaroValue )
		{
			Altitude = BaroRelAltitude;
			ROC = BaroROC;
		}
			
	if ( F.AltHoldEnabled )
	{
		if ( F.NewBaroValue || F.UsingRangefinderAlt )
		{		
			if ( F.ForceFailsafe || (( NavState != HoldingStation ) && F.AllowNavAltitudeHold )) 
			{  // Navigating - using CruiseThrottle
				F.HoldingAlt = true;
				DoAltitudeHold();
			}	
			else
				if ( F.ThrottleMoving )
				{
					#ifndef SIMULATE
					if ( Abs(ROC) < ALT_HOLD_MAX_ROC_CMPS ) 
					{
						NewCruiseThrottle = DesiredThrottle + AltComp;
						CruiseThrottle = HardFilter(CruiseThrottle, NewCruiseThrottle);
						CruiseThrottle = Limit(CruiseThrottle , IdleThrottle, THROTTLE_MAX_CRUISE );
					}
					#endif // !SIMULATE
					ROCIntE = 0;
					F.HoldingAlt = false;
					SetDesiredAltitude(Altitude);
					AltComp = Decay1(AltComp);
				}
				else
				{ // throttle is not moving therefore we are hovering!
					F.HoldingAlt = true;
					DoAltitudeHold(); 
				}
		}	
	}
	else
	{
		ROCIntE = 0;
		AltComp = Decay1(AltComp);
		F.HoldingAlt = false;
	}

	F.NewBaroValue = false;

} // AltitudeHold

void DoAttitudeAngle(uint8 a, uint8 c)
{	// Caution: Angles are the opposite to the normal aircraft coordinate conventions

	static int16 Temp;

	#ifdef DEBUG_GYROS
	Temp = RawAngle[a] + Rate[a];
	RawAngle[a] = Limit1(Temp, ANGLE_LIMIT);
	#endif // DEBUG_GYROS

	Temp = Angle[a] + Rate[a];
    Temp = Limit1(Temp, ANGLE_LIMIT);
    Temp = Decay1(Temp);
	Temp += IntCorr[c];			// last for accelerometer compensation
	Angle[a] = Temp;

} // DoAttitudeAngle

void DoYawRate(void)
{ 	// Yaw gyro compensation using compass
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
			HE = MinimumTurn(DesiredHeading - Heading);
			HE = Limit1(HE, SIXTHMILLIPI); // 30 deg limit
			HE = SRS32((int24)HE * (int24)P[CompassKp], 12); 
			Rate[Yaw] -= Limit1(HE, COMPASS_MAXDEV); // yaw gyro drift compensation
		}
	}
	else
		HE = 0;

	#ifdef NEW_YAW

	Angle[Yaw] = HE;

	#else

	Angle[Yaw] += Rate[Yaw];
	Angle[Yaw] = Limit1(Angle[Yaw], YawIntLimit256);

	Angle[Yaw] = DecayX(Angle[Yaw], 2); 				// GKE added to kill gyro drift

	#endif // NEW_YAW

} // DoYawRate

void DoOrientationTransform(void)
{
	static i24u Temp;

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
		NavCorr[Roll] = NavCorr[Pitch] = NavCorr[Yaw] = 0;

	// -PS+RC
	Temp.i24 = -DesiredPitch * OSO + DesiredRoll * OCO;
	ControlRoll = Temp.i2_1;
		
	// PC+RS
	Temp.i24 = DesiredPitch * OCO + DesiredRoll * OSO;
	ControlPitch = Temp.i2_1; 

} // DoOrientationTransform

#ifdef TESTING

void GainSchedule(void)
{
	GS = 256;
} // GainSchedule

#else

void GainSchedule(void)
{  // rudimentary gain scheduling (linear)
	static int16 AttDiff, ThrDiff;

	GS = 256;

	if ( (!F.NavigationActive) || ( F.NavigationActive && (NavState == HoldingStation ) ) )
	{
		// also density altitude?
	
		if ( P[Acro] > 0) // due to Foliage 2009 and Alexinparis 2010
		{
		 	AttDiff = CurrMaxRollPitch  - ATTITUDE_HOLD_LIMIT;
			GS = (int32)GS * ( 2048L - (AttDiff * (int16)P[Acro]) );
			GS = SRS32(GS, 11);
			GS = Limit(GS, 0, 256);
		}
	
		if ( P[GSThrottle] > 0 ) 
		{
		 	ThrDiff = DesiredThrottle - CruiseThrottle;
			GS = (int32)GS * ( 2048L + (ThrDiff * (int16)P[GSThrottle]) );
			GS = SRS32(GS, 11);
		}	
	}
	
} // GainSchedule

#endif

void DoControl(void)
{
	static int16 Temp;
	static i32u Temp32;
	static i24u Temp24;
	static int16 RateE;

	CalculateGyroRates();
	CompensateRollPitchGyros();	

	DoOrientationTransform();

	#ifdef SIMULATE

	FakeDesiredRoll = ControlRoll + NavCorr[Roll];
	FakeDesiredPitch = ControlPitch + NavCorr[Pitch];
	FakeDesiredYaw =  DesiredYaw + NavCorr[Yaw];
	Angle[Roll] = SlewLimit(Angle[Roll], -FakeDesiredRoll * 16, 4);
	Angle[Pitch] = SlewLimit(Angle[Pitch], -FakeDesiredPitch * 16, 4);
	Angle[Yaw] = SlewLimit(Angle[Yaw], FakeDesiredYaw, 4);
	Rl = -FakeDesiredRoll;
	Pl = -FakeDesiredPitch;
	Yl = DesiredYaw;

	CameraRollAngle = Angle[Roll];
	CameraPitchAngle = Angle[Pitch];
	 
    #else

	GainSchedule();

	// Roll
				
	DoAttitudeAngle(Roll, LR);

	Rl  = SRS32((int32)Rate[Roll] * P[RollKp] - (int32)(Rate[Roll] - Ratep[Roll]) * P[RollKd], 5);
	Temp = SRS32((int32)Angle[Roll] * P[RollKi], 9);
	Rl += Limit1(Temp, (int16)P[RollIntLimit]); 

	Temp24.i24 = (int24)Rl * GS;
	Rl = Temp24.i2_1;

	Rl -= (ControlRoll + NavCorr[Roll]);

	Ratep[Roll] = Rate[Roll];

	// Pitch

	DoAttitudeAngle(Pitch, FB);

	Pl  = SRS32((int32)Rate[Pitch] * P[PitchKp] - (int32)(Rate[Pitch] - Ratep[Pitch]) * P[PitchKd], 5);
	Temp = SRS32((int32)Angle[Pitch] * P[PitchKi], 9);
	Pl += Limit1(Temp, (int16)P[PitchIntLimit]);

	Temp24.i24 = (int24)Pl * GS;
	Pl = Temp24.i2_1;

	Pl -= ( ControlPitch + NavCorr[Pitch]);

	Ratep[Pitch] = Rate[Pitch];

	// Yaw - rate control

	#ifdef NAV_WING

		Yl = DesiredYaw + NavCorr[Yaw];

	#else
	
		DoYawRate();
	
		RateE = Rate[Yaw] + ( DesiredYaw + NavCorr[Yaw] );
	
		#ifdef NEW_YAW
	
		YawRateIntE += RateE;
		YawRateIntE = Limit1(YawRateIntE, P[YawIntLimit]);
	
		Yl  = SRS32( RateE * (int16)P[YawKp] + SRS16( YawRateIntE * P[YawKi], 4), 4);
	
		#else
	
		Yl  = SRS16( RateE * (int16)P[YawKp] + SRS16( Angle[Yaw] * P[YawKi], 4), 4);
	
		#endif // NEW_YAW
	
		Ratep[Yaw] = Rate[Yaw];
	
		#ifdef TRICOPTER
			Yl = SlewLimit(Ylp, Yl, 2);
			Ylp = Yl;
			Yl = Limit1(Yl,(int16)P[YawLimit]);
		#else
			Yl = Limit1(Yl, (int16)P[YawLimit]);
		#endif // TRICOPTER

	#endif // NAV_WING

	Temp24.i24 = Angle[Pitch] * OSO + Angle[Roll] * OCO;
	CameraRollAngle = Temp24.i2_1;
	Temp24.i24 = Angle[Pitch] * OCO - Angle[Roll] * OSO;
	CameraPitchAngle = Temp24.i2_1;

	#endif // SIMULATE	

	F.NearLevel = Max(Abs(Angle[Roll]), Abs(Angle[Pitch])) < NAV_RTH_LOCKOUT;

} // DoControl

static int8 RCStart = RC_INIT_FRAMES;

void LightsAndSirens(void)
{
	static int24 Ch5Timeout;

	LEDYellow_TOG;
	if ( F.Signal ) LEDGreen_ON; else LEDGreen_OFF;

	Beeper_OFF;
	Ch5Timeout = mSClock() + 500; // mS.
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
				StickThrottle = DesiredThrottle = 0; 
				OutSignals(); // synced to New RC signals
				if( mSClock() > (uint24)Ch5Timeout )
				{
					if ( F.Ch5Active || !F.ParametersValid )
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
	}
	while( ( P[TxRxType] == UnknownTxRx ) ||  (!F.Signal) || (Armed && FirstPass) || F.Ch5Active || F.GyroFailure || 
		( InitialThrottle >= RC_THRES_START ) || (!F.ParametersValid)  );
				
	FirstPass = false;

	Beeper_OFF;
	LEDRed_OFF;
	LEDGreen_ON;
	LEDYellow_ON;

	mS[LastBattery] = mSClock();
	mS[FailsafeTimeout] = mSClock() + FAILSAFE_TIMEOUT_MS;
	PIDUpdate = mSClock() + PID_CYCLE_MS;
	F.LostModel = false;
	FailState = MonitoringRx;

} // LightsAndSirens

void InitControl(void)
{
	CameraRollAngle = CameraPitchAngle = 0;
	Ylp = AltComp = ROCIntE = 0;
	YawRateIntE = 0;

} // InitControl



