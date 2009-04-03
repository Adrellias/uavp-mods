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


// CC5X Compiler parameters:
// -CC -fINHX8M -a -L -Q -V -FM -DMATHBANK_VARS=bank0 -DMATHBANK_PROG=2

#ifdef ICD2_DEBUG
#pragma	config = 0x377A	// BODEN, HVP, no WDT, MCLRE disabled, PWRTE disabled
#else
#pragma	config = 0x3F72	// BODEN, HVP, no WDT, MCLRE disabled, PWRTE enabled
#endif

#include "c-ufo.h"
#include "bits.h"

#pragma codepage = 0

// Math Library
#include "mymath16.h"

// EEPROM-Data can currently not be used with relocatable assembly
// CC5X limitation!
// #define EEPROM_START	0x2100
// #pragma cdata[EEPROM_START] = 0,0,0,0,0,0,0,0,0,0,20,8,16

// The globals

uns8	IGas;			// actual input channel, can only be positive!
int8 	IRoll,IPitch,IYaw;	// actual input channels, 0 = neutral
uns8	IK5;		// actual channel 5 input
uns8	IK6;		// actual channel 6 input
uns8	IK7;		// actual channel 7 input

// PID Regler Variablen
int8		RE,PE;
int8		YE;	// Fehlersignal aus dem aktuellen Durchlauf
int8		REp,PEp;
int8		YEp;	// Fehlersignal aus dem vorigen Durchlauf (war RE/PE/YE)
int16	YawSum;	// Integrales Fehlersignal fuer Yaw, 0 = neutral
int16	RollSum, PitchSum;	// Integrales Fehlersignal fuer Roll und Pitch
uns16	RollSamples, PitchSamples;
int8		LRIntKorr, FBIntKorr;
uns8	NeutralLR, NeutralFB, NeutralUD;
int8 	UDSum;

int8		NegFact; // general purpose
uns8	BlinkCount, BlinkCycle, BaroCount;
int16	niltemp1;
int16	niltemp;
int8		BatteryVolts; // added by Greg Egan
int8		Rw,Pw;

uns16	BaroBasePressure, BaroBaseTemp, BaroRelTempCorr;
int8		VBaroComp;
int8 	BaroRelPressure;
uns8	BaroType, BaroTemp, BaroRestarts;


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

uns16	MidRoll, MidPitch, MidYaw;


uns8	LedShadow;	// shadow register
uns16	AbsDirection;	// wanted heading (240 = 360 deg)
int8		CurDeviation;	// deviation from correct heading

uns8	MFront,MLeft,MRight,MBack;	// output channels
uns8	MCamRoll,MCamPitch;
int16	Ml, Mr, Mf, Mb;
int16	Rl,Pl,Yl;	// PID output values
int16	Rp,Pp,Yp;
int16	Vud;

uns8	Flags;
uns8	Flags2;

uns8	IntegralCount;
int8		RollNeutral, PitchNeutral, YawNeutral;
uns8	ThrNeutral;
uns16	ThrDownCount;

uns8	DropoutCount;
uns8	LedCount;

void LedGame(void)
{
	if( --LedCount == 0 )
	{
		LedCount = 255-IGas;	// new setup
		LedCount >>= 3;
		LedCount += 5;
		if( _Hovering )
		{
			AUX_LEDS_ON;	// baro locked, all aux-leds on
		}
		else
		if( LedShadow & LedAUX1 )
		{
			AUX_LEDS_OFF;
			LedAUX2_ON;
		}
		else
		if( LedShadow & LedAUX2 )
		{
			AUX_LEDS_OFF;
			LedAUX3_ON;
		}
		else
		{
			AUX_LEDS_OFF;
			LedAUX1_ON;
		}
	}
} // LedGame

void WaitThrottleClosed(void)
{
	DropoutCount = 1;
	while( (IGas >= _ThresStop) )
	{
		if ( _NoSignal)
			break;
		if( _NewValues )
		{
			OutSignals();
			_NewValues = 0;
			if( --DropoutCount <= 0 )
			{
				LedRed_TOG;	// toggle red Led 
				DropoutCount = 10;		// to signal: THROTTLE OPEN
			}
		}
		ProcessComCommand();
	}
	LedRed_OFF;
} // WaitThrottleClosed

