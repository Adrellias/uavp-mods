// =======================================================================
// =                                 UAVX                                =
// =                         Quadrocopter Control                        =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =     Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer       =
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

// Motor control routine
// WARNING THESE ROUTINES USE CLOCK AND PIC SPECIFIC DEAD TIMING

#include "UAVX.h"

// Prototypes
void OutSignals(void);
void EscI2CDelay(void);
void EscI2CStart(void);
void EscI2CStop(void);
void EscWaitClkHi(void);
void SendEscI2CByte(uint8);
void DoPWMFrame(void);
void CheckFrameOverrun(void);
void MixAndLimitCam(void);
void MixAndLimitMotors(void);

// Defines

#define	PulseFront		0
#define	PulseLeft		1
#define	PulseRight		2
#define	PulseBack		3

#define	PulseCamRoll	4
#define	PulseCamPitch	5

#ifdef ESC_PWM 	// bits 0 to 5 of PORTB
	#define PWM_MASK 0x0f
	#define PWM_MASK_CAM 0x3f
#else			// bits 4 to 5 of PORTB
	#define PWM_MASK 0x00
	#define PWM_MASK_CAM 0x30
#endif // ESC_PWM

#if defined ESC_X3D || defined ESC_HOLGER || defined ESC_YGEI2C
// Wolfgang's SW I2C ESC
#define	 ESC_SDA		PORTBbits.RB1
#define	 ESC_SCL		PORTBbits.RB2
#define	 ESC_DIO		TRISBbits.TRISB1
#define	 ESC_CIO		TRISBbits.TRISB2
#endif

enum MotorTags {Front, Left, Right, Back};
#define NoOfMotors 4

#pragma udata assembly_language=0x080 
uint8 SHADOWB, MF, MB, ML, MR, MT, ME; // motor/servo outputs
#pragma udata

#pragma udata outputvars
// PWM output
uint8 PWMPostPulse;  				// Set if performing the initial 1mS of a PWM pulse
uint8 volatile EmitPWMFrame;		// Enable the output of a single PWM frame
int16	DesiredCamRoll, DesiredCamPitch;
int16	Motor[NoOfMotors];
uint8	MCamPitch, MCamRoll;
#pragma udata

void DoMix(int16 CurrThrottle)
{
	int16 Temp;

	Motor[Front] = Motor[Left] = Motor[Right] = CurrThrottle;
	#ifndef TRICOPTER
	Motor[Back] = CurrThrottle;
	#endif // !TRICOPTER

	#ifndef TRICOPTER
	if( FlyCrossMode )
	{	// "Cross" Mode
		Motor[Left] +=   Pl - Rl - Yl;
		Motor[Right] += -Pl + Rl - Yl;
		Motor[Front] += -Pl - Rl + Yl;
		Motor[Back] +=   Pl + Rl + Yl; //*
	}
	else
	{	// "Plus" Mode
		#ifdef MOUNT_45
		Motor[Left]  += -Rl - Pl - Yl; //*	
		Motor[Right] +=  Rl + Pl - Yl;
		Motor[Front] +=  Rl - Pl + Yl;
		Motor[Back]  += -Rl + Pl + Yl;	
		#else
		Motor[Left]  += -Rl - Yl;	
		Motor[Right] +=  Rl - Yl;
		Motor[Front] += -Pl + Yl;
		Motor[Back]  +=  Pl + Yl;
		#endif
	}

	#else	// TRICOPTER
	Temp = SRS16(Rl - Pl, 1); 
	Motor[Front] += Pl ;			// front motor
	Motor[Left]  += Temp;			// rear left
	Motor[Right] -= Temp; 			// rear right
	Motor[Back]   = Yl + _Neutral;	// yaw servo
	#endif

} // DoMix

uint8 	MotorDemandRescale;

void CheckDemand(int16 CurrThrottle)
{
	int8 s;
	int24 Scale, ScaleHigh, ScaleLow, MaxMotor, DemandSwing;

	MaxMotor = Max(Motor[Front], Motor[Left]);
	MaxMotor = Max(MaxMotor, Motor[Right]);
	#ifndef TRICOPTER
	MaxMotor = Max(MaxMotor, Motor[Back]);
	#endif // TRICOPTER

	DemandSwing = MaxMotor - CurrThrottle;

	if ( DemandSwing > 0 )
	{		
		ScaleHigh = (( OUT_MAXIMUM - (int24)CurrThrottle) * 256 )/ DemandSwing;	 
		ScaleLow = (( (int24)CurrThrottle - MotorLowRun) * 256 )/ DemandSwing;
		Scale = Min(ScaleHigh, ScaleLow);
		if ( Scale < 256 )
		{
			MotorDemandRescale = true;
			Rl = (Rl * Scale + 128)/256;  
			Pl = (Pl * Scale + 128)/256; 
			Yl = (Yl * Scale + 128)/256; 
		}
		else
			 MotorDemandRescale = false;	
	}
	else
		MotorDemandRescale = false;	

} // CheckDemand

void MixAndLimitMotors(void)
{ 	// expensive ~400uSec @16MHz
    int16 Temp, CurrThrottle;

	// Altitude stabilization factor
	CurrThrottle = DesiredThrottle; //+ (Vud + VBaroSum); // vertical compensation not optional
	CurrThrottle = Limit(CurrThrottle, 0, (int16)(OUT_MAXIMUM * 90 + 50) / 100); // 10% headroom for control

	DoMix(CurrThrottle);

	CheckDemand(CurrThrottle);

	if ( MotorDemandRescale )
		DoMix(CurrThrottle);

} // MixAndLimit

