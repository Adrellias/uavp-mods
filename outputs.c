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

void DoMulticopterMix(int16);
void CheckDemand(int16);
void MixAndLimitMotors(void);
void MixAndLimitCam(void);
void OutSignals(void);
void InitI2CESCs(void);

boolean OutToggle;
int16 PWM[6];
int16 PWMSense[6];
int16 ESCI2CFail[4];
int16 CurrThrottle;

#pragma udata access outputvars
near uint8 SHADOWB, PWM0, PWM1, PWM2, PWM3, PWM4, PWM5;
near boolean ServoToggle;
#pragma udata

int16 ESCMin, ESCMax;

#ifdef MULTICOPTER

void DoMulticopterMix(int16 CurrThrottle)
{
	static int16 Temp;

	#ifdef TRICOPTER
		PWM[FrontC] = PWM[LeftC] = PWM[RightC] = CurrThrottle;
		if ( F.UsingAltOrientation ) // K1 forward
		{
		//	Temp = (Pl * 50)/115; // compensate for 30deg angle of rear arms
			Temp = SRS16(Pl * 56, 7);
			PWM[K1] -= Pl ;				// front motor
			PWM[K2] += (Temp - Rl);		// right rear
			PWM[K3] += (Temp + Rl); 	// left rear
			PWM[K4] = Yl + RC_NEUTRAL;	// yaw servo
		}
		else // Y K1 rearwards
		{
		//	Temp = -(Pl * 50)/115;
			Temp = SRS16(Pl * -56, 7);  
			PWM[K1] += Pl ;				// rear motor
			PWM[K2] += (Temp + Rl);		// left front
			PWM[K3] += (Temp - Rl); 	// right front
			PWM[K4] = Yl + RC_NEUTRAL;	// yaw servo
		}	
	#else
	    #ifdef HEXACOPTER

			#error NO HEXACOPTER YET!

		#else // QUADROCOPTER
			PWM[FrontC] = PWM[LeftC] = PWM[RightC] = CurrThrottle;
			PWM[BackC] = CurrThrottle;
			if( F.UsingAltOrientation )
			{	// "Cross" Mode
				PWM[LeftC]  +=  Pl - Rl - Yl;
				PWM[RightC] += -Pl + Rl - Yl;
				PWM[FrontC] += -Pl - Rl + Yl;
				PWM[BackC]  +=  Pl + Rl + Yl; 
			}
			else
			{	// Normal "Plus" Mode
				PWM[LeftC]  += -Rl - Yl;	
				PWM[RightC] +=  Rl - Yl;
				PWM[FrontC] += -Pl + Yl;
				PWM[BackC]  +=  Pl + Yl;
			}
		#endif
	#endif

} // DoMulticopterMix

boolean 	MotorDemandRescale;

void CheckDemand(int16 CurrThrottle)
{
	static int24 Scale, ScaleHigh, ScaleLow, MaxMotor, DemandSwing;

	MaxMotor = Max(PWM[FrontC], PWM[LeftC]);
	MaxMotor = Max(MaxMotor, PWM[RightC]);
	#ifndef TRICOPTER
	MaxMotor = Max(MaxMotor, PWM[BackC]);
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
			Rl = (Rl * Scale)/256;  // could get rid of the divides
			Pl = (Pl * Scale)/256; 
			Yl = (Yl * Scale)/256; 
		}
		else
			MotorDemandRescale = false;	
	}
	else
		MotorDemandRescale = false;	

} // CheckDemand

#endif // MULTICOPTER

