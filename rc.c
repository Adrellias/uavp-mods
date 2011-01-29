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

void DoRxPolarity(void);
void InitRC(void);
void MapRC(void);
void CheckSticksHaveChanged(void);
void UpdateControls(void);
void CaptureTrims(void);
void CheckThrottleMoved(void);

const rom uint8 Map[CustomTxRx+1][CONTROLS] = {
	{ 2,0,1,3,4,5,6 }, 	// Futaba Thr 3 Throttle
	{ 1,0,3,2,4,5,6 },	// Futaba Thr 2 Throttle
	{ 4,2,1,0,5,3,6 },	// Futaba 9C Spektrum DM8/AR7000
	{ 0,1,2,3,4,5,6 },	// JR XP8103/PPM
	{ 6,0,3,5,2,4,1 },	// JR 9XII Spektrum DM9 ?

	{ 5,0,3,6,2,1,4 },	// JR DXS12 
	{ 5,0,3,6,2,1,4 },	// Spektrum DX7/AR7000
	{ 4,0,3,5,2,1,6 },	// Spektrum DX7/AR6200

	{ 2,0,1,3,4,6,5 }, 	// Futaba Thr 3 Sw 6/7
	{ 0,1,2,3,4,5,6 },	// Spektrum DX7/AR6000
	{ 0,1,2,3,4,5,6 },	// Graupner MX16S
	{ 4,0,2,3,1,5,6 },	// Spektrum DX6i/AR6200
	{ 2,0,1,3,4,5,6 },	// Futaba Th 3/R617FS
	{ 4,0,2,3,5,1,6 },	// Spektrum DX7a/AR7000
	{ 2,0,1,3,4,6,5 }, 	// External decoder (Futaba Thr 3 6/7 swap)
	{ 5,0,3,6,2,1,4 },	// UNDEFINED Spektrum DX7/AR7000
	{ 2,0,1,3,4,5,6 }	// Custom
//{ 4,0,2,1,3,5,6 }	// Custom
	};

// Rx signalling polarity used only for serial PPM frames usually
// by tapping internal Rx circuitry.
const rom boolean PPMPosPolarity[CustomTxRx+1] =
	{
		false, 	// Futaba Ch3 Throttle
		false,	// Futaba Ch2 Throttle
		true,	// Futaba 9C Spektrum DM8/AR7000
		true,	// JR XP8103/PPM
		true,	// JR 9XII Spektrum DM9/AR7000

		true,	// JR DXS12
		true,	// Spektrum DX7/AR7000
		true,	// Spektrum DX7/AR6200
		false,	// Futaba Thr 3 Sw 6/7
		true,	// Spektrum DX7/AR6000
		true,	// Graupner MX16S
		true,	// Graupner DX6i/AR6200
		true,	// Futaba Thr 3/R617FS
		true, 	// Spektrum DX7a/AR7000
		false, 	// External decoder (Futaba Ch3 Throttle)
		true,	// UNKNOWN using Spektrum DX7/AR7000
		true	// custom Tx/Rx combination
	};

// Reference Internal Quadrocopter Channel Order
// 0 Throttle
// 1 Aileron
// 2 Elevator
// 3 Rudder
// 4 Gear
// 5 Aux1
// 6 Aux2

int8 RMap[CONTROLS];

#pragma udata ppmq
int16x8x4Q PPMQ;
int16 PPMQSum[CONTROLS];
#pragma udata

void DoRxPolarity(void)
{
	if ( F.UsingSerialPPM  ) // serial PPM frame from within an Rx
		CCP1CONbits.CCP1M0 = PPMPosPolarity[P[TxRxType]];
	else
		CCP1CONbits.CCP1M0 = 1;	
}  // DoRxPolarity

void InitRC(void)
{
	int8 c, q;

	DoRxPolarity();

 	mS[StickChangeUpdate] = mSClock();
	mS[RxFailsafeTimeout] = mSClock() + RC_NO_CHANGE_TIMEOUT_MS;
	F.ForceFailsafe = false;

	SignalCount = -RC_GOOD_BUCKET_MAX;
	F.Signal = F.RCNewValues = false;
	
	for (c = 0; c < RC_CONTROLS; c++)
	{
		PPM[c].i16 = 0;
		RC[c] = RCp[c] = RC_NEUTRAL;

		#ifdef CLOCK_16MHZ
		for (q = 0; q <= PPMQMASK; q++)
			PPMQ.B[q][c] = RC_NEUTRAL;
		PPMQSum[c] = RC_NEUTRAL * 4;
		#else
		for (q = 0; q <= PPMQMASK; q++)
			PPMQ.B[q][c] = 625;
		PPMQSum[c] = 2500;
		#endif // CLOCK_16MHZ
	}
	PPMQ.Head = 0;

	DesiredRoll = DesiredPitch = DesiredYaw = DesiredThrottle = StickThrottle = 0;
	Rate[Roll] = Rate[Pitch] = Rate[Yaw] = 0;
	ControlRollP = ControlPitchP = 0;
	Trim[Roll] = Trim[Pitch] = Trim[Yaw] = 0;

	PPM_Index = PrevEdge = RCGlitches = 0;
} // InitRC

