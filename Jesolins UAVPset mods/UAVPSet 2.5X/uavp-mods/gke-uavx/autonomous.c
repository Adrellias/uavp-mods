// =======================================================================
// =                                 UAVX                                =
// =                         Quadrocopter Control                        =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =======================================================================
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

// Autonomous flight routines

#include "UAVX.h"

// Prototypes
void Descend(void);
void DoAutonomous(void);
void ParseGPSSentence(void);
void ResetTimeOuts(void);
uint8 RCLinkRestored(int24);
void Navigate(int16, int16);
void HoldStation(void);

// Defines

#define PROXIMITY	20

// Variables
#ifdef DUMMY_GPS
uint16 DummyGPSTimeout;
#endif

void ResetTimeOuts(void)
{
	mS[FailsafeTimeout] = mS[Clock] + FAILSAFE_TIMEOUT;
	mS[AutonomousTimeout] = mS[Clock] + AUTONOMOUS_TIMEOUT;
	mS[ThrottleClosed] = mS[Clock] + THROTTLE_TIMEOUT;
} // ResetTimeOuts

uint8 RCLinkRestored(int24 d)
{
	uint24 TimerMilliSec;

	// checks for good RC frames for a period d mS
	if ( !_Signal )
		TimerMilliSec = mS[Clock] + d;
	return(mS[Clock] > TimerMilliSec );
} // RCLinkRestored

void Descend(void)
{
	if ( _UseBaro )
	{
        // if desired close to current then go lower
		DesiredAltitude = Limit(CurrAltitude - 1, 0, 32000);
		AltitudeHold(DesiredAltitude);
	}
	else
		DesiredThrottle = 0; // safest	
} // Descend

void AcquireSatellites(void)
{
	InitGPS();
	LedBlue_ON;
	#ifdef USE_GPS
	while ( !_GPSValid )
	{
		if ( GPSSentenceReceived )
		{
			GPSSentenceReceived=false; 
			ParseGPSSentence();
		}
	}
	LedBlue_OFF;
	#endif // USE_GPS

	#ifdef DUMMY_GPS
	RxHead = RxTail = 0;
	DummyGPSTimeout = mS[Clock] + 1000;
	#endif
} // AcquireSatellites

void Navigate(int16 GPSNorthWay, int16 GPSEastWay)
{	// _GPSValid must be true immediately prior to entry	
	// This routine does not point the quadrocopter at the destination
	// waypoint. It simply rolls/pitches towards the destination
	// To avoid cos/sin/arctan calls a simple look up process is used.

	int16 Angle;
	int16 RangeApprox;
	int16 EastDiff, NorthDiff;

	EastDiff = GPSEastWay - GPSEast;
	NorthDiff = GPSNorthWay - GPSNorth;

	RangeApprox = Limit(NorthDiff*NorthDiff + EastDiff*EastDiff, 0, PROXIMITY);
		
	Angle = int16atan2(EastDiff, NorthDiff) - CompassHeading;

	DesiredRoll = -int16sin(Angle) * (MAX_ROLL * RangeApprox)/PROXIMITY;
	DesiredPitch = -int16cos(Angle) * (MAX_PITCH * RangeApprox)/PROXIMITY;
} // Navigate

void HoldStation()
{
	if ( _GPSValid )
	{ 
		if ( !_HoldingStation )
		{
			GPSNorthHold = GPSNorth;
			GPSEastHold = GPSEast;
			_HoldingStation = true;
		}		
		Navigate(GPSNorthHold, GPSEastHold);
	}

	AltitudeHold(CurrAltitude);

} // HoldStation

void DoAutonomous(void)
{
#ifdef DUMMY_GPS
	if ( _GPSValid )
#else
	if ( _GPSValid && _UseCompass )
#endif
	{
		Navigate(0, 0);
		AltitudeHold(RETURN_ALT);
	}
	else
	{
		DesiredRoll = DesiredPitch = DesiredYawRate = 0;
		Descend();
	}
} // DoAutonomous
