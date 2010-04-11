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
//    If not, see http://www.gnu.org/licenses/.

// Autonomous flight routines

#include "uavx.h"

void Navigate(int32, int32);
void SetDesiredAltitude(int16);
void DoFailsafeLanding(void);
void AcquireHoldPosition(void);
void NavGainSchedule(int16);
void DoNavigation(void);
void DoPPMFailsafe(void);
void UAVXNavCommand(void);
void GetWayPointEE(uint8);
void InitNavigation(void);

int16 NavRCorr, NavRCorrP, NavPCorr, NavPCorrP, NavYCorr, SumNavYCorr;
int16 EffNavSensitivity;
int16 EastP, EastDiffSum, EastI, EastCorr, NorthP, NorthDiffSum, NorthI, NorthCorr;
int24 EastD, EastDiffP, NorthD, NorthDiffP;

#pragma udata uavxnav_waypoints
typedef union { 
	uint8 b[256];
	struct {
		uint8 u0;
		int16 u1;
		int8 u3;
		int8 u4;
		uint16 u5;
		uint16 u6;
		uint8 NoOfWPs;
		uint8 ProximityAltitude;
		uint8 ProximityRadius;
		int16 OriginAltitude;
		int32 OriginLatitude;
		int32 OriginLongitude;
		int16 RTHAltitude;
		struct {
			int32 Latitude;
			int32 Longitude;
			int16 Altitude;
			int8 Loiter;
			} WP[23];
		};
	} UAVXNav;
#pragma udata
		
uint8 	CurrWP;
int8 	NoOfWayPoints;
int16	WPAltitude;
int32 	WPLatitude, WPLongitude;
int24 	WPLoiter;
int16	WayHeading;

int16 	NavClosingRadius, NavNeutralRadius, NavCloseToNeutralRadius, NavProximityRadius, NavProximityAltitude, CompassOffset, NavRTHTimeoutmS;

uint8 	NavState;
int16 	NavSensitivity, RollPitchMax;
int16 	AltSum;
int32	NavRTHTimeout;

void SetDesiredAltitude(int16 NewDesiredAltitude) // Metres
{
	if ( F.NavAltitudeHold )
	{
		AltSum = 0;
		DesiredThrottle = CruiseThrottle;
		DesiredAltitude = NewDesiredAltitude * 100L;
	}
	else
	{
		// manual control of altitude
	}
} // SetDesiredAltitude

void DoFailsafeLanding(void)
{ // InTheAir micro switch RC0 Pin 11 to ground when landed

	DesiredAltitude = -200;
	if ( !InTheAir || (( mS[Clock] > mS[LandingTimeout]) && ( NavState == Touchdown )) )
	{
		DesiredThrottle = 0;
		StopMotors();
	}
	else
		DesiredThrottle = CruiseThrottle;
} // DoFailsafeLanding

void AcquireHoldPosition(void)
{
	NavPCorr = NavPCorrP = NavRCorr = NavRCorrP = NavYCorr =  0;
	F.NavComputed = false;

	HoldLatitude = GPSLatitude;
	HoldLongitude = GPSLongitude;
	F.WayPointAchieved = F.WayPointCentred = F.AcquireNewPosition = false;

	NavState = HoldingStation;
} // AcquireHoldPosition

