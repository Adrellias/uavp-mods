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
int24 CompensatedPressure(uint16, uint16);
void GetBaroAltitude(void);
void BaroTest(void);
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

// SMD500 9.5mS (T) 34mS (P)  
// BMP085 4.5mS (T) 25.5mS (P) OSRS=3
#define BARO_TEMP_TIME_MS	12	// 10 increase to make P+T acq time ~50mS
//#define BMP085_PRESS_TIME_MS 	26
//#define SMD500_PRESS_TIME_MS 	34
#define BARO_PRESS_TIME_MS	37	

uint16	BaroPressure, BaroTemperature;
boolean AcquiringPressure;

typedef struct {
	uint8 Head, Tail;
	int24 B[8];
	} Baro24Q;	

#define BARO_BUFF_SIZE 8	// MUST be 8
#pragma udata baroq
Baro24Q BaroPressQ, BaroTempQ;
#pragma udata

int24	OriginBaroAltitude, CompBaroAltitude;
int24	BaroPressSum, BaroTempSum;
int24	RelBaroAltitude, RelBaroAltitudeP;
int16	BaroROC;
i16u	BaroVal;
uint8	BaroType;
uint8 	Samples;

#ifdef SIMULATE
int24	FakeRelBaroAltitude;
#endif // SIMULATE

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

int24 CompensatedPressure(uint16 BaroPress, uint16 BaroTemp)
{
	static int24 BaroTempComp;

	if ( BaroType == BARO_ID_BMP085 )
		BaroTempComp = (int24)((int32)BaroTemp * BARO_BMP085_TEMP_COEFF + 64) >> 7;
	else
		BaroTempComp = (int24)((int32)BaroTemp * BARO_SMD500_TEMP_COEFF + 8) >> 4;
	
	return (BaroPress + BaroTempComp);
} // CompensatedPressure

void GetBaroAltitude(void)
{
	static int24 Temp;

	if ( mS[Clock] >= mS[BaroUpdate] )
	{
		if ( AcquiringPressure )
		{
			ReadBaro(AcquiringPressure);
			BaroPressure = BaroVal.u16;
			BaroPressSum -= (int24)BaroPressQ.B[BaroPressQ.Head];		
			BaroPressQ.B[BaroPressQ.Head] = BaroPressure;
			BaroPressSum += BaroPressure;
			BaroPressQ.Head = (BaroPressQ.Head + 1) & (BARO_BUFF_SIZE -1);
		}
		else
		{
			ReadBaro(AcquiringPressure);
			BaroTemperature = (int24)BaroVal.u16;			
			BaroTempSum -= (int24)BaroTempQ.B[BaroTempQ.Head];		
			BaroTempQ.B[BaroTempQ.Head] = BaroTemperature;
			BaroTempSum += BaroTemperature;
			BaroTempQ.Head = (BaroTempQ.Head + 1) & (BARO_BUFF_SIZE -1);
			Samples++;	// * 50mS		
		}

		AcquiringPressure = !AcquiringPressure;			
		StartBaroADC(AcquiringPressure);

		if ( Samples == 5 ) // 5 => 250mS 4Hz
		{
			Temp = CompensatedPressure(BaroPressSum, BaroTempSum);
			// decreasing pressure is increase in altitude negate and rescale to cm altitude
			CompBaroAltitude = -SRS32((int32)Temp * (int16)P[BaroScale], 4) - OriginBaroAltitude;
	
		//	if ( Abs( DesiredAltitude - RelBaroAltitude ) < ALT_BAND_CM )
		//		RelBaroAltitude = SlewLimit(RelBaroAltitude, CompBaroAltitude, 5); // 1M/S
		//	else
		//		RelBaroAltitude = SlewLimit(RelBaroAltitude, CompBaroAltitude, 25); // 5M/S

			RelBaroAltitude = CompBaroAltitude; // & 0xfffff0;	// discard noise in bottom 4 bits!!!		
	
			#ifdef SIMULATE
			if ( State == InFlight )
			{
				FakeRelBaroAltitude += ( DesiredThrottle - CruiseThrottle ) + AltComp * 5;
				if ( FakeRelBaroAltitude < 0 ) 
					FakeRelBaroAltitude = 0;
				RelBaroAltitude = FakeRelBaroAltitude;
			}			
			#endif // SIMULATE
	
			Temp = ( RelBaroAltitude - RelBaroAltitudeP ) * 4; // 4Hz altitude update
			BaroROC = BaroROCFilter(BaroROC, Temp);
	
			RelBaroAltitudeP = RelBaroAltitude;
			F.NewBaroValue = F.BaroAltitudeValid;
	
			if ( State == InFlight )
			{
//TxVal32(mS[Clock],3,' ');
//TxVal32(RelBaroAltitude,0,' ');
//TxVal32(BaroROC,0,0);
//TxNextLine();
				if ( BaroROC > Stats[MaxBaroROCS] )
					Stats[MaxBaroROCS] = BaroROC;
				else
					if ( BaroROC < Stats[MinBaroROCS] )
						Stats[MinBaroROCS] = BaroROC;
	
				if ( RelBaroAltitude > MaxRelBaroAltitudeS ) 
					MaxRelBaroAltitudeS = RelBaroAltitude;	
			}

			Samples = 0;
		}	
	}
}// GetBaroAltitude

