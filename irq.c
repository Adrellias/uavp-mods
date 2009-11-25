// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008, 2009 by Prof. Greg Egan           =
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

// Interrupt Routine

#define MIN_PPM_SYNC_PAUSE 2500  	// 2500 *2us = 5ms
// no less than 1500

// Simple averaging of last two channel captures
//#define RC_FILTER

// Prototypes

void ReceivingGPSOnly(uint8);
void DoRxPolarity(void);
void InitTimersAndInterrupts(void);
void MapRC(void);
void low_isr_handler(void);
void high_isr_handler(void);

void ReceivingGPSOnly(boolean r)
{
	#ifndef DEBUG_SENSORS
	if ( r != F[ReceivingGPS] )
	{
		PIE1bits.RCIE = false;
		F[ReceivingGPS] = r;
		if ( F[ReceivingGPS] )
			OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
				USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_HIGH, _B9600);
		else
			OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
				USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_HIGH, _B38400);

   		PIE1bits.RCIE = r;
	}
	#endif // DEBUG_SENSORS
} // ReceivingGPSOnly

void MapRC(void)
{  // re-maps captured PPM to Rx channel sequence
	static uint8 c;
	static int16 LastThrottle, Temp; 

	LastThrottle = RC[ThrottleC];

	for (c = 0 ; c < RC_CONTROLS ; c++)
	{
		Temp = PPM[Map[P[TxRxType]][c]-1].low8;
		RC[c] = RxFilter(RC[c], Temp);
	}

	if ( THROTTLE_SLEW_LIMIT > 0 )
		RC[ThrottleC] = SlewLimit(LastThrottle, RC[ThrottleC], THROTTLE_SLEW_LIMIT);

} // MapRC


void DoRxPolarity(void)
{
	if ( P[ConfigBits] & RxSerialPPMMask  ) // serial PPM frame from within an Rx
		CCP1CONbits.CCP1M0 = PPMPosPolarity[TxRxType];
	else
		CCP1CONbits.CCP1M0 = 1;	
}  // DoRxPolarity

void InitTimersAndInterrupts(void)
{
	static uint8 i;

	OpenTimer0(TIMER_INT_OFF&T0_8BIT&T0_SOURCE_INT&T0_PS_1_16);
	OpenTimer1(T1_8BIT_RW&TIMER_INT_OFF&T1_PS_1_8&T1_SYNC_EXT_ON&T1_SOURCE_CCP&T1_SOURCE_INT);

	OpenCapture1(CAPTURE_INT_ON & C1_EVERY_FALL_EDGE); 	// capture mode every falling edge
	DoRxPolarity();

	for (i = Clock; i<= CompassUpdate; i++)
		mS[i] = 0;

	for (i = 0; i < RC_CONTROLS; i++)
		PPM[i].i16 = RC[i] = 0;
	
	RC[RollC] = RC[PitchC] = RC[YawC] = RC_NEUTRAL;

	PPM_Index = PrevEdge = RCGlitches = RxCheckSum =  0;
	SignalCount = -RC_GOOD_BUCKET_MAX;
	F[Signal] = F[NewValues] = false;
   	ReceivingGPSOnly(false);
} // InitTimersAndInterrupts


#pragma interrupt low_isr_handler
void low_isr_handler(void)
{
	return;
} // low_isr_handler

