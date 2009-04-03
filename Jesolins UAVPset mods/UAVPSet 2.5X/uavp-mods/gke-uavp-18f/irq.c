// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
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
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


// Interrupt routine
// Major changes to irq.c including removal of redundant source by Ing. Greg Egan - 
// use at your own risk - see GPL.

// The 64uS assumed for the maximum interrupt latency in the generation of the 
// output pulse preamble in utils.c  most likely applies to the Timer0 interrupt. 
// The original version of irq.c had a much longer path through the receiving of bit5 
// exacerbated by the stick filter code. This caused the edge of the 1mS preamble to 
// be peridocally missed and for the OutSignals routine to emit preambles greater 
// than 1mS. GKE

#include "c-ufo.h"
#include "bits.h"

// Interrupt Routine

#pragma udata isrvars=0x090
int16 	NewK1, NewK2, NewK3, NewK4, NewK5, NewK6, NewK7;
int8	RCState;
int24	PrevEdge, CurrEdge;
int16 	Width;
#pragma udata

#pragma interrupt low_isr_handler
void low_isr_handler(void)
{
	return;
} // low_isr_handler

#pragma interrupt high_isr_handler
void high_isr_handler(void)
{
//	int24	CurrEdge, Width;
		
	// For 2.4GHz systems see README_DSM2_ETC.
	if( PIR1bits.TMR2IF )	// 5 or 14 ms have elapsed without an active edge
	{
		PIR1bits.TMR2IF = false;	// quit int
		#ifndef RX_PPM	// single PPM pulse train from receiver
		if( _FirstTimeout )			// 5 ms have been gone by...
		{
			PR2 = TMR2_5MS;			// set compare reg to 5ms
			goto ErrorRestart;
		}
		_FirstTimeout = true;
		PR2 = TMR2_14MS;			// set compare reg to 14ms
		#endif
		RCState = 0;
	}

	if( PIR1bits.CCP1IF )
	{
		TMR2 = 0;					// re-set timer and postscaler
		PIR1bits.TMR2IF = false;	// quit int
		_FirstTimeout = false;

		CurrEdge = CCPR1;
		if ( CurrEdge < PrevEdge )
			PrevEdge -= 0xffff;		// Deal with Timer1 wraparound
		Width = (CurrEdge - PrevEdge) >> 1;
		PrevEdge = CurrEdge;

		#ifndef RX_PPM				// single PPM pulse train from receiver
									// standard usage (PPM, 3 or 4 channels input)
		CCP1CONbits.CCP1M0 ^= 1;	// toggle edge bit
		PR2 = TMR2_5MS;				// set compare reg to 5ms

		if( NegativePPM ^ CCP1CONbits.CCP1M0  )		// a negative edge
		{
		#endif
			if( RCState == 0 )
			{
			// initial vale of PrevEdge	
			}
			else
			if( RCState == 2 )
				NewK2 = Width;
			else
			if( RCState == 4 )
				NewK4 = Width;
			else
			if( RCState == 6 )
			{
				NewK6 = Width;		
		#ifdef RX_DSM2
				if ( (NewK6>>8) !=1) 	// add glitch detection to 6 & 7
					goto ErrorRestart;
		#else
				IK6 = NewK6 & 0xff;
		#endif // RX_DSM2	
			}
		#ifdef RX_PPM
			else
		#else
			else	// values are unsafe
				goto ErrorRestart;
		}
		else	// a positive edge
		{
		#endif // RX_PPM 
			if( RCState == 1 )
				NewK1 = Width;
			else
			if( RCState == 3 )
				NewK3 = Width;
			else
			if( RCState == 5 )
			{
				NewK5 = Width;

				// sanity check - NewKx has values in 4us units now. 
				// content must be 256..511 (1024-2047us)
				if( ((NewK1 >>8 ) == 1) &&
				    ((NewK2 >>8 ) == 1) &&
				    ((NewK3 >>8 ) == 1) &&
				    ((NewK4 >>8 ) == 1) &&
				    ((NewK5 >>8 ) == 1) )
				{
					#ifndef RX_DSM2									
					if( FutabaMode ) // Ch3 set for Throttle on UAPSet
					{
						IGas = NewK3 & 0xff;
						IRoll = (NewK1 & 0xff) - (int16)_Neutral;
						IPitch = (NewK2 & 0xff) - (int16)_Neutral;
					}
					else
					{
						IGas  = NewK1  & 0xff;
						IRoll = (NewK2 & 0xff) - (int16)_Neutral;
						IPitch = (NewK3 & 0xff) - (int16)_Neutral;
					}
					IYaw = (NewK4 & 0xff) - (int16)_Neutral;					
					IK5 = NewK5 & 0xff;

					_NoSignal = false;
					_NewValues = true; // potentially IK6 & IK7 are still about to change ???
					#endif // !RX_DSM2
				}
				else	// values are unsafe
					goto ErrorRestart;
			}
			else
			if( RCState == 7 )
			{
				NewK7 = Width;	
				#ifdef RX_DSM2
				if ( (NewK7>>8) !=1)	
					goto ErrorRestart;

				if( FutabaMode ) // Ch3 set for Throttle on UAPSet
				{
			//EDIT FROM HERE ->
			// CURRENTLY Futaba 9C with Spektrum DM8 / JR 9XII with DM9 module
					IGas = NewK5 & 0xff;

					IRoll = (NewK3 & 0xff) - (int16)_Neutral; 
					IPitch = (NewK2 & 0xff) - (int16)_Neutral;
					IYaw = (NewK1 & 0xff) - (int16)_Neutral;

					IK5 = NewK6 & 0xff; // do not filter
					IK6 = NewK4 & 0xff;
					IK7 = NewK7 & 0xff;
			// TO HERE
				}
				else // Reference 2.4GHz configuration DX7 Tx and AR7000 Rx
				{
					IGas = NewK6 & 0xff;

					IRoll = (int16)(NewK1 & 0xff) - (int16)_Neutral; 
					IPitch = (int16)(NewK4 & 0xff) - (int16)_Neutral;
					IYaw = (int16)(NewK7 & 0xff) - (int16)_Neutral;

					IK5 = NewK3 & 0xff; // do not filter
					IK6 = NewK5 & 0xff;
					IK7 = NewK2 & 0xff;
				}	

				_NoSignal = false;
				_NewValues = true;
				#else				
				IK7 = NewK7 & 0xff;
				#endif // RX_DSM2 
				RCState = -1;
			}
			else
			{
ErrorRestart:
				_NewValues = false;
				_NoSignal = true;				// Signal lost
				RCGlitchCount++;
				RCState = -1;
				#ifndef RX_PPM
				if( NegativePPM )
					CCP1CONbits.CCP1M0 = true;	// wait for positive edge next
				else
					CCP1CONbits.CCP1M0 = false;	// wait for negative edge next
				#endif
			}	
		#ifndef RX_PPM
		}
		#endif
		PIR1bits.CCP1IF = false;				// quit int
		RCState++;
	}

	if( INTCONbits.TMR0IF && INTCONbits.TMR0IE )
	{
		INTCONbits.TMR0IF = false;				// quit int
		TimeSlot--;
	}
	
} // high_isr_handler
	
#pragma code high_isr = 0x08
void high_isr (void)
{
  _asm goto high_isr_handler _endasm
} // high_isr
#pragma code

#pragma code low_isr = 0x18
void low_isr (void)
{
  _asm goto low_isr_handler _endasm
} // low_isr
#pragma code

