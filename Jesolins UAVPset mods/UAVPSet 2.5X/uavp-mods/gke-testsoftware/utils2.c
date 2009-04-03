// ==============================================
// =      U.A.V.P Brushless UFO Controller      =
// =           Professional Version             =
// = Copyright (c) 2007 Ing. Wolfgang Mahringer =
// ==============================================
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ==============================================
// =  please visit http://www.uavp.de           =
// =               http://www.mahringer.co.at   =
// ==============================================

// Utilities and subroutines

#pragma codepage=1
#include "pu-test.h"
#include "bits.h"


static bank1 int i;

// wait blocking for "dur" * 0.1 seconds
// Motor and servo pulses are still output every 10ms
void Delay100mS(uns8 dur)
{
	bank0	uns8 k;

	// a TMR0 Timeout is 0,25us * 256 * 16 (presc) = 1024 us
	TMR0 = 0;

	for(k = 0; k < 10; k++)
	{
		for(i = 0; i < dur; i++)
		{
// wait ca. 10ms (10*1024us (see _Prescale0)) before outputting
			for( W = 10; W != 0; W-- )
			{
				while( T0IF == 0 );
				T0IF = 0;
			}
			// break loop if a serial command is in FIFO
			if( _SerEnabled && RCIF )
				return;
		}
	}
}



// used for A/D conversion to wait for
// acquisition sample time
void AcqTime(void)
{

	for(W=0; W<10; W++)	// makes about 100us
		;
	GO = 1;
	while( GO ) ;	// wait to complete
}

//
// The LED routines, only needed for 
// PCB revision 3.1 (registered power driver TPIC6B595N)
//
void SendLeds(void)
{
//	uns8	nij;

	/* send LedShadow byte to TPIC */

	LISL_CS = 1;	// CS to 1
	LISL_IO = 0;	// SDA is output
	LISL_SCL = 0;	// because latch on positive edge
	
	i = LedShadow;
	for(W=8; W!=0; W--)
	{
		if( i & 0x80 )
			LISL_SDA = 1;
		else
			LISL_SDA = 0;
		LISL_SCL = 1;
		i<<=1;
		LISL_SCL = 0;
	}

	PORTC.1 = 1;
	PORTC.1 = 0;	// latch into drivers
}

void SwitchLedsOn(uns8 W)
{
	LedShadow |= W;
	SendLeds();
}

void SwitchLedsOff(uns8 W)
{
	LedShadow &= ~W;
	SendLeds();
}

// read the current parameter set into the RAM variables
void ReadEEdata(void) 
{
	int *p;
	EEADR = _EESet1;	// default 1st parameter set
	if( CurrK5 > _Neutral )
		EEADR = _EESet2;	// user selected 2nd parameter set

	for(p = &FirstProgReg; p <= &LastProgReg; p++)
	{
		EEPGD = 0;
		RD = 1;
		*p = EEDATA;
		EEADR++;
	}
}

