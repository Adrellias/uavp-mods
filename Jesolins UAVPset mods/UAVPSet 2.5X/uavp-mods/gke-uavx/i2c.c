// =======================================================================
// =                                 UAVX                                =
// =                         Quadrocopter Control                        =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =     Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer       =
// =                          http://www.uavp.org                        =
// =======================================================================
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

// i2c subroutines

#include "UAVX.h"

void I2CStart(void);
void I2CStop(void);
uint8 I2CWaitClkHi(void);
void I2CDelay(void);
uint8 SendI2CByte(uint8);
uint8 RecvI2CByte(uint8);

// the sensor bus lines

#define	I2C_DLAT		LATBbits.LATB6
#define	I2C_SLAT		LATBbits.LATB7	
#define I2C_SDA			PORTBbits.RB6
#define I2C_DIO			TRISBbits.TRISB6
#define I2C_SCL			PORTBbits.RB7
#define	I2C_CIO			TRISBbits.TRISB7

// -----------------------------------------------------------
#ifdef MICROCHIP_SWI2C

#include <sw_i2c.h>

int8 Clock_test( void )
{
  Delay10TCYx(3);                 // user may need to adjust timeout period

  if ( !SCLK_PIN )                // if clock is still low after wait 
  {
    return ( -2 );                // return with clock error
  }
  else
  {
    return ( 0 );                 // return with no error
  } 
}

void SWStopI2C( void )
{
  SCLK_LAT = 0;                   // set clock pin latch to 0
  CLOCK_LOW;                      // set clock pin to output to drive low
  DATA_LAT = 0;                   // set data pin latch to 0
  DATA_LOW;                       // set data pin output to drive low
  Delay10TCY();                   // user may need to modify based on Fosc
  CLOCK_HI;                       // release clock pin to float high
  Delay10TCY();                   // user may need to modify based on Fosc
  DATA_HI;                        // release data pin to float high
  Delay1TCY();                    // user may need to modify based on Fosc
  Delay1TCY();
}

int8 SWAckI2C( void )
{
  SCLK_LAT = 0;                   // set clock pin latch to 0  
  CLOCK_LOW;                      // set clock pin to output to drive low
  DATA_HI;                        // release data line to float high 
  Delay10TCY();                   // user may need to modify based on Fosc
  CLOCK_HI;                       // release clock line to float high
  Delay1TCY();                    // 1 cycle delay
  Delay1TCY();                    // 1 cycle delay

  if ( DATA_PIN )                 // error if ack = 1, slave did not ack
  {
    return ( -1 );                // return with acknowledge error
  }
  else
  {
    return ( 0 );                 // return with no error
  }
}

void SWRestartI2C( void )
{
  SCLK_LAT = 0;                   // set clock pin latch to 0
  CLOCK_LOW;                      // set clock pin to output to drive low
  DATA_HI;                        // release data pin to float high
  Delay10TCY();                   // user may need to modify based on Fosc
  CLOCK_HI;                       // release clock pin to float high
  Delay10TCY();                   // user may need to modify based on Fosc
  DATA_LAT = 0;                   // set data pin latch to 0
  DATA_LOW;                       // set data pin output to drive low
  Delay10TCY();                   // user may need to modify based on Fosc
}

void SWStartI2C( void )
{
  DATA_LAT = 0;                   // set data pin latch to 0
  DATA_LOW;                       // set pin to output to drive low
  Delay10TCY();                   // user may need to modify based on Fosc
}

uint16 SWReadI2C( void )
{
  BIT_COUNTER = 8;                // set bit count for byte 
  SCLK_LAT = 0;                   // set clock pin latch to 0

  do
  {
    CLOCK_LOW;                    // set clock pin output to drive low
    DATA_HI;                      // release data line to float high
    Delay10TCY();                 // user may need to modify based on Fosc
    CLOCK_HI;                     // release clock line to float high
    Delay1TCY();                  // user may need to modify based on Fosc
    Delay1TCY();

    if ( !SCLK_PIN )              // test for clock low
    {
      if ( Clock_test( ) )        // clock wait routine
      {
        return ( -2 );            // return with error condition       
      }
    }

    I2C_BUFFER <<= 1;             // shift composed byte by 1
    I2C_BUFFER &= 0xFE;           // clear bit 0

    if ( DATA_PIN )               // is data line high
     I2C_BUFFER |= 0x01;          // set bit 0 to logic 1
   
  } while ( --BIT_COUNTER );      // stay until 8 bits have been acquired

  return ( (uint16) I2C_BUFFER ); // return with data
}

