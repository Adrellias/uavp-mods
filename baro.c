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


// Barometers Freescale TI ADC and Bosch BMP085 3.8MHz, Bosch SMD500 400KHz  

#include "uavx.h"

void GetBaroAltitude(void);
void InitBarometer(void);

void ShowBaroType(void);
void BaroTest(void);

#define BaroROCFilter HardFilter

uint16	BaroPressure, BaroTemperature;
boolean AcquiringPressure;
int16	BaroOffsetDAC;

#define BARO_MIN_CLIMB			1500	// dM minimum available barometer climb from origin
#define BARO_MIN_DESCENT		-500	// dM minimum available barometer descent from origin

#define BARO_BUFF_SIZE 4
#pragma udata baroq
struct {
	uint8 Head, Tail;
	int24 B[BARO_BUFF_SIZE];
	} BaroQ;
#pragma udata

int32	OriginBaroPressure, CompBaroPressure;
int24	BaroRelAltitude, BaroRelAltitudeP;
int16	BaroROC;
i16u	BaroVal;
int8	BaroType;
int16 	BaroClimbAvailable, BaroDescentAvailable;
int16	AltitudeUpdateRate;
int8	BaroRetries;
i32u	BaroValF;
int16	BaroFilterA;

#ifdef SIMULATE
int24	FakeBaroRelAltitude;
int8 SimulateCycles = 0;
#endif // SIMULATE

// -----------------------------------------------------------

// Freescale ex Motorola MPX4115 Barometer with ADS7823 12bit ADC

void SetFreescaleMCP4725(int16);
void SetFreescaleOffset(void);
void ReadFreescaleBaro(void);
int16 FreescaleToDM(int24);
void GetFreescaleBaroAltitude(void);
boolean IsFreescaleBaroActive(void);
void InitFreescaleBarometer(void);

#define FS_DAC_MAX	 	4095 	// 12 bits

#define FS_ADC_TIME_MS	50		// 20Hz
#define FS_ADC_MAX	 	4095 	// 12 bits
#define FS_ADC_I2C_ID	0x90 	// ADS7823 ADC
#define FS_ADC_I2C_WR	0x90 	// ADS7823 ADC
#define FS_ADC_I2C_RD	0x91 	// ADS7823 ADC
#define FS_ADC_I2C_CMD	0x00

#define FS_DAC_MCP_WR		0xC8
#define FS_DAC_MCP_RD		0xC9
#define FS_DAC_MCP_CMD		0x40 	// write to DAC registor in next 2 bytes
#define FS_DAC_MCP_EPROM	0x60    // write to DAC registor and eprom

void SetFreescaleMCP4725(int16 d)
{	
	static int8 r;
	static i16u dd;

	dd.u16 = d << 4; // left align
	I2CStart();
		r = WriteI2CByte(FS_DAC_MCP_WR) != I2C_ACK;
		r = WriteI2CByte(FS_DAC_MCP_CMD) != I2C_ACK;
		r = WriteI2CByte(dd.b1) != I2C_ACK;
		r = WriteI2CByte(dd.b0) != I2C_ACK;
	I2CStop();
} // SetFreescaleMCP4725

//#define GKEOFFSET
#ifdef GKEOFFSET

void SetFreescaleOffset(void)
{
	static int16 Hop;
	static int8 i;

	BaroOffsetDAC = 0;
	Hop = FS_DAC_MAX >> 1;

	SetFreescaleMCP4725(BaroOffsetDAC);
	Delay1mS(2);

	ReadFreescaleBaro();
	if ( BaroVal.u16 < 16380 ) // sum of 4 samples - aiming for 25% of FS on Baro ADC
	{
		for (i = 0; i < 12; i++) // 8 should be OK
		{
			BaroOffsetDAC += Hop;
			Hop >>= 2 + 1; // avoid zero hop
			SetFreescaleMCP4725(BaroOffsetDAC);
			Delay1mS(2);
			ReadFreescaleBaro();
			if ( BaroVal.u16 > (uint16)(((uint24)FS_ADC_MAX*4L*3L)/4L) )
				BaroOffsetDAC -= Hop;
			else
				BaroOffsetDAC += Hop;
		}
	}
	else
		F.BaroAltitudeValid = false;

} // SetFreescaleOffset

