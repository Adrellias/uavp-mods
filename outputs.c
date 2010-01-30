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

void DoMix(int16);
void CheckDemand(int16);
void MixAndLimitMotors(void);
void MixAndLimitCam(void);
void OutSignals(void);
void InitI2CESCs(void);

// Constants

#define MAGICNUMBER 	32	//84

// For K1-K4 Connectors
#define	PulseFront		0
#define	PulseLeft		1
#define	PulseRight		2
#define	PulseBack		3

#define	PulseCamRoll	4
#define	PulseCamPitch	5

#define ALL_PULSE_ON	(PORTB |= 0b00001111)
#define ALL_OUTPUTS_OFF	(PORTB &= 0b11110000)
#define ALL_OUTPUTS		(PORTB & 0b00001111)

boolean OutToggle;

void DoMix(int16 CurrThrottle)
{
	static int16 Temp;

	Motor[Front] = Motor[Left] = Motor[Right] = CurrThrottle;
	#ifndef TRICOPTER
	Motor[Back] = CurrThrottle;
	#endif // !TRICOPTER

	#ifndef TRICOPTER
	if( F.UsingXMode )
	{	// "Cross" Mode
		Motor[Left] +=   Pl - Rl - Yl;
		Motor[Right] += -Pl + Rl - Yl;
		Motor[Front] += -Pl - Rl + Yl;
		Motor[Back] +=   Pl + Rl + Yl; 
	}
	else
	{	// Normal "Plus" Mode
		Motor[Left]  += -Rl - Yl;	
		Motor[Right] +=  Rl - Yl;
		Motor[Front] += -Pl + Yl;
		Motor[Back]  +=  Pl + Yl;
	}

	#else	// TRICOPTER
	Temp = SRS16(Rl - Pl, 1); 
	Motor[Front] += Pl ;				// front motor
	Motor[Left]  += Temp;				// rear left
	Motor[Right] -= Temp; 				// rear right
	Motor[Back]   = Yl + RC_NEUTRAL;	// yaw servo
	#endif

} // DoMix

boolean 	MotorDemandRescale;

void CheckDemand(int16 CurrThrottle)
{
	static int8 s;
	static int24 Scale, ScaleHigh, ScaleLow, MaxMotor, DemandSwing;

	MaxMotor = Max(Motor[Front], Motor[Left]);
	MaxMotor = Max(MaxMotor, Motor[Right]);
	#ifndef TRICOPTER
	MaxMotor = Max(MaxMotor, Motor[Back]);
	#endif // TRICOPTER

	DemandSwing = MaxMotor - CurrThrottle;

	if ( DemandSwing > 0 )
	{		
		ScaleHigh = (( OUT_MAXIMUM - (int24)CurrThrottle) * 256 )/ DemandSwing;	 
		ScaleLow = (( (int24)CurrThrottle - IdleThrottle) * 256 )/ DemandSwing;
		Scale = Min(ScaleHigh, ScaleLow);
		if ( Scale < 256 )
		{
			MotorDemandRescale = true;
			Rl = (Rl * Scale)/256;  
			Pl = (Pl * Scale)/256; 
			Yl = (Yl * Scale)/256; 
		}
		else
			MotorDemandRescale = false;	
	}
	else
		MotorDemandRescale = false;	

} // CheckDemand

void MixAndLimitMotors(void)
{ 	// expensive ~400uSec @16MHz
    static int16 Temp, CurrThrottle;

	if ( DesiredThrottle < IdleThrottle )
		CurrThrottle = 0;
	else
		CurrThrottle = DesiredThrottle + (DUComp + BaroComp); // vertical compensation not optional
	
	Temp = (int16)(OUT_MAXIMUM * 90 + 50) / 100; // 10% headroom for control
	CurrThrottle = Limit(CurrThrottle, 0, Temp ); 
	
	if ( CurrThrottle > IdleThrottle )
	{
		DoMix(CurrThrottle);
	
		CheckDemand(CurrThrottle);
	
		if ( MotorDemandRescale )
			DoMix(CurrThrottle);
	}
	else
		Motor[Front] = Motor[Back] = 
		Motor[Left] = Motor[Right] = CurrThrottle;

	Motor[Front] = Limit(Motor[Front], ESCMin, ESCMax);
	Motor[Back] = Limit(Motor[Back], ESCMin, ESCMax);
	Motor[Left] = Limit(Motor[Left], ESCMin, ESCMax);
	Motor[Right] = Limit(Motor[Right], ESCMin, ESCMax);

} // MixAndLimitMotors

