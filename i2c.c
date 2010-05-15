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

// SW I2C Routines for Sensors

#include "uavx.h"

void I2CStart(void);
void I2CStop(void);
boolean I2CWaitClkHi(void);
uint8 SendI2CByte(uint8);
uint8 RecvI2CByte(uint8);
uint8 ScanI2CBus(void);

#ifdef CLOCK_16MHZ
#define	I2C_DELAY		Delay10TCY()
#define I2C_DELAY2		
#else // CLOCK_40MHZ
#define	I2C_DELAY		Delay10TCYx(3)
#define I2C_DELAY2		Delay1TCY();Delay1TCY()	
#endif // CLOCK_16MHZ

#define I2C_IN			1
#define I2C_OUT			0

#define I2C_SDA			PORTBbits.RB6
#define I2C_DIO			TRISBbits.TRISB6
#define I2C_SCL			PORTBbits.RB7
#define I2C_CIO			TRISBbits.TRISB7

#define I2C_DATA_LOW	{I2C_SDA=0;I2C_DELAY2;I2C_DIO=I2C_OUT;I2C_DELAY2;}
#define I2C_DATA_FLOAT	{I2C_DIO=I2C_IN;}
#define I2C_CLK_LOW		{I2C_SCL=0;I2C_DELAY2;I2C_CIO=I2C_OUT;I2C_DELAY2;}
#define I2C_CLK_FLOAT	{I2C_CIO=I2C_IN;I2C_DELAY2;} 

boolean I2CWaitClkHi(void)
{
	static uint8 s;

	I2C_CLK_FLOAT;		// set SCL to input, output a high
	s = 1;
	while( !I2C_SCL )	// timeout wraparound through 255 to 0 1.25mS @ 16MHz
		if( ++s == 0 ) return (false);
	return( true );
} // I2CWaitClkHi

void I2CStart(void)
{
	static boolean r;
	
	I2C_DATA_FLOAT;
	r = I2CWaitClkHi();
	I2C_DATA_LOW;
	I2C_CLK_LOW;
} // I2CStart

void I2CStop(void)
{
	static boolean r;

	I2C_DATA_LOW;
	r = I2CWaitClkHi();
	I2C_DATA_FLOAT;

	I2C_DELAY;

} // I2CStop 

uint8 SendI2CByte(uint8 d)
{
	static uint8 s;

	for ( s = 8; s ; s-- )
	{
		if( d & 0x80 )
			I2C_DATA_FLOAT
		else
			I2C_DATA_LOW
	
		if( I2CWaitClkHi() )
		{ 	
			I2C_CLK_LOW;
			d <<= 1;
		}
		else
			return(I2C_NACK);	
	}

	I2C_DATA_FLOAT;
	if( I2CWaitClkHi() )
		s = I2C_SDA;
	else
		return(I2C_NACK);	

	I2C_CLK_LOW;

	return(s);
} // SendI2CByte

uint8 RecvI2CByte(uint8 r)
{
	static uint8 s, d;

	I2C_DATA_FLOAT;
	d = 0;
	for ( s = 8; s ; s-- )
		if( I2CWaitClkHi() )
		{ 
			d <<= 1;
			if( I2C_SDA )
				d |= 1;
			I2C_CLK_LOW;
 		}
		else
			return(0);

	I2C_SDA = r;
	I2C_DELAY2;
	I2C_DIO = I2C_OUT;
	I2C_DELAY2;
										
	if( I2CWaitClkHi() )
	{
		I2C_CLK_LOW;
		return(d);
	}
	else
		return(0);	
} // RecvI2CByte

#ifdef TESTING

uint8 ScanI2CBus(void)
{
	int16 s;
	uint8 d;

	d = 0;

	TxString("Sensor Bus\r\n");
	for ( s = 0x10 ; s <= 0xf6 ; s += 2 )
	{
		I2CStart();
		if( SendI2CByte(s) == I2C_ACK )
		{
			d++;
			TxString("\t0x");
			TxValH(s);
			TxNextLine();
		}
		I2CStop();

		Delay1mS(2);
	}

	TxString("\r\nESC Bus\r\n");

	if ( (P[ESCType] == ESCHolger)||(P[ESCType] == ESCX3D)||(P[ESCType] == ESCYGEI2C) )
		for ( s = 0x10 ; s <= 0xf6 ; s += 2 )
		{
			ESCI2CStart();
			if( SendESCI2CByte(s) == I2C_ACK )
			{
				d++;
				TxString("\t0x");
				TxValH(s);
				TxNextLine();
			}
			ESCI2CStop();
	
			Delay1mS(2);
		}
	else
		TxString("\tinactive - I2C ESCs not selected..\r\n");

	TxNextLine();

	return(d);
} // ScanI2CBus