#else

void SetFreescaleOffset(void)
{ 	// Steve Westerfeld
	// 470 Ohm, 1uF RC 0.47mS use 2mS for settling?

	BaroOffsetDAC = FS_DAC_MAX;

	SetFreescaleMCP4725(BaroOffsetDAC); 
	Delay1mS(20); // initial settling
	ReadFreescaleBaro();
	while ( (BaroVal.u16 < (uint16)(((uint24)FS_ADC_MAX*4L*7L)/10L) ) 
		&& (BaroOffsetDAC > 20) )	// first loop gets close
	{
		BaroOffsetDAC -= 20;					// approach at 20 steps out of 4095
		SetFreescaleMCP4725(BaroOffsetDAC); 
		Delay1mS(20);
		ReadFreescaleBaro();
		LEDYellow_TOG;
	}
	
	BaroOffsetDAC += 200;						// move back up to come at it a little slower
	SetFreescaleMCP4725(BaroOffsetDAC);
	Delay1mS(100);
	ReadFreescaleBaro();

	while( (BaroVal.u16 < (uint16)(((uint24)FS_ADC_MAX*4L*3L)/4L) ) && (BaroOffsetDAC > 2) )
	{
		BaroOffsetDAC -= 2;
		SetFreescaleMCP4725(BaroOffsetDAC);
		Delay1mS(10);
		ReadFreescaleBaro();
		LEDYellow_TOG;
	}

	Delay1mS(200); // wait for caps to settle
	F.BaroAltitudeValid = BaroOffsetDAC > 0;
	
} // SetFreescaleOffset

#endif // GKEOFFSET

void ReadFreescaleBaro(void)
{
	static uint8 B[8], r;
	static i16u B0, B1, B2, B3;

	mS[BaroUpdate] = mSClock() + FS_ADC_TIME_MS;

	I2CStart();  // start conversion
	if( WriteI2CByte(FS_ADC_I2C_WR) != I2C_ACK ) goto FSError;
	if( WriteI2CByte(FS_ADC_I2C_CMD) != I2C_ACK ) goto FSError;

	I2CStart();	// read block of 4 baro samples
	if( WriteI2CByte(FS_ADC_I2C_RD) != I2C_ACK ) goto FSError;
	r = ReadI2CString(B, 8);
	I2CStop();

	B0.b0 = B[1]; B0.b1 = B[0];
	B1.b0 = B[3]; B1.b1 = B[2];
	B2.b0 = B[5]; B2.b1 = B[4];
	B3.b0 = B[7]; B3.b1 = B[6];

	BaroVal.u16 = B0.u16 + B1.u16 + B2.u16 + B3.u16;
	#ifndef JIM_MPX_INVERT
	BaroVal.u16 = (uint16)16380 - BaroVal.u16; // inverting op-amp
	#endif // !JIM_MPX_INVERT

	F.BaroAltitudeValid = true;
	return;

FSError:
	I2CStop();

	F.BaroAltitudeValid = F.HoldingAlt = false;
	if ( State == InFlight ) 
	{
		Stats[BaroFailS]++; 
		F.BaroFailure = true;
	}
	return;
} // ReadFreescaleBaro

int16 FreescaleToDM(int24 p)
{ // decreasing pressure is increase in altitude negate and rescale to decimetre altitude
	return(-(p * (int24)8 )/(int24)P[BaroScale]);
}  // FreescaleToDM

void GetFreescaleBaroAltitude(void)
{
	static int24 BaroPressure;

	if ( mSClock() >= mS[BaroUpdate] )
	{
		ReadFreescaleBaro();
		if ( F.BaroAltitudeValid )
		{
			BaroPressure = (int24)BaroVal.u16; // sum of 4 samples

			LPFilter24(&BaroPressure, &BaroValF, BaroFilterA); 
		
			BaroRelAltitude = FreescaleToDM(BaroPressure - OriginBaroPressure);

			#ifdef SIMULATE
			if ( State == InFlight )
			{
				if ( ++SimulateCycles == AltitudeUpdateRate )
				{
					FakeBaroRelAltitude += ( DesiredThrottle - CruiseThrottle ) + AltComp;
					if ( FakeBaroRelAltitude < 0 ) 
						FakeBaroRelAltitude = 0;
		
					SimulateCycles = 0;
				}
		
				BaroRelAltitude = FakeBaroRelAltitude;
			}				
			#endif // SIMULATE

			F.NewBaroValue = F.BaroAltitudeValid;
		}
	}	
			
} // GetFreescaleBaroAltitude

