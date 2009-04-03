// ==============================================
// =      U.A.V.P Brushless UFO Controller      =
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
// =  please visit http://www.uavp.org          =
// =               http://www.mahringer.co.at   =
// ==============================================

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
	bank0 uns8 MV, MH, ML, MR;	// must reside on bank0
	uns8 MT@MV;	// cam tilt servo
	uns8 ME@MH; // cam tilt servo


#ifdef NADA
SendComValH(MCamRoll);
SendComValH(MCamNick);
SendComChar(0x0d);
SendComChar(0x0a);
#endif

#ifndef DEBUG_SENSORS

#ifdef DEBUG_MOTORS
	if( _Flying && CamNickFactor.4 )
	{
		SendComValU(IGas);
		SendComChar(';');
		SendComValS(IRoll);
		SendComChar(';');
		SendComValS(INick);
		SendComChar(';');
		SendComValS(ITurn);
		SendComChar(';');
		SendComValU(MVorne);
		SendComChar(';');
		SendComValU(MHinten);
		SendComChar(';');
		SendComValU(MLinks);
		SendComChar(';');
		SendComValU(MRechts);
		SendComChar(0x0d);
		SendComChar(0x0a);
	}
#endif

	TMR0 = 0;
	T0IF = 0;

#ifdef ESC_PPM
	ALL_PULSE_ON;	// turn on all motor outputs
#endif

	MV = MVorne;
	MH = MHinten;
	ML = MLinks;
	MR = MRechts;

#ifdef DEBUG_MOTORS
// if DEBUG_MOTORS is active, CamIntFactor is a bitmap:
// bit 0 = no front motor
// bit 1 = no rear motor
// bit 2 = no left motor
// bit 3 = no right motor
// bit 4 = turns on the serial output

	if( CamNickFactor.0 )
		MV = _Minimum;
	if( CamNickFactor.1 )
		MH = _Minimum;
	if( CamNickFactor.2 )
		ML = _Minimum;
	if( CamNickFactor.3 )
		MR = _Minimum;
#else
#ifdef INTTEST
	MV = _Minimum;
	MH = _Minimum;
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

	GIE = 0;	// BLOCK ALL INTERRUPTS
	while( T0IF == 0 ) ;	// wait for first overflow
	T0IF=0;		// quit TMR0 interrupt

#if !defined DEBUG && !defined DEBUG_MOTORS
	if( _OutToggle )	// driver cam servos only every 2nd pulse
	{
		CAM_PULSE_ON;	// now turn camera servo pulses on too
	}
	_OutToggle ^= 1;
#endif

// This loop is exactly 16 cycles long
// under no circumstances should the loop cycle time be changed
#asm
	BCF	RP0		// clear all bank bits
//	BCF	RP1
OS005
	MOVF	PORTB,W
	ANDLW	0x0F		// output ports 0 to 3
	BTFSC	Zero_
	GOTO	OS006		// stop if all 4 outputs are done

	DECFSZ	MV,f		// front motor
	GOTO	OS007

	BCF	PulseVorne		// stop pulse
OS007
	DECFSZ	ML,f		// left motor
	GOTO	OS008

	BCF	PulseLinks		// stop pulse
OS008
	DECFSZ	MR,f		// right motor
	GOTO	OS009

	BCF	PulseRechts		// stop pulse
OS009
	DECFSZ	MH,f		// rear motor
	GOTO	OS005
	
	BCF	PulseHinten		// stop pulse
	GOTO	OS005
OS006
#endasm
// This will be the corresponding C code:
//	while( ALL_OUTPUTS != 0 )
//	{	// remain in loop as long as any output is still high
//		if( TMR2 = MVorne  ) PulseVorne  = 0;
//		if( TMR2 = MHinten ) PulseHinten = 0;
//		if( TMR2 = MLinks  ) PulseLinks  = 0;
//		if( TMR2 = MRechts ) PulseRechts = 0;
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
	SendEscI2CByte(MV); // for all motors
	SendEscI2CByte(MH);
	SendEscI2CByte(ML);
	SendEscI2CByte(MR);
	EscI2CStop();
#endif	// ESC_X3D

#ifdef ESC_HOLGER
	EscI2CStart();
	SendEscI2CByte(0x52);	// one cmd, one data byte per motor
	SendEscI2CByte(MV); // for all motors
	EscI2CStop();

	EscI2CStart();
	SendEscI2CByte(0x54);
	SendEscI2CByte(MH);
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
	SendEscI2CByte(MV>>1); // for all motors
	EscI2CStop();

	EscI2CStart();
	SendEscI2CByte(0x64);
	SendEscI2CByte(MH>>1);
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

	GIE = 0;	// Int wieder sperren, wegen Jitter

	while( T0IF == 0 ) ;	// wait for 2nd overflow (2 ms)

	// avoid servo overrun when MCamxx == 0
	ME = MCamRoll+1;
	MT = MCamNick+1;

#if !defined DEBUG && !defined DEBUG_SENSORS
// This loop is exactly 16 cycles long
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

	BCF	PulseCamNick
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
}


// convert Roll and Nick gyro values
// using 10-bit A/D conversion.
// Values are ADDED into RollSamples and NickSamples
void GetGyroValues(void)
{

	ADFM = 1;					// select 10 bit mode
#ifdef OPT_ADXRS
	ADCON0 = 0b.10.001.0.0.1;	// select CH1(RA1) Roll
#endif
#ifdef OPT_IDG
#ifdef BOARD_3_1
	PCFG0 = 1;					// select 3,6V as Vref
#endif
	ADCON0 = 0b.10.010.0.0.1;	// select CH2(RA2) Nick
#endif
	AcqTime();

	RollSamples += ADRESL;
	RollSamples.high8 += ADRESH;

#ifdef OPT_ADXRS
	ADCON0 = 0b.10.010.0.0.1;	// select CH2(RA2) Nick
#endif
#ifdef OPT_IDG
	ADCON0 = 0b.10.001.0.0.1;	// select CH1(RA1) Roll
#endif
	AcqTime();

	NickSamples += ADRESL;
	NickSamples.high8 += ADRESH;
#ifdef OPT_IDG
#ifdef BOARD_3_1
	PCFG0 = 0;					// select 5V as Vref
#endif
#endif
}

