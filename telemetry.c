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

void SendPacketHeader(void);
void SendPacketTrailer(void);
void SendTelemetry(void);
void SendCycle(void);
void SendControl(void);
void SendMinPacket(void);
void SendFlightPacket(void);
void SendNavPacket(void);
void SendControlPacket(void);
void SendStatsPacket(void);
void SendParamsPacket(uint8);
void SendArduStation(void);
void SendCustom(void);
void SensorTrace(void);

uint8 UAVXCurrPacketTag;

void CheckTelemetry(void)
{
	#ifndef TESTING // not used for testing - make space!
	if ( mSClock() > mS[TelemetryUpdate] )		
		switch ( P[TelemetryType] ) {
		case UAVXTelemetry:
			mS[TelemetryUpdate] = mSClock() + UAVX_TEL_INTERVAL_MS; 
			SendCycle(); 	
			break;
		case UAVXMinTelemetry:
			mS[TelemetryUpdate] = mSClock() + UAVX_MIN_TEL_INTERVAL_MS; 
			SendMinPacket(); 	
			break;
		case UAVXControlTelemetry:
			mS[TelemetryUpdate] = mSClock() + UAVX_CONTROL_TEL_INTERVAL_MS; 
			SendControlPacket(); 	
			break;
		case ArduStationTelemetry:
			mS[TelemetryUpdate] = mSClock() + ARDU_TEL_INTERVAL_MS; 
			SendArduStation(); 			
			break;
		case CustomTelemetry: 
			mS[TelemetryUpdate] = mSClock() + CUSTOM_TEL_INTERVAL_MS;
			SendCustom(); 
			break;
		case GPSTelemetry: break;
		} 
	#endif // TESTING
} // CheckTelemetry

#define NAV_STATS_INTERLEAVE	10
static int8 StatsNavAlternate = 0; 

void SendPacketHeader(void)
{
	static int8 b;

	F.TxToBuffer = true;

	#ifdef TELEMETRY_PREAMBLE
	for (b=10;b;b--) 
		TxChar(0x55);
	#endif // TELEMETRY_PREAMBLE
	      
	TxChar(0xff); // synchronisation to "jolt" USART	
	TxChar(SOH);	
	TxCheckSum = 0;
} // SendPacketHeader

void SendPacketTrailer(void)
{
	TxESCu8(TxCheckSum);	
	TxChar(EOT);
	
	TxChar(CR);
	TxChar(LF); 

	F.TxToBuffer = false; 
} // SendPacketTrailer

void ShowAttitude(void)
{
	TxESCi16(DesiredRoll);
	TxESCi16(DesiredPitch);
	TxESCi16(DesiredYaw);

	TxESCi16(Rate[Roll]);
	TxESCi16(Rate[Pitch]);
	TxESCi16(Rate[Yaw]);

	TxESCi16(Angle[Roll]);
	TxESCi16(Angle[Pitch]);
	TxESCi16(Angle[Yaw]);

	TxESCi16(Acc[LR]);
	TxESCi16(Acc[FB]);
	TxESCi16(Acc[DU]);
} // ShowAttitude

void SendFlightPacket(void)
{
	static int8 b;

	SendPacketHeader();

	TxESCu8(UAVXFlightPacketTag);
	TxESCu8(48 + TELEMETRY_FLAG_BYTES);
	for ( b = 0; b < TELEMETRY_FLAG_BYTES; b++ )
		TxESCu8(F.AllFlags[b]); 
		
	TxESCu8(State);	

	TxESCi16(BatteryVoltsADC);
	TxESCi16(BatteryCurrentADC);
	TxESCi16(BatteryChargeUsedmAH);
 
	TxESCi16(RCGlitches);			
	TxESCi16(DesiredThrottle);

	ShowAttitude();

	TxESCi8((int8)Comp[LR]);
	TxESCi8((int8)Comp[FB]);
	TxESCi8((int8)Comp[DU]);
	TxESCi8((int8)Comp[Alt]);

	for ( b = 0; b < 6; b++ ) // motor/servo channels
	 	TxESCu8((uint8)PWM[b]);

	TxESCi24(mSClock() - mS[StartTime]);

	SendPacketTrailer();
} // SendFlightPacket