boolean IsFreescaleBaroActive(void)
{ // check for Freescale Barometer
	
	I2CStart();
	if( WriteI2CByte(FS_ADC_I2C_ID) != I2C_ACK ) goto FreescaleInactive;

	BaroType = BaroMPX4115;
	I2CStop();

	return(true);

FreescaleInactive:
	I2CStop();
	return(false);

} // IsFreescaleBaroActive

void InitFreescaleBarometer(void)
{
	static int16 BaroOriginAltitude, MinAltitude, Error;
	static int24 BaroPressureP;

	AltitudeUpdateRate = 1000L/FS_ADC_TIME_MS;

	BaroFilterA = ( (int24) FS_ADC_TIME_MS * 256L) / ( 10000L / ( 6L * (int16) ADC_ALT_FREQ ) + (int16) FS_ADC_TIME_MS );

	BaroTemperature = 0;
	Error = ((int16)P[BaroScale] * 2)/16;  // 0.2M
	BaroPressure =  0;

	BaroRetries = 0;
	do {
		BaroPressureP = BaroPressure;
	
		SetFreescaleOffset();
	
		while ( mSClock() < mS[BaroUpdate] ) {};
		ReadFreescaleBaro();
		BaroPressure = (int24)BaroVal.u16;

	} while ( ( ++BaroRetries < BARO_INIT_RETRIES ) 
			&& ( Abs(BaroPressure - BaroPressureP) > Error ) );

	F.BaroAltitudeValid = BaroRetries < BARO_INIT_RETRIES;

	OriginBaroPressure = BaroPressure;
	BaroValF.i32 = 0;
	BaroValF.iw1 = OriginBaroPressure;

	BaroRelAltitudeP = BaroRelAltitude = 0;
	
	MinAltitude = FreescaleToDM((int24)FS_ADC_MAX*4);
	BaroOriginAltitude = FreescaleToDM(OriginBaroPressure);
	BaroDescentAvailable = MinAltitude - BaroOriginAltitude;
	BaroClimbAvailable = -BaroOriginAltitude;

	//F.BaroAltitudeValid &= (( BaroClimbAvailable >= BARO_MIN_CLIMB ) 
		// && (BaroDescentAvailable <= BARO_MIN_DESCENT));

	#ifdef SIMULATE
	FakeBaroRelAltitude = 0;
	#endif // SIMULATE

} // InitFreescaleBarometer

// -----------------------------------------------------------

// Bosch SMD500 and BMP085 Barometers

void StartBoschBaroADC(boolean);
int24 CompensatedBoschPressure(uint16, uint16);

void GetBoschBaroAltitude(void);
boolean IsBoschBaroActive(void);
void InitBoschBarometer(void);

#define BOSCH_ID_BMP085		((uint8)(0x55))
#define BOSCH_I2C_ID			0xee
#define BOSCH_TEMP_BMP085	0x2e
#define BOSCH_TEMP_SMD500	0x6e
#define BOSCH_PRESS			0xf4
#define BOSCH_CTL			0xf4				// OSRS=3 for BMP085 25.5mS, SMD500 34mS				
#define BOSCH_ADC_MSB		0xf6
#define BOSCH_ADC_LSB		0xf7
#define BOSCH_ADC_XLSB		0xf8				// BMP085
#define BOSCH_TYPE			0xd0

// SMD500 9.5mS (T) 34mS (P)  
// BMP085 4.5mS (T) 25.5mS (P) OSRS=3
#define BOSCH_TEMP_TIME_MS			11	// 10 increase to make P+T acq time ~50mS
//#define BMP085_PRESS_TIME_MS 		26
//#define SMD500_PRESS_TIME_MS 		34
#define BOSCH_PRESS_TIME_MS			38
#define BOSCH_PRESS_TEMP_TIME_MS	50	// pressure and temp time + overheads 	

