// ==============================================
// =      U.A.V.P Brushless UFO Controller      =
// =           Professional Version             =
// = Copyright (c) 2007 Ing. Wolfgang Mahringer =
// = Ported 2008 to 18F2520 by Ing. Greg Egan   =
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

// Program Mode

#include "c-ufo.h"
#include "bits.h"

#ifdef XMIT_PROG
//
// Programming Sequence:
// ---------------------
// 1) Turn ufo's power-switch off.
// 2) power on the ufo. Red light should be on now.
// 3) Put throttle stick to maximum.
// 4) Turn ufo's power-switch on. Yellow light should be on now.
// 5) Wait for change to blue light. You are now in setup mode.
//    If red light is on:  Parameter set 2
//    If red light is off: Parameter set 1
// 6) Put throttle to middle position immediately (5 seconds)
// 7) Perform setup now:
//    Yellow LEDs flashes for setting number (1 to 11)
//    Green LED flashes for positive values, red LED for negative
//    Long flashes count 5, short flashes count 1
//
//    See register numbers in the manual!
//   
// 8) After having gone through all settings, blue light flashes quickly
// 9) If you want to save your modifications, put throttle to maximum
//    (you have 5 seconds time)
//    if you don't wish to save, put throttle to minimum.
// 10) All 4 LEDs light up for a second to acknowledge setup saving!
// 11) Put throttle to minimum to try your setup right away.
// 
// Once you have found a good setup, please write down the values!
//
// Setup values are stored in nonvolatile chip memory.
//
// Put throttle to +50% to increment setting
// Put throttle to -50% to decrement setting
// Put throttle to neutral to watch current setting
// Put throttle to minimum to exit setting

// show blink codes 
// register number flashes yellow
// positive register value flashes green, negative flashes red.
// direction is controlled by throttle stick. Throttle to 0 is exit.

void ShowMode(uns8 Count, int size1 *pvalue)
{
	int nv,v;
	uns8 i;

	v = *pvalue;

	ALL_LEDS_OFF;	// Led off
	LedBlue_ON;	// Blue on
	do
	{
		for(i=Count; i>0;)
		{
			LedBlue_OFF;		// Blue LED off
			LedYellow_ON;		// Yellow LED on
			if( i >= 5 )
			{
				Delaysec(6);
				i -= 5;
			}
			else
			{
				Delaysec(2);
				i--;
			}
			LedYellow_OFF;
			LedBlue_ON;		// Blue LED on
			Delaysec(3);
		}
		Delaysec(5);
		nv = v;
		if( v < 0 )
			nv = -v;

		for(i=nv; i > 0;)
		{
			if( v > 0 )
			{
				LedBlue_OFF;		// Blue LED off
				LedGreen_ON;	// positive value=green
			}
			if( v < 0 )
			{
				LedBlue_OFF;		// Blue LED off
				LedRed_ON;		// negative value=red
			}
			if( i >= 5 )
			{
				Delaysec(6);		// long flash for 5 units
				i -= 5;
			}
			else
			{
				Delaysec(2);		// short flash for 1 unit
				i--;
			}
			LedGreen_OFF;
			LedRed_OFF;		// Leds off
			LedBlue_ON;		// blue LED on
			Delaysec(3);
		}

		Delaysec(20);	// 2 sec wait

		if( IGas > _ProgUp )
			v++;
		if( (IGas < _ProgDown) && (IGas >= _ThresStart) )
			v--;
	}
	while( IGas >= _ThresStart );

	*pvalue = v;
}

// main program for transmitter controlled setup
void DoProgMode(void)
{
	uns8 i;
	int size1 *p;

	ALL_LEDS_OFF;
	InitArrays();
	LedBlue_ON;	// signal prog mode set #1
	if( IK5 > _Neutral )
		LedRed_ON;	// set #2

	Delaysec(50);	// time to put throttle back to neutral

	p = &FirstProgReg;
	for(i = 1; p<=&LastProgReg; i++)
	{
		ShowMode(i, p);
		p++;
	}

	ALL_LEDS_OFF;
// signal prog mode: 5 seconds blue flashing
	for(i=0; i<50; i++)
	{
		Delaysec(1);	// time 1/10 sec
		LedBlue_TOG;	// toggle LED
		if( IK5 > _Neutral )
			LedRed_TOG;	// set #2
	}

	if( IGas > _ThresStart )
	{
		ALL_LEDS_ON;	//  = white color
		Delaysec(10);
// prog values into data flash
		EEPGD = 0;		// access data eeprom
		WREN = 1;		// enable eeprom writes
		EEADR = _EESet1;		// start with address 0
		if( IK5 > _Neutral )
			EEADR = _EESet2;	// set #2
		for(p = &FirstProgReg; p <= &LastProgReg; p++)
		{
			EEDATA = *p;	// the data byte
			GIE = 0;
			EECON2 = 0x55;	// fix prog sequence (see 16F628A datasheet)
			EECON2 = 0xAA;
			WR = 1;			// start write cycle
			GIE = 1;
			while( WR == 1 );	// wait to complete
			EEADR++;
		}
		WREN = 0;		// disable eeprom writes
	}
	ALL_LEDS_OFF;
}

#endif

// read the current parameter set into the RAM variables
void ReadEEdata(void) // 245 uSec @ 16MHz
{
	int *p;
#ifdef MOCK_PPM
	TimeSlot=6;
#else
	EEADR = _EESet1;	// default 1st parameter set
	if( IK5 > _Neutral )
		EEADR = _EESet2;	// user selected 2nd parameter set

	for(p = &FirstProgReg; p <= &LastProgReg; p++)
	{
		EEPGD = 0;
		RD = 1;
		*p = EEDATA;
		EEADR++;
	}

// modified Ing. Greg Egan
#ifdef BOARD_3_0
	if( LowVoltThres < 0 )
		LowVoltThres = -LowVoltThres;
#endif
	BatteryVolts = LowVoltThres;

// Sanity check
// ??? if timing value is lower than 1, set it to 10ms!
	if( TimeSlot < 1 )
		TimeSlot = 10;
#endif
}

