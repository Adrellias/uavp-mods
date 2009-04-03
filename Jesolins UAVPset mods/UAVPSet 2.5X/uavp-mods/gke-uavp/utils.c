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

// Utilities and subroutines

#include "c-ufo.h"
#include "bits.h"

// Math Library
#include "mymath16.h"

#if (defined ESC_X3D || defined ESC_HOLGER || defined ESC_YGEI2C) && !defined DEBUG_SENSORS

void EscI2CDelay(void)
{
	nop2();
	nop2();
	nop2();
}

void EscWaitClkHi(void)
{
	EscI2CDelay();
	ESC_CIO=1;	// set SCL to input, output a high
	while( ESC_SCL == 0 ) ;	// wait for line to come hi
	EscI2CDelay();
}

// send a start condition
void EscI2CStart(void)
{
	ESC_DIO=1;	// set SDA to input, output a high
	EscWaitClkHi();
	ESC_SDA = 0;	// start condition
	ESC_DIO = 0;	// output a low
	EscI2CDelay();
	ESC_SCL = 0;
	ESC_CIO = 0;	// set SCL to output, output a low
}

// send a stop condition
void EscI2CStop(void)
{
	ESC_DIO=0;	// set SDA to output
	ESC_SDA = 0;	// output a low
	EscWaitClkHi();

	ESC_DIO=1;	// set SDA to input, output a high, STOP condition
	EscI2CDelay();		// leave clock high
}


// send a byte to I2C slave and return ACK status
// 0 = ACK
// 1 = NACK
void SendEscI2CByte(uns8 nidata)
{
	//bank2 uns8 nii;

	for(W=8; W!=0; W--)
	{
		if( nidata.7 )
		{
			ESC_DIO = 1;	// switch to input, line is high
		}
		else
		{
			ESC_SDA = 0;			
			ESC_DIO = 0;	// switch to output, line is low
		}
	
		ESC_CIO=1;	// set SCL to input, output a high
		while( ESC_SCL == 0 ) ;	// wait for line to come hi
		EscI2CDelay();
		ESC_SCL = 0;
		ESC_CIO = 0;	// set SCL to output, output a low
		nidata <<= 1;
	}
	ESC_DIO = 1;	// set SDA to input
	EscI2CDelay();
	ESC_CIO=1;	// set SCL to input, output a high
	while( ESC_SCL == 0 ) ;	// wait for line to come hi

	EscI2CDelay();		// here is the ACK
//	nii = I2C_SDA;	

	ESC_SCL = 0;
	ESC_CIO = 0;	// set SCL to output, output a low
	EscI2CDelay();
//	I2C_IO = 0;		// set SDA to output
//	I2C_SDA = 0;	// leave output low
	return;
}


#endif	// ESC_X3D || ESC_HOLGER || ESC_YGEI2C

// Outputs signals to the speed controllers
// using timer 0
// all motor outputs are driven simultaneously
//
// 0x80 gives 1,52ms, 0x60 = 1,40ms, 0xA0 = 1,64ms
//
// This routine needs exactly 3 ms to complete:
//
// Motors:      ___________
//          ___|     |_____|_____________
//
// Camservos:         ___________
//          _________|     |_____|______
//
//             0     1     2     3 ms