void StartBoschBaroADC(boolean ReadPressure)
{
	static uint8 TempOrPress;

	if ( ReadPressure )
	{
		TempOrPress = BOSCH_PRESS;
		mS[BaroUpdate] = mSClock() + BOSCH_PRESS_TIME_MS;
	}
	else
	{
		mS[BaroUpdate] = mSClock() + BOSCH_TEMP_TIME_MS;	
		if ( BaroType == BaroBMP085 )
			TempOrPress = BOSCH_TEMP_BMP085;
		else
			TempOrPress = BOSCH_TEMP_SMD500;
	}

	I2CStart();
		if( WriteI2CByte(BOSCH_I2C_ID) != I2C_ACK ) goto SBerror;
	
		// access control register, start measurement
		if( WriteI2CByte(BOSCH_CTL) != I2C_ACK ) goto SBerror;
	
		// select 32kHz input, measure temperature
		if( WriteI2CByte(TempOrPress) != I2C_ACK ) goto SBerror;
	I2CStop();

	F.BaroAltitudeValid = true;
	return;

SBerror:
	I2CStop();
	F.BaroAltitudeValid = F.HoldingAlt = false; 
	return;
} // StartBoschBaroADC

void ReadBoschBaro(void)
{
	// Possible I2C protocol error - split read of ADC
	I2CStart();
		if( WriteI2CByte(BOSCH_I2C_ID) != I2C_ACK ) goto RVerror;
		if( WriteI2CByte(BOSCH_ADC_MSB) != I2C_ACK ) goto RVerror;
		I2CStart();	// restart
		if( WriteI2CByte(BOSCH_I2C_ID+1) != I2C_ACK ) goto RVerror;
		BaroVal.b1 = ReadI2CByte(I2C_NACK);
	I2CStop();
			
	I2CStart();
		if( WriteI2CByte(BOSCH_I2C_ID) != I2C_ACK ) goto RVerror;
		if( WriteI2CByte(BOSCH_ADC_LSB) != I2C_ACK ) goto RVerror;
		I2CStart();	// restart
		if( WriteI2CByte(BOSCH_I2C_ID+1) != I2C_ACK ) goto RVerror;
		BaroVal.b0 = ReadI2CByte(I2C_NACK);
	I2CStop();

	F.BaroAltitudeValid = true;
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
} // ReadBoschBaro

#define BOSCH_BMP085_TEMP_COEFF		62L 	
#define BOSCH_SMD500_TEMP_COEFF		50L

int24 CompensatedBoschPressure(uint16 BaroPress, uint16 BaroTemp)
{
	static int24 BaroTempComp;

	if ( BaroType == BaroBMP085 )
		BaroTempComp = (BaroTemp * BOSCH_BMP085_TEMP_COEFF + 64L) >> 7;
	else
		BaroTempComp = (BaroTemp * BOSCH_SMD500_TEMP_COEFF + 8L) >> 4;
	
	return ((int24)BaroPress + BaroTempComp - OriginBaroPressure);

} // CompensatedBoschPressure