void CheckThrottleMoved(void)
{
	if( _NewValues )
	{
		if( ThrDownCount > 0 )
		{
			if( LedCount.0 == 0 )
				ThrDownCount--;
			if( ThrDownCount == 0 )
				ThrNeutral = IGas;	// remember current Throttle level
		}
		else
		{
			if( ThrNeutral < THR_MIDDLE )
				NegFact = 0;
			else
				NegFact = ThrNeutral - THR_MIDDLE;

			if( IGas < THR_HOVER ) // no hovering below this throttle setting
				ThrDownCount = THR_DOWNCOUNT;	// left dead area

			if( IGas < NegFact )
				ThrDownCount = THR_DOWNCOUNT;	// left dead area
			if( IGas > ThrNeutral + THR_MIDDLE )
				ThrDownCount = THR_DOWNCOUNT;	// left dead area
		}
	}
} // CheckThrottleMoved

void WaitForRxSignal(void)
{
	DropoutCount = MODELLOSTTIMER;
	do
	{
		Delay100mSWithOutput(2);	// wait 2/10 sec until signal is there
		ProcessComCommand();
		if( _NoSignal )
		{
			if( Switch )
			{
				if( --DropoutCount == 0 )
				{
					_LostModel = 1;
					DropoutCount = MODELLOSTTIMERINT;
				}
			}
			else
				_LostModel = 0;
		}
	}
	while( _NoSignal || !Switch);	// no signal or switch is off
} // WaitForRXSignal

