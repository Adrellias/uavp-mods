// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008, 2009 by Prof. Greg Egan           =
// =   Original V3.15 Copyright (c) 2007, 2008 Ing. Wolfgang Mahringer   =
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

// Accelerator sensor routine

#include "uavx.h"

// Prototypes

void SendCommand(int8);
uint8 ReadLISL(uint8);
uint8 ReadLISLNext(void);
void WriteLISL(uint8, uint8);
void IsLISLActive(void);
void InitLISL(void);
void ReadAccelerations(void);
void GetNeutralAccelerations(void);

// Constants

#define SPI_HI_DELAY Delay10TCY()
#define SPI_LO_DELAY Delay10TCY()

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

//	SPI_SDA = 1;	// very important!! really!! LIS3L likes it
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

	F[AccelerationsValid] = false;
	SPI_CS = DSEL_LISL;
	WriteLISL(0b01001010, LISL_CTRLREG_2); // enable 3-wire, BDU=1, +/-2g

	r = ReadLISL(LISL_WHOAMI + LISL_READ);
	if( r == 0x3A )	// a LIS03L sensor is there!
	{
		WriteLISL(0b11000111, LISL_CTRLREG_1); // startup, enable all axis
		WriteLISL(0b00000000, LISL_CTRLREG_3);
		WriteLISL(0b01001000, LISL_FF_CFG); // Y-axis is height
		WriteLISL(0b00000000, LISL_FF_THS_L);
		WriteLISL(0b11111100, LISL_FF_THS_H); // -0,5g threshold
		WriteLISL(255, LISL_FF_DUR);
		WriteLISL(0b00000000, LISL_DD_CFG);
		F[AccelerationsValid] = true;
	}
} // IsLISLActive

void InitLISL(void)
{
	Delay100mSWithOutput(5);	// wait 0.5 sec until LISL is ready to talk

	NeutralLR = 0;
	NeutralFB = 0;
	NeutralDU = 0;
	#ifdef USE_ACCELEROMETER
	IsLISLActive();	
	if( F[AccelerationsValid] )
	{
		LEDYellow_ON;
		GetNeutralAccelerations();
		LEDYellow_OFF;
	}
	#endif  // USE_ACCELEROMETER
} // InitLISL

void ReadAccelerations()
{
	static uint8 r;

	r = ReadLISL(LISL_STATUS + LISL_READ);

	Ax.low8  = ReadLISL(LISL_OUTX_L + LISL_INCR_ADDR + LISL_READ);
	Ax.high8 = ReadLISLNext();
	Ay.low8  = ReadLISLNext();
	Ay.high8 = ReadLISLNext();
	Az.low8  = ReadLISLNext();
	Az.high8 = ReadLISLNext();
	SPI_CS = DSEL_LISL;	// end transmission

	if ( ((r & 0xf0) != 0) && (State == InFlight) ) Stats[AccFailS].i16++;	// data over run - acc out of range

} // ReadAccelerations

void GetNeutralAccelerations(void)
{
	// this routine is called ONLY ONCE while booting
	// and averages accelerations over 16 samples.
	// Puts values in Neutralxxx registers.
	static uint8 i;
	static int16 Rp, Pp, Yp;

	Delay100mSWithOutput(2);	// wait 1/10 sec until LISL is ready to talk
	// already done in caller program
	Rp = Pp = Yp = 0;
	for ( i = 16; i; i--)
	{
		while( (ReadLISL(LISL_STATUS + LISL_READ) & 0x08) == 0 );
		ReadAccelerations();
		
		Rp += Ax.i16;
		Pp += Az.i16;
		Yp += Ay.i16;
	}
	Rp = SRS16(Rp, 4);
	Pp = SRS16(Pp, 4);
	Yp = SRS16(Yp, 4);

	NeutralLR = Limit(Rp, -99, 99);
	NeutralFB = Limit(Pp, -99, 99);
	NeutralDU = Limit(Yp-1024, -99, 99); // -1g
} // GetNeutralAccelerations

