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

void Navigate(int16, int16);
void AltitudeHold(int16);
void Descend(void);
void AcquireHoldPosition(void);
void DoNavigation(void);
void CheckThrottleMoved(void);
void DoFailsafe(void);
void InitNavigation(void);

// Variables
extern int16 ValidGPSSentences;
extern boolean GPSSentenceReceived;

int16 NavRCorr, SumNavRCorr, NavPCorr, SumNavPCorr, NavYCorr, SumNavYCorr;

WayPoint WP[MAX_WAYPOINTS];

void AltitudeHold(int16 DesiredAltitude) // Metres
{
	static int16 Temp;

	if ( _RTHAltitudeHold )
		if ( P[ConfigBits] & UseGPSAltMask )
		{
			AE = Limit(DesiredAltitude*10L - GPSRelAltitude, -GPSALT_BAND*10L, GPSALT_BAND*10L);
			AltSum += AE;
			AltSum = Limit(AltSum, -10, 10);	
			Temp = SRS16(AE*P[NavAltKp] + AltSum*P[NavAltKi], 5);
		
			DesiredThrottle = HoverThrottle + Limit(Temp, GPSALT_LOW_THR_COMP, GPSALT_HIGH_THR_COMP);
			DesiredThrottle = Limit(DesiredThrottle, 0, OUT_MAXIMUM);
		}
		else
		{	
			DesiredThrottle = HoverThrottle;
			BaroAltitudeHold(-DesiredAltitude * BARO_FROM_METRES);
		}
	else
	{
		// manual control of altitude
	}
} // AltitudeHold

void Descend(void)
{ // uses Baro only
	int16 DesiredBaroPressure;

	if (  mS[Clock] > mS[AltHoldUpdate] )
	{
		// zzz could have micro switch cutout on RC0 Pin 11

		// 0.5 Metres/Sec
		DesiredThrottle = HoverThrottle;
		DesiredBaroPressure = Limit (CurrentBaroPressure + BARO_FROM_METRES/2, -BARO_FROM_METRES*100, BARO_FROM_METRES*10);
		BaroAltitudeHold( DesiredBaroPressure );	
		mS[AltHoldUpdate] = mS[Clock] + 1000L;
	}
} // Descend

void AcquireHoldPosition(void)
{
	SumNavRCorr = SumNavPCorr = SumNavYCorr = 0;
	_NavComputed = false;

	GPSNorthHold = GPSNorth;
	GPSEastHold = GPSEast;
	_Proximity = true;

	NavState = HoldingStation;
} // AcquireHoldPosition

void Navigate(int16 GPSNorthWay, int16 GPSEastWay)
{	// _GPSValid must be true immediately prior to entry	
	// This routine does not point the quadrocopter at the destination
	// waypoint. It simply rolls/pitches towards the destination
	// cos/sin/arctan lookup tables are used for speed.
	// BEWARE magic numbers for integer arithmetic

	#define NavKi 1

	static int32 Temp;
	static int16 NavKp, GPSGain;
	static int16 Range, EastDiff, NorthDiff, WayHeading, RelHeading;

	if ( _NavComputed ) // maintain previous corrections
	{
		DesiredRoll = Limit(DesiredRoll + NavRCorr, -RC_NEUTRAL, RC_NEUTRAL);
		DesiredPitch = Limit(DesiredPitch + NavPCorr, -RC_NEUTRAL, RC_NEUTRAL);
		DesiredYaw = Limit(DesiredYaw + NavYCorr, -RC_NEUTRAL, RC_NEUTRAL);
	}
	else
	{
		EastDiff = GPSEastWay - GPSEast;
		NorthDiff = GPSNorthWay - GPSNorth;

		#define BRASHLEY_ZONE 10
		if ( (Abs(EastDiff) > BRASHLEY_ZONE ) || (Abs(NorthDiff) > BRASHLEY_ZONE )) // zzz Brashley Zone
		{ 
			Range = Max(Abs(NorthDiff), Abs(EastDiff)); 
			_Proximity = Range < NavClosingRadius;
			if ( _Proximity )
				Range = int16sqrt( NorthDiff*NorthDiff + EastDiff*EastDiff); 
			else
				Range = NavClosingRadius;

			GPSGain = Limit(NavSensitivity, 0, 256);
			NavKp = ( GPSGain * MAX_ANGLE ) / NavClosingRadius; // /OUT_MAXIMUM) * 256L
		
			Temp = ((int32)Range * NavKp )>>8; // allways +ve so can use >>
			WayHeading = int16atan2(EastDiff, NorthDiff);

			RelHeading = Make2Pi(WayHeading - Heading);
			NavRCorr = SRS32((int32)int16sin(RelHeading) * Temp, 8);			
			NavPCorr = SRS32(-(int32)int16cos(RelHeading) * Temp, 8);

			if ( _TurnToHome && (Range >= NavClosingRadius) )
			{
				RelHeading = MakePi(WayHeading - Heading); // make +/- MilliPi
				NavYCorr = -(RelHeading * NAV_YAW_LIMIT) / HALFMILLIPI;
				NavYCorr = Limit(NavYCorr, -NAV_YAW_LIMIT, NAV_YAW_LIMIT); // gently!
			}
			else
				NavYCorr = 0;
	
			DesiredRoll += NavRCorr;
			SumNavRCorr = Limit (SumNavRCorr + Range, -NavIntLimit256, NavIntLimit256);
			#ifdef ZERO_NAVINT
			if ( Sign(SumNavRCorr) == Sign(NavRCorr) )
				DesiredRoll += (SumNavRCorr * NavKi) / 256L;
			else
				SumNavRCorr = 0;
			#else
			DesiredRoll += (SumNavRCorr * NavKi) / 256L;
			#endif //ZERO_NAVINT
			DesiredRoll = Limit(DesiredRoll , -RC_NEUTRAL, RC_NEUTRAL);
	
			DesiredPitch += NavPCorr;
			SumNavPCorr = Limit (SumNavPCorr + Range, -NavIntLimit256, NavIntLimit256);
			#ifdef ZERO_NAVINT
			if ( Sign(SumNavPCorr) == Sign(NavPCorr) )
				DesiredPitch += (SumNavPCorr * NavKi) / 256L;
			else
				SumNavPCorr = 0;
			#else
			DesiredPitch += (SumNavPCorr * NavKi) / 256L;
			#endif //ZERO_NAVINT
			DesiredPitch = Limit(DesiredPitch , -RC_NEUTRAL, RC_NEUTRAL);

			DesiredYaw += NavYCorr;
		}
		else
			NavRCorr = NavPCorr = NavYCorr = 0;

		_NavComputed = true;
	}
} // Navigate