int8 SWWriteI2C( unint8 data_out )
{
  BIT_COUNTER = 8;                // initialize bit counter
  I2C_BUFFER = data_out;          // data to send out
  SCLK_LAT = 0;                   // set latch to 0
                                 
  do
  {
    if ( !SCLK_PIN )              // test if clock is low
    {                             // if it is then ..
      if ( Clock_test( ) )        // wait on clock for a short time
      {
        return ( -1 );            // return with error condition      
      }
    }

    else 
    {
     I2C_BUFFER &= 0xFF;          // generate movlb instruction
      _asm
      rlcf I2C_BUFFER,1,1         // rotate into carry and test  
      _endasm

      if ( STATUS & 0x01 )        // if carry set, transmit out logic 1
      {
       CLOCK_LOW;                 // set clock pin output to drive low
       DATA_HI;                   // release data line to float high 
       Delay10TCY();              // user may need to modify based on Fosc
       CLOCK_HI;                  // release clock line to float high 
       Delay10TCY();              // user may need to modify based on Fosc
      }
      else                        // transmit out logic 0
      {
        CLOCK_LOW;                // set clock pin output to drive low
        DATA_LAT = 0;             // set data pin latch to 0
        DATA_LOW;                 // set data pin output to drive low 
        Delay10TCY();             // user may need to modify based on Fosc
        CLOCK_HI;                 // release clock line to float high 
        Delay10TCY();             // user may need to modify based on Fosc
      }

     BIT_COUNTER --;              // reduce bit counter by 1
    }
  } while ( BIT_COUNTER );        // stay in transmit loop until byte sent 

  return ( 0 );                   // return with no error
}

#else

// Wolfgang's I2C Routines

void I2CDelay(void) 
{
	Delay10TCY();
} // I2CDelay

// put SCL to high-z and wait until line is really hi
// returns != 0 if ok
uint8 I2CWaitClkHi(void)
{
	uint8 s;

	s = 1;

	I2CDelay();
	I2C_CIO=1;	// set SCL to input, output a high
	while( !I2C_SCL )						// timeout wraparound through 255 1.25mS							
		if( ++s == 0 )			 
			break;	
	I2CDelay();
	return(s);
} // I2CWaitClkHi

// send a start condition
void I2CStart(void)
{
	uint8 r;

	I2C_DIO=1;								// set SDA to input, output a high
	r = I2CWaitClkHi();
	I2C_SDA = 0;							// start condition
	I2C_DIO = 0;							// output a low
	I2CDelay();
	I2C_SCL = 0;
	I2C_CIO = 0;							// set SCL to output, output a low
} // I2CStart

// send a stop condition
void I2CStop(void)
{
	uint8 r;

	I2C_DIO=0;								// set SDA to output
	I2C_SDA = 0;							// output a low
	r = I2CWaitClkHi();

	I2C_DIO=1;								// set SDA to input, output a high, STOP condition
	I2CDelay();								// leave clock high
} // I2CStop 

// send a byte to I2C slave and return ACK status
// 0 = ACK
// 1 = NACK
uint8 SendI2CByte(uint8 d)
{
	uint8 s;

	for(s=8; s ; s--)
	{
		if( d & 0x80 )
			I2C_DIO = 1;					// switch to input, line is high
		else
		{
			I2C_SDA = 0;			
			I2C_DIO = 0;					// switch to output, line is low
		}
	
		if(I2CWaitClkHi())
		{ 	
			I2C_SCL = 0;
			I2C_CIO = 0;					// set SCL to output, output a low
			d <<= 1;
		}
		else
			return(I2C_NACK);	
	}

	I2C_DIO = 1;							// set SDA to input
	if(I2CWaitClkHi())
		s = I2C_SDA;
	else
		return(I2C_NACK);	

	I2C_SCL = 0;
	I2C_CIO = 0;							// set SCL to output, output a low

	return(s);
} // SendI2CByte


