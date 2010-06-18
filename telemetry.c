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
void SendUAVX(void);
void SendArduStation(void);
void SendCustom(void);
void SensorTrace(void);

uint8 UAVXCurrPacketTag;

void CheckTelemetry(void)
{
	#ifndef TESTING // not used for testing - make space!
	if ( mS[Clock] > mS[TelemetryUpdate] )
	{
		
		switch ( P[TelemetryType] ) {
		case UAVXTelemetry: 
			SendUAVX(); 
			mS[TelemetryUpdate] = mS[Clock] + UAVX_TELEMETRY_INTERVAL_MS;
			break;
		case ArduStationTelemetry: 
			SendArduStation(); 
			mS[TelemetryUpdate] = mS[Clock] + ARDU_TELEMETRY_INTERVAL_MS;
			break;
		case CustomTelemetry: 
			SendCustom(); 
			mS[TelemetryUpdate] = mS[Clock] + CUSTOM_TELEMETRY_INTERVAL_MS;
			break;
		case GPSTelemetry: break;
		} 
	}
	#endif // TESTING
} // CheckTelemetry

void SendUAVX(void) // 800uS at 40MHz?
{
	static uint8 b;
	static i32u Temp;

	F.TxToBuffer = true;

	#ifdef TELEMETRY_PREAMBLE
	for (b=10;b;b--) 
		TxChar(0x55);
	#endif // TELEMETRY_PREAMBLE
	      
	TxChar(0xff); // synchronisation to "jolt" USART	
	TxChar(SOH);	
	TxCheckSum = 0;
	
	switch ( UAVXCurrPacketTag ) {
	case UAVXFlightPacketTag:
		TxESCu8(UAVXFlightPacketTag);
		TxESCu8(45 + FLAG_BYTES);
		for ( b = 0; b < FLAG_BYTES; b++ )
			TxESCu8(F.AllFlags[b]); 
		
		TxESCu8(State);	

		TxESCi16(BatteryVoltsADC);
		TxESCi16(BatteryCurrentADC);
		TxESCi16(BatteryChargeUsedmAH);
 
		TxESCi16(RCGlitches);			
		TxESCi16(DesiredThrottle);

		TxESCi16(DesiredRoll);
		TxESCi16(DesiredPitch);
		TxESCi16(DesiredYaw);

		TxESCi16(RollRateADC - GyroMidRoll);
		TxESCi16(PitchRateADC - GyroMidPitch);
		TxESCi16(YawRateADC - GyroMidYaw);

		TxESCi16(RollSum);
		TxESCi16(PitchSum);
		TxESCi16(YawSum);

		TxESCi16(LRAcc);
		TxESCi16(FBAcc);
		TxESCi16(DUAcc);
		TxESCi8((int8)LRComp);
		TxESCi8((int8)FBComp);
		TxESCi8((int8)DUComp);
		TxESCi8((int8)AltComp);

		for ( b = 0; b < 6; b++ ) // motor/servo channels
	 		TxESCu8(PWM[b]);
		
		UAVXCurrPacketTag = UAVXNavPacketTag;
		break;
	
	case UAVXNavPacketTag:
		TxESCu8(UAVXNavPacketTag);
		TxESCu8(49);
	
		TxESCu8(NavState);
		TxESCu8(FailState);
		TxESCu8(GPSNoOfSats);
		TxESCu8(GPSFix);

		TxESCu8(CurrWP);	

		TxESCi16(BaroROC); 							// dm/S
		TxESCi24(BaroRelAltitude);

		TxESCi16(RangefinderROC); 					// dm/S 
		TxESCi16(RangefinderAltitude); 				// dm

		TxESCi16(GPSHDilute);
		TxESCi16(Heading);
		TxESCi16(WayHeading);

		TxESCi16(GPSVel);
		TxESCi16(GPSROC); 							// dm/S

		TxESCi24(GPSRelAltitude); 					// dm
		TxESCi32(GPSLatitude); 						// 5 decimal minute units
		TxESCi32(GPSLongitude); 

		TxESCi24(DesiredAltitude);
		TxESCi32(DesiredLatitude); 
		TxESCi32(DesiredLongitude);

		TxESCi24(mS[NavStateTimeout] - mS[Clock]);	// mS
		
		UAVXCurrPacketTag = UAVXFlightPacketTag;
		break;
	
	default:
		UAVXCurrPacketTag = UAVXFlightPacketTag;
		break;		
	}
		
	TxESCu8(TxCheckSum);	
	TxChar(EOT);
	
	TxChar(CR);
	TxChar(LF); 

	F.TxToBuffer = false; 
	
} // SendUAVX

