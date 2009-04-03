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

#include "UAVX.h"

// Prototypes
void InitTimersAndInterrupts(void);
//uint24 mSClock(void);
void MapRC(void);
void low_isr_handler(void);
void high_isr_handler(void);

// Defines

#define MIN_PPM_SYNC_PAUSE 2500  	/* 2500 *2us = 5ms */

#define OUT_CHANNELS 6				/* hardware LIMIT  do not change */
#define PPM_CHANNELS 9

// Variables

// Timers
#pragma udata clocks
uint24 	mS[CompassUpdate+1];
#pragma udata

// Received PPM
uint8 PPM[PPM_CHANNELS];
uint8 PPM_Index;					// Index into PPM[]
uint8 volatile RC_Channels;			// The number of bits in the RC frame 
int24 volatile RCGlitches;			// The number of under/over sizes pulses received.
int24 PrevEdge;						// The time at which the previous RC pulse edge occured.
uint8 RCFrameOK;	

// Remapped PPM
int16	RC[CONTROLS];				// Remapped PPM[] depending on actual receiver channel ordering

// Serial Input
uint8 RxCheckSum, RxHead, RxTail;
uint8 RxBuff[RXBUFFMASK+1];

void InitTimersAndInterrupts()
{
	uint8 c;

	OpenTimer0(TIMER_INT_OFF&T0_8BIT&T0_SOURCE_INT&T0_PS_1_16);
	OpenTimer1(T1_8BIT_RW&TIMER_INT_OFF&T1_PS_1_8&T1_SYNC_EXT_ON&T1_SOURCE_CCP&T1_SOURCE_INT);
	OpenCapture1(CAPTURE_INT_ON & C1_EVERY_FALL_EDGE); 	// capture mode every falling edge		
	CCP1CONbits.CCP1M0 = NegativePPM;

	for (c = 0; c < PPM_CHANNELS; c++) PPM[c]  = 0;

	for (c = 0; c < CONTROLS; c++) RC[c] = 0;
	RC[RollC] = RC[PitchC] = RC[YawC] = RC_NEUTRAL;

	RC_Channels = 0;
	RCGlitches = 0;

	PPM_Index =0;
	PrevEdge = 0;

	_Signal = _NewValues = false;

	
	RxCheckSum = RxHead = RxTail = 0;
   	PIE1bits.RCIE = false;

	INTCONbits.TMR0IE = true;
	INTCONbits.PEIE = true;

	for (c = Clock; c<=CompassUpdate; c++ )
		mS[c] = 0;

	EnableInterrupts;	

} // InitTimersAndInterrupts

void MapRC(void)
{  // re-maps captured PPM to Rx channel sequence
	uint8 c;

	for (c = 0 ; c < CONTROLS ; c++)
		RC[c] = RCFilter(RC[c], PPM[Map[FutabaMode][c]]);

} // MapRC

/*
uint24 mSClock(void)
{ // make clock access atomic - assumes interrupts are always ON
	uint24 c;

	//DisableInterrupts;
	c = mS[Clock];
	//EnableInterrupts;
	return(c);
}  // mSClock
*/

#pragma interrupt low_isr_handler
void low_isr_handler(void)
{
	return;
} // low_isr_handler

#pragma interrupt high_isr_handler
void high_isr_handler(void)
{	
	uint16 	Width, CurrCCPR1;
	uint8 ch;
		
	if( INTCONbits.T0IF )
	{ 
		DoPWMFrame();	
#ifdef DUMMY_GPS
PollGPS();
#endif
		mS[Clock]++;
		if ( (mS[Clock] > mS[RCSignalTimeout]) && _Signal )
		{
			RC_Channels = 0;
			_Signal = false;
		}	
		INTCONbits.T0IF = 0;
	}
		 
	if( PIR1bits.CCP1IF && PIE1bits.CCP1IE ) // A captured RC edge has been detected
	{
		CurrCCPR1 = CCPR1;
		if ( CurrCCPR1 < PrevEdge )
			PrevEdge -= 0xffff;	// Deal with wraparound

		Width = CurrCCPR1 - PrevEdge;
		PrevEdge = CurrCCPR1;		

		if ( Width > MIN_PPM_SYNC_PAUSE ) 		// A pause in 2us ticks > 5ms 
		{
			RC_Channels = PPM_Index;
			PPM_Index = 0;						// Sync pulse detected - next CH is CH1
			RCFrameOK = true; 
		}
		else // An actual channel -- Record the variable part of the PWM time 
			if (PPM_Index < PPM_CHANNELS)
			{	
				Width >>= 1; 					// Width in 4us ticks.				

				if ( Upper8(Width) == 1 ) 		// Check pulse is 1.024 to 2.048mS
				{
					Width = Lower8(Width); 		// Limit to 0 to 1.024mS  
					if (Width > RC_MAXIMUM)
						Width = RC_MAXIMUM;  	// limit maximum for symmetry around RC_NEUTRAL 0.5mS
					PPM[PPM_Index] = Width;	
				}
				else
				{
					// preserve old value i.e. default hold
					RCGlitches++;
					RCFrameOK = false;
				}
				PPM_Index++;
				// must demand rock solid RC frames for autonomous functions not
				// to be cancelled by noise-generated partially correct frames
				if ((PPM_Index == RC_MIN_CHANNELS) && RCFrameOK )
				{
#ifndef DUMMY_GPS
					mS[RCSignalTimeout] = mS[Clock] + RC_SIGNAL_TIMEOUT;
					_Signal = true;	  
					_NewValues = true;
#endif
				}	
			}

		#ifndef RX_PPM							
		CCP1CONbits.CCP1M0 ^= 1;				// For Composite PPM signal not using wired OR
		#endif // RX_PPM
		PIR1bits.CCP1IF = false;				// Clear this interrupt flag for next time around 
	}	

	if (PIR1bits.RCIF && PIE1bits.RCIE)	// GPS and commands
	{
		if (RCSTAbits.OERR)
		{
			ch = RCREG; // flush
			RCSTAbits.CREN = false;
			RCSTAbits.CREN = true;
		}
		else
		{
			RxTail = (RxTail+1) & RXBUFFMASK;	// no check for overflow yet
			RxBuff[RxTail] = RCREG;
		}
		PIR1bits.RCIF = false;
		
		#ifdef USE_GPS		
		if ( Switch )
			PollGPS();
		#endif // USE_GPS
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


