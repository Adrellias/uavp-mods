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

#include "uavx.h"

#ifdef TESTING

void DoLEDs(void);
void ReceiverTest(void);
void PowerOutput(int8);
void LEDsAndBuzzer(void);
void BatteryTest(void);

void DoLEDs(void)
{
	if ( F.AccelerationsValid  ) LEDYellow_ON; else LEDYellow_OFF;

	if( F.Signal )
	{
		LEDRed_OFF;
		LEDGreen_ON;
	}
	else
	{
		LEDGreen_OFF;
		LEDRed_ON;
	}
} // DoLEDs

void ReceiverTest(void)
{
	uint8 s;
	uint16 v;

	TxString("\r\nRx: ");
	ShowRxSetup();
	TxString("\r\n");
	
	TxString("\tRAW Rx frame values - neutrals NOT applied\r\n");
	TxString("\tChannel order is: ");
	for ( s = 0; s < RC_CONTROLS; s++)
		TxChar(RxChMnem[RMap[s]-1]);

	if ( F.Signal )
		TxString("\r\nSignal OK ");
	else
		TxString("\r\nSignal FAIL ");	
	TxVal32(mSClock() - mS[LastValidRx], 0, 0);
	TxString(" mS ago\r\n");
	
	// Be wary as new RC frames are being received as this
	// is being displayed so data may be from overlapping frames

	for ( s = 0; s < RC_CONTROLS ; s++ )
	{
		TxChar(s+'1');
		TxString(": ");
		TxChar(RxChMnem[RMap[s]-1]);
		TxString(":\t");
 
		#ifdef CLOCK_16MHZ

		TxVal32((int32)PPM[s].i16 * 4L, 3, 0);
		TxChar(HT);
	    TxVal32(((int32)(PPM[s].i16 & 0x00ff)*100)/RC_MAXIMUM, 0, '%');
		if( ( PPM[s].i16 & 0xff00) != (uint16)0x0100 ) 
			TxString(" FAIL");

		#else // CLOCK_40MHZ

		TxVal32(( PPM[s].i16 * 8L + 5L ) / 10L, 3, 0);
		TxChar(HT);
		TxVal32(((int32)PPM[s].i16*100L + 625L ) / 1250L, 0, '%');
		if( ( PPM[s].i16 < 0 ) || ( PPM[s].i16 > 1250 ) ) 
			TxString(" FAIL");

		#endif // CLOCK_16MHZ

		TxNextLine();
	}

	// show pause time
	TxString("Gap:\t");
	#ifdef CLOCK_16MHZ
	v = PauseTime * 2L;
	#else // CLOCK_40MHZ
	v = ( PauseTime * 8L + 5 )/10L;
	#endif // CLOCK_16MHZ
	TxVal32( v, 3, 0);		
	TxString("mS\r\n");
	TxString("Glitches:\t");
	TxVal32(RCGlitches,0,0);
	TxNextLine();

} // ReceiverTest

void PowerOutput(int8 d)
{
	int8 s;
	uint8 m;

	m = 1 << d;
	for( s=0; s < 10; s++ )	// 10 flashes (count MUST be even!)
	{
		LEDShadow ^= m;
		SendLEDs();
		Delay1mS(50);
	}		
} // PowerOutput

void LEDsAndBuzzer(void)
{
	uint8 s, m, mask, LEDSave;

	LEDSave = LEDShadow;
	LEDShadow  = 0;
	SendLEDs();	

	TxString("\r\nOutput test\r\n");
	mask = 1;
	for ( m = 1; m <= 8; m++ )		
	{
		TxChar(m+'0');
		TxString(":\t");
		switch( m ) {
		case 1: TxString("Aux2   "); break;
		case 2: TxString("Blue   "); break;
		case 3: TxString("Red    "); break;
		case 4: TxString("Green  "); break;
		case 5: TxString("Aux1   "); break;
		case 6: TxString("Yellow "); break;
		case 7: TxString("Aux3   "); break;
		case 8: TxString("Beeper "); break;
		}
		TxString("\tPress any key (x) to continue\r\n");	
		while( PollRxChar() != 'x' ); // UAVPSet uses 'x' for any key button

		for( s = 0; s < 10; s++ )	// 10 flashes (count MUST be even!)
		{
			LEDShadow ^= mask;
			SendLEDs();
			Delay1mS(100);
		}
		mask <<= 1;
	}
	LEDShadow  = LEDSave;
	SendLEDs();	
	TxString("Test Finished\r\n");		
} // PowerOutput

void BatteryTest(void)
{
	int32 v;

	TxString("\r\nBattery test");

	// Battery
	v = ((int24)ADC(ADCBattVoltsChan)* 61L + 102L)/205L; // resistive divider
	TxString("Batt:\t");
	TxVal32(v, 1, 'V');
	if ( v < 90 )
		TxString(" ** LIPO ALARM < 9V ** ");
	else	
	if ( v < 95 )
		TxString(" ** LOW < 9.5V ** ");
	TxNextLine();
	
} // BatteryTest

#endif // TESTING
