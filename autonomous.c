// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =                 Copyright (c) 2008 by Prof. Greg Egan               =
// =       Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer     =
// =           http://code.google.com/p/uavp-mods/ http://uavp.ch        =
// =======================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    UAVXP is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Autonomous flight routines

#include "uavx.h"

// Prototypes

void Navigate(int16, int16);
void SetDesiredAltitude(int16);
void Descend(void);
void AcquireHoldPosition(void);
void NavGainSchedule(int16);
void DoNavigation(void);
void DoPPMFailsafe(void);
void InitNavigation(void);

// Variables
int16 NavRCorr, NavRCorrP, NavPCorr, NavPCorrP, NavYCorr, SumNavYCorr;
int16 Range, RangeToNeutral, EffNavSensitivity;
int16 EastP, SumEastP, EastI, EastD, EastDiffP, EastCorr, NorthP, SumNorthP, NorthI, NorthD, NorthDiffP, NorthCorr;

WayPoint WP[NAV_MAX_WAYPOINTS];

void SetDesiredAltitude(int16 DesiredAltitude) // Centimetres
{
	static int16 Temp;

	if ( F.RTHAltitudeHold )
		if ( F.UsingGPSAlt || !F.BaroAltitudeValid )
		{
			AE = DesiredAltitude - GPSRelAltitude;
			AE = Limit(AE, -GPS_ALT_BAND_CM, GPS_ALT_BAND_CM);
			AltSum += AE;
			AltSum = Limit(AltSum, -10, 10);	
			Temp = SRS16(AE*(int16)P[GPSAltKp] + AltSum*(int16)P[GPSAltKi], 7);
		
			DesiredThrottle = HoverThrottle + Limit(Temp, ALT_LOW_THR_COMP, ALT_HIGH_THR_COMP);
			DesiredThrottle = Limit(DesiredThrottle, 0, OUT_MAXIMUM);
		}
		else
		{
			DesiredThrottle = HoverThrottle;
			DesiredRelBaroAltitude = DesiredAltitude;
		}
	else
	{
		// manual control of altitude
	}
} // SetDesiredAltitude

void Descend(void)
{ // uses Baro only

	if ( InTheAir ) 							//  micro switch RC0 Pin 11 to ground when landed
	{
		DesiredThrottle = HoverThrottle;
		DesiredRelBaroAltitude = 0;
	}
	else
		DesiredThrottle = 0;

} // Descend

void AcquireHoldPosition(void)
{
	NavPCorr = NavPCorrP = NavRCorr = NavRCorrP = NavYCorr =  0;
	F.NavComputed = false;

	GPSNorthHold = GPSNorth;
	GPSEastHold = GPSEast;
	F.Proximity = F.CloseProximity = F.AcquireNewPosition = false;

	NavState = HoldingStation;
} // AcquireHoldPosition