void SendArduStation(void)
{
	// This form of telemetry using the flight controller to convert 
	// to readable text is FAR to EXPENSIVE in computation time.

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

	#ifdef CLOCK_40MHZ	// ArdStation required scaling etc. too slow for 16MHz

	F.TxToBuffer = true;

	if ( ++Count == 4 ) // ~2.5mS @ 40MHz?
	{
		TxString("!!!");
		TxString("LAT:"); TxVal32(GPSLatitude / 6000, 3, 0);
		TxString(",LON:"); TxVal32(GPSLongitude / 6000, 3, 0);
		TxString(",ALT:"); TxVal32(Altitude / 10,0,0);
		TxString(",ALH:"); TxVal32(DesiredAltitude / 10, 0, 0);
		TxString(",CRT:"); TxVal32(ROC / 100, 0, 0);
		TxString(",CRS:"); TxVal32(((int24)Heading * 180) / MILLIPI, 0, 0); // scaling to degrees?
		TxString(",BER:"); TxVal32(((int24)WayHeading * 180) / MILLIPI, 0, 0);
		TxString(",SPD:"); TxVal32(GPSVel, 0, 0);
		TxString(",WPN:"); TxVal32(CurrWP,0,0);
		TxString(",DST:"); TxVal32(0, 0, 0); // distance to WP
		TxString(",BTV:"); TxVal32((BatteryVoltsADC * 61)/205, 1, 0);
		TxString(",RSP:"); TxVal32(DesiredRoll, 0, 0);

		Count = 0;
	}
	else // ~1.1mS @ 40MHz
	{
	   	TxString("+++");
		TxString("ASP:"); TxVal32(GPSVel / 100, 0, 0);
		TxString(",RLL:"); TxVal32(RollSum / 35, 0, 0); // scale to degrees?
		TxString(",PCH:"); TxVal32(PitchSum / 35, 0, 0);
		TxString(",THH:"); TxVal32( ((int24)DesiredThrottle * 100L) / RC_MAXIMUM, 0, 0);
	}

	TxString(",***\r\n");

	F.TxToBuffer = false;

	#endif // CLOCK_40MHZ

} // SendArduStation

void SendCustom(void)
{ // user defined telemetry human readable OK for small amounts of data < 1mS

	F.TxToBuffer = true;
	
	// insert values here using TxVal32(n, dp, separator)
	// dp is the scaling to decimal places, separator is 0 or a 'char'
	// -> 

	// 800uS @ 40MHz

	TxVal32(mS[Clock], 3, HT);

	if ( F.HoldingAlt ) // are we holding
		TxChar('H');
	else
		TxChar('N');
	TxChar(HT);

	if (F.UsingRangefinderAlt ) // are we using the rangefinder
		TxChar('R');
	else
		TxChar('B');
	TxChar(HT);

	TxVal32(GPSRelAltitude, 1, HT);
	TxVal32(BaroRelAltitude, 1, HT);
	TxVal32(RangefinderAltitude, 2, HT);

	TxVal32(BaroPressure, 0, HT);			// eff. sensor reading
	TxVal32(BaroTemperature, 0, HT); 		// eff. sensor reading redundant for MXP4115
	TxVal32(CompBaroPressure, 0, HT);  		// moving sum of last 8 readings

	// <-

	TxChar(CR);
	TxChar(LF);

	F.TxToBuffer = false;
} // SendCustom

void SensorTrace(void)
{
	#ifdef TESTING

	if ( DesiredThrottle > 20 ) 
	{
		F.TxToBuffer = false; // direct to USART

		TxValH16(((int24)Heading * 180)/MILLIPI); TxChar(';');

		TxValH16(BaroRelAltitude); TxChar(';');
		TxValH16(RangefinderAltitude); TxChar(';');
		TxValH16(0); TxChar(';');
			
		TxValH16(DesiredThrottle); TxChar(';');
		TxValH16(DesiredRoll); TxChar(';');
		TxValH16(DesiredPitch); TxChar(';');
		TxValH16(DesiredYaw); TxChar(';');

		TxValH16(RollRateADC - GyroMidRoll); TxChar(';');
		TxValH16(PitchRateADC - GyroMidPitch); TxChar(';');
		TxValH16(YawRateADC - GyroMidYaw); TxChar(';');

		TxValH16(RollSum); TxChar(';');
		TxValH16(PitchSum); TxChar(';');
		TxValH16(YawSum); TxChar(';');

		TxValH16(LRAcc); TxChar(';');
		TxValH16(FBAcc); TxChar(';');
		TxValH16(DUAcc); TxChar(';');

		TxValH16(LRComp); TxChar(';');
		TxValH16(FBComp); TxChar(';');
		TxValH16(DUComp); TxChar(';');
		TxValH16(AltComp); TxChar(';');
		TxNextLine();
	} 
	#endif // TESTING
} // SensorTrace