void MapRC(void) // re-arrange arithmetic reduces from 736uS to 207uS @ 40MHz
{  // re-maps captured PPM to Rx channel sequence
	static int8 c;
	static int16 LastThrottle, Temp, i;
	static uint16 Sum;
	static i32u Temp2; 

	LastThrottle = RC[ThrottleC];

	for (c = 0 ; c < RC_CONTROLS ; c++) 
	{
		Sum = PPM[c].u16;
		PPMQSum[c] -= PPMQ.B[PPMQ.Head][c];
		PPMQ.B[PPMQ.Head][c] = Sum;
		PPMQSum[c] += Sum;
		PPMQ.Head = (PPMQ.Head + 1) & PPMQMASK;
	}

	for (c = 0 ; c < RC_CONTROLS ; c++) 
	{
		i = Map[P[TxRxType]][c];
		#ifdef CLOCK_16MHZ
			RC[c] = SRS16(PPMQSum[i], 2) & 0xff; // clip to bottom byte 0..255
		#else // CLOCK_40MHZ	
			//RC[c] = ( (int32)PPMQSum[i] * RC_MAXIMUM + 2500L )/5000L; // scale to 4uS res. for now	
			Temp2.i32 = (int32)PPMQSum[i] * (RC_MAXIMUM * 13L) + 32768L; 
			RC[c] = Temp2.iw1;
		#endif // CLOCK_16MHZ		
	}

	if ( THROTTLE_SLEW_LIMIT > 0 )
		RC[ThrottleC] = SlewLimit(LastThrottle, RC[ThrottleC], THROTTLE_SLEW_LIMIT);

} // MapRC

void CheckSticksHaveChanged(void)
{

// DISABLED AS UNSAFE ON THE BENCH
#ifdef ENABLE_STICK_CHANGE_FAILSAFE

	#ifndef TESTING

	static boolean Change;
	static uint8 c;

	if ( F.ReturnHome || F.Navigate  )
	{
		if ( mSClock() > mS[StickChangeUpdate] )
		{
			mS[StickChangeUpdate] = mSClock() + 500;
			if ( !F.ForceFailsafe && ( State == InFlight ))
			{
				Stats[RCFailsafesS]++;
				mS[NavStateTimeout] = mSClock() + NAV_RTH_LAND_TIMEOUT_MS;
				mS[DescentUpdate]  = mSClock() + ALT_DESCENT_UPDATE_MS;
				DescentComp = 0; // for no Baro case
			}
		
			F.ForceFailsafe = State == InFlight; // abort if not navigating
		}
	}
	else
	{
		if ( mSClock() > mS[StickChangeUpdate] )
		{
			mS[StickChangeUpdate] = mSClock() + 500;
	
			Change = false;
			for ( c = ThrottleC; c <= (uint8)RTHC; c++ )
			{
				Change |= Abs( RC[c] - RCp[c]) > RC_STICK_MOVEMENT;
				RCp[c] = RC[c];
			}
		}
	
		if ( Change )
		{
			mS[RxFailsafeTimeout] = mSClock() + RC_NO_CHANGE_TIMEOUT_MS;
			mS[NavStateTimeout] = mSClock();
			F.ForceFailsafe = false;

			if ( FailState == MonitoringRx )
			{
				if ( F.LostModel )
				{
					Beeper_OFF;
					F.LostModel = false;
					DescentComp = 0;
				}
			}
		}
		else
			if ( mSClock() > mS[RxFailsafeTimeout] )
			{
				if ( !F.ForceFailsafe && ( State == InFlight ) )
				{
					Stats[RCFailsafesS]++;
					mS[NavStateTimeout] = mSClock() + NAV_RTH_LAND_TIMEOUT_MS;
					mS[DescentUpdate]  = mSClock() + ALT_DESCENT_UPDATE_MS;
					DescentComp = 0; // for no Baro case
					F.ForceFailsafe = true;
				}
			}
	}
	#endif // !TESTING

#else

	F.ForceFailsafe = false;

#endif // ENABLE_STICK_CHANGE_FAILSAFE

} // CheckSticksHaveChanged

