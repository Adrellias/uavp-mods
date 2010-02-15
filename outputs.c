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
int16 Motor[NoOfMotors];
boolean ESCI2CFail[NoOfMotors];

#pragma udata access outputvars
near uint8 SHADOWB, MF, MB, ML, MR, MT, ME;
near uint8 ESCMin, ESCMax;
near boolean ServoToggle;
#pragma udata

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
	Cr = SRS32((int24)RollSum * P[CamRollKp], 8);
	Cr += (int16)P[CamRollTrim] + OUT_NEUTRAL;

	Cp = SRS32((int24)PitchSum * P[CamPitchKp], 8);
	Cp += DesiredCamPitchTrim + OUT_NEUTRAL; 			

	MT = Limit(Cr, 1, OUT_MAXIMUM);
	ME = Limit(Cp, 1, OUT_MAXIMUM);

} // MixAndLimitCam

void OutSignals(void)
{	// The PWM pulses are in two parts these being a 1mS preamble followed by a 0-1mS part. 
	// Interrupts are enabled during the first part which uses TMR0.  TMR0 is monitored until 
	// there is just sufficient time for one remaining interrupt latency before disabling 
	// interrupts.  We do this because there appears to be no atomic method of detecting the 
	// remaining time AND conditionally disabling the interupt. 
	static uint8 m;
	static uint8 r;
	static i16u SaveTimer0;
	static uint24 SaveClockmS;

	#ifdef DEBUG_SENSORS

	Trace[TDesiredThrottle] = DesiredThrottle;

	Trace[TDesiredRoll] = DesiredRoll;
	Trace[TDesiredPitch] = DesiredPitch;
	Trace[TDesiredYaw] = DesiredYaw;

	Trace[TMFront] = Motor[Front];
	Trace[TMBack] = Motor[Back];
	Trace[TMLeft] = Motor[Left];
	Trace[TMRight] = Motor[Right];

	Trace[TMCamRoll] = MT;
	Trace[TMCamPitch] = ME;

	#else // !DEBUG_SENSORS

	if ( !F.MotorsArmed )
	{
		Motor[Front] = Motor[Back] = 
		Motor[Left] = Motor[Right] = ESCMin;
		MT = ME = OUT_NEUTRAL;
	}

	// Save TMR0 and reset
	DisableInterrupts;
	SaveClockmS = mS[Clock];
	GetTimer0;
	SaveTimer0.u16 = Timer0.u16;
	FastWriteTimer0(TMR0_1MS);
	INTCONbits.TMR0IF = false;
	EnableInterrupts;

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

		SyncToTimer0AndDisableInterrupts();

		if( ServoToggle )	// driver cam servos only every 2nd pulse
		{
			_asm
			MOVLB	0						// select Bank0
			MOVLW	0x3f					// turn on all motors
			MOVWF	SHADOWB,1
			_endasm	
			PORTB |= 0x3f;
		}
		else
		{
			Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY();
		}

		_asm
		MOVLB	0							// select Bank0
OS005:
		MOVF	SHADOWB,0,1	
		MOVWF	PORTB,0
		ANDLW	0x0f
		BZ		OS006
			
		DECFSZ	MF,1,1				
		GOTO	OS007
					
		BCF		SHADOWB,PulseFront,1
OS007:
		DECFSZ	ML,1,1		
		GOTO	OS008
					
		BCF		SHADOWB,PulseLeft,1
OS008:
		DECFSZ	MR,1,1
		GOTO	OS009
					
		BCF		SHADOWB,PulseRight,1
OS009:
		DECFSZ	MB,1,1	
		GOTO	OS010
						
		BCF		SHADOWB,PulseBack,1			

OS010:
		_endasm
		#ifdef CLOCK_40MHZ
		Delay10TCYx(2); 
		Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY(); 
		Delay1TCY(); Delay1TCY(); 
		#endif // CLOCK_40MHZ
		_asm				
		GOTO	OS005
OS006:
		_endasm
		
		EnableInterrupts;
		SyncToTimer0AndDisableInterrupts();	
	} 
	else
	{ // I2C ESCs
		if( ServoToggle )	// driver cam servos only every 2nd pulse
		{
			_asm
			MOVLB	0					// select Bank0
			MOVLW	0x30				// turn on motors
			MOVWF	SHADOWB,1
			_endasm	
			PORTB |= 0x30;
		}
		else
		{
			Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY();
		}
		
		// in X3D and Holger-Mode, K2 (left motor) is SDA, K3 (right) is SCL.
		// ACK (r) not checked as no recovery is possible.
		// All motors driven with fourth motor ignored for Tricopter.	
		if ( P[ESCType] ==  ESCHolger )
			for ( m = 0 ; m < NoOfMotors ; m++ )
			{
				ESCI2CStart();
				r = SendESCI2CByte(0x52 + ( m*2 ));		// one command, one data byte per motor
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

	if ( ServoToggle )
	{	
		_asm
		MOVLB	0
OS001:
		MOVF	SHADOWB,0,1	
		MOVWF	PORTB,0
		ANDLW	0x30		
		BZ		OS002	
	
		DECFSZ	MT,1,1
		GOTO	OS003
	
		BCF		SHADOWB,PulseCamRoll,1
OS003:
		DECFSZ	ME,1,1
		GOTO	OS004
	
		BCF		SHADOWB,PulseCamPitch,1
OS004:
		_endasm
	
		Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY(); 
		Delay1TCY();
	
		#ifdef CLOCK_40MHZ
		Delay10TCYx(2); 
		Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY(); Delay1TCY(); 
		Delay1TCY(); Delay1TCY(); 
		#endif // CLOCK_40MHZ
	
		_asm
	
		GOTO	OS001
OS002:
		_endasm

		EnableInterrupts;
		SyncToTimer0AndDisableInterrupts();
	}

	FastWriteTimer0(SaveTimer0.u16);
	// the 1mS clock seems to get in for 40MHz but not 16MHz so like this for now?
	if ( P[ESCType] == ESCPPM )
		if ( ServoToggle )
			Clock[mS] = SaveClockmS + 3;
		else
			Clock[mS] = SaveClockmS + 2;
	else
		if ( ServoToggle )
			Clock[mS] = SaveClockmS + 2;
		else
			Clock[mS] = SaveClockmS;
	EnableInterrupts;

	ServoToggle ^= true;

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
