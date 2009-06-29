// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =     Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer       =
// =                          http://uavp.ch                             =
// =======================================================================

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

//#ifdef CLOCK_40MHZ
//#pragma	config OSC=HSPLL, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC
//#else
#pragma	config OSC=HS, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC  
//#endif

#include "uavx.h"

// Global Variables

uint8	State;
uint8	CurrentParamSet;
int8	TimeSlot;

uint8	IGas;						// actual input channel, can only be positive!
int8 	IRoll,IPitch,IYaw;			// actual input channels, 0 = neutral
uint8	IK5;						// actual channel 5 input
uint8	IK6;						// actual channel 6 input
uint8	IK7;						// actual channel 7 input

// PID Regler Variablen
int16	RE, PE, YE;					// gyro rate error	
int16	REp, PEp, YEp;				// previous error for derivative
int16	RollSum, PitchSum, YawSum;	// integral 	
int16	RollRate, PitchRate, YawRate;
int16	RollIntLimit256, PitchIntLimit256, YawIntLimit256, NavIntLimit256;
int16	GyroMidRoll, GyroMidPitch, GyroMidYaw;
int16	HoverThrottle, DesiredThrottle;
int16	DesiredRoll, DesiredPitch, DesiredYaw, Heading;
i16u	Ax, Ay, Az;
int8	LRIntKorr, FBIntKorr;
int8	NeutralLR, NeutralFB, NeutralUD;
int16 	UDAcc, UDSum, VUDComp;
uint8	IdleThrottle;

int16 	SqrNavClosingRadius, NavClosingRadius, CompassOffset;

uint8 	NavState;
uint8 	NavSensitivity;
int16	AltSum, AE;

// Failsafes
uint8	ThrNeutral;

// Variables for barometric sensor PD-controller
int24	DesiredBaroPressure, OriginBaroPressure;
int16   BaroRelPressure, BaroRelTempCorr;
i16u	BaroVal;
int16	VBaroComp;
uint8	BaroType, BaroTemp, BaroRestarts;

uint8	LEDShadow;		// shadow register
int16	AbsDirection;	// wanted heading (240 = 360 deg)
int16	CurDeviation;	// deviation from correct heading

uint8	MCamRoll,MCamPitch;
int16	Motor[NoOfMotors];

int16	Rl,Pl,Yl;		// PID output values
int16	Vud;

int16	Trace[LastTrace];

boolean	Flags[32];

int16	ThrDownCycles, DropoutCycles, GPSCycles, LEDCycles, BlinkCycle, BaroCycles;
uint32	Cycles;
int8	IntegralCount;
uint24	RCGlitches;
int8	BatteryVolts;
int8	Rw,Pw;

#pragma udata params
// Principal quadrocopter parameters - MUST remain in this order
// for block read/write to EEPROM
int8	RollKp;
int8	RollKi;
int8	RollKd;
int8	BaroTempCoeff;
int8	RollIntLimit;
int8	PitchKp;
int8	PitchKi;	
int8	PitchKd;	 
int8	BaroCompKp;
int8	PitchIntLimit;
int8	YawKp;
int8	YawKi;
int8	YawKd;
int8	YawLimit;
int8	YawIntLimit;
int8	ConfigParam;
int8	TimeSlots;	// control update interval + LEGACY_OFFSET
int8	LowVoltThres;
int8	CamRollKp;	
int8	PercentHoverThr;
int8	VertDampKp; 
int8	MiddleUD;
int8	PercentIdleThr;
int8	MiddleLR;
int8	MiddleFB;
int8	CamPitchKp;
int8	CompassKp;
int8	BaroCompKd;
int8	NavRadius;
int8	NavIntLimit;
int8	NavAltKp;
int8	NavAltKi;
int8	NavRTHAlt;
int8	NavMagVar;
int8 	GyroType;			
int8 	ESCType;		
#pragma udata

// mask giving common variables across parameter sets
const rom int8	ComParms[]={
	0,0,0,1,0,0,0,0,1,0,
	0,0,0,0,0,1,1,1,0,1,
	1,1,1,1,1,0,0,1,0,0,
	0,0,0,1,1,1
	};

