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

#include "c-ufo.h"
#include "bits.h"

// Prototypes
void Descend(void);
void ReturnHome(void);
void ParseGPSSentence(void);
void ResetTimeOuts(void);
void Navigate(int16, int16);
void HoldStation(void);

// Defines

#define PROXIMITY	20

// Variables
extern boolean FirstGPSSentence;
extern int16 GPSNorth, GPSEast, GPSNorthHold, GPSEastHold;
extern boolean GPSSentenceReceived;

void Descend(void)
{	
		DesiredThrottle = 0; // safest	
} // Descend

void AcquireSatellites(void)
{
	#ifdef USE_GPS
	LedBlue_ON;
	if ( !FirstGPSSentence )
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
} // AcquireSatellites

#define GPSFilter MediumFilter

void Navigate(int16 GPSNorthWay, int16 GPSEastWay)
{	// _GPSValid must be true immediately prior to entry	
	// This routine does not point the quadrocopter at the destination
	// waypoint. It simply rolls/pitches towards the destination
	// To avoid cos/sin/arctan calls a simple look up process is used.

	int16 Angle, Temp;
	int16 RangeApprox;
	int16 EastDiff, NorthDiff;

	EastDiff = GPSEastWay - GPSEast;
	NorthDiff = GPSNorthWay - GPSNorth;

	RangeApprox = Limit((int24)NorthDiff*(int24)NorthDiff + (int24)EastDiff*(int24)EastDiff, 0, PROXIMITY);
		
	Angle = int16atan2(-EastDiff, NorthDiff) - CompassHeading;
	while ( Angle < MILLIPI ) Angle += TWOMILLIPI;
	while ( Angle >= MILLIPI ) Angle -= TWOMILLIPI;

	Temp = -int16sin(Angle) * RangeApprox/PROXIMITY;
	Temp = Limit(Temp, -MAX_ANGLE, MAX_ANGLE); // removed scaling of trig result
	DesiredRoll = GPSFilter(DesiredRoll, Temp);

	Temp = -int16cos(Angle) * RangeApprox/PROXIMITY;
	Temp = Limit(Temp, -MAX_ANGLE, MAX_ANGLE);
	DesiredPitch = GPSFilter(DesiredPitch, Temp);

} // Navigate

void HoldStation()
{
	if ( _GPSValid ) // zzz && _UseCompass ) 
	{ 
		if ( !_HoldingStation )
		{
			GPSNorthHold = GPSNorth;
			GPSEastHold = GPSEast;
			_HoldingStation = true;
		}		
		Navigate(GPSNorthHold, GPSEastHold);
		//AltitudeHold(CurrAltitude);
	}
	else
	{
		DesiredRoll = IRoll;
		DesiredPitch = IPitch;
	}

} // HoldStation

void ReturnHome(void)
{
	if ( _GPSValid ) // zzz && _UseCompass )
	{
		Navigate(0, 0);
		//AltitudeHold(RETURN_ALT);
	}
	else
	{
		DesiredRoll = DesiredPitch = 0;
		Descend();
	}
} // ReturnHome

void CheckAutonomous(void)
{
	DesiredThrottle = IGas;
	DesiredYaw = IYaw;

	#ifdef ENABLE_AUTONOMOUS

	UpdateGPS();

	if ( _Hovering )
		HoldStation();
	else
	{
		_HoldingStation = false;
		if ( IK5 > _Neutral )
			ReturnHome();
		else
		{
			DesiredRoll = IRoll;
			DesiredPitch = IPitch;
		}
	}
	#else
		DesiredRoll = IRoll;
		DesiredPitch = IPitch;
	#endif // ENABLE_AUTONOMOUS

} // CheckAutonomous