#pragma interrupt high_isr_handler
void high_isr_handler(void)
{
	if( PIR1bits.CCP1IF ) 						// An Rx PPM pulse edge has been detected
	{
		CurrEdge = CCPR1;
		if ( CurrEdge < PrevEdge )
			PrevEdge -= (int24)0x00ffff;		// Deal with wraparound

		Width.i16 = (int16)(CurrEdge - PrevEdge);
		PrevEdge = CurrEdge;		

		if ( Width.i16 > MIN_PPM_SYNC_PAUSE ) 	// A pause in 2us ticks > 5ms 
		{
			PPM_Index = 0;						// Sync pulse detected - next CH is CH1
			F[RCFrameOK] = true;
			F[NewValues] = false; 
			PauseTime = Width.i16;
		}
		else 
			if (PPM_Index < RC_CONTROLS)
			{	
				Width.i16 >>= 1; 				// Width in 4us ticks.
	
				if ( Width.high8 == 1 ) 		// Check pulse is 1.024 to 2.048mS
					#ifdef RC_FILTER
					PPM[PPM_Index].i16 = (OldPPM + Width.i16 ) >> 1;
					#else
					PPM[PPM_Index].i16 = Width.i16;
					#endif // RC_FILTER		
				else
				{
					// preserve old value i.e. default hold
					RCGlitches++;
					F[RCFrameOK] = false;
				}
				PPM_Index++;
				// MUST demand rock solid RC frames for autonomous functions not
				// to be cancelled by noise-generated partially correct frames
				if ( PPM_Index == RC_CONTROLS )
				{
					if ( F[RCFrameOK] )
					{
						F[NewValues] = true;
 						SignalCount++;
					}
					else
					{
						F[NewValues] = false;
						SignalCount -= RC_GOOD_RATIO;
					}

					SignalCount = Limit(SignalCount, -RC_GOOD_BUCKET_MAX, RC_GOOD_BUCKET_MAX);
					F[Signal] = SignalCount > 0;

					if ( F[Signal])
						mS[LastValidRx] = mS[Clock];
					mS[RCSignalTimeout] = mS[Clock] + RC_SIGNAL_TIMEOUT_MS;
				}
			}

		if ( (P[ConfigBits] & RxSerialPPMMask) == 0 )						
			CCP1CONbits.CCP1M0 ^= 1;

		PIR1bits.CCP1IF = false;
	}	

	if ( PIR1bits.RCIF & PIE1bits.RCIE )			// RCIE enabled for GPS
	{
		if ( RCSTAbits.OERR | RCSTAbits.FERR )
		{
			gps_ch = RCREG; // flush
			RCSTAbits.CREN = false;
			RCSTAbits.CREN = true;
		}
		else
		{ // PollGPS in-lined to avoid EXPENSIVE context save and restore within irq
			gps_ch = RCREG;
			switch ( GPSRxState ) {
			case WaitGPSCheckSum:
				if (GPSCheckSumChar < 2)
				{
					GPSTxCheckSum *= 16;
					if ( gps_ch >= 'A' )
						GPSTxCheckSum += ( gps_ch - ('A' - 10) );
					else
						GPSTxCheckSum += ( gps_ch - '0' );
		
					GPSCheckSumChar++;
				}
				else
				{
					NMEA.length = ll;	
					F[GPSSentenceReceived] = GPSTxCheckSum == RxCheckSum;
					GPSRxState = WaitGPSSentinel;
				}
				break;
			case WaitGPSBody: 
				if ( gps_ch == '*' )      
				{
					GPSCheckSumChar = GPSTxCheckSum = 0;
					GPSRxState = WaitGPSCheckSum;
				}
				else         
					if ( gps_ch == '$' ) // abort partial Sentence 
					{
						ll = tt = RxCheckSum = 0;
						GPSRxState = WaitNMEATag;
					}
					else
					{
						RxCheckSum ^= gps_ch;
						NMEA.s[ll++] = gps_ch; 
						if ( ll > ( GPSRXBUFFLENGTH-1 ) )
							GPSRxState = WaitGPSSentinel;
					}
							
				break;
			case WaitNMEATag:
				RxCheckSum ^= gps_ch;
				if ( gps_ch == NMEATag[tt] ) 
					if ( tt == MAXTAGINDEX )
						GPSRxState = WaitGPSBody;
			        else
						tt++;
				else
			        GPSRxState = WaitGPSSentinel;
				break;
			case WaitGPSSentinel: // highest priority skipping unused sentence types
				if ( gps_ch == '$' )
				{
					ll = tt = RxCheckSum = 0;
					GPSRxState = WaitNMEATag;
				}
				break;	
		    } 
		}
		PIR1bits.RCIF = false;
	}

	if ( INTCONbits.T0IF )  // MilliSec clock with some "leaks" in output.c etc.
	{ 
		mS[Clock]++;
		if ( F[Signal] && (mS[Clock] > mS[RCSignalTimeout]) ) 
		{
			F[Signal] = false;
			SignalCount = -RC_GOOD_BUCKET_MAX;
		}
		INTCONbits.TMR0IF = false;	
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

