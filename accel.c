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

// Accelerator 400KHz I2C or SPI

#include "uavx.h"

void SendCommand(int8);
uint8 ReadLISL(uint8);
uint8 ReadLISLNext(void);
void WriteLISL(uint8, uint8);
void IsLISLActive(void);

void ReadAccelerations(void);
void DoAccelerations(void);
void GetNeutralAccelerations(void);
void AccelerometerTest(void);
void InitAccelerometers(void);

#pragma udata accs
uint8 	AccWhoAmI;
i16u Ax, Ay, Az;
int8 IntCorr[2];
int8 Neutral[3];
int16 Vel[3], Acc[3], Comp[4];
#pragma udata

#ifdef UAVX_HW

void ReadAccelerations(void)
{
 	// I2C Acc not implemented yet - not used in UAVXLight
} // ReadAccelerations

void GetNeutralAccelerations(void)
{
	// I2C Acc not implemented yet - not used in UAVXLight
	NeutralLR = NeutralFB = NeutralDU = 0;
} // GetNeutralAccelerations

#ifdef TESTING

void AccelerometerTest(void)
{
	TxString("\r\nAccelerometer test:\r\n");
	TxString("\r\n(Acc. not active for UAVXLight)\r\n");
} // AccelerometerTest

#endif // TESTING

void InitAccelerometers(void)
{
	F.AccelerationsValid = false;
	F.AccFailure = true;
} // InitAccelerometers

#else

#ifdef CLOCK_16MHZ
	#define SPI_HI_DELAY Delay10TCY()
	#define SPI_LO_DELAY Delay10TCY()
#else // CLOCK_40MHZ
	#define SPI_HI_DELAY Delay10TCYx(2)
	#define SPI_LO_DELAY Delay10TCYx(2)
#endif // CLOCK_16MHZ

// LISL-Register mapping

#define	LISL_WHOAMI		(uint8)(0x0f)
#define	LISL_OFFSET_X	(uint8)(0x16)
#define	LISL_OFFSET_Y	(uint8)(0x17)
#define	LISL_OFFSET_Z	(uint8)(0x18)
#define	LISL_GAIN_X		(uint8)(0x19)
#define	LISL_GAIN_Y		(uint8)(0x1A)
#define	LISL_GAIN_Z		(uint8)(0x1B)
#define	LISL_CTRLREG_1	(uint8)(0x20)
#define	LISL_CTRLREG_2	(uint8)(0x21)
#define	LISL_CTRLREG_3	(uint8)(0x22)
#define	LISL_STATUS		(uint8)(0x27)
#define LISL_OUTX_L		(uint8)(0x28)
#define LISL_OUTX_H		(uint8)(0x29)
#define LISL_OUTY_L		(uint8)(0x2A)
#define LISL_OUTY_H		(uint8)(0x2B)
#define LISL_OUTZ_L		(uint8)(0x2C)
#define LISL_OUTZ_H		(uint8)(0x2D)
#define LISL_FF_CFG		(uint8)(0x30)
#define LISL_FF_SRC		(uint8)(0x31)
#define LISL_FF_ACK		(uint8)(0x32)
#define LISL_FF_THS_L	(uint8)(0x34)
#define LISL_FF_THS_H	(uint8)(0x35)
#define LISL_FF_DUR		(uint8)(0x36)
#define LISL_DD_CFG		(uint8)(0x38)
#define LISL_INCR_ADDR	(uint8)(0x40)
#define LISL_READ		(uint8)(0x80)

void SendCommand(int8 c)
{
	static int8 s;

	SPI_IO = WR_SPI;	
	SPI_CS = SEL_LISL;	
	for( s = 8; s; s-- )
	{
		SPI_SCL = 0;
		if( c & 0x80 )
			SPI_SDA = 1;
		else
			SPI_SDA = 0;
		c <<= 1;
		SPI_LO_DELAY;
		SPI_SCL = 1;
		SPI_HI_DELAY;
	}
} // SendCommand

