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
//    If not, see http://www.gnu.org/licenses/.

// Barometer

#include "uavx.h"

void StartBaroADC(boolean);
int24 CompensatedPressure(int24, int24);
void ReadBaro(boolean);
void GetBaroAltitude(void);
void InitBarometer(void);

#define BaroFilter SoftFilter
#define BaroROCFilter HardFilter

// Baro (altimeter) sensor
#define BARO_I2C_ID			0xee
#define BARO_TEMP_BMP085	0x2e
#define BARO_TEMP_SMD500	0x6e
#define BARO_PRESS			0xf4
#define BARO_CTL			0xf4				// OSRS=3 for BMP085 25.5mS, SMD500 34mS				
#define BARO_ADC_MSB		0xf6
#define BARO_ADC_LSB		0xf7
#define BARO_ADC_XLSB		0xf8				// BMP085
#define BARO_TYPE			0xd0

#define BARO_TEMP_TIME_MS	20	// 10
//#define BMP085_PRESS_TIME_MS 	26
//#define SMD500_PRESS_TIME_MS 	34
#define BARO_PRESS_TIME_MS	45	

typedef struct {
	uint8 Head, Tail;
	int24 B[8];
	} Baro24Q;	

#define BARO_BUFF_SIZE 8	// MUST be 8
#pragma udata baroq
Baro24Q BaroPressQ;
Baro24Q BaroTempQ;
#pragma udata

int24	OriginBaroPressure, CompBaroPress;
int24 	BaroPressSum, BaroTempSum;
int24	RelBaroAltitude, RelBaroAltitudeP;
int16	BaroROC;
i16u	BaroVal;
int8	BaroSample;
uint8	BaroType;

void StartBaroADC(boolean ReadPressure)
{
	uint8 TempOrPress;
	if ( ReadPressure )
	{
		TempOrPress = BARO_PRESS;
		mS[BaroUpdate] = mS[Clock] + BARO_PRESS_TIME_MS;
	}
	else
	{
		mS[BaroUpdate] = mS[Clock] + BARO_TEMP_TIME_MS;	
		if ( BaroType == BARO_ID_BMP085 )
			TempOrPress = BARO_TEMP_BMP085;
		else
			TempOrPress = BARO_TEMP_SMD500;
	}

	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto SBerror;

	// access control register, start measurement
	if( SendI2CByte(BARO_CTL) != I2C_ACK ) goto SBerror;

	// select 32kHz input, measure temperature
	if( SendI2CByte(TempOrPress) != I2C_ACK ) goto SBerror;
	I2CStop();

	F.BaroAltitudeValid = true;
	return;
SBerror:
	I2CStop();
	F.BaroAltitudeValid = F.HoldingAlt = false; 
	return;
} // StartBaroADC

void ReadBaro(boolean ReadPressure)
{
	// Possible I2C protocol error - split read of ADC
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto RVerror;
	if( SendI2CByte(BARO_ADC_MSB) != I2C_ACK ) goto RVerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto RVerror;
	BaroVal.b1 = RecvI2CByte(I2C_NACK);
	I2CStop();
			
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto RVerror;
	if( SendI2CByte(BARO_ADC_LSB) != I2C_ACK ) goto RVerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto RVerror;
	BaroVal.b0 = RecvI2CByte(I2C_NACK);
	I2CStop();

	return;

RVerror:
	I2CStop();

	F.BaroAltitudeValid = F.HoldingAlt = false;
	if ( State == InFlight ) 
	{
		Stats[BaroFailS]++; 
		F.BaroFailure = true;
	}
	return;
} // ReadBaro

#define BARO_BMP085_TEMP_COEFF		62L 	
#define BARO_SMD500_TEMP_COEFF		50L

int24 CompensatedPressure(int24 BaroPressSum, int24 BaroTempSum)
{
	static int24 BaroTempComp;

	if ( BaroType == BARO_ID_BMP085 )
		BaroTempComp = (int24)((int32)BaroTempSum * BARO_BMP085_TEMP_COEFF + 64) >> 7;
	else
		BaroTempComp = (int24)((int32)BaroTempSum * BARO_SMD500_TEMP_COEFF + 16) >> 4;
	
	return (BaroPressSum + BaroTempComp - OriginBaroPressure);
} // CompensatedPressure

