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
// =  please visit http://www.uavp.org          =
// =               http://www.mahringer.co.at   =
// ==============================================

// Utilities and subroutines

#pragma codepage=1
#include "c-ufo.h"
#include "bits.h"

// Math Library
#include "mymath16.h"

static bank1 int i;

// wait blocking for "dur" * 0.1 seconds
// Motor and servo pulses are still output every 10ms
void Delaysec(uns8 dur)
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
			OutSignals(); // 1-2 ms Duration
			// break loop if a serial command is in FIFO
#ifdef BOARD_3_1
			if( RCIF )
#endif
#ifdef BOARD_3_0
			if( _SerEnabled && RCIF )
#endif
				return;
		}
	}
}


#ifdef DEBUGOUT
// output a value on connector K5
// for observation via an oscilloscope
// with 8 narrow (bit=0) or broad (bit=1) pulses
// MSB first
void Out(uns8 l)
{

	for(i=0; i<8; i++)
	{
		PORTB.5=1;
		if(l & 0x80)
		{
			nop2();
			nop2();
			nop2();
		}
		PORTB.5=0;
		if(!(l & 0x80))
		{
			nop2();
			nop2();
			nop2();
		}
		l<<=1;
	}
}
#endif

#ifdef DEBUGOUTG
// output a signed value in a graphic manner on connector K5
// use an oscilloscope to observe
//     Trigger    ______________
// ____|_________|_____|________|_____|
//      128  negative  0  positive  127
void OutG(uns8 l)
{
	uns8 nii @i;

	PORTB.5=1;
	PORTB.5=0;
	for(nii=128; nii!=1; nii++)	// -128 .. 0
	{
		if(nii==l)
			PORTB.5=1;
	}
	PORTB.5^=1;
	for(nii=1; nii!=128; nii++)	// +1 .. +127
	{
		if(nii==l)
			PORTB.5=0;
	}

	PORTB.5=0;
}
#endif

// resets all important variables
// Do NOT call that while in flight!
void InitArrays(void)
{
	W = _Minimum;
	MVorne = W;	// stop all motors
	MLinks = W;
	MRechts = W;
	MHinten = W;

	W = _Neutral;
	MCamNick = W;
	MCamRoll = W;

// bank 1
	_Flying = 0;
	REp = 0;
	NEp = 0;
	TEp = 0;
//	LRSumPosi = 0;
//	FBSumPosi = 0;
	
	Rp = 0;
	Np = 0;
	Vud = 0;
#ifdef BOARD_3_1
	VBaroComp = 0;
	BaroCompSum = 0;
#endif
	
// bank 2
	LRIntKorr = 0;
	FBIntKorr = 0;
	YawSum = 0;
    RollSum = 0;
    NickSum = 0;
//	LRSum = 0;
//	FBSum = 0;
//	UDSum = 0;
}

// used for A/D conversion to wait for
// acquisition sample time and A/D conversion completion
void AcqTime(void)
{
// W was 10 originally
	for(W=30; W!=0; W--)	// makes about 100us
		;
	GO = 1;
	while( GO ) ;	// wait to complete
}

// this routine is called ONLY ONCE while booting
// read 16 time all 3 axis of linear sensor.
// Puts values in Neutralxxx registers.
void GetEvenValues(void)
{	// get the even values

	Delaysec(2);	// wait 1/10 sec until LISL is ready to talk
// already done in caller program
	Rp = 0;
	Np = 0;
	Tp = 0;
	for( i=0; i < 16; i++)
	{
		// wait for new set of data
		while( (ReadLISL(LISL_STATUS + LISL_READ) & 0x08) == 0 );
		
		Rl.low8  = ReadLISL(LISL_OUTX_L + LISL_INCR_ADDR + LISL_READ);
		Rl.high8 = ReadLISLNext();
		Tl.low8  = ReadLISLNext();
		Tl.high8 = ReadLISLNext();
		Nl.low8  = ReadLISLNext();
		Nl.high8 = ReadLISLNext();
		LISL_CS = 1;	// end transmission
		
		Rp += (long)Rl;
		Np += (long)Nl;
		Tp += (long)Tl;
	}
	Rp += 8;
	Np += 8;
	Tp += 8;
	Rp >>= 4;
	Np >>= 4;
	Tp >>= 4;
	NeutralLR = Rp.low8;
	NeutralFB = Np.low8;
	NeutralUD = Tp.low8;
}


// read accu voltage using 8 bit A/D conversion
// Bit _LowBatt is set if voltage is below threshold
// Modified by Ing. Greg Egan
// Filter battery volts to remove ADC/Motor spikes and set _LoBatt alarm accordingly 
void GetVbattValue(void)
{
	int NewBatteryVolts, Temp;

	ADFM = 0;	// select 8 bit mode
	ADCON0 = 0b.10.000.0.0.1;	// turn AD on, select CH0(RA0) Ubatt
	AcqTime();
	NewBatteryVolts = (int) (ADRESH >> 1);
#ifndef DEBUG_SENSORS

// cc5x limitation	BatteryVolts = (BatteryVolts+NewBatteryVolts+2)>>2;
// cc5x limitation	_LowBatt =  (BatteryVolts < LowVoltThres) & 1;

	Temp = BatteryVolts+NewBatteryVolts+2;
	BatteryVolts = Temp/2;

	if (BatteryVolts < LowVoltThres)
		_LowBatt = 1;
	else
		_LowBatt = 0;
#endif // DEBUG_SENSORS
}

#ifdef BOARD_3_1
//
// The LED routines, only needed for 
// PCB revision 3.1 (registered power driver TPIC6B595N)
//
void SendLeds(void)
{
	uns8	nij@i;

	/* send LedShadow byte to TPIC */

	nij = LedShadow;
	LISL_CS = 1;	// CS to 1
	LISL_IO = 0;	// SDA is output
	LISL_SCL = 0;	// because shift is on positive edge
	
	for(W=8; W!=0; W--)
	{
		if( nij & 0x80 )
			LISL_SDA = 1;
		else
			LISL_SDA = 0;
		nij<<=1;
		LISL_SCL = 1;
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

#endif /* BOARD_3_1 */
