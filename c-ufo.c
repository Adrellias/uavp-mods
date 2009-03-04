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
//#pragma	config = 0x377A	// BODEN, HVP, no WDT, MCLRE disabled, PWRTE disabled
#else
#pragma	config OSC=HS, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC 
#endif

#include "c-ufo.h"
#include "bits.h"

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
int16	RE,PE;
int16	YE;	// Fehlersignal aus dem aktuellen Durchlauf
int16	REp,PEp;
int16	YEp;	// Fehlersignal aus dem vorigen Durchlauf (war RE/PE/YE)
int16	YawSum;	// Integrales Fehlersignal fuer Yaw, 0 = neutral
int16	RollSum, PitchSum;	// Integrales Fehlersignal fuer Roll und Pitch
int16	RollSamples, PitchSamples;
int8	LRIntKorr, FBIntKorr;
int8	NeutralLR, NeutralFB, NeutralUD;
int16 	UDSum;

uns8	BlinkCount, BlinkCycle, BaroCount;
uns8 	mSTick;
int8	BatteryVolts;
int8	Rw,Pw;

// Variables for barometric sensor PD-controller
int24	BaroBasePressure, BaroBaseTemp;
int24	BaroRelTempCorr;
int16	VBaroComp;
int16   BaroRelPressure;
uns8	BaroType, BaroTemp, BaroRestarts;

#pragma idata params
// Principal quadrocopter parameters - MUST remain in this order
int8	RollPropFactor		=18;
int8	RollIntFactor		=4;
int8	RollDiffFactor		=-40;
int8	BaroTempCoeff		=13;
int8	RollIntLimit		=16;
int8	PitchPropFactor		=18;
int8	PitchIntFactor		=4;	
int8	PitchDiffFactor		=-40;	 
int8	BaroThrottleProp	=2;
int8	PitchIntLimit		=16;
int8	YawPropFactor		=20;
int8	YawIntFactor		=40;
int8	YawDiffFactor		=6;
int8	YawLimit			=50;
int8	YawIntLimit			=6;
int8	ConfigParam			=0b00000000;
int8	TimeSlot			=4;	// control update interval + LEGACY_OFFSET
int8	LowVoltThres		=43;
int8	CamRollFactor		=0;	// unused
int8	LinFBIntFactor		=0;	// unused
int8	LinUDIntFactor		=8;
int8	MiddleUD			=0;
int8	MotorLowRun			= 40;
int8	MiddleLR			=0;
int8	MiddleFB			=0;
int8	CamPitchFactor		=0x44;
int8	CompassFactor		=5;
int8	BaroThrottleDiff	=4;
#pragma idata


// Ende Reihenfolgezwang

int16	MidRoll, MidPitch, MidYaw;

uns8	LedShadow;		// shadow register
int16	AbsDirection;	// wanted heading (240 = 360 deg)
int16	CurDeviation;	// deviation from correct heading

uns8	MFront,MLeft,MRight,MBack;	// output channels
uns8	MCamRoll,MCamPitch;
int16	Ml, Mr, Mf, Mb;
int16	Rl,Pl,Yl;		// PID output values
int16	Rp,Pp,Yp;
int16	Vud;

uns8	Flags[8];
uns8	Flags2[8];

uns8	IntegralCount;
int8	RollNeutral, PitchNeutral, YawNeutral;
uns8	ThrNeutral;
int16	ThrDownCount;

uns8	DropoutCount;
uns8	LedCount;

void LedGame(void)
{
	if( --LedCount == 0 )
	{
		LedCount = ((255-IGas)>>3) +5;	// new setup
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
	int16 Temp;

	if( _NewValues )
	{
		if( ThrDownCount > 0 )
		{
			if( (LedCount & 1) == 0 )
				ThrDownCount--;
			if( ThrDownCount == 0 )
				ThrNeutral = IGas;	// remember current Throttle level
		}
		else
		{
			if( ThrNeutral < THR_MIDDLE )
				Temp = 0;
			else
				Temp = ThrNeutral - THR_MIDDLE;

			if( IGas < THR_HOVER ) // no hovering below this throttle setting
				ThrDownCount = THR_DOWNCOUNT;	// left dead area

			if( IGas < Temp )
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
	while( _NoSignal || !Switch);	// no signal or switch is off
} // WaitForRXSignal

void main(void)
{
	uns8	i;
	uns8	LowGasCount;

	DisableInterrupts;

	InitPorts();
	OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
			USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_HIGH, _B38400);
	
	InitADC();
	
	OpenTimer0(TIMER_INT_OFF&T0_8BIT&T0_SOURCE_INT&T0_PS_1_16);
	OpenTimer1(T1_8BIT_RW&TIMER_INT_OFF&T1_PS_1_8&T1_SYNC_EXT_ON&T1_SOURCE_CCP&T1_SOURCE_INT);

	OpenCapture1(CAPTURE_INT_ON & C1_EVERY_FALL_EDGE); 	// capture mode every falling edge
	CCP1CONbits.CCP1M0 = NegativePPM;

	OpenTimer2(TIMER_INT_ON&T2_PS_1_16&T2_POST_1_16);		
	PR2 = TMR2_5MS;		// set compare reg to 9ms

	// setup flags register
	for ( i = 0; i<8; i++ )
		Flags2[i] = Flags[i] = 0; 
	_NoSignal = 1;		// assume no signal present

	LedShadow = 0;
    ALL_LEDS_OFF;

	InitArrays();

	#ifdef COMMISSIONING
	for (i=_EESet2*2; i ; i--)					// clear EEPROM parameter space
		WriteEE(i, -1);
	WriteParametersEE(1);						// copy RAM initial values to EE
	WriteParametersEE(2);
	#endif // COMMISSIONING
	ReadParametersEE();

	INTCONbits.PEIE = 1;		// Enable peripheral interrupts
	EnableInterrupts;

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
		INTCONbits.TMR0IE = 0;		// Disable TMR0 interrupt

		ALL_LEDS_OFF;
		LedRed_ON;		// Red LED on
		if(_UseLISL)
			LedYellow_ON;	// To signal LISL sensor is active

		InitArrays();
		ThrNeutral = 0xFF;

		EnableInterrupts;		// Enable all interrupts
	
		WaitForRxSignal(); // Wait until a valid RX signal is received

		ReadParametersEE();

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
			WriteTimer0(0);
			INTCONbits.TMR0IF = 0;
			INTCONbits.TMR0IE = 1;	// enable TMR0

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

			INTCONbits.TMR0IE = 0;	// disable timer
			GetGyroValues();

			ReadParametersEE();	// re-sets TimeSlot

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

