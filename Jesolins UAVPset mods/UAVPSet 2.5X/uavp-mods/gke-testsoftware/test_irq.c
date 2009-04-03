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

// Interrupt routine

#include "pu-test.h"
#include "bits.h"

#include <int16cxx.h>	//interrupt support

#pragma origin 4

// Interrupt Routine

bank0	uns16	NewK1, NewK2, NewK3, NewK4, NewK5;
uns16	NewK6@NewK1;
uns16	NewK7@NewK2;
uns8	RecFlags;
uns16 	CCPR1@0x15;
uns16	PauseTime;

interrupt irq(void)
{

	int_save_registers;	// save W and STATUS

	if( TMR2IF )	// 5 or 14 ms have elapsed without an active edge
	{
		TMR2IF = 0;	// quit int
		if( _FirstTimeout )	// 5 ms have been gone by...
		{
			PR2 = TMR2_5MS;		// set compare reg to 5ms
			goto ErrorRestart;
		}
		_FirstTimeout = 1;
		PR2 = TMR2_14MS;		// set compare reg to 14ms
		RecFlags = 0;
		TMR1ON = 0;
		TMR1L = 0;
		TMR1H = 0;
		TMR1ON = 1;
	}
	if( CCP1IF )
	{
		CCPR1.low8 = CCPR1L;
		CCPR1.high8 = CCPR1H;
		CCP1M0 ^= 1;	// toggle edge bit
		if( (RecFlags == 0) && _FirstTimeout )
		{
			PauseTime.high8 = CCPR1.high8;
			PauseTime.low8 = CCPR1.low8;
		}
		TMR2 = 0;	// re-set timer and postscaler
		PR2 = TMR2_5MS;		// set compare reg to 5ms
		TMR2IF = 0;	// quit int
		_FirstTimeout = 0;

		if( NegativePPM ^ CCP1M0  )	// a negative edge
		{
			if( RecFlags == 0 )
			{
				NewK1 = CCPR1;
			}
			else
			if( RecFlags == 2 )
			{
				NewK3 = CCPR1;
				NewK2 = CCPR1 - NewK2;
			}
			else
			if( RecFlags == 4 )
			{
				NewK5 = CCPR1;
				NewK4 = CCPR1 - NewK4;
			}
			else
			if( RecFlags == 6 )
			{
				NewK7 = CCPR1;
				NewK6 = CCPR1 - NewK6;
				CurrK6 = NewK6 >> 1;
			}
			else	// values are unsafe
				goto ErrorRestart;
		}
		else	// a positive edge
		{
			if( RecFlags == 1 )
			{
				NewK2 = CCPR1;
				NewK1 = CCPR1 - NewK1;
			}
			else
			if( RecFlags == 3 )
			{
				NewK4 = CCPR1;
				NewK3 = CCPR1 - NewK3;
			}
			else
			if( RecFlags == 5 )
			{
				NewK5 = CCPR1 - NewK5;
// all complete! New copy all the values at once
				NewK1 >>= 1;
				NewK2 >>= 1;
				NewK3 >>= 1;
				NewK4 >>= 1;
				NewK5 >>= 1;
				CurrK1 = NewK1;
				CurrK2 = NewK2;
				CurrK3 = NewK3;
				CurrK4 = NewK4;
				CurrK5 = NewK5;
				_NewValues = 1;
// sanity check
// NewKx has values in 4us units now. content must be 256..511 (1024-2047us)
				if( (NewK1.high8 == 1) &&
				    (NewK2.high8 == 1) &&
				    (NewK3.high8 == 1) &&
				    (NewK4.high8 == 1) &&
				    (NewK5.high8 == 1) )
				{
					_NoSignal = 0;
					NewK6 = CCPR1;
				}
				else	// values are unsafe
					goto ErrorRestart;
			}
			else
			if( RecFlags == 7 )
			{
				NewK7 = CCPR1 - NewK7;
				CurrK7 = NewK7 >> 1;
				RecFlags = -1;
			}
			else
			{
ErrorRestart:
				_NoSignal = 1;		// Signal lost
				RecFlags = -1;
				if( NegativePPM )
					CCP1M0 = 1;	// wait for positive edge next
				else
					CCP1M0 = 0;	// wait for negative edge next
			}	
		}
		CCP1IF = 0;		// quit int
		RecFlags++;
	}

	int_restore_registers;
}

