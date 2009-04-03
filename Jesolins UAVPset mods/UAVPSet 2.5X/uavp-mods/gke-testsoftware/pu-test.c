// ==============================================
// =    U.A.V.P Brushless UFO Test-Software     =
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


// CC5X Compiler parameters:
// -CC -fINHX8M -a -L -Q -V -FM -DMATHBANK_VARS=bank0 -DMATHBANK_PROG=2


#ifdef ICD2_DEBUG
#pragma	config = 0x377A	// BODEN, HVP, no WDT, MCLRE disabled, PWRTE disabled
#else
#pragma	config = 0x3F72	// BODEN, HVP, no WDT, MCLRE disabled, PWRTE enabled
#endif

#include "pu-test.h"
#include "bits.h"

#pragma codepage = 0


// The globals


uns8 ConfigReg;


uns8	LedShadow;	// shadow register

uns8	Flags;

uns16	CurrK1,CurrK2,CurrK3,CurrK4,CurrK5,CurrK6,CurrK7;

uns8	BaroType, BaroTemp;
uns16 	BaroTemperature, BaroPressure;
uns16	PauseTime;
uns8	MVorne, MHinten, MLinks, MRechts;
uns8	MCamRoll, MCamNick;
uns8	EscI2CFlags;

// Die Reihenfolge dieser Variablen MUSS gewahrt bleiben!!!!
int8	RollPropFactor;
int8	RollIntFactor;
int8	RollDiffFactor;
int8 RollLimit;
int8 RollIntLimit;

int8	PitchPropFactor;
int8	PitchIntFactor;
int8	PitchDiffFactor;
int8 PitchLimit;
int8 PitchIntLimit;

int8	YawPropFactor;
int8	YawIntFactor;
int8	YawDiffFactor;
int8	YawLimit;
int8 YawIntLimit;

int8	ConfigParam;
int8	TimeSlot;
int8 LowVoltThres;

int8	LinLRIntFactor;
int8	LinFBIntFactor;
int8	LinUDIntFactor;
int8 MiddleUD;
int8 MotorLowRun;
int8	MiddleLR;
int8 MiddleFB;
int8	CamPitchFactor;
int8 CompassFactor;

int8 BaroThrottleDiff;

// Ende Reihenfolgezwang

void main(void)
{

	OPTION_REG = 0b.10000.011;	// TMR0 w/ int clock, 1:16 presc (see _PreScale0),
					// weak pullups off
// general ports setup
	TRISA = 0b.00111111;	// all inputs
	ADCON1 = 0b.0.000.0010;	// uses 5V as Vref

	PORTB = 0b.1100.0000;		// all outputs to low, except RB6 & 7 (I2C)!
	TRISB = 0b.0100.0000;	// all servo and LED outputs
	PORTC = 0b.0110.0000;		// all outputs to low, except TxD and CS
	TRISC = 0b.10000100;	// RC7, RC2 are inputs
	CKE = 1;		// default I2C - enable SMBus thresholds for 3.3V LISL

	RBPU_ = 0;		// now enable internal pullups

	LedShadow = 0;

    ALL_LEDS_OFF;
	LedBlue_ON;

// timer setup
	T1CON = 0b.00.11.0001;	// enable TMR1, 1:8 prescaler, run with 2us clock
	CCP1CON = 0b.00.00.0100;	// capture mode every falling edge

	TMR2 = 0;
	T2CON = 0b.0.1111.1.11;	// enable TMR2, 1:16 prescaler, 1:16 postscaler (see _Pre/PostScale2)
	PR2 = TMR2_5MS;		// set compare reg to 9ms

// setup flags register
	Flags = 0;
	_NoSignal = 1;		// assume no signal present

	LedRed_ON;		// red LED on
	Delay100mS(1);	// wait 1/10 sec until LISL is ready to talk
	IsLISLactive();
#ifdef ICD2_DEBUG
	_UseLISL = 1;	// because debugger uses RB7 (=LISL-CS) :-(
#endif

// setup serial port for 8N1
	TXSTA = 0b.0010.0100;	// async mode, BRGH = 1
	RCSTA = 0b.1001.0000;	// receive mode
	SPBRG = _B38400;
	W = RCREG;			// be sure to empty FIFO

// enable the interrupts
	CCP1IE = 1;
	TMR2IE = 1;		// T1-Capture and T2 interrupt enable
		
	PEIE = 1;		// enable peripheral interrupts

	ConfigReg = 0;
	CurrK1 =
	CurrK2 =
	CurrK3 =
	CurrK4 =
	CurrK5 =
	CurrK6 =
	CurrK7 = 0xFFFF;
	PauseTime = 0;
	MVorne =
	MLinks =
	MRechts =
	MHinten = _Minimum;

	MCamRoll = 
	MCamNick = _Neutral;

	// send hello text to serial COM
	Delay100mS(1);	// just to see the output after powerup

	GIE = 1;

	ShowSetup(1);

	while(1)
	{
		// turn red LED on of signal missing or invalid, green if OK
		if( _NoSignal )
		{
			LedRed_ON;
			LedGreen_OFF;
		}
		else
		{
			LedGreen_ON;
			LedRed_OFF;
		}
		// output servo values
		TimeSlot = 10;
		while( TimeSlot > 0 )
		{
			while( T0IF == 0 ) 	// 1024us wait
			{	
				ProcessComCommand();
			}
			T0IF = 0;
			TimeSlot--;
		}
		OutSignals();
	}
} // main