void OutSignals(void)
{
	bank0 uns8 MF, MB, ML, MR;	// must reside on bank0
	uns8 MT@MF;	// cam tilt servo
	uns8 ME@MB; // cam tilt servo


	#ifdef NADA
	SendComValH(MCamRoll);
	SendComValH(MCamPitch);
	SendComChar(0x0d);
	SendComChar(0x0a);
	#endif

	#ifndef DEBUG_SENSORS

	#ifdef DEBUG_MOTORS
	if( _Flying && CamPitchFactor.4 )
	{
		SendComValU(IGas);
		SendComChar(';');
		SendComValS(IRoll);
		SendComChar(';');
		SendComValS(IPitch);
		SendComChar(';');
		SendComValS(IYaw);
		SendComChar(';');
		SendComValU(MFront);
		SendComChar(';');
		SendComValU(MBack);
		SendComChar(';');
		SendComValU(MLeft);
		SendComChar(';');
		SendComValU(MRight);
		SendComChar(0x0d);
		SendComChar(0x0a);
	}
	#endif

	TMR0 = 0;
	T0IF = 0;

	#ifdef ESC_PPM
	ALL_PULSE_ON;	// turn on all motor outputs
	#endif

	MF = MFront;
	MB = MBack;
	ML = MLeft;
	MR = MRight;

	#ifdef DEBUG_MOTORS
	// if DEBUG_MOTORS is active, CamIntFactor is a bitmap:
	// bit 0 = no front motor
	// bit 1 = no rear motor
	// bit 2 = no left motor
	// bit 3 = no right motor
	// bit 4 = turns on the serial output

	if( CamPitchFactor.0 )
		MF = _Minimum;
	if( CamPitchFactor.1 )
		MB = _Minimum;
	if( CamPitchFactor.2 )
		ML = _Minimum;
	if( CamPitchFactor.3 )
		MR = _Minimum;
	#else
	#ifdef INTTEST
	MF = _Minimum;
	MB = _Minimum;
	ML = _Minimum;
	MR = _Minimum;
	#endif
	#endif

	#ifdef ESC_PPM

	// simply wait for nearly 1 ms
	// irq service time is max 256 cycles = 64us = 16 TMR0 ticks
	while( TMR0 < 0x100-3-16 ) ;

	// now stop CCP1 interrupt
	// capture can survive 1ms without service!

	// Strictly only if the masked interrupt region below is
	// less than the minimum valid Rx pulse/gap width which
	// is 1027uS less capture time overheads

	GIE = 0;	// BLOCK ALL INTERRUPTS for NO MORE than 1mS
	while( T0IF == 0 ) ;	// wait for first overflow
	T0IF=0;		// quit TMR0 interrupt

	#if !defined DEBUG && !defined DEBUG_MOTORS
	if( _OutToggle )	// driver cam servos only every 2nd pulse
	{
		CAM_PULSE_ON;	// now turn camera servo pulses on too
	}
	_OutToggle ^= 1;
	#endif

// This loop is exactly 16 cycles int16
// under no circumstances should the loop cycle time be changed
#asm
	BCF	RP0		// clear all bank bits
//	BCF	RP1
OS005
	MOVF	PORTB,W
	ANDLW	0x0F		// output ports 0 to 3
	BTFSC	Zero_
	GOTO	OS006		// stop if all 4 outputs are done

	DECFSZ	MF,f		// front motor
	GOTO	OS007

	BCF	PulseFront		// stop pulse
OS007
	DECFSZ	ML,f		// left motor
	GOTO	OS008

	BCF	PulseLeft		// stop pulse
OS008
	DECFSZ	MR,f		// right motor
	GOTO	OS009

	BCF	PulseRight		// stop pulse
OS009
	DECFSZ	MB,f		// rear motor
	GOTO	OS005
	
	BCF	PulseBack		// stop pulse
	GOTO	OS005
OS006
#endasm
// This will be the corresponding C code:
//	while( ALL_OUTPUTS != 0 )
//	{	// remain in loop as int16 as any output is still high
//		if( TMR2 = MFront  ) PulseFront  = 0;
//		if( TMR2 = MBack ) PulseBack = 0;
//		if( TMR2 = MLeft  ) PulseLeft  = 0;
//		if( TMR2 = MRight ) PulseRight = 0;
//	}

	GIE = 1;	// Re-enable interrupt

	#endif	// ESC_PPM

	#if defined ESC_X3D || defined ESC_HOLGER || defined ESC_YGEI2C

	#if !defined DEBUG && !defined DEBUG_MOTORS
	if( _OutToggle )	// driver cam servos only every 2nd pulse
	{
		CAM_PULSE_ON;	// now turn camera servo pulses on too
	}
	_OutToggle ^= 1;
	#endif

	// in X3D- and Holger-Mode, K2 (left motor) is SDA, K3 (right) is SCL
	#ifdef ESC_X3D
	EscI2CStart();
	SendEscI2CByte(0x10);	// one command, 4 data bytes
	SendEscI2CByte(MF); // for all motors
	SendEscI2CByte(MB);
	SendEscI2CByte(ML);
	SendEscI2CByte(MR);
	EscI2CStop();
	#endif	// ESC_X3D

	#ifdef ESC_HOLGER
	EscI2CStart();
	SendEscI2CByte(0x52);	// one cmd, one data byte per motor
	SendEscI2CByte(MF); // for all motors
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
	SendEscI2CByte(0x62);	// one cmd, one data byte per motor
	SendEscI2CByte(MF>>1); // for all motors
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

	#ifndef DEBUG_MOTORS
	while( TMR0 < 0x100-3-16 ) ; // wait for 2nd TMR0 near overflow

	GIE = 0;					// Int wieder sperren, wegen Jitter

	while( T0IF == 0 ) ;	// wait for 2nd overflow (2 ms)

	// avoid servo overrun when MCamxx == 0
	ME = MCamRoll+1;
	MT = MCamPitch+1;

	#if !defined DEBUG && !defined DEBUG_SENSORS
	// This loop is exactly 16 cycles int16
	// under no circumstances should the loop cycle time be changed
#asm
	BCF	RP0		// clear all bank bits
	BCF	RP1
OS001
	MOVF	PORTB,W
	ANDLW	0x30		// output ports 4 and 5
	BTFSC	Zero_
	GOTO	OS002		// stop if all 2 outputs are 0

	DECFSZ	MT,f
	GOTO	OS003

	BCF	PulseCamRoll
OS003
	DECFSZ	ME,f
	GOTO	OS004

	BCF	PulseCamPitch
OS004
#endasm
	nop2();
	nop2();
#asm
	GOTO	OS001
OS002
#endasm
#endif	// DEBUG
	GIE = 1;	// re-enable interrupt

	while( T0IF == 0 ) ;	// wait for 3rd TMR2 overflow
#endif	// DEBUG_MOTORS

#endif  // !DEBUG_SENSORS
} // OutSignals