void MixAndLimitMotors(void)
{ 	// expensive ~160uSec @ 40MHz
    static int16 Temp, TempElevon, TempElevator;

	if ( DesiredThrottle < IdleThrottle )
		CurrThrottle = 0;
	else
		CurrThrottle = DesiredThrottle;

	#ifdef MULTICOPTER
		if ( State == InFlight )
			 CurrThrottle += (DUComp + AltComp); // vertical compensation not optional
			
		Temp = (int16)(OUT_MAXIMUM * 90 + 50) / 100; // 10% headroom for control
		CurrThrottle = Limit(CurrThrottle, 0, Temp ); 
			
		if ( CurrThrottle > IdleThrottle )
		{
			DoMulticopterMix(CurrThrottle);
			
			CheckDemand(CurrThrottle);
			
			if ( MotorDemandRescale )
				DoMulticopterMix(CurrThrottle);
		}
		else
			PWM[FrontC] = PWM[BackC] = PWM[LeftC] = PWM[RightC] = CurrThrottle;
	#else
		CurrThrottle += AltComp; // simple - faster to climb with no elevator yet
		
		PWM[ThrottleC] = CurrThrottle;
		PWM[RudderC] = PWMSense[RudderC] * Yl + OUT_NEUTRAL;
		
		#if ( defined AILERON | defined HELICOPTER )
			PWM[AileronC] = PWMSense[AileronC] * Rl + OUT_NEUTRAL;
			PWM[ElevatorC] = PWMSense[ElevatorC] * Pl + OUT_NEUTRAL;
		#else // ELEVON
			TempElevator = PWMSense[2] * Pl + OUT_NEUTRAL;
			PWM[RightElevonC] = PWMSense[RightElevonC] * (TempElevator + Rl);
			PWM[LeftElevonC] = PWMSense[LeftElevonC] * (TempElevator -  Rl);		
		#endif
	#endif
} // MixAndLimitMotors

void MixAndLimitCam(void)
{
	static int16 Cr, Cp;

	// use only roll/pitch angle estimates
	Cr = SRS32((int24)RollSum * P[CamRollKp], 8) + (int16)P[CamRollTrim];
	Cr = PWMSense[CamRollC] * Cr + OUT_NEUTRAL;

	Cp = SRS32((int24)PitchSum * P[CamPitchKp], 8) + DesiredCamPitchTrim;
	Cp = PWMSense[CamPitchC] * Cp + OUT_NEUTRAL; 			

	PWM[CamRollC] = Limit(Cr, 1, OUT_MAXIMUM);
	PWM[CamPitchC] = Limit(Cp, 1, OUT_MAXIMUM);

} // MixAndLimitCam