#endif // TESTING

// -----------------------------------------------------------
// SW I2C Routines for ESCs

boolean ESCWaitClkHi(void);
void ESCI2CStart(void);
void ESCI2CStop(void);
uint8 SendESCI2CByte(uint8);
void ProgramSlaveAddress(uint8);
void ConfigureESCs(void);

// Constants

#define	 ESC_SDA		PORTBbits.RB1
#define	 ESC_SCL		PORTBbits.RB2
#define	 ESC_DIO		TRISBbits.TRISB1
#define	 ESC_CIO		TRISBbits.TRISB2

#define ESC_DATA_LOW	{ESC_SDA=0;ESC_DIO=I2C_OUT;}
#define ESC_DATA_FLOAT	{ESC_DIO=I2C_IN;}
#define ESC_CLK_LOW		{ESC_SCL=0;ESC_CIO=I2C_OUT;}
#define ESC_CLK_FLOAT	{ESC_CIO=I2C_IN;}

boolean ESCWaitClkHi(void)
{
	static uint8 s;

	ESC_CLK_FLOAT;
	s = 1;						
	while( !ESC_SCL ) 
		if( ++s == 0 ) return (false);					

	return ( true );
} // ESCWaitClkHi

void ESCI2CStart(void)
{
	static uint8 r;

	ESC_DATA_FLOAT;
	r = ESCWaitClkHi();
	ESC_DATA_LOW;
	I2C_DELAY;		
	ESC_CLK_LOW;				
} // ESCI2CStart

void ESCI2CStop(void)
{
	ESC_DATA_LOW;
	ESCWaitClkHi();
	ESC_DATA_FLOAT;

    I2C_DELAY;

} // ESCI2CStop

uint8 SendESCI2CByte(uint8 d)
{
	static uint8 s, t;

	for ( s = 8; s ; s-- )
	{
		if( d & 0x80 )
			ESC_DATA_FLOAT
		else
			ESC_DATA_LOW
	
		if( ESCWaitClkHi() )
		{ 	
			ESC_CLK_LOW;
			d <<= 1;
		}
		else
			return(I2C_NACK);	
	}

	ESC_DATA_FLOAT;
	if( ESCWaitClkHi() )
		s = ESC_SDA;
	else
		return(I2C_NACK);	
	ESC_CLK_LOW;
										
	return( s );
} // SendESCI2CByte

#ifdef TESTING

void ProgramSlaveAddress(uint8 addr)
{
	static uint8 s;

	for (s = 0x10 ; s < 0xf0 ; s += 2 )
	{
		ESCI2CStart();
		if( SendESCI2CByte(s) == I2C_ACK )
			if( s == addr )
			{	// ESC is already programmed OK
				ESCI2CStop();
				TxString("\tESC at SLA 0x");
				TxValH(addr);
				TxString(" is already programmed OK\r\n");
				return;
			}
			else
			{
				if( SendESCI2CByte(0x87) == I2C_ACK ) // select register 0x07
					if( SendESCI2CByte( addr ) == I2C_ACK ) // new slave address
					{
						ESCI2CStop();
						TxString("\tESC at SLA 0x");
						TxValH(s);
						TxString(" reprogrammed to SLA 0x");
						TxValH(addr);
						TxNextLine();
						return;
					}
			}
		ESCI2CStop();
	}
	TxString("\tESC at SLA 0x");
	TxValH(addr);
	TxString(" no response - check cabling and pullup resistors!\r\n");
} // ProgramSlaveAddress

boolean CheckESCBus(void)
{
	return ( true );
} // CheckESCBus

void ConfigureESCs(void)
{
	uint8 m, s;

	if ( P[ESCType] == ESCYGEI2C )		
	{
		TxString("\r\nProgram YGE ESCs\r\n");
		for ( m = 0 ; m < NoOfI2CESCOutputs ; m++ )
		{
			TxString("Connect ONLY ");
			switch( m )
			{
				#ifdef HEXACOPTER
					not yet!
				#else
					case 0 : TxString("Front"); break;
					case 1 : TxString("Back");  break;
					case 2 : TxString("Right"); break;
					case 3 : TxString("Left");  break;
				#endif // HEXACOPTER
			}
			TxString(" ESC, then press any key \r\n");
			while( PollRxChar() != 'x' ); // UAVPSet uses 'x' for any key button
		//	TxString("\r\n");
			ProgramSlaveAddress( 0x62 + ( m*2 ));
		}
		TxString("\r\nConnect ALL ESCs and power-cycle the Quadrocopter\r\n");
	}
	else
		TxString("\r\nYGEI2C not selected as ESC?\r\n");
} // ConfigureESCs

#endif // TESTING

