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

// Accelerator 400KHz I2C or SPI

#include "uavx.h"

void SendCommand(int8);
uint8 ReadLISL(uint8);
uint8 ReadLISLNext(void);
void WriteLISL(uint8, uint8);
void IsLISLActive(void);

void ReadAccelerations(void);
void GetNeutralAccelerations(void);
void AccelerometerTest(void);
void InitAccelerometers(void);

//#ifdef CLOCK_16MHZ
	#define SPI_HI_DELAY Delay10TCY()
	#define SPI_LO_DELAY Delay10TCY()
//#else // CLOCK_40MHZ
//	#define SPI_HI_DELAY Delay10TCYx(2)
//	#define SPI_LO_DELAY Delay10TCYx(2)
//#endif // CLOCK_16MHZ

// LISL-Register mapping

#define	LISL_WHOAMI		(0x0f)
#define	LISL_OFFSET_X	(0x16)
#define	LISL_OFFSET_Y	(0x17)
#define	LISL_OFFSET_Z	(0x18)
#define	LISL_GAIN_X		(0x19)
#define	LISL_GAIN_Y		(0x1A)
#define	LISL_GAIN_Z		(0x1B)
#define	LISL_CTRLREG_1	(0x20)
#define	LISL_CTRLREG_2	(0x21)
#define	LISL_CTRLREG_3	(0x22)
#define	LISL_STATUS		(0x27)
#define LISL_OUTX_L		(0x28)
#define LISL_OUTX_H		(0x29)
#define LISL_OUTY_L		(0x2A)
#define LISL_OUTY_H		(0x2B)
#define LISL_OUTZ_L		(0x2C)
#define LISL_OUTZ_H		(0x2D)
#define LISL_FF_CFG		(0x30)
#define LISL_FF_SRC		(0x31)
#define LISL_FF_ACK		(0x32)
#define LISL_FF_THS_L	(0x34)
#define LISL_FF_THS_H	(0x35)
#define LISL_FF_DUR		(0x36)
#define LISL_DD_CFG		(0x38)
#define LISL_INCR_ADDR	(0x40)
#define LISL_READ		(0x80)

#pragma udata accs
i16u	Ax, Ay, Az;
int8	LRIntCorr, FBIntCorr;
int16	Rl,Pl,Yl;						// PID output values
int8	NeutralLR, NeutralFB, NeutralDU;
int16	DUVel, LRVel, FBVel, DUAcc, LRAcc, FBAcc, DUComp, LRComp, FBComp;
#pragma udata

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

	//SPI_SDA = 1;	// very important!! really!! LIS3L likes it
	SendCommand(c);
	SPI_IO = RD_SPI;	// SDA is input
	d=ReadLISLNext();
	
	if( (c & LISL_INCR_ADDR) == 0 )
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
		if( SPI_SDA == 1 )
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
	static int8 r;

	F.AccelerationsValid = false;
	SPI_CS = DSEL_LISL;
	WriteLISL(0b01001010, LISL_CTRLREG_2); // enable 3-wire, BDU=1, +/-2g

	r = ReadLISL(LISL_WHOAMI + LISL_READ);
	if( r == 0x3A )	// a LIS03L sensor is there!
	{
		WriteLISL(0b11000111, LISL_CTRLREG_1); // on always, 40Hz, enable all axis
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
	F.AccelerationsValid = ReadLISL(LISL_WHOAMI + LISL_READ) == 0x3a; // Acc still there?
	if ( F.AccelerationsValid ) 
	{
		Ax.b0  = ReadLISL(LISL_OUTX_L + LISL_INCR_ADDR + LISL_READ);
		Ax.b1 = ReadLISLNext();
		Ay.b0  = ReadLISLNext();
		Ay.b1 = ReadLISLNext();
		Az.b0  = ReadLISLNext();
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
	static uint8 i;
	static int16 LR, FB, DU;

	Delay100mSWithOutput(2);	// wait 1/10 sec until LISL is ready to talk
	// already done in caller program
	LR = FB = DU = 0;
	for ( i = 16; i; i--)
	{
		while( (ReadLISL(LISL_STATUS + LISL_READ) & 0x08) == 0 );
		ReadAccelerations();

		if (F.UsingFlatAcc )
		{
			LR -= Ax.i16;
			FB += Ay.i16;
			DU += Az.i16;
		}
		else
		{
			LR += Ax.i16;
			DU += Ay.i16;
			FB += Az.i16;
		}
	}	

	LR = SRS16(LR, 4);
	FB = SRS16(FB, 4);
	DU = SRS16(DU, 4);

	NeutralLR = Limit(LR, -99, 99);
	NeutralFB = Limit(FB, -99, 99);
	NeutralDU = Limit(DU - 1024, -99, 99); // -1g
} // GetNeutralAccelerations

#ifdef TESTING

void AccelerometerTest(void)
{
	TxString("\r\nAccelerometer test:\r\n");
	TxString("Read once - no averaging\r\n");

	InitAccelerometers();
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

		TxString("\tF->B: \t");	
		TxVal32(((int32)Az.i16*1000L)/1024, 3, 'G');
		TxString(" (");
		TxVal32((int32)Az.i16, 0 , ')');
		if ( Abs((Az.i16)) > 128 )
			TxString(" fault?");	
		TxNextLine();

		TxString("\tD->U:    \t");
	
		TxVal32(((int32)Ay.i16*1000L)/1024, 3, 'G');
		TxString(" (");
		TxVal32((int32)Ay.i16 - 1024, 0 , ')');
		if ( ( Ay.i16 < 896 ) || ( Ay.i16 > 1152 ) )
			TxString(" fault?");	
		TxNextLine();
	}
	else
		TxString("\r\n(Acc. not present)\r\n");
} // AccelerometerTest

#endif // TESTING

void InitAccelerometers(void)
{
	int8 i;

	Delay100mSWithOutput(5);	// wait 0.5 sec until LISL is ready to talk

	NeutralLR = NeutralFB = NeutralDU = Ax.i16 = Ay.i16 = Az.i16 = 0;

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

