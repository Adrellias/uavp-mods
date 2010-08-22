// =================================================================================================
// =                                  UAVX Quadrocopter Controller                                 =
// =                             Copyright (c) 2008 by Prof. Greg Egan                             =
// =                   Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                       http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// =================================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even 
//    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//    General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

// Interrupt Routines

#include "uavx.h"

#ifdef CLOCK_16MHZ
#define MIN_PPM_SYNC_PAUSE 2500  	// 2500 *2us = 5ms
#else // CLOCK_40MHZ
#define MIN_PPM_SYNC_PAUSE 6250  	// 6250 *0.8us = 5ms
#endif //  CLOCK_16MHZ
// no less than 1500

void SyncToTimer0AndDisableInterrupts(void);
void ReceivingGPSOnly(uint8);
void InitTimersAndInterrupts(void);
int24 mSClock(void);
void low_isr_handler(void);
void high_isr_handler(void);

#pragma udata clocks
int24	mS[CompassUpdate+1];
#pragma udata

#pragma udata access isrvars
near int24 	PrevEdge, CurrEdge;
near uint8 	Intersection, PrevPattern, CurrPattern;
near i16u 	Width, Timer0;
near i16u 	PPM[MAX_CONTROLS];
near int8 	PPM_Index;
near int24 	PauseTime;
near uint8 	GPSRxState;
near uint8 	ll, tt, gps_ch;
near uint8 	RxCheckSum, GPSCheckSumChar, GPSTxCheckSum;
near int8	State, FailState;
near boolean WaitingForSync;
near i16u 	ADCValue;
#pragma udata

int8	SignalCount;
uint16	RCGlitches;

void SyncToTimer0AndDisableInterrupts(void)
{
	do { GetTimer0; } while ( Timer0.u16 < INT_LATENCY ); 
	// one interrupt service which may occur between the check and the disable! 	
	DisableInterrupts;

	while( !INTCONbits.TMR0IF ) ;		// now wait overflow
	FastWriteTimer0(TMR0_1MS);	
	INTCONbits.TMR0IF = false;			// quit TMR0 interrupt
} // SyncToTimer0AndDisableInterrupts

void ReceivingGPSOnly(boolean r)
{
	if ( r != F.ReceivingGPS )
	{
		PIE1bits.RCIE = false;
		F.ReceivingGPS = r;

		#ifndef TESTING // not used for testing - make space!
		if ( F.ReceivingGPS )			
			#ifdef CLOCK_16MHZ
			OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
				USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_HIGH, _B9600);
			#else // CLOCK_40MHZ
			OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
				USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_LOW, _B9600);
			#endif // CLOCK_16MHZ
		else
		#endif // !TESTING
			OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
				USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_HIGH, _B38400);
   		PIE1bits.RCIE = r;
	}
} // ReceivingGPSOnly

void InitTimersAndInterrupts(void)
{
	static int8 i, j;

	#ifdef CLOCK_16MHZ
	OpenTimer0(TIMER_INT_OFF&T0_8BIT&T0_SOURCE_INT&T0_PS_1_16);
	#else // CLOCK_40MHZ
	OpenTimer0(TIMER_INT_OFF&T0_16BIT&T0_SOURCE_INT&T0_PS_1_16);	
	#endif // CLOCK_16MHZ

	OpenTimer1(T1_16BIT_RW&TIMER_INT_OFF&T1_PS_1_8&T1_SYNC_EXT_ON&T1_SOURCE_CCP&T1_SOURCE_INT);
	OpenCapture1(CAPTURE_INT_ON & C1_EVERY_FALL_EDGE); 	// capture mode every falling edge

	TxQ.Head = TxQ.Tail = RxCheckSum = 0;

	for (i = Clock; i<= CompassUpdate; i++)
		mS[i] = 0;

   	ReceivingGPSOnly(false);
} // InitTimersAndInterrupts