void GetBoschBaroAltitude(void)
{
	static int24 Temp;

	if ( mSClock() >= mS[BaroUpdate] )
	{
		ReadBoschBaro();
		if ( F.BaroAltitudeValid )
			if ( AcquiringPressure )
			{
				BaroPressure = (int24)BaroVal.u16;
				AcquiringPressure = false;
			}
			else
			{
				BaroTemperature = (int24)BaroVal.u16;
				AcquiringPressure = true;
		
				Temp = CompensatedBoschPressure(BaroPressure, BaroTemperature);			
				CompBaroPressure -= BaroQ.B[BaroQ.Head];		
				BaroQ.B[BaroQ.Head] = Temp;
				CompBaroPressure += Temp;
				BaroQ.Head = (BaroQ.Head + 1) & (BARO_BUFF_SIZE -1);

				// Pressure queue has 4 entries corresponding to an average delay at 20Hz of 0.1Sec
				// decreasing pressure is increase in altitude negate and rescale to decimetre altitude

				Temp = (int24)CompBaroPressure;

				LPFilter24(&Temp, &BaroValF, BaroFilterA);

				BaroRelAltitude = -SRS32(Temp * (int16)P[BaroScale], 7);

				#ifdef SIMULATE
				if ( State == InFlight )
				{
					if ( ++SimulateCycles == AltitudeUpdateRate )
					{
						FakeBaroRelAltitude += ( DesiredThrottle - CruiseThrottle ) + AltComp;
						if ( FakeBaroRelAltitude < 0 ) 
							FakeBaroRelAltitude = 0;
				
						SimulateCycles = 0;
					}
			
					BaroRelAltitude = FakeBaroRelAltitude;
				}				
				#endif // SIMULATE

				F.NewBaroValue = F.BaroAltitudeValid;			
			}
			else
			{
				AcquiringPressure = true;
				Stats[BaroFailS]++;
			}	

		StartBoschBaroADC(AcquiringPressure);
	}			
} // GetBoschBaroAltitude

boolean IsBoschBaroActive(void)
{ // check for Bosch Barometers
	static uint8 r;

	I2CStart();
		if( WriteI2CByte(BOSCH_I2C_ID) != I2C_ACK ) goto BoschInactive;
		if( WriteI2CByte(BOSCH_TYPE) != I2C_ACK ) goto BoschInactive;
		I2CStart();	// restart
		if( WriteI2CByte(BOSCH_I2C_ID+1) != I2C_ACK ) goto BoschInactive;
		r = ReadI2CByte(I2C_NACK);
	I2CStop();

	if (r == BOSCH_ID_BMP085 )
		BaroType = BaroBMP085;
	else
		BaroType = BaroSMD500;

	return(true);

BoschInactive:
	return(false);

} // IsBoschBaroActive

void InitBoschBarometer(void)
{
	int8 s;
	int24 Temp, Diff, CompBaroPressureP;

	AltitudeUpdateRate = 1000L / BOSCH_PRESS_TEMP_TIME_MS;

	BaroFilterA = ( (int24) BOSCH_PRESS_TEMP_TIME_MS * 256L) / ( 10000L / ( 6L * (int16) ADC_ALT_FREQ ) + (int16) BOSCH_PRESS_TEMP_TIME_MS ); 

	F.NewBaroValue = false;
	CompBaroPressure = 0;

	BaroRetries = 0;
	do // occasional I2C misread of Temperature so keep doing it until the Origin is stable!!
	{	
		CompBaroPressureP = CompBaroPressure;
		CompBaroPressure = BaroQ.Head = 0;
		
		AcquiringPressure = true;
		StartBoschBaroADC(AcquiringPressure); // Pressure
	
		for ( s = 0; s < 4; s++ )
		{
			while ( mSClock() < mS[BaroUpdate] );
			ReadBoschBaro(); // Pressure	
			BaroPressure = BaroVal.u16;
			
			AcquiringPressure = !AcquiringPressure;
			StartBoschBaroADC(AcquiringPressure); // Temperature
			while ( mSClock() < mS[BaroUpdate] );
			ReadBoschBaro();
			BaroTemperature = BaroVal.u16;
			
			Temp = CompensatedBoschPressure(BaroPressure, BaroTemperature);
			BaroQ.B[s] = Temp;
			CompBaroPressure += Temp;
	
			AcquiringPressure = !AcquiringPressure;
			StartBoschBaroADC(AcquiringPressure); 	
		}

	} while ( ( ++BaroRetries < BARO_INIT_RETRIES ) && ( Abs(CompBaroPressure - CompBaroPressureP) > 12 ) ); // stable within ~0.5M
	
	OriginBaroPressure = SRS32(CompBaroPressure, 2);

	BaroValF.i32 = 0;

	F.BaroAltitudeValid = BaroRetries < BARO_INIT_RETRIES;		
	BaroRelAltitudeP = BaroRelAltitude = 0;

	#ifdef SIMULATE
		FakeBaroRelAltitude = 0;
	#endif // SIMULATE

} // InitBoschBarometer

// -----------------------------------------------------------