void MixAndLimitCam(void)
{
	static int16 Cr, Cp;

	// use only roll/pitch angle estimates
	if( P[CamPitchKp] != 0 )
		Cp = (PitchSum * 4L) / (int16)P[CamPitchKp];
	else
		Cp = OUT_NEUTRAL;
	Cp += DesiredCamPitchTrim; 			

	if( P[CamRollKp] != 0 )
		Cr = (RollSum * 4L) / (int16)P[CamRollKp];
	else
		Cr = OUT_NEUTRAL;
	Cr += P[CamRollTrim];

	MCamRoll = Limit(Cr, 1, OUT_MAXIMUM);
	MCamPitch = Limit(Cp, 1, OUT_MAXIMUM);

} // MixAndLimitCam

void OutSignals(void)
{
	static uint8 m;
	static uint8 r;

	#ifdef DEBUG_SENSORS

	Trace[TDesiredThrottle] = DesiredThrottle;

	Trace[TDesiredRoll] = DesiredRoll;
	Trace[TDesiredPitch] = DesiredPitch;
	Trace[TDesiredYaw] = DesiredYaw;

	Trace[TMFront] = Motor[Front];
	Trace[TMBack] = Motor[Back];
	Trace[TMLeft] = Motor[Left];
	Trace[TMRight] = Motor[Right];

	Trace[TMCamRoll] = MCamRoll;
	Trace[TMCamPitch] = MCamPitch;

	#else // !DEBUG_SENSORS

	if ( !F.MotorsArmed )
	{
		Motor[Front] = Motor[Back] = 
		Motor[Left] = Motor[Right] = ESCMin;
		MCamPitch = MCamRoll = OUT_NEUTRAL;
	}

	WriteTimer0(0);
	INTCONbits.TMR0IF = false;

	MT = MCamRoll;
	ME = MCamPitch;

	if ( P[ESCType] == ESCPPM )
	{
		_asm
		MOVLB	0						// select Bank0
		MOVLW	0x0f					// turn on motors
		MOVWF	SHADOWB,1
		_endasm	
		PORTB |= 0x0f;
	
		MF = Motor[Front];
		MB = Motor[Back];
		ML = Motor[Left];
		MR = Motor[Right];

		// simply wait for nearly 1 ms
		// irq service time is max 256 cycles = 64us = 16 TMR0 ticks
		while( ReadTimer0() < (uint16)(0x100-3-MAGICNUMBER) ) ; // 16
		
		// now stop CCP1 interrupt
		// capture can survive 1ms without service!
		
		// Strictly only if the masked interrupt region below is
		// less than the minimum valid Rx pulse width which
		// is 1027uS less capture time overheads
		
		DisableInterrupts;	// BLOCK ALL INTERRUPTS for NO MORE than 1mS
		while( !INTCONbits.TMR0IF ) ;	// wait for first overflow
		INTCONbits.TMR0IF=0;			// quit TMR0 interrupt
		
		if( OutToggle )	// driver cam servos only every 2nd pulse
		{
			_asm
			MOVLB	0					// select Bank0
			MOVLW	0x3f				// turn on motors
			MOVWF	SHADOWB,1
			_endasm	
			PORTB |= 0x3f;
		}
		OutToggle ^= 1;
		
		// This loop is exactly 16 cycles 
		// under no circumstances should the loop cycle time be changed
			_asm
			MOVLB	0						// select Bank0
OS005:
			MOVF	SHADOWB,0,1				// Cannot read PORTB ???
			MOVWF	PORTB,0
			ANDLW	0x0f
			BZ		OS006
					
			DECFSZ	MF,1,1					// front motor
			GOTO	OS007
					
			BCF		SHADOWB,PulseFront,1	// stop Front pulse
OS007:
			DECFSZ	ML,1,1					// left motor
			GOTO	OS008
					
			BCF		SHADOWB,PulseLeft,1		// stop Left pulse
OS008:
			DECFSZ	MR,1,1					// right motor
			GOTO	OS009
					
			BCF		SHADOWB,PulseRight,1	// stop Right pulse
OS009:
			DECFSZ	MB,1,1					// rear motor
			GOTO	OS005
						
			BCF		SHADOWB,PulseBack,1		// stop Back pulse			
		
			GOTO	OS005
OS006:
			_endasm
			// This will be the corresponding C code:
			//	while( ALL_OUTPUTS != 0 )
			//	{	// remain in loop as int16 as any output is still high
			//		if( TMR2 = MFront  ) PulseFront  = 0;
			//		if( TMR2 = MBack ) PulseBack = 0;
			//		if( TMR2 = MLeft  ) PulseLeft  = 0;
			//		if( TMR2 = MRight ) PulseRight = 0;
			//	}
		
		mS[Clock]++;
		EnableInterrupts;
	
	} 
	else
	{
		if( OutToggle )	// driver cam servos only every 2nd pulse
		{
			_asm
			MOVLB	0					// select Bank0
			MOVLW	0x3f				// turn on motors
			MOVWF	SHADOWB,1
			_endasm	
			PORTB |= 0x3f;
		}
		OutToggle ^= 1;
		
		// in X3D- and Holger-Mode, K2 (left motor) is SDA, K3 (right) is SCL
		// Ack (r) not checked as no recovery is possible
		// All motors driven with fourth motor ignored for Tricopter	
		if ( P[ESCType] ==  ESCHolger )
			for ( m = 0 ; m < NoOfMotors ; m++ )
			{
				ESCI2CStart();
				r = SendESCI2CByte(0x52 + ( m*2 ));		// one cmd, one data byte per motor
				r = SendESCI2CByte( Motor[m] );
				ESCI2CStop();
			}
		else
			if ( P[ESCType] == ESCYGEI2C )
				for ( m = 0 ; m < NoOfMotors ; m++ )
				{
					ESCI2CStart();
					r = SendESCI2CByte(0x62 + ( m*2) );	// one cmd, one data byte per motor
					r = SendESCI2CByte( Motor[m]>>1 );
					ESCI2CStop();
				}
			else
				if ( P[ESCType] == ESCX3D )
				{
					ESCI2CStart();
					r = SendESCI2CByte(0x10);			// one command, 4 data bytes
					r = SendESCI2CByte( Motor[Front] ); 
					r = SendESCI2CByte( Motor[Back] );
					r = SendESCI2CByte( Motor[Left] );
					r = SendESCI2CByte( Motor[Right] );
					ESCI2CStop();
				}
	}

	while( ReadTimer0() < (uint16)(0x100-3-MAGICNUMBER) ) ; 	// wait for 2nd TMR0 near overflow

	DisableInterrupts;				
	while( !INTCONbits.TMR0IF ) ;		// wait for 2nd overflow (2 ms)

	// This loop is exactly 16 cycles 
	// under no circumstances should the loop cycle time be changed
_asm
	MOVLB	0
OS001:
	MOVF	SHADOWB,0,1				// Cannot read PORTB ???
	MOVWF	PORTB,0
	ANDLW	0x30		// output ports 4 and 5
	BZ		OS002		// stop if all 2 outputs are 0

	DECFSZ	MT,1,1
	GOTO	OS003

	BCF		SHADOWB,PulseCamRoll,1
OS003:
	DECFSZ	ME,1,1
	GOTO	OS004

	BCF		SHADOWB,PulseCamPitch,1
OS004:
_endasm
	Delay1TCY();
	Delay1TCY();
	Delay1TCY();
	Delay1TCY();
_asm
	GOTO	OS001
OS002:
_endasm

	mS[Clock]++;
	EnableInterrupts;

#endif  // DEBUG_SENSORS

} // OutSignals