void OutSignals(void)
{	// The PWM pulses are in two parts these being a 1mS preamble followed by a 0-1mS part. 
	// Interrupts are enabled during the first part which uses TMR0.  TMR0 is monitored until 
	// there is just sufficient time for one remaining interrupt latency before disabling 
	// interrupts.  We do this because there appears to be no atomic method of detecting the 
	// remaining time AND conditionally disabling the interupt. 
	static int8 m;
	static uint8 r;
	static i16u SaveTimer0;
	static uint24 SaveClockmS;

	#if !( defined SIMULATE | defined TESTING )

	for ( m = 0; m < 6; m++ )
		PWM[m] = Limit(PWM[m], ESCMin, ESCMax);

	if ( !F.MotorsArmed )
	{
		#ifdef MULTICOPTER
		PWM[FrontC] = PWM[BackC] = 
		PWM[LeftC] = PWM[RightC] = ESCMin;	
		#else
		PWM[ThrottleC] = ESCMin;
		PWM[1] = PWM[2] = PWM[3] = OUT_NEUTRAL;
		#endif // MULTICOPTER
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
	
		#ifdef MULTICOPTER	
			PWM0 = PWM[FrontC];
			PWM1 = PWM[LeftC];
			PWM2 = PWM[RightC];
			PWM3 = PWM[BackC];
		#else
			#ifdef ELEVON
				PWM0 = PWM[ThrottleC];
				PWM1 = PWM[RightElevonC];
				PWM2 = PWM[LeftElevonC];
				PWM3 = PWM[RudderC];
			#else
				PWM0 = PWM[ThrottleC];
				PWM1 = PWM[AileronC];
				PWM2 = PWM[ElevatorC];
				PWM3 = PWM[RudderC];
			#endif
		#endif // MULTICOPTER

		PWM4 = PWM[CamRollC];
		PWM5 = PWM[CamPitchC];

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
			
		DECFSZ	PWM0,1,1				
		GOTO	OS007
					
		BCF		SHADOWB,0,1
OS007:
		DECFSZ	PWM1,1,1		
		GOTO	OS008
					
		BCF		SHADOWB,1,1
OS008:
		DECFSZ	PWM2,1,1
		GOTO	OS009
					
		BCF		SHADOWB,2,1
OS009:
		DECFSZ	PWM3,1,1	
		GOTO	OS010
						
		BCF		SHADOWB,3,1			

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

		#ifdef MULTICOPTER
		// in X3D and Holger-Mode, K2 (left motor) is SDA, K3 (right) is SCL.
		// ACK (r) not checked as no recovery is possible. 
		// Octocopters may have ESCs paired with common address so ACK is meaningless.
		// All motors driven with fourth motor ignored for Tricopter.	
		if ( P[ESCType] ==  ESCHolger )
			for ( m = 0 ; m < NoOfI2CESCOutputs ; m++ )
			{
				ESCI2CStart();
				r = WriteESCI2CByte(0x52 + ( m*2 ));		// one command, one data byte per motor
				r += WriteESCI2CByte( PWM[m] );
				ESCI2CFail[m] += r;
				ESCI2CStop();
			}
		else
			if ( P[ESCType] == ESCYGEI2C )
				for ( m = 0 ; m < NoOfI2CESCOutputs ; m++ )
				{
					ESCI2CStart();
					r = WriteESCI2CByte(0x62 + ( m*2) );	// one cmd, one data byte per motor
					r += WriteESCI2CByte( PWM[m]>>1 );
					ESCI2CFail[m] += r;
					ESCI2CStop();
				}
			else
				if ( P[ESCType] == ESCX3D )
				{
					ESCI2CStart();
					r = WriteESCI2CByte(0x10);				// one command, 4 data bytes
					r += WriteESCI2CByte( PWM[FrontC] ); 
					r += WriteESCI2CByte( PWM[BackC] );
					r += WriteESCI2CByte( PWM[LeftC] );
					r += WriteESCI2CByte( PWM[RightC] );
					ESCI2CFail[0] += r;
//  other ESCs if a Hexacopter
					ESCI2CStop();
				}
		#endif //  MULTICOPTER
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
	
		DECFSZ	PWM4,1,1
		GOTO	OS003
	
		BCF		SHADOWB,4,1
OS003:
		DECFSZ	PWM5,1,1
		GOTO	OS004
	
		BCF		SHADOWB,5,1
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

	ServoToggle ^= true;

	EnableInterrupts;
	
	#endif // !(SIMULATE | TESTING)

} // OutSignals

void InitI2CESCs(void)
{
	static int8 m;
	static uint8 r;
	
	#ifdef MULTICOPTER

	if ( P[ESCType] ==  ESCHolger )
		for ( m = 0 ; m < NoOfI2CESCOutputs ; m++ )
		{
			ESCI2CStart();
			r = WriteESCI2CByte(0x52 + ( m*2 ));		// one cmd, one data byte per motor
			r += WriteESCI2CByte(0);
			ESCI2CFail[m] += r;  
			ESCI2CStop();
		}
	else
		if ( P[ESCType] == ESCYGEI2C )
			for ( m = 0 ; m < NoOfPWMOutputs ; m++ )
			{
				ESCI2CStart();
				r = WriteESCI2CByte(0x62 + ( m*2 ));	// one cmd, one data byte per motor
				r += WriteESCI2CByte(0);
				ESCI2CFail[m] += r; 
				ESCI2CStop();
			}
		else
			if ( P[ESCType] == ESCX3D )
			{
				ESCI2CStart();
				r = WriteESCI2CByte(0x10);			// one command, 4 data bytes
				r += WriteESCI2CByte(0); 
				r += WriteESCI2CByte(0);
				r += WriteESCI2CByte(0);
				r += WriteESCI2CByte(0);
				ESCI2CFail[0] += r;
				ESCI2CStop();
			}
	#endif // MULTICOPTER
} // InitI2CESCs