void ShowBaroType(void)
{
	switch ( BaroType ) {
		case BaroMPX4115: TxString("MPX4115\r\n"); break;
		case BaroSMD500: TxString("SMD500\r\n"); break;
		case BaroBMP085: TxString("BMP085\r\n"); break;
		case BaroUnknown: TxString("None\r\n"); break;
		default: break;
	}
} // ShowBaroType

#ifdef TESTING
void BaroTest(void)
{
	TxString("\r\nAltitude test\r\n");

	TxString("Type:\t"); ShowBaroType();
	
	TxString("Init Retries:\t");
	TxVal32((int32)BaroRetries - 2, 0, ' '); // alway minimum of 2
	if ( BaroRetries >= BARO_INIT_RETRIES )
		TxString(" FAILED Init.\r\n");
	else
		TxNextLine();

	if ( BaroType == BaroMPX4115 )
	{
		TxString("Range   :\t");			
		TxVal32((int32) BaroDescentAvailable,1, ' ');
		TxString("-> ");
		TxVal32((int32) BaroClimbAvailable,1, 'M');
		TxString(" {Offset ");TxVal32((int32)BaroOffsetDAC, 0,'}'); 
		if (( BaroClimbAvailable < BARO_MIN_CLIMB ) || (BaroDescentAvailable > BARO_MIN_DESCENT))
			TxString(" Bad climb or descent range - offset adjustment?");
		TxNextLine();
	}

	if ( !F.BaroAltitudeValid ) goto BAerror;	

	while ( !F.NewBaroValue )
		GetBaroAltitude();	
	F.NewBaroValue = false;

	TxString("Alt.:     \t");	
	TxVal32((int32)BaroRelAltitude, 1, ' ');
	TxString("M\r\n");

	TxString("\r\nR.Finder: \t");
	if ( F.RangefinderAltitudeValid )
	{
		GetRangefinderAltitude();
		TxVal32((int32)RangefinderAltitude, 2, ' ');
		TxString("M\r\n");
	}
	else
		TxString("no rangefinder\r\n");
	
	TxString("\r\nAmbient :\t");
	TxVal32((int32)AmbientTemperature.i16, 1, ' ');
	TxString("C\r\n");

	return;
BAerror:
	TxString("FAIL\r\n");
} // BaroTest

#endif // TESTING

void GetBaroAltitude(void)
{
	static int24 Temp, AltChange;

	if ( BaroType == BaroMPX4115 )
		GetFreescaleBaroAltitude();
	else
		GetBoschBaroAltitude();

	AltChange = BaroRelAltitude - BaroRelAltitudeP;

	if ( Abs(AltChange) > BARO_SANITY_CHECK_DMPS )
	{
		BaroRelAltitude = BaroRelAltitudeP;	// use previous value
		Stats[BaroFailS]++;
	}

	Temp = AltChange * AltitudeUpdateRate;
	BaroROC = BaroROCFilter(BaroROC, Temp);					
	BaroRelAltitudeP = BaroRelAltitude;
	BaroRelAltitudeP = BaroRelAltitude;

	if ( ( State == InFlight ) && F.NewBaroValue )
	{
		if ( BaroROC > Stats[MaxBaroROCS] )
			Stats[MaxBaroROCS] = BaroROC;
		else
			if ( BaroROC < Stats[MinBaroROCS] )
				Stats[MinBaroROCS] = BaroROC;
	
		if ( BaroRelAltitude > Stats[BaroRelAltitudeS] ) 
			Stats[BaroRelAltitudeS] = BaroRelAltitude;
	}
	
} // GetBaroAltitude

void InitBarometer(void)
{
	BaroRelAltitude = BaroRelAltitudeP = BaroROC = CompBaroPressure = OriginBaroPressure = 0;
	BaroType = BaroUnknown;

	AltComp = AltDiffSum = AltDSum = 0;
	F.BaroAltitudeValid= true; // optimistic

	if ( IsFreescaleBaroActive() )	
		InitFreescaleBarometer();
	else
		if ( IsBoschBaroActive() )
			InitBoschBarometer();
		else	
		{
			F.BaroAltitudeValid = F.HoldingAlt = false;
			Stats[BaroFailS]++;
		}
} // InitBarometer



