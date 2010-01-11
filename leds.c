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

#include "uavx.h"

// Prototypes

void SendLEDs(void);
void LEDsOn(uint8);
void LEDsOff(uint8);
void LEDGame(void);

void SendLEDs(void)
{
	static int8	i, s;

	i = LEDShadow;
	SPI_CS = DSEL_LISL;	
	SPI_IO = WR_SPI;	// SDA is output
	SPI_SCL = 0;		// because shift is on positive edge
	
	for(s = 8; s ; s--)
	{
		if( i & 0x80 )
			SPI_SDA = 1;
		else
			SPI_SDA = 0;
		i<<=1;
		Delay10TCY();
		SPI_SCL = 1;
		Delay10TCY();
		SPI_SCL = 0;
	}

	PORTCbits.RC1 = 1;
	PORTCbits.RC1 = 0;	// latch into drivers
	SPI_SCL = 1;		// rest state for LISL
	SPI_IO = RD_SPI;
} // SendLEDs

void LEDsOn(uint8 l)
{
	LEDShadow |= l;
	SendLEDs();
} // LEDsOn

void LEDsOff(uint8 l)
{
	LEDShadow &= ~l;
	SendLEDs();
} // LEDsOff

void LEDGame(void)
{
	// ? needs rewriting
	if( --LEDCycles == 0 )
	{
		LEDCycles = (( 255 - DesiredThrottle ) >> 3) + 5;	// new setup
		if( F.Hovering )
			AUX_LEDS_ON;	// baro locked, all Aux-LEDs on
		else
			if( LEDShadow & AUX1M )
			{
				AUX_LEDS_OFF;
				LEDAUX2_ON;
			}
			else
				if( LEDShadow & AUX2M )
				{
					AUX_LEDS_OFF;
					LEDAUX3_ON;
				}
				else
				{
					AUX_LEDS_OFF;
					LEDAUX1_ON;
				}
	}
} // LEDGame