void UpdateControls(void)
{
	static int16 HoldRoll, HoldPitch, RollPitchScale;
	static boolean NewCh5Active;

	F.RCNewValues = false;

	MapRC();								// remap channel order for specific Tx/Rx

	StickThrottle = RC[ThrottleC];

	//_________________________________________________________________________________________

	// Navigation

	F.ReturnHome = F.Navigate = false;
	NewCh5Active = RC[RTHC] > RC_NEUTRAL;

	if ( F.UsingPositionHoldLock )
		if ( NewCh5Active & !F.Ch5Active )
			F.AllowTurnToWP = true;
		else
			F.AllowTurnToWP = SaveAllowTurnToWP;
	else
		if ( RC[RTHC] > ((2L*RC_MAXIMUM)/3L) )
			F.ReturnHome = true;
		else
			if ( RC[RTHC] > (RC_NEUTRAL/3L) )
				F.Navigate = true;
	
	F.Ch5Active = NewCh5Active;

	#ifdef RX6CH
		DesiredCamPitchTrim = RC_NEUTRAL;
		// NavSensitivity set in ReadParametersEE
	#else
		DesiredCamPitchTrim = RC[CamPitchC] - RC_NEUTRAL;
		NavSensitivity = RC[NavGainC];
		NavSensitivity = Limit(NavSensitivity, 0, RC_MAXIMUM);
	#endif // !RX6CH

	//_________________________________________________________________________________________

	// Altitude Hold
	
	F.AltHoldEnabled = NavSensitivity > NAV_SENS_ALTHOLD_THRESHOLD;

	if ( NavState == HoldingStation )
	{ // Manual
		if ( StickThrottle < RC_THRES_STOP )	// to deal with usual non-zero EPA
			StickThrottle = 0;
	}
	else // Autonomous
		if ( F.AllowNavAltitudeHold &&  F.AltHoldEnabled )
			StickThrottle = CruiseThrottle;

	if ( (! F.HoldingAlt) && (!(F.Navigate || F.ReturnHome )) ) // cancel any current altitude hold setting 
		DesiredAltitude = Altitude;	

	//_________________________________________________________________________________________
			
	// Attitude
		
	#ifdef ATTITUDE_NO_LIMITS
		DesiredRoll = RC[RollC] - RC_NEUTRAL;
		DesiredPitch = RC[PitchC] - RC_NEUTRAL;		
	#else
		RollPitchScale = MAX_ROLL_PITCH - (NavSensitivity >> 2);
		DesiredRoll = SRS16((RC[RollC] - RC_NEUTRAL) * RollPitchScale, 7);
		DesiredPitch = SRS16((RC[PitchC] - RC_NEUTRAL) * RollPitchScale, 7);
	#endif // ATTITUDE_NO_LIMITS
	DesiredYaw = RC[YawC] - RC_NEUTRAL;
						
	HoldRoll = DesiredRoll - Trim[Roll];
	HoldRoll = Abs(HoldRoll);
	HoldPitch = DesiredPitch - Trim[Pitch];
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

	//_________________________________________________________________________________________
			
	// Rx has gone to failsafe

	CheckSticksHaveChanged();
	
	F.NewCommands = true;

} // UpdateControls

void CaptureTrims(void)
{ 	// only used in detecting movement from neutral in hold GPS position
	// Trims are invalidated if Nav sensitivity is changed - Answer do not use trims ?
	Trim[Roll] = Limit(DesiredRoll, -NAV_MAX_TRIM, NAV_MAX_TRIM);
	Trim[Yaw] = Limit(DesiredPitch, -NAV_MAX_TRIM, NAV_MAX_TRIM);
	Trim[Yaw] = Limit(DesiredYaw, -NAV_MAX_TRIM, NAV_MAX_TRIM);

	HoldYaw = 0;
} // CaptureTrims

void CheckThrottleMoved(void)
{
	if( mSClock() < mS[ThrottleUpdate] )
		ThrNeutral = DesiredThrottle;
	else
	{
		ThrLow = ThrNeutral - THROTTLE_MIDDLE;
		ThrLow = Max(ThrLow, THROTTLE_MIN_ALT_HOLD);
		ThrHigh = ThrNeutral + THROTTLE_MIDDLE;
		if ( ( DesiredThrottle <= ThrLow ) || ( DesiredThrottle >= ThrHigh ) )
		{
			mS[ThrottleUpdate] = mSClock() + THROTTLE_UPDATE_MS;
			F.ThrottleMoving = true;
		}
		else
			F.ThrottleMoving = false;
	}
} // CheckThrottleMoved