#ifdef TESTING

void BaroTest(void)
{
	TxString("\r\nAltitude test\r\n");
	if ( !F.BaroAltitudeValid ) goto BAerror;

	if ( BaroType == BARO_ID_BMP085 )
		TxString("Type:\tBMP085\r\n");
	else
		TxString("Type:\tSMD500\r\n");
	
	while ( !F.NewBaroValue )
		GetBaroAltitude();	
	F.NewBaroValue = false;	

	TxString("P/T.:     \t");
	TxVal32((int32)BaroPressure, 0, ' ');
	TxVal32((int32)BaroTemperature, 0, 0);
	TxNextLine();

	TxString("Alt.:     \t");	
	TxVal32((int32)RelBaroAltitude, 2, ' ');
	TxString("M\r\n");

	TxString("R.Finder: \t");
	if ( F.RangefinderAltitudeValid )
	{
		GetRangefinderAltitude();
		TxVal32((int32)RangefinderAltitude, 2, ' ');
		TxString("M\r\n");
	}
	else
		TxString("no rangefinder\r\n");	

	return;
BAerror:
	TxString("FAIL\r\n");
} // BaroTest

#endif // TESTING

void InitBarometer(void)
{
	uint8 s;
	int24 Temp;

	RelBaroAltitude = RelBaroAltitudeP = BaroROC = CompBaroAltitude = 0;
	AltComp = AltDiffSum = AltDSum = 0;
	OriginBaroAltitude = 0;

	BaroPressQ.Head = BaroTempQ.Head = 0;
	BaroPressSum = BaroTempSum = 0;

	#ifdef SIMULATE
	FakeRelBaroAltitude = 0;
	#endif // SIMULATE

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

	StartBaroADC(true); // Pressure

	if ( !F.BaroAltitudeValid ) goto BAerror;

	for ( s = 0; s < 8 ; s++ )
	{
		while ( mS[Clock] < mS[BaroUpdate] );
		ReadBaro(true); // Pressure	
		BaroPressure = BaroVal.u16;
		BaroPressQ.B[s] = BaroPressure;
		BaroPressSum += BaroPressure;
		
		StartBaroADC(false); // Temperature
		while ( mS[Clock] < mS[BaroUpdate] );
		ReadBaro(false);

		BaroTemperature = BaroVal.u16;
		BaroTempQ.B[s] = BaroTemperature;
		BaroTempSum += BaroTemperature;

		StartBaroADC(true); // Pressure	
	}

	Temp = CompensatedPressure(BaroPressSum, BaroTempSum);
	OriginBaroAltitude = -SRS32((int32)Temp * (int16)P[BaroScale], 4);
	RelBaroAltitudeP = RelBaroAltitude = 0;
	Samples = 1;

	AcquiringPressure = true;

	return;

BAerror:
	F.BaroAltitudeValid = F.HoldingAlt = false;
	Stats[BaroFailS]++;
	I2CStop();
} // InitBarometer