void MixAndLimitCam(void)
{
	int24 Rp, Pp;

	if ((CamKi != 0) & _MotorsEnabled )
	{
		Rp = RollAngle/CamKi;
		Pp = PitchAngle/CamKi;
	}
	else
		Rp = Pp = 0;

	if( _UseCh7Trigger )
		Rp += RC_NEUTRAL;
	else
		Rp += DesiredCamRoll;
		
	Pp += DesiredCamPitch;

	MCamRoll = Limit(Rp, OUT_MINIMUM , OUT_MAXIMUM);
	MCamPitch = Limit(Pp, OUT_MINIMUM , OUT_MAXIMUM);

} // MixAndLimitCam

void DoPWMFrame(void)
{
	if ( EmitPWMFrame )
		// embedding camera control every cycle and keeping total PWM
		// interval less than 2mS reduces precision of output PWM to
		// RC_MAXIMUM-OUT_MAXIMUM
		if ( PWMPostPulse ) 
		{	// must be less than minimum RC pulse capture inter-arrival
			// locks out all other interrupts for < 1mS including GPS
			_asm
			MOVLB	0						// select Bank0
OS005:					
			MOVF	SHADOWB,0,1				// edges lock step
			MOVWF	PORTB,0
			ANDLW	PWM_MASK
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
			DECFSZ	MT,1,1				
			GOTO	OS011
			
			BCF		SHADOWB,PulseCamRoll,1	
OS011:
			DECFSZ	ME,1,1			
			GOTO	OS005
			
			BCF		SHADOWB,PulseCamPitch,1
			GOTO	OS005			
OS006:				
			_endasm

			EmitPWMFrame = PWMPostPulse = false;
		}
		else
		{
			PWMPostPulse = true; 
			if ( 1 ) // camera servo update
				SHADOWB = PWM_MASK_CAM;
			else		
				SHADOWB = PWM_MASK;
			PORTB |= SHADOWB;
			// still possible for edge interrupts to squeeze in but 
			// not if running synchronously with RC packets arriving!
			// Only other interrupt is from GPS.
		}
} // DoPWMFrame

void CheckFrameOverrun(void)
{
	if ( EmitPWMFrame )
	{
		// CATASTROPHE - Impulse Period or RC_FRAME_TIMEOUT too short for 
		// whatever reason causing overrun of PWM output frames.

		_MotorsEnabled = false;
		MF = ML = MR = MB = OUT_MINIMUM;
		MT = ME = OUT_NEUTRAL;
		ALL_LEDS_ON;
		mS[UpdateTimeout] = mS[Clock];

		while ( 1 )
			if ( mS[Clock] >= mS[UpdateTimeout] )
			{
				mS[UpdateTimeout] += RC_FRAME_TIMEOUT;
				Beeper_TOG;
				OutSignals();
			}	
	}
} // CheckFrameOverrun

void OutSignals(void)
{
	// for positive PID coeffs MF/MB anticlockwise, ML/MR clockwise

	MixAndLimitMotors();

	MF = Motor[Front];
	ML = Motor[Left];
	MR = Motor[Right];
	MB = Motor[Back];

	MixAndLimitCam();

	MT = MCamRoll;
	ME = MCamPitch;
		
#ifndef DEBUG

	EmitPWMFrame = true;

	#if defined ESC_X3D || defined ESC_HOLGER || defined ESC_YGEI2C
	
	// in X3D- and Holger-Mode, K2 (left motor) is SDA, K3 (right) is SCL
	#ifdef ESC_X3D
	EscI2CStart();
	SendEscI2CByte(0x10);					// one command, 4 data bytes
	SendEscI2CByte(MF); 					// for all motors
	SendEscI2CByte(MB);
	SendEscI2CByte(ML);
	SendEscI2CByte(MR);
	EscI2CStop();
	#endif	// ESC_X3D

	#ifdef ESC_HOLGER
	EscI2CStart();
	SendEscI2CByte(0x52);					// one cmd, one data byte per motor
	SendEscI2CByte(MF); 					// for all motors
	EscI2CStop();

	EscI2CStart();
	SendEscI2CByte(0x54);
	SendEscI2CByte(MB);
	EscI2CStop();

	EscI2CStart();
	SendEscI2CByte(0x58);
	SendEscI2CByte(ML);
	EscI2CStop();

	EscI2CStart();
	SendEscI2CByte(0x56);
	SendEscI2CByte(MR);
	EscI2CStop();
	#endif	// ESC_HOLGER

	#ifdef ESC_YGEI2C
	EscI2CStart();
	SendEscI2CByte(0x62);					// one cmd, one data byte per motor
	SendEscI2CByte(MF>>1); 					// for all motors
	EscI2CStop();

	EscI2CStart();
	SendEscI2CByte(0x64);
	SendEscI2CByte(MB>>1);
	EscI2CStop();

	EscI2CStart();
	SendEscI2CByte(0x68);
	SendEscI2CByte(ML>>1);
	EscI2CStop();

	EscI2CStart();
	SendEscI2CByte(0x66);
	SendEscI2CByte(MR>>1);
	EscI2CStop();
	#endif	// ESC_YGEI2C

	#endif	// ESC_X3D or ESC_HOLGER or ESC_YGEI2C

#endif	// !DEBUG

} // OutSignals

void InitOutputs(void)
{
	int8 i;

	_MotorsEnabled = false;
	for ( i = 0; i<NoOfMotors; i++ )
		Motor[i] = OUT_MINIMUM;
	MCamPitch = MCamRoll = OUT_NEUTRAL;

	EmitPWMFrame = false;
	PWMPostPulse = false;

} // InitOutputs

