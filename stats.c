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

//    UAVX is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "uavx.h"

// Prototypes

void ZeroStats(void);
void ReadStatsEE(void);
void WriteStatsEE(void);
void ShowStats(void);

void ZeroStats(void)
{
	uint8 s;

	for (s = 0 ; s < MaxStats ; s++ )
		Stats[s].i16 = 0;

	Stats[MinHDiluteS].i16 = 10000L;
	Stats[MaxHDiluteS].i16 = 0;
	Stats[MinBaroROCS].i16 = 10000L;
	Stats[MaxBaroROCS].i16 = -10000L;

} // ZeroStats

void ReadStatsEE(void)
{
	uint8 s;

	for (s = 0 ; s < MaxStats ; s++ )
	{
		Stats[s].low8 = ReadEE(STATS_ADDR_EE + s*2);
		Stats[s].high8 = ReadEE(STATS_ADDR_EE + s*2 + 1);
	}
} // InitStats

void WriteStatsEE()
{
	uint8 s;
	int16 Temp;

	for (s = 0 ; s < MaxStats ; s++ )
	{
		WriteEE(STATS_ADDR_EE + s*2, Stats[s].low8);
		WriteEE(STATS_ADDR_EE + s*2 + 1, Stats[s].high8);
	}

	Temp = ToPercent(HoverThrottle, OUT_MAXIMUM);
	WriteEE(PercentHoverThr, Temp);
} // WriteStatsEE

void ShowStats(void)
{
	static int16 Scale;

	Scale = 3000;
	if ( P[GyroType] == IDG300 )
		Scale = 5000;
	else
		if ( P[GyroType] == ADXRS150 )
			Scale = 1500;

	TxString("\r\nFlight Statistics\r\n");
	TxString("Glitch: \t");TxVal32((int32) Stats[RCGlitchesS].i16,0,' '); TxString("\r\n");

	TxString("\r\nGPSAlt: \t");TxVal32(Stats[GPSAltitudeS].i16, 2,' '); TxString("M\r\n"); 
	TxString("BaroAlt:\t");TxVal32((int32)Stats[RelBaroAltitudeS].i16, 2, ' '); TxString("M (");
	TxVal32((int32)Stats[RelBaroPressureS].i16, 0, ' '); TxString("clicks)\r\n");
	TxString("ROC:    \t");TxVal32((int32)Stats[MinBaroROCS].i16, 2, ' '); 
							TxVal32((int32)Stats[MaxBaroROCS].i16, 2, ' ');TxString("M/S\r\n");

	#ifdef STATS_INC_GYRO_ACC
	TxString("\r\nRoll:   \t"); TxVal32((int32)(Stats[RollRateS].i16 - Stats[GyroMidRollS].i16), 0,'/'); TxString("512\r\n");
	TxString("Pitch:  \t"); TxVal32((int32)(Stats[PitchRateS].i16 - Stats[GyroMidPitchS].i16), 0,'/'); TxString("512\r\n");
	TxString("Yaw:    \t"); TxVal32((int32)(Stats[YawRateS].i16 - Stats[GyroMidYawS].i16), 0,'/'); TxString("512\r\n");

	TxString("\r\nLRAcc:  \t"); TxVal32(((int32)Stats[LRAccS].i16 * 1000L)/1024L, 3, 'G'); TxNextLine(); 
	TxString("FBAcc:  \t"); TxVal32(((int32)Stats[FBAccS].i16 * 1000L)/1024L, 3, 'G'); TxNextLine();
	TxString("DUAcc:  \t"); TxVal32(((int32)Stats[DUAccS].i16 * 1000L)/1024L, 3, 'G'); TxNextLine();
	#endif // STATS_INC_GYRO_ACC

	TxString("\r\n\r\nSensor Failures (Count)\r\n");
	TxString(" Acc:    \t");TxVal32((int32)Stats[AccFailS].i16,0 , 0); TxNextLine();
	TxString(" Compass:\t");TxVal32((int32)Stats[CompassFailS].i16,0 , 0); TxNextLine();
	TxString(" Baro:   \t");TxVal32((int32)Stats[BaroFailS].i16,0 , 0); TxNextLine(); 

	TxString("\r\nGPS\r\n");
	#ifdef GPS_INC_GROUNDSPEED 
	TxString("Vel:    \t");TxVal32((Stats[GPSVelS].i16*10L)/METRES_TO_GPS,1,' '); TxString("M/S\r\n"); 
	#endif // GPS_INC_GROUNDSPEED
	TxString("HDilute:\t");TxVal32((int32)Stats[MinHDiluteS].i16, 2, ' ');
	TxVal32((int32)Stats[MaxHDiluteS].i16, 2, 0); TxNextLine();
	TxString("Invalid:\t");TxVal32(((int32)Stats[GPSInvalidS].i16*(int32)10000)/Stats[GPSSentencesS].i16, 4, '%'); TxNextLine();
	if ( !F.NavValid )
	TxString("No GPS lock before launch - Navigation DISABLED\r\n");

	if ( (Stats[AccFailS].i16 > 0)|(Stats[CompassFailS].i16 > 0)|(Stats[BaroFailS].i16 > 0)|(Stats[GPSInvalidS].i16 > 0))
		LEDYellow_ON;
} // ShowStats