void SendControlPacket(void)
{
	static int8 b;

	SendPacketHeader();

	TxESCu8(UAVXControlPacketTag);
	TxESCu8(35);

	TxESCi16(DesiredThrottle);
 			
	ShowAttitude();

	for ( b = 0; b < 6; b++ ) // motor/servo channels
	 	TxESCu8((uint8)PWM[b]);

	TxESCi24(mSClock() - mS[StartTime]);

	SendPacketTrailer();

} // SendControlPacket

void SendNavPacket(void)
{
	SendPacketHeader();

	TxESCu8(UAVXNavPacketTag);
	TxESCu8(59);
		
	TxESCu8(NavState);
	TxESCu8(FailState);
	TxESCu8(GPSNoOfSats);
	TxESCu8(GPSFix);
	
	TxESCu8(CurrWP);	
	
	TxESCi16(BaroROC); 							// dm/S
	TxESCi24(BaroRelAltitude);
	
	TxESCi16(0); 								// dm/S was RF ROC
	TxESCi16(RangefinderAltitude); 				// dm
	
	TxESCi16(GPSHDilute);
	TxESCi16(Heading);
	TxESCi16(WayHeading);
	
	TxESCi16(GPSVel);
	TxESCi16(0); 							    // dm/S
	
	TxESCi24(GPSRelAltitude); 					// dm
	TxESCi32(GPSLatitude); 						// 5 decimal minute units
	TxESCi32(GPSLongitude); 
	
	TxESCi24(DesiredAltitude);
	TxESCi32(DesiredLatitude); 
	TxESCi32(DesiredLongitude);
	
	TxESCi24(mS[NavStateTimeout] - mSClock());	// mS
	
	TxESCi16(AmbientTemperature.i16);			// 0.1C
	TxESCi32(GPSMissionTime);

	TxESCu8(NavSensitivity);
	TxESCi8(NavCorr[Roll]);
	TxESCi8(NavCorr[Pitch]);
	TxESCi8(NavCorr[Yaw]);

	SendPacketTrailer();

} // SendNavPacket

void SendStatsPacket(void) 
{
	SendPacketHeader();

	TxESCu8(UAVXStatsPacketTag);
	TxESCu8(44);
	
	TxESCi16(Stats[I2CFailS]);
	TxESCi16(Stats[GPSInvalidS]); 
	TxESCi16(Stats[AccFailS]); 
	TxESCi16(Stats[GyroFailS]); 
	TxESCi16(Stats[CompassFailS]); 
	TxESCi16(Stats[BaroFailS]); 
	TxESCi16(Stats[ESCI2CFailS]); 
			 
	TxESCi16(Stats[RCFailsafesS]); 
			
	TxESCi16(Stats[GPSAltitudeS]);
	TxESCi16(Stats[GPSVelS]);
	TxESCi16(Stats[GPSMinSatsS]);
	TxESCi16(Stats[GPSMaxSatsS]);
	TxESCi16(Stats[MinHDiluteS]);
	TxESCi16(Stats[MaxHDiluteS]);
			
	TxESCi16(Stats[BaroRelAltitudeS]);
	TxESCi16(Stats[MinBaroROCS]);
	TxESCi16(Stats[MaxBaroROCS]);
			
	TxESCi16(Stats[MinTempS]);
	TxESCi16(Stats[MaxTempS]);
	
	TxESCi16(Stats[BadS]);

	TxESCu8(UAVXAirframe);
	TxESCu8(Orientation);
	TxESCi16(Stats[BadNumS]);

	SendPacketTrailer();

} // SendStatsPacket