uint8 ReadLISL(uint8 c)
{
	static uint8 d;

	// SPI_SDA = 1;	// very important!! really!! LIS3L likes it
	SendCommand(c);
	SPI_IO = RD_SPI;	// SDA is input
	d=ReadLISLNext();
	
	if( (c & LISL_INCR_ADDR) == (uint8)0 )
		SPI_CS = DSEL_LISL;
	return(d);
} // ReadLISL

uint8 ReadLISLNext(void)
{
	static int8 s;
	static uint8 d;

	for( s = 8; s; s-- )
	{
		SPI_SCL = 0;
		SPI_LO_DELAY;
		d <<= 1;
		if( SPI_SDA == (uint8)1 )
			d |= 1;	
		SPI_SCL = 1;
		SPI_HI_DELAY;
	}
	return(d);
} // ReadLISLNext

void WriteLISL(uint8 d, uint8 c)
{
	static int8 s;

	SendCommand(c);
	for( s = 8; s; s-- )
	{
		SPI_SCL = 0;
		if( d & 0x80 )
			SPI_SDA = 1;
		else
			SPI_SDA = 0;
		d <<= 1;
		SPI_LO_DELAY;
		SPI_SCL = 1;
		SPI_HI_DELAY;
	}
	SPI_CS = DSEL_LISL;
	SPI_IO = RD_SPI;	// IO is input (to allow RS232 reception)
} // WriteLISL

void IsLISLActive(void)
{
	F.AccelerationsValid = false;
	SPI_CS = DSEL_LISL;
	WriteLISL(0b01001010, LISL_CTRLREG_2); // enable 3-wire, BDU=1, +/-2g

	AccWhoAmI = ReadLISL(LISL_WHOAMI + LISL_READ);
	if( AccWhoAmI == (uint8)0x3A )	// a LIS03L sensor is there!
	{
		WriteLISL(0b11000111, LISL_CTRLREG_1); // on always, 40Hz sampling rate,  10Hz LP cutoff, enable all axes
		WriteLISL(0b00000000, LISL_CTRLREG_3);
		WriteLISL(0b01000000, LISL_FF_CFG); // latch, no interrupts; 
		WriteLISL(0b00000000, LISL_FF_THS_L);
		WriteLISL(0b11111100, LISL_FF_THS_H); // -0,5g threshold
		WriteLISL(255, LISL_FF_DUR);
		WriteLISL(0b00000000, LISL_DD_CFG);
		F.AccelerationsValid = true;
	}
	else
		F.AccFailure = true;
} // IsLISLActive

void ReadAccelerations()
{
	F.AccelerationsValid = ReadLISL(LISL_WHOAMI + LISL_READ) == (uint8)0x3a; // Acc still there?
	if ( F.AccelerationsValid ) 
	{
		Ax.b0 = ReadLISL(LISL_OUTX_L + LISL_INCR_ADDR + LISL_READ);
		Ax.b1 = ReadLISLNext();
		Ay.b0 = ReadLISLNext();
		Ay.b1 = ReadLISLNext();
		Az.b0 = ReadLISLNext();
		Az.b1 = ReadLISLNext();
		SPI_CS = DSEL_LISL;	// end transmission
	}
	else
	{
		Ax.i16 = Ay.i16 = Az.i16 = 0;
		if ( State == InFlight )
		{
			Stats[AccFailS]++;	// data over run - acc out of range
			// use neutral values!!!!
			F.AccFailure = true;
		}
	}
} // ReadAccelerations