int24 mSClock(void)
{ // MUST make locked accesses to the millisecond clock
	static int24 m;

	DisableInterrupts;
	m = mS[Clock];
	EnableInterrupts;
	return(m);
} // mSClock

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

		if ( Width.i16 > MIN_PPM_SYNC_PAUSE ) 	// A pause  > 5ms
		{
			PPM_Index = 0;						// Sync pulse detected - next CH is CH1
			F.RCFrameOK = true;
			F.RCNewValues = false;
			PauseTime = Width.i16;	
		}
		else 
			if (PPM_Index < RC_CONTROLS)
			{
				#ifdef CLOCK_16MHZ	
				Width.i16 >>= 1; 				// Width in 4us ticks.	
				if ( Width.b1 == (uint8)1 ) 			// Check pulse is 1.024 to 2.048mS
					PPM[PPM_Index].i16 = (int16) Width.i16;	
				#else // CLOCK_40MHZ
				if ( (Width.i16 >= 1250 ) && (Width.i16 <= 2500) ) // Width in 0.8uS ticks 	
					PPM[PPM_Index].i16 = Width.i16 - 1250;	
				#endif // CLOCK_16MHZ	
				else
				{
					// preserve old value i.e. default hold
					RCGlitches++;
					F.RCFrameOK = false;
				}
				
				PPM_Index++;
				// MUST demand rock solid RC frames for autonomous functions not
				// to be cancelled by noise-generated partially correct frames
				if ( PPM_Index == RC_CONTROLS )
				{
					if ( F.RCFrameOK )
					{
						F.RCNewValues = true;
 						SignalCount++;
					}
					else
					{
						F.RCNewValues = false;
						SignalCount -= RC_GOOD_RATIO;
					}

					SignalCount = Limit(SignalCount, -RC_GOOD_BUCKET_MAX, RC_GOOD_BUCKET_MAX);
					F.Signal = SignalCount > 0;

					if ( F.Signal )
						mS[LastValidRx] = mS[Clock];
					mS[RCSignalTimeout] = mS[Clock] + RC_SIGNAL_TIMEOUT_MS;
				}
			}

		if ( !F.UsingSerialPPM )						
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
				if (GPSCheckSumChar < (uint8)2)
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
					F.GPSSentenceReceived = GPSTxCheckSum == RxCheckSum;
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
						if ( ll > (uint8)( GPSRXBUFFLENGTH-1 ) )
							GPSRxState = WaitGPSSentinel;
					}
							
				break;
			case WaitNMEATag:
				RxCheckSum ^= gps_ch;
				if ( gps_ch == NMEATag[tt] ) 
					if ( tt == (uint8)MAXTAGINDEX )
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
		#ifndef TESTING // not used for testing - make space!
		if ( Armed && ( P[TelemetryType] == GPSTelemetry) ) // piggyback GPS telemetry on GPS Rx
			TXREG = gps_ch;
		#endif // TESTING
	
		PIR1bits.RCIF = false;
	}

	if ( INTCONbits.T0IF )  
	{
		#ifdef CLOCK_16MHZ
			// do nothing - just let TMR0 wrap around for 1.024mS intervals
		#else // CLOCK_40MHZ
			Timer0.b0 = TMR0L;
			Timer0.b1 = TMR0H;
			Timer0.u16 += TMR0_1MS; 
			TMR0H = Timer0.b1;
			TMR0L = Timer0.b0;
		#endif // CLOCK_40MHZ

		mS[Clock]++;

		if ( ( mS[UpdateTimeout] - mS[Clock] ) > 15 ) // should not happen! 
		{
			WaitingForSync = false;
			if ( State == InFlight )
			{
				Stats[BadS]++;
				Stats[BadNumS] = mS[UpdateTimeout] - mS[Clock];
			}
		}
		else
			WaitingForSync = mS[Clock] < mS[UpdateTimeout];

		if ( F.Signal && (mS[Clock] > mS[RCSignalTimeout]) ) 
		{
			F.Signal = false;
			SignalCount = -RC_GOOD_BUCKET_MAX;
		}

		#ifndef TESTING // not used for testing - make space!
		if ( Armed  && ( P[TelemetryType] !=  GPSTelemetry ) )
			if (( TxQ.Head != TxQ.Tail) && PIR1bits.TXIF )
			{
				TXREG = TxQ.B[TxQ.Head];
				TxQ.Head = (TxQ.Head + 1) & TX_BUFF_MASK;
			}
		#endif // TESTING
 
		// Scan ADC ports even if using ITG-3200
		if ( !ADCON0bits.GO)
		{
			ADCValue.b1 = ADRESH;
			ADCValue.b0 = ADRESL;
			
			#ifdef USE_ADC_FILTERS
				ADCVal[ADCChannel].v.i24 += (int24)(ADCValue.i16 - ADCVal[ADCChannel].v.b2_1) * ADCVal[ADCChannel].a;
			#else
				ADCVal[ADCChannel].v.b2_1 = ADCValue.i16;
			#endif // USE_ADC_FILTERS

			if ( ++ADCChannel > ADC_TOP_CHANNEL )
				ADCChannel = 0;
				
			ADCON0 = ((ADCChannel << 2) & 0b00111100) | (ADCON0 & 0b11000011);
			ADCON0bits.GO = true;
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

