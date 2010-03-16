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
//    If not, see http://www.gnu.org/licenses/

#include "uavx.h"

void SendTelemetry(void);
void SendUAVXNav(void);
void SendArduStation(void);

uint8 UAVXCurrPacketTag;

void SendTelemetry(void)
{
	switch ( P[TelemetryType] ) {
	case UAVXTelemetry: SendUAVXNav(); break;
	case ArduStationTelemetry: SendArduStation(); break;
	}
} // SendTelemetry

void SendUAVXNav(void) // 800uS at 40MHz
{
	uint8 b;
	i32u Temp;

	// packet must be shorter than GPS shortest valid packet ($GPGGA)
	// which is ~64 characters - so limit to 48?.

	F.TxToBuffer = true;

	for (b=10;b;b--) 
		TxChar(0x55);
	      
	TxChar(0xff); // synchronisation to "jolt" USART	
	TxChar(SOH);	
	TxCheckSum = 0;
	
	switch ( UAVXCurrPacketTag ) {
	case UAVXFlightPacketTag:

		SendESCByte(UAVXFlightPacketTag);
		SendESCByte(32 + FLAG_BYTES);
		for ( b = 0; b < FLAG_BYTES; b++ )
			SendESCByte(F.AllFlags[b]); 
		
		SendESCByte(State);
	
		SendESCByte(BatteryVolts);
		SendESCWord(0); 						// Battery Current
		SendESCWord(RCGlitches);
			
		SendESCWord(DesiredThrottle);
		SendESCWord(DesiredRoll);
		SendESCWord(DesiredPitch);
		SendESCWord(DesiredYaw);
		SendESCWord(RollRate);
		SendESCWord(PitchRate);
		SendESCWord(YawRate);
		SendESCWord(RollSum);
		SendESCWord(PitchSum);
		SendESCWord(YawSum);
		SendESCWord(LRAcc);
		SendESCWord(FBAcc);
		SendESCWord(DUAcc);
			
		UAVXCurrPacketTag = UAVXNavPacketTag;
		break;
	
	case UAVXNavPacketTag:
		F.TxToBuffer = true;
		SendESCByte(UAVXNavPacketTag);
		SendESCByte(38);
	
		SendESCByte(NavState);
		SendESCByte(FailState);
		SendESCByte(GPSNoOfSats);
		SendESCByte(GPSFix);

		SendESCByte(CurrWP);	
		SendESCByte(0); // padding

		SendESCWord(BaroROC); // cm/S
		Temp.i32 = RelBaroAltitude;
		SendESCWord(Temp.w0);
		SendESCWord(Temp.w1);

		SendESCWord(RangefinderROC); // cm/S 
		SendESCWord(RangefinderAltitude); // cm

		SendESCWord(GPSHDilute);
		SendESCWord(Heading);
		SendESCWord(WayHeading);

		SendESCWord(GPSVel);
		SendESCWord(GPSROC); // cm/S
		Temp.i32 = GPSRelAltitude; // cm
		SendESCWord(Temp.w0);
		SendESCWord(Temp.w1);

		Temp.i32 = GPSLongitude; // 5 decimal minute units
		SendESCWord(Temp.w0);
		SendESCWord(Temp.w1);
	
		Temp.i32 = GPSLatitude;
		SendESCWord(Temp.w0);
		SendESCWord(Temp.w1);
	
		UAVXCurrPacketTag = UAVXFlightPacketTag;
		break;
	
	default:
		UAVXCurrPacketTag = UAVXFlightPacketTag;
		break;		
	}
		
	SendESCByte(TxCheckSum);
	
	TxChar(EOT);
	
	TxChar(CR);
	TxChar(LF); 

	F.TxToBuffer = false; 
	
} // SendUAVXNav

void SendArduStation(void)
{
	static uint8 Count = 0;
	/*      
	Definitions of the low rate telemetry (1Hz):
    LAT: Latitude
    LON: Longitude
    SPD: Speed over ground from GPS
    CRT: Climb Rate in M/S
    ALT: Altitude in meters
    ALH: The altitude is trying to hold
    CRS: Course over ground in degrees.
    BER: Bearing is the heading you want to go
    WPN: Waypoint number, where WP0 is home.
    DST: Distance from Waypoint
    BTV: Battery Voltage.
    RSP: Roll setpoint used to debug, (not displayed here).
    
    Definitions of the high rate telemetry (~4Hz):
    ASP: Airspeed, right now is the raw data.
    TTH: Throttle in 100% the autopilot is applying.
    RLL: Roll in degrees + is right - is left
    PCH: Pitch in degrees
    SST: Switch Status, used for debugging, but is disabled in the current version.
	*/

	#ifndef TESTS_FULL_STATS

	#ifdef CLOCK_40MHZ	// ArdStation required scaling etc. too slow for 16MHz

	F.TxToBuffer = true;

	if ( ++Count == 4 ) // ~2.0mS @ 40MHz
	{ // some fields suppressed to stay within 80 char GPS sentence length constraint
		TxString("!!!");
		TxString("LAT:"); TxVal32(GPSLatitude, 0, 0);
		TxString(",LON:"); TxVal32(GPSLongitude, 0, 0); 
		TxString(",ALT:"); TxVal32(Altitude / 100,0,0);
		//TxString(",ALH:"); TxVal32(DesiredAltitude / 100, 0, 0);
		//TxString(",CRT:"); TxVal32(ROC / 100, 0, 0);
		TxString(",CRS:"); TxVal32(((int24)Heading*180) / MILLIPI, 0, 0); // scaling to degrees?
		TxString(",BER:"); TxVal32(((int24)WayHeading*180) / MILLIPI, 0, 0);
		//TxString(",SPD:"); TxVal32(GPSVel, 0, 0);    
		TxString(",WPN:"); TxVal32(CurrWP,0,0);
		//TxString(",DST:"); TxVal32(0, 0, 0); // distance to WP
		TxString(",BTV:"); TxVal32((BatteryVoltsADC * 61)/205, 1, 0);
		//TxString(",RSP:"); TxVal32(DesiredRoll, 0, 0);

		Count = 0;
	}
	else // ~1.1mS @ 40MHz
	{
	   	TxString("+++");
		TxString("ASP:"); TxVal32(GPSVel / 100, 0, 0);
		TxString(",RLL:"); TxVal32(RollSum / 35, 0, 0); // scale to degrees?
		TxString(",PCH:"); TxVal32(PitchSum / 35, 0, 0);
		TxString(",THH:"); TxVal32( ((int24)DesiredThrottle * 100) / RC_MAXIMUM, 0, 0);
	}

	TxString(",***\r\n");

	F.TxToBuffer = false;

	#endif // CLOCK_40MHZ

	#endif // TESTS_FULL_STATS

} // SendArduStation