void InitI2CESCs(void)
{
	static uint8 m;
	static boolean r;

	if ( P[ESCType] ==  ESCHolger )
		for ( m = 0 ; m < NoOfMotors ; m++ )
		{
			ESCI2CStart();
			r = SendESCI2CByte(0x52 + ( m*2 ));		// one cmd, one data byte per motor
			r |= SendESCI2CByte(0);
			ESCI2CFail[m] |= r;  
			ESCI2CStop();
		}
	else
		if ( P[ESCType] == ESCYGEI2C )
			for ( m = 0 ; m < NoOfMotors ; m++ )
			{
				ESCI2CStart();
				r = SendESCI2CByte(0x62 + ( m*2 ));	// one cmd, one data byte per motor
				r |= SendESCI2CByte(0);
				ESCI2CFail[m] |= r; 
				ESCI2CStop();
			}
		else
			if ( P[ESCType] == ESCX3D )
			{
				ESCI2CStart();
				r = SendESCI2CByte(0x10);			// one command, 4 data bytes
				r |= SendESCI2CByte(0); 
				r |= SendESCI2CByte(0);
				r |= SendESCI2CByte(0);
				r |= SendESCI2CByte(0);
				ESCI2CFail[0] |= r;
				ESCI2CStop();
			}
} // InitI2CESCs