// convert Roll and Pitch gyro values
// using 10-bit A/D conversion.
// Values are ADDED into RollSamples and PitchSamples
void GetGyroValues(void)
{

	ADFM = 1;					// select 10 bit mode
	#ifdef OPT_ADXRS
	ADCON0 = 0b.10.001.0.0.1;	// select CH1(RA1) Roll
	#endif
	#ifdef OPT_IDG
	PCFG0 = 1;					// select 3,6V as Vref
	ADCON0 = 0b.10.010.0.0.1;	// select CH2(RA2) Pitch
	#endif
	AcqTime();

	RollSamples += ADRESL;
	RollSamples.high8 += ADRESH;

	#ifdef OPT_ADXRS
	ADCON0 = 0b.10.010.0.0.1;	// select CH2(RA2) Pitch
	#endif
	#ifdef OPT_IDG
	ADCON0 = 0b.10.001.0.0.1;	// select CH1(RA1) Roll
	#endif
	AcqTime();

	PitchSamples += ADRESL;
	PitchSamples.high8 += ADRESH;
	#ifdef OPT_IDG
	PCFG0 = 0;					// select 5V as Vref
	#endif
} // GetGyroValues

// ADXRS300: The Integral (RollSum & Pitchsum) has
// a resolution of about 1000 LSBs for a 25° angle
// IDG300: (TBD)
//