// read a byte from I2C slave and set ACK status
// 0 = ACK
// 1 = NACK
// returns read byte
uint8 RecvI2CByte(uint8 r)
{
	uint8 s, d;

	d = 0;
	I2C_DIO = 1;							// set SDA to input, output a high

	for(s=8; s ; s--)
		if( I2CWaitClkHi() )
		{ 
			d <<= 1;
			if( I2C_SDA )
				d |= 1;
			I2C_SCL = 0;
			I2C_CIO = 0;
 		}
		else
			return(0);

	I2C_SDA = r;
	I2C_DIO = 0;
											// set SDA to output
	if( I2CWaitClkHi() )
	{
		I2C_SCL = 0;
		I2C_CIO = 0;						// set SCL to output, output a low
	//	I2CDelay();
	//	I2C_IO = 0;	// set SDA to output
		return(d);
	}
	else
		return(0);	
} // RecvI2CByte

#endif // MICROCHIP_SWI2C

// -----------------------------------------------------------
// Wolfgang's SW I2C Routines for ESCs

#if (defined ESC_X3D || defined ESC_HOLGER || defined ESC_YGEI2C) && !defined DEBUG_SENSORS

void EscI2CDelay(void)
{
	Delay1TCY();
	Delay1TCY();
	Delay1TCY();
} // EscI2CDelay

void EscWaitClkHi(void)
{
	uint8 s;

	s = 1;

	EscI2CDelay();
	ESC_CIO=1;								// set SCL to input, output a high
	while( !ESC_SCL ) 
		if( ++s == 0 )			 
			break;;							// wait for line to come hi with timeout
	EscI2CDelay();
} // EscWaitClkHi

// send a start condition
void EscI2CStart(void)
{
	ESC_DIO=1;								// set SDA to input, output a high
	EscWaitClkHi();
	ESC_SDA = 0;							// start condition
	ESC_DIO = 0;							// output a low
	EscI2CDelay();
	ESC_SCL = 0;
	ESC_CIO = 0;							// set SCL to output, output a low
} // EscI2CStart

// send a stop condition
void EscI2CStop(void)
{
	ESC_DIO=0;								// set SDA to output
	ESC_SDA = 0;							// output a low
	EscWaitClkHi();

	ESC_DIO=1;								// set SDA to input, output a high, STOP condition
	EscI2CDelay();							// leave clock high
} // EscI2CStop


// send a byte to I2C slave and return ACK status
// 0 = ACK
// 1 = NACK
void SendEscI2CByte(uint8 d)
{
	uint8 s;

	for(s=8; s; s--)
	{
		if( d & 0x80 )
			ESC_DIO = 1;					// switch to input, line is high
		else
		{
			ESC_SDA = 0;			
			ESC_DIO = 0;					// switch to output, line is low
		}
	
		ESC_CIO=1;							// set SCL to input, output a high
		while( !ESC_SCL ) ;					// wait for line to come hi ??? hang
		EscI2CDelay();
		ESC_SCL = 0;
		ESC_CIO = 0;						// set SCL to output, output a low
		d <<= 1;
	}
	ESC_DIO = 1;							// set SDA to input
	EscI2CDelay();
	ESC_CIO=1;								// set SCL to input, output a high
	while( !ESC_SCL ) ;						// wait for line to come hi ??? hang

	EscI2CDelay();							// here is the ACK	

	ESC_SCL = 0;
	ESC_CIO = 0;							// set SCL to output, output a low
	EscI2CDelay();

} // SendEscI2CByte

#endif	// ESC_X3D || ESC_HOLGER || ESC_YGEI2C


