// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008, 2009 by Prof. Greg Egan           =
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

int16 Make2Pi(int16);
int16 MakePi(int16);
void Navigate(int16, int16);
void AltitudeHold(int16);
void Descend(void);
void AcquireHoldPosition(void);
void NavGainSchedule(int16);
void DoNavigation(void);
void CheckThrottleMoved(void);
void DoFailsafe(void);
void InitNavigation(void);

// Variables
int16 NavRCorr, NavRCorrP, SumNavRCorr, NavPCorr, NavPCorrP, SumNavPCorr, NavYCorr, SumNavYCorr;
int16 NavKp, NavVel, Range, RangeToNeutral;  
int16 RWeight, PWeight, RollP, RollI, RollD, PitchP, PitchI, PitchD; 

WayPoint WP[NAV_MAX_WAYPOINTS];

int16 Make2Pi(int16 A)
{
	while ( A < 0 ) A += TWOMILLIPI;
	while ( A >= TWOMILLIPI ) A -= TWOMILLIPI;
	return( A );
} // Make2Pi

int16 MakePi(int16 A)
{
	while ( A < -MILLIPI ) A += TWOMILLIPI;
	while ( A >= MILLIPI ) A -= TWOMILLIPI;
	return( A );
} // MakePi

void AltitudeHold(int16 DesiredAltitude) // Decimetres
{
	static int16 Temp;

	if ( F[RTHAltitudeHold] )
		if ( (P[ConfigBits] & UseGPSAltMask) || !F[BaroAltitudeValid] )
		{
			AE = DesiredAltitude - GPSRelAltitude;
			AE = Limit(AE, -GPS_ALT_BAND_DM, GPS_ALT_BAND_DM);
			AltSum += AE;
			AltSum = Limit(AltSum, -10, 10);	
			Temp = SRS16(AE*(int16)P[GPSAltKp] + AltSum*(int16)P[GPSAltKi], 7);
		
			DesiredThrottle = HoverThrottle + Limit(Temp, GPS_ALT_LOW_THR_COMP, GPS_ALT_HIGH_THR_COMP);
			DesiredThrottle = Limit(DesiredThrottle, 0, OUT_MAXIMUM);
		}
		else
		{
			DesiredThrottle = HoverThrottle;
			Temp = -SRS32( (int32)DesiredAltitude * BARO_SCALE, 8);
			BaroPressureHold(Temp);	
		}
	else
	{
		// manual control of altitude
	}
} // AltitudeHold

void Descend(void)
{ // uses Baro only
	static int16 DesiredRelBaroPressure;

	if (  mS[Clock] > mS[AltHoldUpdate] )
		if ( InTheAir ) 							//  micro switch RC0 Pin 11 to ground when landed
		{
			DesiredThrottle = HoverThrottle;

			if ( CurrentRelBaroPressure < -( BARO_DESCENT_TRANS_DM * BARO_SCALE)/256L )
				DesiredRelBaroPressure = CurrentRelBaroPressure + (BARO_MAX_DESCENT_DMPS * BARO_SCALE)/256L;
			else
				DesiredRelBaroPressure = CurrentRelBaroPressure + (BARO_FINAL_DESCENT_DMPS * BARO_SCALE)/256L; 

			BaroPressureHold( DesiredRelBaroPressure );	
			mS[AltHoldUpdate] += 1000L;
		}
		else
			DesiredThrottle = 0;

} // Descend

void AcquireHoldPosition(void)
{
	NavPCorr = NavPCorrP = NavRCorr = NavRCorrP = SumNavRCorr = SumNavPCorr = NavYCorr =  0;
	RangeP = MAXINT16;
	F[NavComputed] = false;

	GPSNorthHold = GPSNorth;
	GPSEastHold = GPSEast;
	F[Proximity] = F[CloseProximity] = false;

	NavState = HoldingStation;
} // AcquireHoldPosition