void SendMinPacket(void)
{
	static int8 b;

	SendPacketHeader();

	TxESCu8(UAVXMinPacketTag);
	TxESCu8(33 + TELEMETRY_FLAG_BYTES);
	for ( b = 0; b < TELEMETRY_FLAG_BYTES; b++ )
		TxESCu8(F.AllFlags[b]); 
		
	TxESCu8(State);	
	TxESCu8(NavState);	
	TxESCu8(FailState);

	TxESCi16(BatteryVoltsADC);
	TxESCi16(BatteryCurrentADC);
	TxESCi16(BatteryChargeUsedmAH);	

	TxESCi16(Angle[Roll]);
	TxESCi16(Angle[Pitch]);
		
	TxESCi24(BaroRelAltitude);
	TxESCi16(RangefinderAltitude); 				// cm

	TxESCi16(Heading);
	
	TxESCi32(GPSLatitude); 						// 5 decimal minute units
	TxESCi32(GPSLongitude);

	TxESCu8(UAVXAirframe);
	TxESCu8(Orientation);

	TxESCi24(mSClock() - mS[StartTime]);

	SendPacketTrailer();

} // SendMinPacket

void SendParamsPacket(uint8 p) {

	static uint8 b;

    SendPacketHeader();

    TxESCu8(UAVXParamsPacketTag);
    TxESCu8(MAX_PARAMETERS+1);
    TxESCu8(p);
    for (b = 0; b < (uint8)MAX_PARAMETERS; b++ )
         TxESCi8(ReadEE(MAX_PARAMETERS*2+b));

	SendPacketTrailer();
 
} // SendParamPacket

void SendCycle(void) // 800uS at 40MHz?
{
	
	switch ( UAVXCurrPacketTag ) {
	case UAVXFlightPacketTag:
		SendFlightPacket();

		UAVXCurrPacketTag = UAVXNavPacketTag;
		break;
	
	case UAVXNavPacketTag:
		if ( ++StatsNavAlternate < NAV_STATS_INTERLEAVE)
			SendNavPacket();		
		else
		{
			SendStatsPacket();
			StatsNavAlternate = 0;
		}

		UAVXCurrPacketTag = UAVXFlightPacketTag;
		break;
	
	default:
		UAVXCurrPacketTag = UAVXFlightPacketTag;
		break;		
	}
			
} // SendCycle

void SendArduStation(void)
{
	#ifdef USE_ARDU	// ArdStation required scaling etc. too slow for 16MHz

	// This form of telemetry using the flight controller to convert 
	// to readable text is FAR to EXPENSIVE in computation time.

	static int8 Count = 0;
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
		TxString(",RLL:"); TxVal32(Angle[Roll]/ 35, 0, 0); // scale to degrees?
		TxString(",PCH:"); TxVal32(Angle[Pitch]/ 35, 0, 0);
		TxString(",THH:"); TxVal32( ((int24)DesiredThrottle * 100L) / RC_MAXIMUM, 0, 0);
	}

	TxString(",***\r\n");

	F.TxToBuffer = false;

	#endif // USE_ARDU

} // SendArduStation

void SendCustom(void) // 1.2mS @ 40MHz
{ // user defined telemetry human readable OK for small amounts of data < 1mS

	F.TxToBuffer = true;
	
	// insert values here using TxVal32(n, dp, separator)
	// dp is the scaling to decimal places, separator
	// separator may be a single 'char', HT for tab, or 0 (no space)
	// -> 

	// add user specific code

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

		TxValH16(Rate[Roll]); TxChar(';');
		TxValH16(Rate[Pitch]); TxChar(';');
		TxValH16(Rate[Yaw]); TxChar(';');

		TxValH16(Angle[Roll]); TxChar(';');
		TxValH16(Angle[Pitch]); TxChar(';');
		TxValH16(Angle[Yaw]); TxChar(';');

		TxValH16(Acc[LR]); TxChar(';');
		TxValH16(Acc[FB]); TxChar(';');
		TxValH16(Acc[DU]); TxChar(';');

		TxValH16(Comp[LR]); TxChar(';');
		TxValH16(Comp[FB]); TxChar(';');
		TxValH16(Comp[DU]); TxChar(';');
		TxValH16(Comp[Alt]); TxChar(';');
		TxNextLine();
	} 
	#endif // TESTING
} // SensorTrace