void Navigate(int16 GPSNorthWay, int16 GPSEastWay)
{	// F.GPSValid] must be true immediately prior to entry	
	// This routine does not point the quadrocopter at the destination
	// waypoint. It simply rolls/pitches towards the destination
	// cos/sin/arctan lookup tables are used for speed.
	// BEWARE magic numbers for integer arithmetic

	static int16 SinHeading, CosHeading;
	static int16 Temp, EastDiff, NorthDiff, WayHeading, RelHeading;

	F.NewCommands = false;	// Navigate modifies Desired Roll, Pitch and Yaw values.

	if ( !F.NavComputed ) 
	{
		EastDiff = GPSEastWay - GPSEast;
		NorthDiff = GPSNorthWay - GPSNorth;

		if ( (Abs(EastDiff) >= NavNeutralRadius ) || (Abs(NorthDiff) >= NavNeutralRadius )) 
		{
			Range = (int16)int32sqrt( (int32)NorthDiff*(int32)NorthDiff + (int32)EastDiff*(int32)EastDiff );
			WayHeading = int16atan2(EastDiff, NorthDiff);

			F.Proximity = Range < NavClosingRadius; 
			F.CloseProximity = false;

		//	EffNavSensitivity = (NavSensitivity * ( ATTITUDE_HOLD_LIMIT * 4 - CurrMaxRollPitch )) / (ATTITUDE_HOLD_LIMIT * 4);
			EffNavSensitivity = SRS16(NavSensitivity * ( 32 - Limit(CurrMaxRollPitch, 0, 32) ) + 16, 5);

			// direct solution make North and East coordinate errors zero
			SinHeading = int16sin(Heading);
			CosHeading = int16cos(Heading);
			
			// East
			if ( EastDiff < NavClosingRadius )
			{
				EastP = ( EastDiff * NAV_MAX_ROLL_PITCH )/ NavCloseToNeutralRadius;
				EastP = Limit(EastP, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
			}
			else
				EastP = Limit(EastDiff, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);

			SumEastP += EastP;
			SumEastP = Limit(SumEastP, -NAV_INT_LIMIT, NAV_INT_LIMIT);
			EastI = SRS16(SumEastP * (int16)P[NavKi], 4);
			SumEastP = DecayX(SumEastP, 1);

			EastD = SRS16((EastDiffP - EastDiff) * (int16)P[NavKd], 7);
			EastDiffP = EastDiff;
			EastD = Limit(EastD, -NAV_DIFF_LIMIT, NAV_DIFF_LIMIT);

			#ifdef NAV_SUPPRESS_P
			EastP = 0;
			#endif // NAV_SUPPRESS_P
	
			EastCorr = SRS16((EastP + EastI + EastD) * EffNavSensitivity, 8);

			// North
			if ( NorthDiff < NavClosingRadius )
			{
				NorthP = ( NorthDiff * NAV_MAX_ROLL_PITCH )/ NavCloseToNeutralRadius;
				NorthP = Limit(NorthP, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
			}
			else
				NorthP = Limit(NorthDiff, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);

			SumNorthP += NorthP;
			SumNorthP = Limit(SumNorthP, -NAV_INT_LIMIT, NAV_INT_LIMIT);
			NorthI = SRS16(SumNorthP * (int16)P[NavKi], 4);
			SumNorthP = DecayX(SumNorthP, 1);

			NorthD = SRS16((NorthDiffP - NorthDiff) * (int16)P[NavKd], 7); 
			NorthDiffP = NorthDiff;
			NorthD = Limit(NorthD, -NAV_DIFF_LIMIT, NAV_DIFF_LIMIT);

			#ifdef NAV_SUPPRESS_P
			NorthP = 0;
			#endif // NAV_SUPPRESS_P

			NorthCorr = SRS16((NorthP + NorthI + NorthD) * EffNavSensitivity, 8); 
			
			// Roll & Pitch
			NavRCorr = SRS16(CosHeading * EastCorr - SinHeading * NorthCorr, 8);	
			NavRCorr = SlewLimit(NavRCorrP, NavRCorr, NAV_CORR_SLEW_LIMIT);
			NavRCorrP = NavRCorr;

			NavPCorr = SRS16(-SinHeading * EastCorr - CosHeading * NorthCorr, 8);
			NavPCorr = SlewLimit(NavPCorrP, NavPCorr, NAV_CORR_SLEW_LIMIT);
			NavPCorrP = NavPCorr;
			
			// Yaw
			if ( F.TurnToHome && !F.Proximity )
			{
				RelHeading = MakePi(WayHeading - Heading); // make +/- MilliPi
				NavYCorr = -(RelHeading * NAV_YAW_LIMIT) / HALFMILLIPI;
				NavYCorr = Limit(NavYCorr, -NAV_YAW_LIMIT, NAV_YAW_LIMIT); // gently!
			}
			else
				NavYCorr = 0;	

		}
		else
		{
			// Neutral Zone - no GPS influence
			NavPCorr = DecayX(NavPCorr, 2);
			NavRCorr = DecayX(NavRCorr, 2);
			NavYCorr = 0;
			F.CloseProximity = true;
		}

	}

	DesiredRoll = Limit(DesiredRoll + NavRCorr, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
	DesiredPitch = Limit(DesiredPitch + NavPCorr, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
	DesiredYaw += NavYCorr;
	F.NavComputed = true;

} // Navigate

void FakeFlight()
{
	#ifdef FAKE_FLIGHT

	static int16 CosH, SinH, A;

	#define FAKE_NORTH_WIND 	0L
	#define FAKE_EAST_WIND 		0L
    #define SCALE_VEL			256L

	if ( Armed )
	{
		Heading = MILLIPI;
	
		GPSEast = 1000L; GPSNorth = 1000L;
		InitNavigation();

		TxString("\r\n Sens MRP Eff East North Head DRoll DPitch DYaw Range ");
		TxString("EP EI ED EC | NP NI ND NC \r\n");
	
		while ( Armed )
		{
			UpdateControls();

		//	DesiredRoll = DesiredPitch = 0;
			Heading += DesiredYaw / 5;

			F.NavComputed = false;
			if (( NavSensitivity > NAV_GAIN_THRESHOLD ) && F.NewCommands )
				Navigate(0,0);

			Delay100mSWithOutput(5);
			CosH = int16cos(Heading);
			SinH = int16sin(Heading);
			GPSEast += ((int32)(-DesiredPitch) * SinH)/SCALE_VEL;
			GPSNorth += ((int32)(-DesiredPitch) * CosH)/SCALE_VEL;
			
			A = Make2Pi(Heading + HALFMILLIPI);
			CosH = int16cos(A);
			SinH = int16sin(A);
			GPSEast += ((int32)DesiredRoll * SinH) / SCALE_VEL;
			GPSEast += FAKE_EAST_WIND; // wind	
			GPSNorth += ((int32)DesiredRoll * CosH) / SCALE_VEL;
			GPSNorth += FAKE_NORTH_WIND; // wind
		
			TxVal32((int32)NavSensitivity,0,' ');
			TxVal32((int32)CurrMaxRollPitch,0,' ');
			TxVal32((int32)EffNavSensitivity,0,' ');
			TxVal32(GPSEast, 0, ' '); TxVal32(GPSNorth, 0, ' '); TxVal32(Heading, 0, ' '); 
			TxVal32((int32)DesiredRoll, 0, ' '); TxVal32((int32)DesiredPitch, 0, ' '); TxVal32((int32)DesiredYaw, 0, ' ');
			TxVal32((int32)Range, 0, ' '); 
			TxVal32((int32)EastP, 0, ' '); TxVal32((int32)EastI, 0, ' '); TxVal32((int32)EastD, 0, ' '); 
			TxVal32((int32)EastCorr, 0, ' ');
			TxString("| ");
			TxVal32((int32)NorthP, 0, ' '); TxVal32((int32)NorthI, 0, ' '); TxVal32((int32)NorthD, 0, ' ');
			TxVal32((int32)NorthCorr, 0, ' '); 
			TxNextLine();
		}
	}
	#endif // FAKE_FLIGHT
} // FakeFlight

void DoNavigation(void)
{
	if ( F.GPSValid && F.CompassValid  && F.NewCommands && ( NavSensitivity > NAV_GAIN_THRESHOLD ) && ( mS[Clock] > mS[NavActiveTime]) )
	{
		switch ( NavState ) { // most probable case last - switches in C18 are IF chains not branch tables!
		case Navigating:
			// not implemented yet
			break;
		case Descending:
			Navigate(WP[0].N, WP[0].E);
			if ( F.ReturnHome )
				Descend();
			else
				AcquireHoldPosition();
			break;
		case AtHome:
			Navigate(WP[0].N, WP[0].E);
			if ( F.ReturnHome )
				if ( F.UsingRTHAutoDescend && ( mS[Clock] > mS[RTHTimeout] ) )
				{
					mS[AltHoldUpdate] = mS[Clock];
					NavState = Descending;
				}
				else
					SetDesiredAltitude(WP[0].A);
			else
				AcquireHoldPosition();
			break;
		case ReturningHome:
			if ( F.ReturnHome )
			{
				Navigate(WP[0].N, WP[0].E);
				SetDesiredAltitude(WP[0].A);
				if ( F.Proximity )
				{
					mS[RTHTimeout] = mS[Clock] + NAV_RTH_TIMEOUT_MS;					
					NavState = AtHome;
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
						mS[BeeperTimeout] = mS[Clock] + 500;
						Beeper_ON;				
					} 
					#endif // NAV_ACQUIRE_BEEPER
				}	
			}
			else
				F.AcquireNewPosition = true;
			
			Navigate(GPSNorthHold, GPSEastHold);

			if ( F.ReturnHome )
				if ( Max(Abs(RollSum), Abs(PitchSum)) <= NAV_RTH_LOCKOUT )
				{
					AltSum = 0; 
					NavState = ReturningHome;
				}
			break;
		} // switch NavState
	}

} // DoNavigation

void DoPPMFailsafe(void)
{ // only relevant to PPM Rx or Quad NOT synchronising with Rx
	if ( State == InFlight )
		switch ( FailState ) {
		case Terminated:
			DesiredRoll = DesiredPitch = DesiredYaw = 0;
			if ( CurrentRelBaroAltitude < -BARO_FAILSAFE_MIN_ALT_CM ) // zzz
			{
				Descend();							// progressively increase desired baro pressure
				if ( mS[Clock ] > mS[AbortTimeout] )
					if ( F.Signal )
					{
						LEDRed_OFF;
						LEDGreen_ON;
						FailState = Waiting;
					}
					else
						mS[AbortTimeout] += ABORT_UPDATE_MS;
			}
			else
			{ // shutdown motors to avoid prop injury
				DesiredThrottle = 0;
				StopMotors();
			}
			break;
		case Returning:
			DesiredRoll = DesiredPitch = DesiredYaw = DesiredThrottle = 0;
			NavSensitivity = RC_NEUTRAL;		// 50% gain
			F.ReturnHome = F.TurnToHome = true;
			NavState = ReturningHome;
			DoNavigation();						// if GPS is out then check for hover will see zero throttle
			if ( mS[Clock ] > mS[AbortTimeout] )
				if ( F.Signal )
				{
					LEDRed_OFF;
					LEDGreen_ON;
					FailState = Waiting;
				}
				else
					mS[AbortTimeout] += ABORT_UPDATE_MS;
			break;
		case Aborting:
			if( mS[Clock] > mS[AbortTimeout] )
			{
				F.LostModel = true;
				LEDGreen_OFF;
				LEDRed_ON;

				mS[AltHoldUpdate] = mS[Clock];
				SetDesiredAltitude(WP[0].A);
				mS[AbortTimeout] += ABORT_TIMEOUT_MS;

				#ifdef NAV_PPM_FAILSAFE_RTH
				FailState = Returning;
				#else
				FailState = Terminated;
				#endif // PPM_FAILSAFE_RTH
			}
			break;
		case Waiting:
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

void InitNavigation(void)
{
	static uint8 w;

	for (w = 0; w < NAV_MAX_WAYPOINTS; w++)
	{
		WP[w].N = WP[w].E = 0; 
		WP[w].A = (int16)P[NavRTHAlt]*100L; // Centimetres
	}

	GPSNorthHold = GPSEastHold = 0;
	NavPCorr = NavPCorrP = NavRCorr = NavRCorrP = NavYCorr = 0;
	EastDiffP, NorthDiffP = 0;

	NavState = HoldingStation;
	AttitudeHoldResetCount = 0;
	CurrMaxRollPitch = 0;
	F.Proximity = F.CloseProximity = true;
	F.NavComputed = false;
} // InitNavigation