// Calc the gyro values from added RollSamples 
// and PitchSamples (global variable "nisampcnt")
void CalcGyroValues(void)
{
	int16 Temp;

	// RollSamples & Pitchsamples hold the sum of 2 consecutive conversions
	// Approximately 4 bits of precision are discarded in this and related 
	// presumably because of the range of the 16 bit arithmetic.

	#ifdef OPT_ADXRS150
	RollSamples +=2;			// for a correct round-up
	PitchSamples +=2;
	(int16)RollSamples >>= 2;	// recreate the 10 bit resolution
	(int16)PitchSamples >>= 2;
	#else
	RollSamples ++;				// for a correct round-up
	PitchSamples ++;
	(int16)RollSamples >>= 1;	// recreate the 10 bit resolution
	(int16)PitchSamples >>= 1;
	#endif
	
	if( IntegralCount > 0 )
	{
		// pre-flight auto-zero mode
		RollSum += RollSamples;
		PitchSum += PitchSamples;

		if( IntegralCount == 1 )
		{
			RollSum += 8;
			PitchSum += 8;
			if( !_UseLISL )
			{
				niltemp = RollSum + MiddleLR;
				RollSum = niltemp;
				niltemp = PitchSum + MiddleFB;
				PitchSum = niltemp;
			}
			MidRoll = (uns16)RollSum / (uns16)16;	
			MidPitch = (uns16)PitchSum / (uns16)16;
			RollSum = 0;
			PitchSum = 0;
			LRIntKorr = 0;
			FBIntKorr = 0;
		}
	}
	else
	{
		// standard flight mode
		RollSamples -= MidRoll;
		PitchSamples -= MidPitch;

		// calc Cross flying mode
		if( FlyCrossMode )
		{
			// Real Roll = 0.707 * (N + R)
			//      Pitch = 0.707 * (N - R)
			// the constant factor 0.667 is used instead
			niltemp = RollSamples + PitchSamples;	
			PitchSamples = PitchSamples - RollSamples;	
			RollSamples = niltemp * 2;
			(int16)RollSamples /= 3;
			(int16)PitchSamples *= 2;
			(int16)PitchSamples /= 3;
		
		}

		#ifdef DEBUG_SENSORS
		SendComValH(RollSamples.high8);
		SendComValH(RollSamples.low8);
		SendComChar(';');
		SendComValH(PitchSamples.high8);
		SendComValH(PitchSamples.low8);
		SendComChar(';');
		#endif
	
		// Roll
		niltemp = RollSamples;

		#ifdef OPT_ADXRS
		RollSamples += 2;
		(int16)RollSamples >>= 2;
		#endif
		#ifdef OPT_IDG
		RollSamples += 1;
		(int16)RollSamples >>= 1;
		#endif
		RE = RollSamples.low8;	// use 8 bit res. for PD controller

		#ifdef OPT_ADXRS
		RollSamples = niltemp + 1;
		(int16)RollSamples >>= 1;	// use 9 bit res. for I controller
		#endif
		#ifdef OPT_IDG
		RollSamples = niltemp;
		#endif

		LimitRollSum();		// for roll integration

		// Pitch
		niltemp = PitchSamples;

		#ifdef OPT_ADXRS
		PitchSamples += 2;
		(int16)PitchSamples >>= 2;
		#endif
		#ifdef OPT_IDG
		PitchSamples += 1;
		(int16)PitchSamples >>= 1;
		#endif
		PE = PitchSamples.low8;

		#ifdef OPT_ADXRS
		PitchSamples = niltemp + 1;
		(int16)PitchSamples >>= 1;
		#endif
		#ifdef OPT_IDG
		PitchSamples = niltemp;
		#endif
		LimitPitchSum();		// for pitch integration

		// Yaw is sampled only once every frame, 8 bit A/D resolution
		ADFM = 0;
		ADCON0 = 0b.10.100.0.0.1;	// select CH4(RA5) Yaw
		AcqTime();
		YE = ADRESH;
		if( MidYaw == 0 )
			MidYaw = YE;
		YE -= MidYaw;

		LimitYawSum();

		#ifdef DEBUG_SENSORS
		Temp = (int16)YE;
		SendComValH(Temp.high8);
		SendComValH(Temp.low8);
		SendComChar(';');
		SendComValH(RollSum.high8);
		SendComValH(RollSum.low8);
		SendComChar(';');
		SendComValH(PitchSum.high8);
		SendComValH(PitchSum.low8);
		SendComChar(';');
		SendComValH(YawSum.high8);
		SendComValH(YawSum.low8);
		SendComChar(';');
		#endif
	}
} // CalcGyroValues


// Mix the Camera tilt channel (Ch6) and the
// ufo air angles (roll and nick) to the 
// camera servos. 
void MixAndLimitCam(void)
{
// Cam Servos

	if( IntegralCount > 0 ) // while integrator are adding up
	{			// do not use the gyros values to correct
		Rp = 0;		// in non-flight mode, these are already cleared in InitArrays()
		Pp = 0;
	}

	if( _UseCh7Trigger )
		Rp += _Neutral;
	else
		Rp += IK7;
		
	Pp += IK6;		// only Pitch servo is controlled by channel 6

	if( Rp > _Maximum )
		MCamRoll = _Maximum;
	else
		if( Rp < _Minimum )
			MCamRoll = _Minimum;
		else
			MCamRoll = Rp;

	if( Pp > _Maximum )
		MCamPitch = _Maximum;
	else
		if( Pp < _Minimum )
			MCamPitch = _Minimum;
		else
			MCamPitch = Pp;
} // MixAndLimitCam