void main(void)
{
	static uint8	i;
	static uint8	LowGasCycles;
	static int16	Temp;

	DisableInterrupts;

	InitPorts();
	OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
			USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_HIGH, _B38400);
	
	InitADC();
	
	InitTimersAndInterrupts();

	CurrentParamSet = 1;
	ReadParametersEE();

	for ( i = 0; i<32 ; i++ )
		Flags[i] = false; 
	
	LEDShadow = 0;
    ALL_LEDS_OFF;
	LEDRed_ON;
	Beeper_OFF;

	InitArrays();
	ThrNeutral = 255;	
	IGas = DesiredThrottle = IK5 = IK6 = IK7 = _Minimum;

	INTCONbits.PEIE = true;		// Enable peripheral interrupts
	EnableInterrupts;

	Delay100mSWithOutput(5);	// wait 0.5 sec until LISL is ready to talk
	InitLISL();

	InitDirection();
	InitBarometer();
	InitGPS();
	InitNavigation();

	ShowSetup(1);
	
	while( true )
	{
		ReceivingGPSOnly(false);

		if ( !_Signal )
		{
			IGas = DesiredThrottle = IK5 = _Minimum;
			Beeper_OFF;
		}

		INTCONbits.TMR0IE = false;
		ALL_LEDS_OFF; 
		LEDRed_ON;	
		if( _AccelerationsValid )
			LEDYellow_ON;

		InitArrays();
		EnableInterrupts;	
		WaitForRxSignal();
		UpdateControls();
 
		ReadParametersEE();
		WaitThrottleClosed();
			
		DropoutCycles = MAXDROPOUT;
		_Failsafe = false;
		TimeSlot = Limit(TimeSlots, 2, 20);
		Cycles = 0;
		State = Starting;

		while ( Armed  )
		{	
			ReceivingGPSOnly(true); // no Command processing while the Quadrocopter is armed

			Cycles++;
			UpdateGPS();

			if ( _Signal && !_Failsafe )
			{
				UpdateControls();

				switch ( State && !_Failsafe ) {
				case Starting:
					ThrDownCycles = THR_DOWNCOUNT;
					InitArrays();

					#ifdef NEW_ERECT_GYROS

					IntegralCount = 0;
					#else
					IntegralCount = 16; // erect gyros - old style
					#endif

					ALL_LEDS_OFF;				
					AUX_LEDS_OFF;

					LEDGreen_ON;
					State = Landed;
					break;
				case Landed:
					if ( (DesiredThrottle >= IdleThrottle) && (IntegralCount == 0) )
					{
						AbsDirection = COMPASS_INVAL;						
						LEDCycles = 1;
						State = Flying;
					}
					break;
				case Flying:
					DoNavigation();

					LEDGame();
					LowGasCycles = LOWGASDELAY;
					if ( DesiredThrottle < IdleThrottle )
					{
						DesiredThrottle = IdleThrottle;
						State = Landing;
					}
					break;
				case Landing:
					if ( DesiredThrottle >= IdleThrottle )
						State = Flying;
					else
						if ( --LowGasCycles > 0 )
							DesiredThrottle = IdleThrottle;
						else
						{
							State = Starting;
							Temp = (HoverThrottle * 100)/_Maximum;
							WriteEE(_EESet1 + (&PercentHoverThr - &FirstProgReg), Temp);
						}
					break;
				} // Switch State
				_LostModel = false;
				DropoutCycles = MAXDROPOUT;
			}
			else
				DoFailsafe();

			WriteTimer0(0);
			INTCONbits.TMR0IF = false;
			INTCONbits.TMR0IE = true;

				RollRate = PitchRate = 0;
				GetGyroValues();
				GetDirection();
				ComputeBaroComp();
	
				while( TimeSlot > 0 ) {}
			INTCONbits.TMR0IE = false;	// disable timer

			TimeSlot = Limit(TimeSlots, 2, 20);

			GetGyroValues();

			DoControl();
			MixAndLimitMotors();
			MixAndLimitCam();
			OutSignals();

			CheckAlarms();
			DumpTrace();
		
		} // flight while armed
	
		Beeper_OFF;
	}
} // main