void Navigate(int32 NavLatitude, int32 NavLongitude )
{	// F.GPSValid must be true immediately prior to entry	
	// This routine does not point the quadrocopter at the destination
	// waypoint. It simply rolls/pitches towards the destination
	// cos/sin/arctan lookup tables are used for speed.
	// BEWARE magic numbers for integer arithmetic

	static int16 SinHeading, CosHeading, Radius;
	static int24 Temp;
	static int32 EastDiff, NorthDiff;
	static int16 RelHeading;

	DesiredLatitude = NavLatitude;
	DesiredLongitude = NavLongitude;

	F.NewCommands = false;	// Navigate modifies Desired Roll, Pitch and Yaw values.

	if ( !F.NavComputed ) 
	{
		EastDiff = DesiredLongitude - GPSLongitude;
		// EastDiff = SRS32((int32)EastDiff * GPSLongitudeCorrection, 8);
		NorthDiff = DesiredLatitude - GPSLatitude;

		Radius = Max(Abs(EastDiff), Abs(NorthDiff));
		if ( Radius < NavClosingRadius ) 
			Radius = int32sqrt( EastDiff * EastDiff + NorthDiff * NorthDiff );
		else
			Radius = NavClosingRadius;

		F.WayPointCentred =  Radius < NavNeutralRadius;
		F.WayPointAchieved = ( Radius < NavProximityRadius) && ( Abs(DesiredAltitude - Altitude) < NavProximityAltitude );

		//	EffNavSensitivity = (NavSensitivity * ( ATTITUDE_HOLD_LIMIT * 4 - 
		// 							CurrMaxRollPitch )) / (ATTITUDE_HOLD_LIMIT * 4);
		EffNavSensitivity = SRS16(NavSensitivity * ( 32 - Limit(CurrMaxRollPitch, 0, 32) ) + 16, 5);
		WayHeading = int32atan2((int32)EastDiff, (int32)NorthDiff);

		if ( ( EffNavSensitivity > NAV_GAIN_THRESHOLD ) && !F.WayPointCentred )
		{	

			#ifdef NAV_WING
		
			// no Nav for conventional aircraft - yet!

			// Just use simple rudder only for now.
			if ( !F.WayPointAchieved )
			{
				RelHeading = MakePi(WayHeading - Heading); // make +/- MilliPi
				NavYCorr = -(RelHeading * NAV_YAW_LIMIT) / HALFMILLIPI;
				NavYCorr = Limit(NavYCorr, -NAV_YAW_LIMIT, NAV_YAW_LIMIT); // gently!
			}

			#else // MULTICOPTER

			// direct solution make North and East coordinate errors zero

			SinHeading = int16sin(Heading);
			CosHeading = int16cos(Heading);

			// East
			if ( Abs(EastDiff) < NavClosingRadius )
			{
				Temp = ( EastDiff * NAV_MAX_ROLL_PITCH )/ NavCloseToNeutralRadius;
				EastP = Limit(Temp, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
	
				EastDiffSum += EastDiff;
				EastDiffSum = Limit(EastDiffSum, -NAV_INT_WINDUP_LIMIT, NAV_INT_WINDUP_LIMIT);
			}
			else
			{
				EastP = Limit(EastDiff, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
				EastDiffSum = Limit(EastDiff, -NAV_INT_WINDUP_LIMIT, NAV_INT_WINDUP_LIMIT);
			}
	
			EastI = SRS16((int32)EastDiffSum * (int16)P[NavKi], 6); 
			EastI = Limit(EastI, (int16)(-P[NavIntLimit]), (int16)P[NavIntLimit]);
			EastDiffSum = Decay1(EastDiffSum);
	
			EastD = SRS32((int32)(EastDiffP - EastDiff) * (int16)P[NavKd], 8);
			EastDiffP = EastDiff;
			EastD = Limit(EastD, -NAV_DIFF_LIMIT, NAV_DIFF_LIMIT);
		
			EastCorr = SRS16((EastP + EastI + EastD) * EffNavSensitivity, 8);
	
			// North
			if ( Abs(NorthDiff) < NavClosingRadius )
			{
				Temp = ( NorthDiff * NAV_MAX_ROLL_PITCH )/ NavCloseToNeutralRadius;
				NorthP = Limit(Temp, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
	
				NorthDiffSum += NorthDiff;
				NorthDiffSum = Limit(NorthDiffSum, -NAV_INT_WINDUP_LIMIT, NAV_INT_WINDUP_LIMIT);
			}
			else
			{
				NorthP = Limit(NorthDiff, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
				NorthDiffSum = Limit(NorthDiff, -NAV_INT_WINDUP_LIMIT, NAV_INT_WINDUP_LIMIT);
			}

			NorthI = SRS16((int32)NorthDiffSum * (int16)P[NavKi], 6);
			NorthI = Limit(NorthI, (int16)(-P[NavIntLimit]), (int16)P[NavIntLimit]);
			NorthDiffSum = Decay1(NorthDiffSum);
	
			NorthD = SRS32((int32)(NorthDiffP - NorthDiff) * (int16)P[NavKd], 8); 
			NorthDiffP = NorthDiff;
			NorthD = Limit(NorthD, -NAV_DIFF_LIMIT, NAV_DIFF_LIMIT);
	
			NorthCorr = SRS16((NorthP + NorthI + NorthD) * EffNavSensitivity, 8); 
				
			// Roll & Pitch
			NavRCorr = SRS16(CosHeading * EastCorr - SinHeading * NorthCorr, 8);
			NavRCorr = Limit(NavRCorr, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);	
			NavRCorr = SlewLimit(NavRCorrP, NavRCorr, NAV_CORR_SLEW_LIMIT);
			NavRCorrP = NavRCorr;
	
			NavPCorr = SRS16(-SinHeading * EastCorr - CosHeading * NorthCorr, 8);
			NavPCorr = Limit(NavPCorr, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
			NavPCorr = SlewLimit(NavPCorrP, NavPCorr, NAV_CORR_SLEW_LIMIT);
			NavPCorrP = NavPCorr;
				
			// Yaw
			if ( F.TurnToWP && !F.WayPointAchieved )
			{
				RelHeading = MakePi(WayHeading - Heading); // make +/- MilliPi
				NavYCorr = -(RelHeading * NAV_YAW_LIMIT) / HALFMILLIPI;
				NavYCorr = Limit(NavYCorr, -NAV_YAW_LIMIT, NAV_YAW_LIMIT); // gently!
			}
			else
				NavYCorr = 0;

			#endif // NAV_WING	
		}	
		else 
		{
			// Neutral Zone - no GPS influence
			NavPCorr = DecayX(NavPCorr, 2);
			NavRCorr = DecayX(NavRCorr, 2);
			NavYCorr = 0;
			EastDiffP = NorthDiffP = EastDiffSum = NorthDiffSum = 0;
		}
	}

	DesiredRoll = DesiredRoll + NavRCorr;
	DesiredPitch = DesiredPitch + NavPCorr;
	DesiredYaw += NavYCorr;

	F.NavComputed = true;

} // Navigate

void DoNavigation(void)
{
	switch ( NavState ) { // most case last - switches in C18 are IF chains not branch tables!
	case Touchdown:
		Navigate(OriginLatitude, OriginLongitude);
		if ( F.ReturnHome )
			DoFailsafeLanding();
		else
			AcquireHoldPosition();
		break;
	case Descending:
		Navigate( OriginLatitude, OriginLongitude );
		if ( F.ReturnHome )
			if ( RelBaroAltitude < LAND_CM )
			{
				mS[LandingTimeout] = mS[Clock] + NAV_RTH_LAND_TIMEOUT_MS;
				NavState = Touchdown;
			}
			else
				DoFailsafeLanding();
		else
			AcquireHoldPosition();
		break;
	case AtHome:
		Navigate(OriginLatitude, OriginLongitude);
		SetDesiredAltitude((int16)P[NavRTHAlt]);
		if ( F.ReturnHome )
		{
			if ( F.UsingRTHAutoDescend && ( mS[Clock] > mS[RTHTimeout] ) )
				NavState = Descending;
		}
		else
			AcquireHoldPosition();
		break;
	case ReturningHome:		
		Navigate(OriginLatitude, OriginLongitude);
		SetDesiredAltitude((int16)P[NavRTHAlt]);
		if ( F.ReturnHome )
		{
			if ( F.WayPointAchieved )
			{
				mS[RTHTimeout] = mS[Clock] + NavRTHTimeout;			
				NavState = AtHome;
			}	
		}
		else
			AcquireHoldPosition();					
		break;
	case Loitering:
		Navigate(WPLatitude, WPLongitude);
		SetDesiredAltitude(WPAltitude);
		if ( F.Navigate )
		{
			if ( F.WayPointAchieved && (mS[Clock] > mS[LoiterTimeout]) )
			{	
				GetWayPointEE(++CurrWP); // wrap around through origin and repeat
				NavState = Navigating;
			}
		}
		else
			AcquireHoldPosition();
		break;
	case Navigating:
		Navigate(WPLatitude, WPLongitude);
		SetDesiredAltitude(WPAltitude);			
		if ( F.Navigate )
		{
			if ( F.WayPointAchieved )
			{
				mS[LoiterTimeout] = mS[Clock] + WPLoiter;
				NavState = Loitering;
			}		
		}
		else
			AcquireHoldPosition();					
		break;
	case HoldingStation:
		if ( F.AttitudeHold )
		{		
			if ( F.AcquireNewPosition )
			{
				AcquireHoldPosition();
				#ifdef NAV_ACQUIRE_BEEPER
				if ( !F.BeeperInUse )
				{
					mS[BeeperTimeout] = mS[Clock] + 500L;
					Beeper_ON;				
				} 
				#endif // NAV_ACQUIRE_BEEPER
			}	
		}
		else
			F.AcquireNewPosition = true;
			
		Navigate(HoldLatitude, HoldLongitude);

		if ( F.NavValid )  // Origin must be valid for ANY navigation!
		{
			F.NearLevel = Max(Abs(RollSum), Abs(PitchSum)) < NAV_RTH_LOCKOUT;
			if ( F.ReturnHome && F.NearLevel )
				NavState = ReturningHome;
			else
				if ( F.Navigate && F.NearLevel )
				{
					GetWayPointEE(CurrWP); // resume from previous WP
					SetDesiredAltitude(WPAltitude);
					NavState = Navigating;
				}
		}		
		break;
	} // switch NavState

	DesiredRoll = Limit(DesiredRoll, -MAX_ROLL_PITCH, MAX_ROLL_PITCH);
	DesiredPitch = Limit(DesiredPitch, -MAX_ROLL_PITCH, MAX_ROLL_PITCH);

} // DoNavigation

void DoPPMFailsafe(void)
{ // only relevant to PPM Rx or Quad NOT synchronising with Rx
	if ( State == InFlight )
		switch ( FailState ) {
		case Terminated:
			#ifdef FIXED_WING
			DesiredRoll = DesiredPitch = DesiredYaw = 0; // zzz need spin/stall down
			#else
			DesiredRoll = DesiredPitch = DesiredYaw = 0;
			DoFailsafeLanding();
			#endif // FIXED_WING
			if ( mS[Clock ] > mS[AbortTimeout] )
				if ( F.Signal )
				{
					LEDRed_OFF;
					LEDGreen_ON;
					FailState = MonitoringRx;
				}
				else
					mS[AbortTimeout] += ABORT_UPDATE_MS;
			break;
		case Returning:
			DesiredRoll = DesiredPitch = DesiredYaw = DesiredThrottle = 0;
			F.NewCommands = true;
			if ( F.NavValid && F.GPSValid && F.CompassValid )
			{
				DoNavigation();	
				if ( mS[Clock ] > mS[AbortTimeout] )
					if ( F.Signal )
					{
						LEDRed_OFF;
						LEDGreen_ON;
						FailState = MonitoringRx;
					}
					else
						mS[AbortTimeout] += ABORT_UPDATE_MS;
			}
			else
			{
				mS[LandingTimeout] = mS[Clock] + NAV_RTH_LAND_TIMEOUT_MS;
				FailState = Terminated;
			}
			break;
		case Aborting:
			if( mS[Clock] > mS[AbortTimeout] )
			{
				F.LostModel = true;
				LEDGreen_OFF;
				LEDRed_ON;

				F.NavAltitudeHold = true;
				mS[AbortTimeout] += ABORT_TIMEOUT_MS;

				#ifdef NAV_PPM_FAILSAFE_RTH
				if ( F.NavValid )
				{
					NavSensitivity = RC_NEUTRAL;	// 50% gain
					F.Navigate = F.TurnToWP = true;
					GetWayPointEE(0);
					SetDesiredAltitude(WPAltitude);
					NavState = Navigating;
					FailState = Returning;
				}
				else
				#endif // NAV_PPM_FAILSAFE_RTH
				{
					mS[LandingTimeout] = mS[Clock] + NAV_RTH_LAND_TIMEOUT_MS;
					FailState = Terminated;
				}
			}
			break;
		case MonitoringRx:
			if ( mS[Clock] > mS[FailsafeTimeout] ) 
			{
				LEDRed_ON;
				mS[AbortTimeout] = mS[Clock] + ABORT_TIMEOUT_MS;
				DesiredRoll = DesiredPitch = DesiredYaw = 0;
				FailState = Aborting;
				// use last "good" throttle; 
			}
			break;
		} // Switch FailState
	else
		DesiredRoll = DesiredRollP = DesiredPitch = DesiredPitchP = DesiredYaw = DesiredThrottle = 0;			
} // DoPPMFailsafe

#pragma udata eebuffer
uint8 BufferEE[256];
#pragma udata

void UAVXNavCommand(void)
{ 	// NavPlan adapted from ArduPilot ConfigTool GUI - quadrocopter must be disarmed

	static uint16 b;
	static uint8 c, d, csum;

	c = RxChar();
	LEDBlue_ON;

	switch ( c ) {
	case '0': // hello
		TxChar(ACK);
		break;
	case '1': // write
		csum = 0;
		for ( b = 0; b < 256; b++) // cannot write fast enough so buffer
		{
			d = RxChar();
			csum ^= d;
			BufferEE[b] = d;
		}
		if ( csum == 0 )
		{
			for ( b = 0; b < 256; b++)
				WriteEE(NAV_ADDR_EE + b, BufferEE[b]);
			TxChar(ACK);
		}
		else
			TxChar(NAK);

		InitNavigation();
	
		break;
	case '2':
		csum = 0;
		for ( b = 0; b < 255; b++)
		{	
			d = ReadEE(NAV_ADDR_EE + b);
			csum ^= d;
			BufferEE[b] = d;
		}
		BufferEE[255] = csum;
		for ( b = 0; b < 256; b++)
			TxChar(BufferEE[b]);
		TxChar(ACK);
		break;
	case '3':
		csum = 0;
		for ( b = 0; b < 63; b++)
		{	
			d = ReadEE(STATS_ADDR_EE + b);
			csum ^= d;
			BufferEE[b] = d;
		}
		BufferEE[63] = csum;
		for ( b = 0; b < 64; b++)
			TxChar(BufferEE[b]);
		TxChar(ACK);
		break;
	default:
		break;
	} // switch

	LEDBlue_OFF;
} // UAVXNavCommand

void GetWayPointEE(uint8 wp)
{ 
	static uint16 w;
	
	if ( wp > NoOfWayPoints )
		CurrWP = wp = 0;
	if ( wp == 0 ) 
	{  // force to Origin
		WPLatitude = OriginLatitude;
		WPLongitude = OriginLongitude;
		WPAltitude = (int16)P[NavRTHAlt];
		WPLoiter = 30000;
	}
	else
	{	
		w = NAV_WP_START + (wp-1) * WAYPOINT_REC_SIZE;
		WPLatitude = Read32EE(w + 0);
		WPLongitude = Read32EE(w + 4);			
		WPAltitude = Read16EE(w + 8);

		#ifdef NAV_ENFORCE_ALTITUDE_CEILING
		if ( WPAltitude > NAV_CEILING )
			WPAltitude = NAV_CEILING;
		#endif // NAV_ENFORCE_ALTITUDE_CEILING
		WPLoiter = Read16EE(w + 10) * 1000;
	}

	F.WayPointCentred =  F.WayPointAchieved = false;

} // GetWaypointEE

void InitNavigation(void)
{
	static uint8 wp;

	HoldLatitude = HoldLongitude = WayHeading = 0;
	NavPCorr = NavPCorrP = NavRCorr = NavRCorrP = NavYCorr = 0;
	EastDiffP, NorthDiffP = 0;

	NavState = HoldingStation;
	AttitudeHoldResetCount = 0;
	CurrMaxRollPitch = 0;
	F.WayPointAchieved = F.WayPointCentred = false;
	F.NavComputed = false;

	NavProximityRadius = ConvertMToGPS(Read16EE(NAV_PROX_RADIUS));
	NavProximityAltitude = Read16EE(NAV_PROX_ALT);

	NoOfWayPoints = ReadEE(NAV_NO_WP);
	if ( NoOfWayPoints <= 0 )
		CurrWP = 0;
	else
		CurrWP = 1;
	GetWayPointEE(0);

} // InitNavigation