void main(void)
{
	uns8	LowGasCount;

	OPTION_REG = 0b.00000.011;	// TMR0 w/ int clock, 1:16 presc (see _PreScale0),
					// weak pullups on
	// general ports setup
	TRISA = 0b.00111111;	// all inputs
	ADCON1 = 0b.0.000.0010;	// uses 5V as Vref

	PORTB = 0b.1100.0000;		// all outputs to low, except RB6 & 7 (I2C)!
	TRISB = 0b.0100.0000;	// all servo and LED outputs
	PORTC = 0b.0110.0000;		// all outputs to low, except TxD and CS
	TRISC = 0b.10000100;	// RC7, RC2 are inputs
	RBPU_ = 1;			// disable weak pullups
	CKE = 1;		// default I2C - enable SMBus thresholds for 3.3V LISL
	LedShadow = 0;
    ALL_LEDS_OFF;

	// setup serial port for 8N1
	TXSTA = 0b.0010.0100;	// async mode, BRGH = 1
	RCSTA = 0b.1001.0000;	// receive mode
	SPBRG = _B38400;
	//SPBRG = _B115200;
	//SPBRG = _B230400;

	W = RCREG;			// be sure to empty FIFO

	// timer setup
	T1CON = 0b.00.11.0001;	// enable TMR1, 1:8 prescaler, run with 2us clock
	CCP1CON = 0b.00.00.0100;	// capture mode every falling edge

	TMR2 = 0;
	T2CON = 0b.0.1111.1.11;	// enable TMR2, 1:16 prescaler, 1:16 postscaler (see _Pre/PostScale2)
	PR2 = TMR2_5MS;		// set compare reg to 9ms

	// setup flags register
	Flags = 0;
	Flags2 = 0;
	_NoSignal = 1;		// assume no signal present

	InitArrays();

	LedRed_ON;		// red LED on
	Delay100mSWithOutput(1);	// wait 1/10 sec until LISL is ready to talk

	#ifdef USE_ACCSENS
	IsLISLactive();
	#ifdef ICD2_DEBUG
	_UseLISL = 1;	// because debugger uses RB7 (=LISL-CS) :-(
	#endif

	NeutralLR = 0;
	NeutralFB = 0;
	NeutralUD = 0;
	if( _UseLISL )
		GetEvenValues();	// into Rp, Pp, Yp
	#endif  // USE_ACCSENS

	// enable the interrupts
	CCP1IE = 1;
	TMR2IE = 1;		// T1-Capture and T2 interrupt enable
		
	PEIE = 1;		// Enable peripheral interrupts

	ThrNeutral = 0xFF;

	// send hello text to serial COM
	Delay100mSWithOutput(1);	// just to see the output after powerup

	InitDirection();	// init compass sensor
	InitAltimeter();

	ShowSetup(1);

	IK6 = IK7 = _Neutral;

Restart:
	IGas =IK5 = _Minimum;	// Assume parameter set #1
	Beeper_OFF;

	// DON'T MOVE THE UFO!
	// ES KANN LOSGEHEN!

	while(1)
	{
		T0IE = 0;		// Disable TMR0 interrupt

		ALL_LEDS_OFF;
		LedRed_ON;		// Red LED on
		if(_UseLISL)
			LedYellow_ON;	// To signal LISL sensor is active

		InitArrays();
		ThrNeutral = 0xFF;

		GIE = 1;		// Enable all interrupts
	
		WaitForRxSignal(); // Wait until a valid RX signal is received

		ReadEEdata();

		WaitThrottleClosed();

		if ( _NoSignal )
			goto Restart;

		// ######## MAIN LOOP ########

		// loop length is controlled by a programmable variable "TimeSlot"

		DropoutCount = 0;
		BlinkCount = 0;
		BlinkCycle = 0;			// number of full blink cycles

		IntegralCount = 16;	// do 16 cycles to find integral zero point

		ThrDownCount = THR_DOWNCOUNT;

		// if Ch7 below +20 (near minimum) assume use for camera trigger
		// else assume use for camera roll trim	
		_UseCh7Trigger = IK7 < 30;
			
		while ( Switch == 1 )	// as int16 as power switch is ON
		{
			// wait pulse pause delay time (TMR0 has 1024us for one loop)
			TMR0 = 0;
			T0IF = 0;
			T0IE = 1;	// enable TMR0

			UpdateBlinkCount();
			RollSamples =PitchSamples = 0;	// zero gyros sum-up memory
			// sample gyro data and add everything up while waiting for timing delay

			GetGyroValues();
			GetDirection();	
			ComputeBaroComp();

			while( TimeSlot > 0 )
			{
				// Here is the place to insert own routines
				// It should consume as less time as possible!
				// ATTENTION:
				// Your routine must return BEFORE TimeSlot reaches 0
				// or non-optimal flight behavior might occur!!!
			}

			T0IE = 0;	// disable timer
			GetGyroValues();

			ReadEEdata();	// re-sets TimeSlot

			CalcGyroValues();

			// check for signal dropout while in flight
			if( _NoSignal && _Flying )
			{
				if( (BlinkCount & 0x0f) == 0 )
					DropoutCount++;
				if( DropoutCount < MAXDROPOUT )
				{	// FAILSAFE	
					// hold last throttle
					_LostModel = 1;
					ALL_LEDS_OFF;
					IRoll = RollNeutral;
					IPitch = PitchNeutral;
					IYaw = YawNeutral;
					goto DoPID;
				}
				break;	// timeout, stop everything
			}

// IF THE THROTTLE IS CLOSED FOR MORE THAN 2 SECONDS AND THE QUADROCOPTER IS STILL 
// IN FLIGHT (SAY A RAPID THROTTLE CLOSED DESCENT) THEN THE FOLLOWING CODE MAY RESET 
// THE INTEGRAL SUMS (PITCH AND ROLL ANGLES) WHEN THE QUADROCOPTER IS NOT "LEVEL".

			// allow motors to run on low throttle 
			// even if stick is at minimum for a short time
			if( _Flying && (IGas <= _ThresStop) )
				if( --LowGasCount > 0 )
					goto DoPID;

			if( _NoSignal || 
			    ( (_Flying && (IGas <= _ThresStop)) ||
			      (!_Flying && (IGas <= _ThresStart)) ) )
			{	// UFO is landed, stop all motors

				TimeSlot += 2; // to compensate PID() calc time!

				IntegralCount = 16;	// do 16 cycles to find integral zero point

				ThrDownCount = THR_DOWNCOUNT;
				
				InitArrays();	// resets _Flying flag!
				MidRoll = 0;	// gyro neutral points
				MidPitch = 0;
				MidYaw = 0;
				if( _NoSignal && Switch )	// _NoSignal set, but Switch is on?
					break;	// then RX signal was lost

				ALL_LEDS_OFF;				
				AUX_LEDS_OFF;
				LedGreen_ON;

				ProcessComCommand();
			}
			else
			{	// UFO is flying!
				if( !_Flying )	// about to start
				{	// set current stick values as FAILSAFES
					RollNeutral = IRoll;
					PitchNeutral = IPitch;
					YawNeutral = IYaw;

					AbsDirection = COMPASS_INVAL;

					ProcessComCommand();
						
					LedCount = 1;
				}

				_Flying = 1;
				_LostModel = 0;
				DropoutCount = 0;
				LowGasCount = 100;		
				LedGreen_ON;

				LedGame();

DoPID:
				// do the calculations
				Rp = 0;
				Pp = 0;

				CheckThrottleMoved();

				if(	IntegralCount > 0 )
					IntegralCount--;
				else
				{
					PID();
					MixAndLimit();
				}

				// remember old gyro values
				REp = RE;
				PEp = PE;
				YEp = YE;
			}
			
			MixAndLimitCam();
			OutSignals();

			CheckBattery();
			CheckAlarms();

			#ifdef DEBUG_SENSORS
			if( IntegralCount == 0 )
			{
				SendComChar(0x0d);
				SendComChar(0x0a);
			}
			#endif			

		}	// END NORMAL OPERATION WHILE LOOP

		Beeper_OFF;

		// CPU kommt hierher wenn der Schalter ausgeschaltet wird
	}
	// CPU does /never/ arrive here
	//	ALL_OUTPUTS_OFF;
} // main