void DoNavigation(void)
{
	if ( _GPSValid && _CompassValid  && ( NavSensitivity > NAV_GAIN_THRESHOLD ) && ( mS[Clock] > mS[NavActiveTime]) )
		switch ( NavState ) {
		case PIC:
		case HoldingStation:

			HoldRoll = Abs(DesiredRoll - RollTrim);
			HoldPitch = Abs(DesiredPitch - PitchTrim);

			if ( ( HoldRoll > NAV_HOLD_LIMIT )||( HoldPitch > NAV_HOLD_LIMIT ) )
				if ( NavHoldResetCount > NAV_HOLD_RESET_INTERVAL )
					AcquireHoldPosition();
				else
					NavHoldResetCount++;
			else
			{
				if ( NavHoldResetCount > 1 )
					NavHoldResetCount -= 2;		// Faster decay
			}
				
			// Keep GPS hold active regardless
			NavState = HoldingStation;
			Navigate(GPSNorthHold, GPSEastHold);
	
			if ( _ReturnHome )
			{
				AltSum = 0; 
				NavState = ReturningHome;
			}

			CheckForHover();

			break;
		case ReturningHome:
			Navigate(WP[0].N, WP[0].E);
			AltitudeHold(WP[0].A);
			if ( _ReturnHome )
			{
				if ( _Proximity )
				{
					mS[RTHTimeout] = mS[Clock] + NAV_RTH_TIMEOUT_S*1000L;					
					NavState = AtHome;
				}
			}
			else
				AcquireHoldPosition();					
			break;
		case AtHome:
			Navigate(WP[0].N, WP[0].E);
			if ( _ReturnHome )
				if ( mS[Clock] > mS[RTHTimeout] )
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
			if ( _ReturnHome )
				Descend();
			else
				AcquireHoldPosition();
			break;
		case Navigating:
			// not implemented yet
			break;
		} // switch NavState
	else // no Navigation
		CheckForHover();
} // DoNavigation

void DoFailsafe(void)
{ // only relevant to PPM Rx or Quad NOT synchronising with Rx

	ALL_LEDS_OFF;
	if ( _Failsafe )
	{
		LEDRed_ON;
		_LostModel = true;
		DesiredRoll = DesiredPitch = DesiredYaw = 0;
		DesiredThrottle = 0;
		// Descend();	
	}
	else
		if( mS[Clock] > mS[AbortTimeout] ) // timeout - immediate shutdown/abort
		{
			mS[AltHoldUpdate] = mS[Clock];
			_Failsafe = true;
		}
		else
			if ( mS[Clock] > mS[FailsafeTimeout] ) 
			{
				DesiredRoll = DesiredPitch = DesiredYaw = 0;
				if ( State != InFlight )
					DesiredThrottle = 0;
				// use last "good" throttle; 
			}
			else
			{
				// continue on last "good" signals
			}
					
} // DoFailsafe

void CheckThrottleMoved(void)
{
	if( mS[Clock] < mS[ThrottleUpdate] )
		ThrNeutral = DesiredThrottle;
	else
	{
		ThrLow = ThrNeutral - THR_MIDDLE;
		ThrLow = Max(ThrLow, THR_HOVER);
		ThrHigh = ThrNeutral + THR_MIDDLE;
		if ( ( DesiredThrottle <= ThrLow ) || ( DesiredThrottle >= ThrHigh ) )
		{
			mS[ThrottleUpdate] = mS[Clock] + THROTTLE_UPDATE_S*1000L;
			_ThrottleMoving = true;
		}
		else
			_ThrottleMoving = false;
	}
} // CheckThrottleMoved

void InitNavigation(void)
{
	int8 w;

	for (w = 0; w < MAX_WAYPOINTS; w++)
	{
		WP[w].N = WP[w].E = 0; 
		WP[w].A = P[NavRTHAlt]; // Metres
	}

	GPSNorthHold = GPSEastHold = 0;
	NavRCorr = SumNavRCorr = NavPCorr = SumNavPCorr = NavYCorr = SumNavYCorr = 0;
	NavState = PIC;
	NavHoldResetCount = 0;
	_NavComputed = false;
} // InitNavigation

