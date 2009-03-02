// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
// =             Copyright (c) 2007 Ing. Wolfgang Mahringer              =
// =           Extensively modified 2008-9 by Prof. Greg Egan            =
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

// Utilities and subroutines

#include "c-ufo.h"
#include "bits.h"

static int8 i;

// wait blocking for "dur" * 0.1 seconds
// Motor and servo pulses are still output every 10ms
void Delay100mSWithOutput(uns8 dur)
{ // max 255x10mS
	uns8 i, k, j;

	// a TMR0 Timeout is 0,25us * 256 * 16 (presc) = 1024 us
	WriteTimer0(0);

	for (k = 0; k < 10; k++)
	{
		for(i = 0; i < dur; i++)
		{
			// wait ca. 10ms (10*1024us (see _Prescale0)) before outputting
			for( j = 10; j != 0; j-- )
			{
				while( INTCONbits.T0IF == 0 );
				INTCONbits.T0IF = 0;
			}
			OutSignals(); // 1-2 ms Duration
			// break loop if a serial command is in FIFO
			if( PIR1bits.RCIF )
				return;
		}
	}
} // Delay100mSWithOutput

void nop2()
{
	Delay1TCY();
	Delay1TCY();
}

int16 SRS16(int16 x, uns8 s)
{
	return((x<0) ? -((-x)>>s) : (x>>s));
} // SRS16

#ifdef DEBUGOUT
// output a value on connector K5
// for observation via an oscilloscope
// with 8 narrow (bit=0) or broad (bit=1) pulses
// MSB first
void Out(uns8 l)
{
	for(i=0; i<8; i++)
	{
		PORTBbits.RB5=1;
		if(l & 0x80)
		{
			nop2();
			nop2();
			nop2();
		}
		PORTBbits.RB5=0;
		if(!(l & 0x80))
		{
			nop2();
			nop2();
			nop2();
		}
		l<<=1;
	}
} // Out
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
} // OutG
#endif

// resets all important variables
// Do NOT call that while in flight!
void InitArrays(void)
{
	MFront = _Minimum;	// stop all motors
	MLeft = _Minimum;
	MRight = _Minimum;
	MBack = _Minimum;

	MCamPitch = _Neutral;
	MCamRoll = _Neutral;

	_Flying = 0;
	REp = PEp = YEp = 0;
	
	Rp = Pp = Vud = VBaroComp = 0;
	
	UDSum = 0;
	LRIntKorr = 0;
	FBIntKorr = 0;
	YawSum = RollSum = PitchSum = 0;

	BaroRestarts = 0;
} // InitArrays

// used for A/D conversion to wait for
// acquisition sample time and A/D conversion completion
void AcqTime(void)
{
	uns8 i;

	for(i=30; i!=0; i--)	// makes about 100us
		;
	ADCON0bits.GO = 1;
	while( ADCON0bits.GO ) ;	// wait to complete
} // AcqTime

// this routine is called ONLY ONCE while booting
// read 16 time all 3 axis of linear sensor.
// Puts values in Neutralxxx registers.
void GetEvenValues(void)
{	// get the even values

	Delay100mSWithOutput(2);	// wait 1/10 sec until LISL is ready to talk
	// already done in caller program
	Rp = 0;
	Pp = 0;
	Yp = 0;
	for( i=0; i < 16; i++)
	{
		// wait for new set of data
		while( (ReadLISL(LISL_STATUS + LISL_READ) & 0x08) == 0 );
		
		Rl  = ReadLISL(LISL_OUTX_L + LISL_INCR_ADDR + LISL_READ);
		Rl |= ReadLISLNext() << 8;
		Yl  = ReadLISLNext();
		Yl |= ReadLISLNext() << 8;
		Pl  = ReadLISLNext();
		Pl |= ReadLISLNext() << 8;
		LISL_CS = 1;	// end transmission
		
		Rp += (int16)Rl;
		Pp += (int16)Pl;
		Yp += (int16)Yl;
	}
	Rp = SRS16(Rp + 8, 4);
	Pp = SRS16(Pp + 8, 4);
	Rp = SRS16(Yp + 8, 4);

	NeutralLR = Limit(Rp, -128, 127);
	NeutralFB = Limit(Pp, -128, 127);
	NeutralUD = Limit(Yp, -128, 127);
} // GetEvenValues

// read accu voltage using 8 bit A/D conversion
// Bit _LowBatt is set if voltage is below threshold
// Modified by Ing. Greg Egan
// Filter battery volts to remove ADC/Motor spikes and set _LoBatt alarm accordingly 
void CheckBattery(void)
{
	int8 NewBatteryVolts, Temp;

	ADCON2bits.ADFM = 0;	// select 8 bit mode
	ADCON0 = 0b10000001;	// turn AD on, select CH0(RA0) Ubatt
	AcqTime();
	NewBatteryVolts = (int8) (ADRESH >> 1);

	#ifndef DEBUG_SENSORS
	// cc5x limitation	BatteryVolts = (BatteryVolts+NewBatteryVolts+1)>>1;
	// 					_LowBatt =  (BatteryVolts < LowVoltThres) & 1;

	Temp = BatteryVolts+NewBatteryVolts+1;
	BatteryVolts = Temp>>1;

	if (BatteryVolts < LowVoltThres)
		_LowBatt = 1;
	else
		_LowBatt = 0;

	#endif // DEBUG_SENSORS
} // CheckBattery

void CheckAlarms(void)
{
	if( _LowBatt )
	{
		if( BlinkCount < BLINK_LIMIT/2 )
		{
			Beeper_ON;
			LedRed_ON;
		}
		else
		{
			Beeper_OFF;
			LedRed_OFF;
		}	
	}
	else
	if ( _LostModel )
		if( (BlinkCount < (BLINK_LIMIT/2)) && ( BlinkCycle < (BLINK_CYCLES/4 )) )
		{
			Beeper_ON;
			LedRed_ON;
		}
		else
		{
			Beeper_OFF;
			LedRed_OFF;
		}	
	else
	if ( _BaroRestart )
		if( (BlinkCount < (BLINK_LIMIT/2)) && ( BlinkCycle == 0 ) )
		{
			Beeper_ON;
			LedRed_ON;
		}
		else
		{
			Beeper_OFF;
			LedRed_OFF;
		}	
	else
	{
		Beeper_OFF;				
		LedRed_OFF;
	}

} // CheckAlarms

void UpdateBlinkCount(void)
{
	if( BlinkCount == 0 )
	{
		BlinkCount = BLINK_LIMIT;
		if ( BlinkCycle == 0)
			BlinkCycle = BLINK_CYCLES;
		BlinkCycle--;
	}
	BlinkCount--;
} // UpdateBlinkCount

void SendLeds(void)
{
	uns8	i, s;

	/* send LedShadow byte to TPIC */

	i = LedShadow;
	LISL_CS = 1;	// CS to 1
	LISL_IO = 0;	// SDA is output
	LISL_SCL = 0;	// because shift is on positive edge
	
	for(s=8; s!=0; s--)
	{
		if( i & 0x80 )
			LISL_SDA = 1;
		else
			LISL_SDA = 0;
		i<<=1;
		LISL_SCL = 1;
		LISL_SCL = 0;
	}

	PORTCbits.RC1 = 1;
	PORTCbits.RC1 = 0;	// latch into drivers
}

void SwitchLedsOn(uns8 l)
{
	LedShadow |= l;
	SendLeds();
} // SwitchLedsOn

void SwitchLedsOff(uns8 l)
{
	LedShadow &= ~l;
	SendLeds();
} // SwitchLedsOff


