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

void ZeroStats(void);
void ReadStatsEE(void);
void WriteStatsEE(void);
void ShowStats(void);

#pragma udata stats
int24 	MaxRelBaroAltitudeS, MaxGPSAltitudeS;
int16 	Stats[MAX_STATS];
#pragma udata

void ZeroStats(void)
{
	uint8 s;

	for (s = 0 ; s < MAX_STATS ; s++ )
		Stats[s] = 0;

	Stats[MinHDiluteS] = 10000L;
	Stats[MaxHDiluteS] = 0;
	Stats[MinBaroROCS] = 10000L;
	Stats[MaxBaroROCS] = -10000L;
	MaxGPSAltitudeS = MaxRelBaroAltitudeS = -10000L;

} // ZeroStats

void ReadStatsEE(void)
{
	uint8 s;

	for (s = 0 ; s < MAX_STATS ; s++ )
		Stats[s] = Read16EE(STATS_ADDR_EE + s*2);

	MaxGPSAltitudeS = (int24)Stats[GPSAltitudeS] * 10L;
	MaxRelBaroAltitudeS = (int24)Stats[RelBaroAltitudeS] * 10L;
} // InitStats

void WriteStatsEE()
{
	uint8 s;
	int16 Temp;

	Stats[GPSAltitudeS] = (int16)(MaxGPSAltitudeS/10L);
	Stats[RelBaroAltitudeS] = (int16)(MaxRelBaroAltitudeS/10L);

	if ( SumCompBaroPress != 0 )
		Stats[GPSBaroScaleS] = (SumGPSRelAltitude * 16)/(-SumCompBaroPress); 

	for (s = 0 ; s < MAX_STATS ; s++ )
		Write16EE(STATS_ADDR_EE + s*2, Stats[s]);

	Temp = ToPercent(HoverThrottle, OUT_MAXIMUM);
	WriteEE(PercentHoverThr, Temp);

	Write16EE(NAV_MAX_ALT, (int16)(MaxGPSAltitudeS/100)); // ??? temporary
	Write16EE(NAV_MAX_AS, GPSVel); // ??? temporary

} // WriteStatsEE

void ShowStats(void)
{
	TxString("\r\nFlight Statistics\r\n");

	TxString("\r\nSensor/Rx Failures (Count)\r\n");
	TxString("Acc:      \t");TxVal32((int32)Stats[AccFailS], 0, 0); TxNextLine();
	TxString("Comp:     \t");TxVal32((int32)Stats[CompassFailS], 0, 0); TxNextLine();
	TxString("Baro:     \t");TxVal32((int32)Stats[BaroFailS],0 , 0); TxNextLine();
	TxString("Rx:       \t");TxVal32((int32)Stats[RCGlitchesS],0,' '); TxNextLine(); 

	TxString("\r\nBaro\r\n"); // can only display to 3276M
	TxString("Alt:      \t");TxVal32((int32)Stats[RelBaroAltitudeS], 1, ' '); TxString("M (");
	TxVal32((int32)Stats[RelBaroPressureS], 0, ' '); TxString("clicks)\r\n");
	TxString("ROC:      \t");TxVal32((int32)Stats[MinBaroROCS], 2, ' '); 
							TxVal32((int32)Stats[MaxBaroROCS], 2, ' '); TxString("M/S\r\n");

	if ( Stats[GPSBaroScaleS] !=0 )
	{
		TxString("Scale:    \t");TxVal32((int32)Stats[GPSBaroScaleS], 0, ' '); TxString("UAVPSet?\r\n");
	}

	TxString("\r\nGPS\r\n");
	TxString("Alt:      \t");TxVal32((int32)Stats[GPSAltitudeS], 1,' '); TxString("M\r\n"); 
	#ifdef GPS_INC_GROUNDSPEED 
	TxString("Vel:      \t");TxVal32(ConvertGPSToM((int32)Stats[GPSVelS]), 0, ' '); TxString("M/S\r\n"); 
	#endif // GPS_INC_GROUNDSPEED
	TxString("HDilute:  \t");TxVal32((int32)Stats[MinHDiluteS], 2, ' ');
	TxVal32((int32)Stats[MaxHDiluteS], 2, 0); TxNextLine();
	TxString("Invalid:  \t");TxVal32(((int32)Stats[GPSInvalidS]*(int32)1000000L)/(int32)Stats[GPSSentencesS], 4, '%'); TxNextLine();
	if ( Stats[NavValidS] )
		TxString("Navigation ENABLED\r\n");	
	else
		TxString("Navigation DISABLED (No fix at launch)\r\n");

} // ShowStats