// ADXRS300: The Integral (RollSum & Nicksum) has
// a resolution of about 1000 LSBs for a 25° angle
// IDG300: (TBD)
//

// Calc the gyro values from added RollSamples 
// and NickSamples (global variable "nisampcnt")
void CalcGyroValues(void)
{
// RollSamples & Nicksamples hold the sum of 2 consecutive conversions
	RollSamples ++;	// for a correct round-up
	NickSamples ++;

#ifdef OPT_ADXRS150
	(long)RollSamples >>= 2;	// recreate the 10 bit resolution
	(long)NickSamples >>= 2;
#else
	(long)RollSamples >>= 1;	// recreate the 10 bit resolution
	(long)NickSamples >>= 1;
#endif

	if( IntegralCount > 0 )
	{
// pre-flight auto-zero mode
		RollSum += RollSamples;
		NickSum += NickSamples;

		if( IntegralCount == 1 )
		{
			RollSum += 8;
			NickSum += 8;
			if( !_UseLISL )
			{
				niltemp = RollSum + MiddleLR;
				RollSum = niltemp;
				niltemp = NickSum + MiddleFB;
				NickSum = niltemp;
			}
			MidRoll = (uns16)RollSum / (uns16)16;	
			MidNick = (uns16)NickSum / (uns16)16;
			RollSum = 0;
			NickSum = 0;
			LRIntKorr = 0;
			FBIntKorr = 0;
		}
	}
	else
	{
// standard flight mode
		RollSamples -= MidRoll;
		NickSamples -= MidNick;

// calc Cross flying mode
		if( FlyCrossMode )
		{
// Real Roll = 0.707 * (N + R)
//      Nick = 0.707 * (N - R)
// the constant factor 0.667 is used instead
			niltemp = RollSamples + NickSamples;	// 12 valid bits!
			NickSamples = NickSamples - RollSamples;	// 12 valid bits!
			RollSamples = niltemp * 2;
			(long)RollSamples /= 3;
			(long)NickSamples *= 2;
			(long)NickSamples /= 3;
		
		}
#ifdef DEBUG_SENSORS
		SendComValH(RollSamples.high8);
		SendComValH(RollSamples.low8);
		SendComChar(';');
		SendComValH(NickSamples.high8);
		SendComValH(NickSamples.low8);
		SendComChar(';');
#endif
	
// Roll
		niltemp = RollSamples;

#ifdef OPT_ADXRS
		RollSamples += 2;
		(long)RollSamples >>= 2;
#endif
#ifdef OPT_IDG
		RollSamples += 1;
		(long)RollSamples >>= 1;
#endif
		RE = RollSamples.low8;	// use 8 bit res. for PD controller

#ifdef OPT_ADXRS
		RollSamples = niltemp + 1;
		(long)RollSamples >>= 1;	// use 9 bit res. for I controller
#endif
#ifdef OPT_IDG
		RollSamples = niltemp;
#endif
		LimitRollSum();		// for roll integration

// Nick
		niltemp = NickSamples;

#ifdef OPT_ADXRS
		NickSamples += 2;
		(long)NickSamples >>= 2;
#endif
#ifdef OPT_IDG
		NickSamples += 1;
		(long)NickSamples >>= 1;
#endif
		NE = NickSamples.low8;

#ifdef OPT_ADXRS
		NickSamples = niltemp + 1;
		(long)NickSamples >>= 1;
#endif
#ifdef OPT_IDG
		NickSamples = niltemp;
#endif
		LimitNickSum();		// for nick integration

// Yaw is sampled only once every frame, 8 bit A/D resolution
		ADFM = 0;
		ADCON0 = 0b.10.100.0.0.1;	// select CH4(RA5) Yaw
		AcqTime();
		TE = ADRESH;
		if( MidTurn == 0 )
			MidTurn = TE;
		TE -= MidTurn;

		LimitYawSum();
#ifdef DEBUG_SENSORS
		SendComValH(TE);
		SendComChar(';');
		SendComValH(RollSum.high8);
		SendComValH(RollSum.low8);
		SendComChar(';');
		SendComValH(NickSum.high8);
		SendComValH(NickSum.low8);
		SendComChar(';');
		SendComValH(YawSum.high8);
		SendComValH(YawSum.low8);
		SendComChar(';');
#endif
	}
}


// Mix the Camera tilt channel (Ch6) and the
// ufo air angles (roll and nick) to the 
// camera servos. 
void MixAndLimitCam(void)
{
// Cam Servos

	if( IntegralCount > 0 ) // while integrator are adding up
	{						// do not use the gyros values to correct
		Rp = 0;		// in non-flight mode, these are already cleared in InitArrays()
		Np = 0;
	}

	if( _UseCh7Trigger )
		Rp += _Neutral;
	else
		Rp += IK7;
		
	Np += IK6;		// only Nick servo is controlled by channel 6

	if( Rp > _Maximum )
		MCamRoll = _Maximum;
	else
	if( Rp < _Minimum )
		MCamRoll = _Minimum;
	else
		MCamRoll = Rp;

	if( Np > _Maximum )
		MCamNick = _Maximum;
	else
	if( Np < _Minimum )
		MCamNick = _Minimum;
	else
		MCamNick = Np;
}