void GetNeutralAccelerations(void)
{
	// this routine is called ONLY ONCE while booting
	// and averages accelerations over 16 samples.
	// Puts values in Neutralxxx registers.

	// Sensor vertical mount Ax right, Ay up, Az back
	// AccX = -Az, AccY = Ax, AccZ = -Ay 

	static uint8 i;
	static int16 TempBF, TempLR, TempUD;

	// already done in caller program
	 TempBF = TempLR = TempUD = 0;
	if ( F.AccelerationsValid )
	{
		for ( i = 16; i; i--)
		{
			while( (ReadLISL(LISL_STATUS + LISL_READ) & 0x08) == (uint8)0 );
			ReadAccelerations();
	
		#ifdef USE_FLAT_ACC
			TempFB -= Ay.i16;
			TempLR += Ax.i16;
			TempUD -= Az.i16;
		#else
			TempBF += Az.i16;
			TempLR -= Ax.i16;
			TempUD += Ay.i16;
		#endif // USE_FLAT_ACC
		}	
	
		TempBF = SRS16(TempBF, 4);
		TempLR = SRS16(TempLR, 4);
		TempUD = SRS16(TempUD, 4);
	
		Neutral[BF] = Limit(BF, -99, 99);
		Neutral[LR] = Limit(LR, -99, 99);
		Neutral[UD] = Limit(UD - GRAVITY, -99, 99); // 1g
	}
	else
		Neutral[LR] = Neutral[BF] = Neutral[UD] = 0;

} // GetNeutralAccelerations

void DoAccelerations(void)
{
	if( F.AccelerationsValid )
	{
		ReadAccelerations();

		#ifdef USE_FLAT_ACC // chip up and twisted over
			Acc[BF] = -Ay.i16;
			Acc[LR] =  Ax.i16;	
			Acc[UD] = -Az.i16;
		#else
			Acc[BF] =  Az.i16;	
			Acc[LR] = -Ax.i16;
			Acc[UD] =  Ay.i16;
		#endif // USE_FLAT_ACC

		// Neutral[ {LR, BF, UD} ] pass through UAVPSet 
		// and come back as AccMiddle[LR] etc.

		Acc[BF] -= (int16)P[MiddleBF];
		Acc[LR] -= (int16)P[MiddleLR];
		Acc[UD] -= (int16)P[MiddleUD];
	}
	else
	{
		Acc[LR] = Acc[BF] = 0;
		Acc[UD] = GRAVITY;
	}
} // DoAccelerations

#ifdef TESTING

void AccelerometerTest(void)
{
	TxString("\r\nAccelerometer test:\r\n");
	TxString("Read once - no averaging\r\n");

	InitAccelerometers();

	TxString("Acc ID: 0x");
	TxValH( AccWhoAmI);
	TxString(" (0x3A)\r\n");

	if( F.AccelerationsValid )
	{
		ReadAccelerations();
	
		TxString("\tL->R: \t");
		TxVal32(((int32)Ax.i16*1000L)/1024, 3, 'G');
		TxString(" (");
		TxVal32((int32)Ax.i16, 0 , ')');
		if ( Abs((Ax.i16)) > 128 )
			TxString(" fault?");
		TxNextLine();

		TxString("\tB->F: \t");	
		TxVal32(-((int32)Az.i16*1000L)/1024, 3, 'G');
		TxString(" (");
		TxVal32(-(int32)Az.i16, 0 , ')');
		if ( Abs((Az.i16)) > 128 )
			TxString(" fault?");	
		TxNextLine();

		TxString("\tU->D:    \t");
	
		TxVal32(-((int32)Ay.i16*1000L)/1024, 3, 'G');
		TxString(" (");
		TxVal32(-(int32)Ay.i16, 0 , ')');
		if ( ( Ay.i16 < 896 ) || ( Ay.i16 > 1152 ) ) // zzz
			TxString(" fault?");	
		TxNextLine();
	}
	else
		TxString("\r\n(Acc. not present)\r\n");
} // AccelerometerTest

#endif // TESTING

void InitAccelerometers(void)
{
	Neutral[LR] = Neutral[BF] = Neutral[UD] = Ax.i16 = Ay.i16 = Az.i16 = 0;

	IsLISLActive();
	if( F.AccelerationsValid )
	{
		LEDYellow_ON;
		GetNeutralAccelerations();
		LEDYellow_OFF;
	}
	else
		F.AccFailure = true;
} // InitAccelerometers

#endif // UAVX_HW