void Navigate(int16 GPSNorthWay, int16 GPSEastWay)
{	// F[GPSValid] must be true immediately prior to entry	
	// This routine does not point the quadrocopter at the destination
	// waypoint. It simply rolls/pitches towards the destination
	// cos/sin/arctan lookup tables are used for speed.
	// BEWARE magic numbers for integer arithmetic

	static int16 Temp, Correction, XP, XD;
	static int16 EastDiff, NorthDiff, WayHeading, RelHeading, DiffHeading;

	if ( F[NavComputed] ) // maintain previous corrections
	{
		Temp = DesiredRoll + NavRCorr;
		DesiredRoll = Limit(Temp, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
		Temp = DesiredPitch + NavPCorr;
		DesiredPitch = Limit(Temp, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
		Temp = DesiredYaw + NavYCorr;
		DesiredYaw = Limit(Temp, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH);
	}
	else
	{
		EastDiff = GPSEastWay - GPSEast;
		NorthDiff = GPSNorthWay - GPSNorth;

		if ( (Abs(EastDiff) >= NavNeutralRadius ) || (Abs(NorthDiff) >= NavNeutralRadius )) 
		{
		//	Temp = (NavSensitivity * ( ATTITUDE_HOLD_LIMIT * 2 - CurrMaxRollPitch )) / (ATTITUDE_HOLD_LIMIT * 2);
			Temp = SRS16(NavSensitivity * ( 16 - CurrMaxRollPitch ), 4);
			Temp = Limit(Temp, 0, NavSensitivity);
			NavKp = ( Temp * NAV_MAX_ROLL_PITCH ) / NavCloseToNeutralRadius;
			WayHeading = int16atan2(EastDiff, NorthDiff);
			DiffHeading = WayHeading - Heading;
			RelHeading = Make2Pi(DiffHeading);

			// Expectation is ~4Hz GPS

			Range = int32sqrt( (int32)NorthDiff*(int32)NorthDiff + (int32)EastDiff*(int32)EastDiff );

			if ( RangeP == MAXINT16 )
				RangeP = Range;
			NavVel = RangeP - Range;
			if ( Abs(NavVel) < 5 ) 
				NavVel = 0;
			RangeP = Range;
		
			F[Proximity] = Range < NavClosingRadius; 
			F[CloseProximity] = false;

			if ( !F[Proximity] )
				Range = NavClosingRadius;

			RangeToNeutral = Range - NavNeutralRadius;
			XP = SRS16(RangeToNeutral * NavKp, 8);
			XD = SRS16(NavVel * (int16)P[NavKd], 6);

			// Roll
			RWeight = int16sin(RelHeading);

			RollP = SRS16(RWeight * XP, 8);

			Temp = SumNavRCorr + RollP;
			SumNavRCorr = Limit (Temp, -NAV_INT_LIMIT, NAV_INT_LIMIT);
			RollI = SRS16(SumNavRCorr * (int16)P[NavKi], 4);
			SumNavRCorr = DecayX(SumNavRCorr, 1);

			RollD = SRS16(RWeight * XD, 8);
			RollD = Limit(RollD, -NAV_DIFF_LIMIT, NAV_DIFF_LIMIT);

			NavRCorr = RollP + RollI + RollD;

			NavRCorr = Limit(NavRCorr, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH );
			NavRCorr = SlewLimit(NavRCorrP, NavRCorr, NAV_CORR_SLEW_LIMIT);
			NavRCorrP = NavRCorr;

			DesiredRoll += NavRCorr;

			// Pitch
			PWeight = -(int32)int16cos(RelHeading);	

			PitchP = SRS16(PWeight * XP, 8);

			Temp = SumNavPCorr + PitchP;
			SumNavPCorr = Limit (Temp, -NAV_INT_LIMIT, NAV_INT_LIMIT);
			PitchI = SRS16(SumNavPCorr * (int16)P[NavKi], 4);
			SumNavPCorr = DecayX(SumNavPCorr, 1);

			PitchD = SRS16(PWeight * XD, 8);
			PitchD = Limit(PitchD, -NAV_DIFF_LIMIT, NAV_DIFF_LIMIT);

			NavPCorr = PitchP + PitchI + PitchD;

			NavPCorr = Limit(NavPCorr, -NAV_MAX_ROLL_PITCH, NAV_MAX_ROLL_PITCH );
			NavPCorr = SlewLimit(NavPCorrP, NavPCorr, NAV_CORR_SLEW_LIMIT);
			NavPCorrP = NavPCorr;

			DesiredPitch += NavPCorr;

			// Yaw
			if ( F[TurnToHome] && !F[Proximity] )
			{
				RelHeading = MakePi(DiffHeading); // make +/- MilliPi
				NavYCorr = -(RelHeading * NAV_YAW_LIMIT) / HALFMILLIPI;
				NavYCorr = Limit(NavYCorr, -NAV_YAW_LIMIT, NAV_YAW_LIMIT); // gently!
			}
			else
				NavYCorr = 0;	
			DesiredYaw += NavYCorr;
		}
		else
		{
			// Neutral Zone - no GPS influence
			NavPCorr = NavPCorrP = NavRCorr = NavRCorrP = SumNavRCorr = SumNavPCorr = NavYCorr =  0;
			F[CloseProximity] = true;
		}

		F[NavComputed] = true;
	}
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
		Heading = 0;

		RangeP = MAXINT16;	
		GPSEast = 200L; GPSNorth = 200L;
		InitNavigation();
	
		TxString("\r\n Sens Kp East North DRoll DPitch Range Vel ");
		TxString("RP RI RD RWt | PP PI PD PWt\r\n");
	
		while ( Armed )
		{
			UpdateControls();

			DesiredRoll = DesiredPitch = 0;

			F[NavComputed] = false;
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
		
			TxVal32((int32)NavSensitivity,0,' ');TxVal32((int32)NavKp,0,' ');
			TxVal32(GPSEast, 0, ' '); TxVal32(GPSNorth, 0, ' '); 
			TxVal32((int32)DesiredRoll, 0, ' '); TxVal32((int32)DesiredPitch, 0, ' ');
			TxVal32((int32)Range, 0, ' '); 
			TxVal32((int32)NavVel, 0, ' ');
			TxVal32((int32)RollP, 0, ' '); TxVal32((int32)RollI, 0, ' '); TxVal32((int32)RollD, 0, ' '); 
			TxVal32((int32)RWeight, 0, ' ');
			TxString("| ");
			TxVal32((int32)PitchP, 0, ' '); TxVal32((int32)PitchI, 0, ' '); TxVal32((int32)PitchD, 0, ' '); 
			TxVal32((int32)PWeight, 0, ' ');
			TxNextLine();
		}
	}
	#endif // FAKE_FLIGHT
} // FakeFlight

void DoNavigation(void)
{
	if ( F[GPSValid] && F[CompassValid]  && ( NavSensitivity > NAV_GAIN_THRESHOLD ) && ( mS[Clock] > mS[NavActiveTime]) )
	{
		#ifdef NAV_ON_NEUTRAL_ONLY
		switch ( NavState ) {
		case HoldingStation:
			if ( F[AttitudeHold] )
			{
				#ifdef NAV_ACQUIRE_BEEPER
				if ( F[HoldBeeperArmed] && !F[BeeperInUse] )
				{
					mS[BeeperTimeout] = mS[Clock] + 500;
					Beeper_ON;
					F[HoldBeeperArmed] = false;
				} 
				#endif // NAV_ACQUIRE_BEEPER

				NavState = HoldingStation;			// Keep GPS hold active regardless
				Navigate(GPSNorthHold, GPSEastHold);
			}
			else
			{
				F[HoldBeeperArmed] = true;
				AcquireHoldPosition();
			}
			
			if ( F[ReturnHome] )
			{
				AltSum = 0; 
				NavState = ReturningHome;
			}

			CheckForHover();

			break;
		case ReturningHome:
			if ( F[ReturnHome] && F[AttitudeHold] )
			{
				Navigate(WP[0].N, WP[0].E);
				AltitudeHold(WP[0].A);
				if ( F[Proximity] )
				{
					mS[RTHTimeout] = mS[Clock] + NAV_RTH_TIMEOUT_MS;					
					NavState = AtHome;
				}
			}
			else
				AcquireHoldPosition();					
			break;
		case AtHome:
			Navigate(WP[0].N, WP[0].E);
			if ( F[ReturnHome] )
				if ( (( P[ConfigBits] & UseRTHDescendMask ) != 0 ) && ( mS[Clock] > mS[RTHTimeout] ) )
				{
					mS[AltHoldUpdate] = mS[Clock];
					NavState = Descending;
				}
				else
					AltitudeHold(WP[0].A);
			else
				AcquireHoldPosition();
			break;
		case Descending:
			Navigate(WP[0].N, WP[0].E);
			if ( F[ReturnHome] )
				Descend();
			else
				AcquireHoldPosition();
			break;
		case Navigating:
			// not implemented yet
			break;
		} // switch NavState


		#else

		switch ( NavState ) {
		case HoldingStation:
			if ( F[AttitudeHold] )
			{
				#ifdef NAV_ACQUIRE_BEEPER
				if ( F[HoldBeeperArmed] && !F[BeeperInUse] )
				{
					mS[BeeperTimeout] = mS[Clock] + 500;
					Beeper_ON;
					F[HoldBeeperArmed] = false;
				} 
				#endif // NAV_ACQUIRE_BEEPER
			}
			else
			{
				F[HoldBeeperArmed] = true;
				AcquireHoldPosition();
			}
			
			NavState = HoldingStation;			// Keep GPS hold active regardless
			Navigate(GPSNorthHold, GPSEastHold);

			if ( F[ReturnHome] )
			{
				AltSum = 0; 
				NavState = ReturningHome;
			}

			CheckForHover();

			break;
		case ReturningHome:
			if ( F[ReturnHome] )
			{
				Navigate(WP[0].N, WP[0].E);
				AltitudeHold(WP[0].A);
				if ( F[Proximity] )
				{
					mS[RTHTimeout] = mS[Clock] + NAV_RTH_TIMEOUT_MS;					
					NavState = AtHome;
				}
			}
			else
				AcquireHoldPosition();					
			break;
		case AtHome:
			Navigate(WP[0].N, WP[0].E);
			if ( F[ReturnHome] )
				if ( (( P[ConfigBits] & UseRTHDescendMask ) != 0 ) && ( mS[Clock] > mS[RTHTimeout] ) )
				{
					mS[AltHoldUpdate] = mS[Clock];
					NavState = Descending;
				}
				else
					AltitudeHold(WP[0].A);
			else
				AcquireHoldPosition();
			break;
		case Descending:
			Navigate(WP[0].N, WP[0].E);
			if ( F[ReturnHome] )
				Descend();
			else
				AcquireHoldPosition();
			break;
		case Navigating:
			// not implemented yet
			break;
		} // switch NavState
		#endif // NAV_ON_NEUTRAL_ONLY
	}
	else // no Navigation
	{
		Beeper_OFF;
		CheckForHover();
	}


} // DoNavigation

void DoFailsafe(void)
{ // only relevant to PPM Rx or Quad NOT synchronising with Rx
	if ( State == InFlight )
		switch ( FailState ) {
		case Terminated:
			DesiredRoll = DesiredPitch = DesiredYaw = 0;
			if ( CurrentRelBaroPressure < -(BARO_FAILSAFE_MIN_ALT_DM * BARO_SCALE)/256L )
			{
				Descend();							// progressively increase desired baro pressure
				if ( mS[Clock ] > mS[AbortTimeout] )
					if ( F[Signal] )
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
			F[ReturnHome] = F[TurnToHome] = true;
			NavState = ReturningHome;
			DoNavigation();						// if GPS is out then check for hover will see zero throttle
			if ( mS[Clock ] > mS[AbortTimeout] )
				if ( F[Signal] )
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
				F[LostModel] = true;
				LEDGreen_OFF;
				LEDRed_ON;

				mS[AltHoldUpdate] = mS[Clock];
				AltitudeHold(WP[0].A);
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
		DesiredRoll = DesiredPitch = DesiredYaw = DesiredThrottle = 0;
			
} // DoFailsafe

void CheckThrottleMoved(void)
{
	if( mS[Clock] < mS[ThrottleUpdate] )
		ThrNeutral = DesiredThrottle;
	else
	{
		ThrLow = ThrNeutral - THROTTLE_MIDDLE;
		ThrLow = Max(ThrLow, THROTTLE_HOVER);
		ThrHigh = ThrNeutral + THROTTLE_MIDDLE;
		if ( ( DesiredThrottle <= ThrLow ) || ( DesiredThrottle >= ThrHigh ) )
		{
			mS[ThrottleUpdate] = mS[Clock] + THROTTLE_UPDATE_MS;
			F[ThrottleMoving] = true;
		}
		else
			F[ThrottleMoving] = false;
	}
} // CheckThrottleMoved

void InitNavigation(void)
{
	static uint8 w;

	for (w = 0; w < NAV_MAX_WAYPOINTS; w++)
	{
		WP[w].N = WP[w].E = 0; 
		WP[w].A = (int16)P[NavRTHAlt]*10L; // Decimetres
	}

	GPSNorthHold = GPSEastHold = 0;
	NavPCorr = NavPCorrP = NavRCorr = NavRCorrP = SumNavRCorr = SumNavPCorr = NavYCorr = 0;

	RollP, RollI = RollD = PitchP = PitchI = PitchD = 0;
	RangeP = MAXINT16;

	NavState = HoldingStation;
	AttitudeHoldResetCount = 0;
	CurrMaxRollPitch = 0;
	F[Proximity] = F[CloseProximity] = true;
	F[NavComputed] = false;
} // InitNavigation