void GetBaroAltitude(void)
{ 	// Use sum of 8 samples as the "pressure" to give some noise cancellation	
	static int24 Temp;
	// SMD500 9.5mS (T) 34mS (P)  
	// BMP085 4.5mS (T) 25.5mS (P) OSRS=3
	// Use 50mS => 5 samples per altitude hold update

	if ( mS[Clock] >= mS[BaroUpdate] ) // 5 pressure readings and 1 temperature reading
	{
		BaroSample++;
		if ( BaroSample < 6)		
		{
			ReadBaro(true);
			if ( BaroSample == 5 )
				StartBaroADC(false);
			else
				StartBaroADC(true);	
			BaroPressQ.Head = (BaroPressQ.Head + 1) & (BARO_BUFF_SIZE -1);
			BaroPressSum -= (int24)BaroPressQ.B[BaroPressQ.Head];		
			BaroPressQ.B[BaroPressQ.Head] = (int24)BaroVal.u16;
			BaroPressSum += BaroPressQ.B[BaroPressQ.Head];
		}
		else
		{
			ReadBaro(false);
			StartBaroADC(true);
			Temp = (int24)BaroVal.u16;
			if ( Temp < 70000 ) // skip transient temperature measurement error bug!
			{
				BaroTempQ.Head = (BaroTempQ.Head + 1) & (BARO_BUFF_SIZE -1);
				BaroTempSum -= (int24)BaroTempQ.B[BaroTempQ.Head];		
				BaroTempQ.B[BaroTempQ.Head] = Temp;
				BaroTempSum += Temp;			
			}
			else
				if ( State == InFlight ) 
				{
					Stats[BaroFailS]++; 
					F.BaroFailure = true;
				}
		}

		if ( BaroSample == 6 ) // time BARO_PRESS_TIME_MS intervals
		{
			// decreasing pressure is increase in altitude
			// negate and rescale to cm altitude
			CompBaroPress = CompensatedPressure(BaroPressSum, BaroTempSum);
			RelBaroAltitude = -SRS32((int32)CompBaroPress * (int16)P[BaroScale], 4);

			Temp = ( RelBaroAltitude - RelBaroAltitudeP ) * 4;
			BaroROC = BaroROCFilter(BaroROC, Temp);
		
			RelBaroAltitudeP = RelBaroAltitude;

			if ( State == InFlight )
			{
				if ( BaroROC > Stats[MaxBaroROCS] )
					Stats[MaxBaroROCS] = BaroROC;
				else
					if ( BaroROC < Stats[MinBaroROCS] )
						Stats[MinBaroROCS] = BaroROC;

				if ( RelBaroAltitude > MaxRelBaroAltitudeS ) 
				{ 
					MaxRelBaroAltitudeS = RelBaroAltitude;
					Stats[RelBaroPressureS] = CompBaroPress;
				}
			}
			F.NewBaroValue = F.BaroAltitudeValid;
			BaroSample = 0;

			#ifdef DEBUG_SENSORS	
			Trace[TRelBaroAltitude] = RelBaroAltitude/10L;
			#endif
		}
	}
}// GetBaroAltitude

void InitBarometer(void)
{	
	int24 Temp, CurrBaroTemp;
	uint8 s;

	RelBaroAltitude = RelBaroAltitudeP = BaroROC = 0;
	BaroPressSum = BaroSample = OriginBaroPressure = CompBaroPress = 0;
	AltComp = AltDiffSum = AltDSum = 0;
	BaroPressQ.Head = BaroTempQ.Head = 0;

	F.NewBaroValue = false;
	F.BaroAltitudeValid = true;

	// Determine baro type
	I2CStart();

	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto BAerror;
	if( SendI2CByte(BARO_TYPE) != I2C_ACK ) goto BAerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto BAerror;
	BaroType = RecvI2CByte(I2C_NACK);
	I2CStop();

	StartBaroADC(true);

	if ( !F.BaroAltitudeValid ) goto BAerror;

	for ( s = 0; s < 8 ; s++ )
	{
		while ( mS[Clock] < mS[BaroUpdate] );
		ReadBaro(true);
		StartBaroADC(s < 7); // start temperature read on last sample
		Temp = (int24)BaroVal.u16;
		BaroPressQ.B[s] = Temp;
		BaroPressSum += Temp;	
	}

	while ( mS[Clock] < mS[BaroUpdate] );
	ReadBaro(false);
	CurrBaroTemp = (int24)BaroVal.u16;	 
	for ( s = 0; s < 8 ; s++ )
		BaroTempQ.B[s] =  CurrBaroTemp;
	BaroTempSum = CurrBaroTemp * 8L;
	OriginBaroPressure = CompensatedPressure(BaroPressSum, BaroTempSum);

	StartBaroADC(true);
	RelBaroAltitudeP = 0;

	return;

BAerror:
	F.BaroAltitudeValid = F.HoldingAlt = false;
	Stats[BaroFailS]++;
	I2CStop();
} // InitBarometer

