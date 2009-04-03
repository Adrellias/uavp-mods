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

// Misc

#include "UAVX.h"

// Prototypes
int24 SRS24(int24, uint8);
int16 SRS16(int16, uint8);
void GetBatteryVolts(void);
void WriteADCVolts(void);
void CheckAlarms(void);
void ReadParametersEE(void);
void WriteParametersEE(uint8);
void WriteEE(uint8, int8);
int8 ReadEE(uint8);
void SendLeds(void);
void SwitchLedsOn(uint8);
void SwitchLedsOff(uint8);
void LEDGame(void);
void DoDebugTraces(void);
void Delay100mSec(uint8);
void InitPorts(void);
int16 int16sin(int16);
int16 int16cos(int16);

#pragma udata utilvars
uint8	LedShadow;								// shadow register
int16	BatteryVolts;
#pragma udata 	

// SRS16/24 are used for divisions by powers of 2. A simple shift 
// does not (with C18 at least) preserve the sign on negative 
// arguments even if declared signed hence the conditionals.
// It is defined as a function as a simple macro would result in 
// possibly quite complicated expressions for "x" being expanded
// inline and not recognised as common sub expressions by the
// compiler.
// Typically divisions take 200uS vs SRS 20uS on a 18F2520 @ 16MHz.
int24 SRS24(int24 x, uint8 s)
{
	return((x<0) ? -((-x)>>s) : (x>>s));
} // SRS24

int16 SRS16(int16 x, uint8 s)
{
	return((x<0) ? -((-x)>>s) : (x>>s));
} // SRS16

#ifndef USE_MACROS

int16 Limit(int16 i, int16 l, int16 u)
{
	return((i<l) ? l : ((i>u) ? u : i));
} // Limit

int16 Max(int16 i,int16 j)
{
 		return((i<j) ? j : ((i>j) ? j : i));
} // Max

int16 DecayBand(int16 i, int16 l, int16 u, int16 d)
{
	return ((i<l) ? i+d : ((i>u) ? i-d : i));
} // DecayBand

int16 Decay(int16 i)
{
	return((i<0) ? i+1 : ((i>0) ? i-1 : 0));
} // Decay

#endif

/*
int16 int16sqrt(int16 n)
{ // 16 bit numbers 
	int16 r, b;

	r=0;
	b=256;
	while (b>0) 
	{
		if (r*r>n)
			r-=b;
		b=(b>>1);
		r+=b;
	}
	return(r);
}

int16 int32sqrt(int32 n)
// 32 bit numbers 
{
	int32 b;
	int16 r;

	b=65536;
	r=0;
	while (b>0) 
	{
		if (((int32)(r)*(int32)(r))>n)
			r-=b;
		b=(b>>1);
		r+=b;
	}
	return(r);
} // int32sqrt

*/

#pragma idata sintable
const uint8 SineTable[17]={ 
	0, 50, 98, 142, 180, 212, 236, 250, 255,
	250, 236, 212, 180, 142, 98, 50, 0
   };
#pragma idata

int16 Table16(int16 Val, uint8 *T)
{
	uint8 Index,Offset;
	int16 Temp, Low, High, Result;

	Index = (uint8) (Val >> 4);
	Offset = (uint8) (Val & 0x0f);
	Low = T[Index];
	High = T[++Index];
	Temp = (High-Low) * Offset;
	Result = Low + SRS16(Temp, 4);

	return(Result);
} // Table16

int16 int16sin(int16 A)
{	// A is in milliradian 0 to 2000Pi, result is -128 to 127
	int16 	v;
	uint8	Negate;

	while ( A < 0 ) A += TWOMILLIPI;
	while ( A >= TWOMILLIPI ) A -= TWOMILLIPI;

	Negate = A >= MILLIPI;
	if ( Negate )
		A -= MILLIPI;

	v = Table16((((int24)A * 256 + HALFMILLIPI)/MILLIPI)-1, &SineTable);

	if ( Negate )
		v= -v;

	return(v);
} // int16sin

int16 int16cos(int16 A)
{	// A is in milliradian 0 to 2000Pi, result is -128 to 127
	return(int16sin(A + HALFMILLIPI));
} // int16cos

#pragma idata arctan
const int16 ArctanTable[17]={
    0,  62, 124, 185, 245, 303, 359, 412,
  464, 512, 559, 602, 644, 682, 719, 753,785
   };
#pragma idata

int16 int16atan2(int16 y, int16 x)
{	// Result is in milliradian
 
	int16 Absx, Absy;
	int16 A;

	Absy = Abs(y);
	Absx = Abs(x);

	if ( x == 0 )
		if ( y < 0 )
			A = -HALFMILLIPI;
		else
			A = HALFMILLIPI;
	else
		if (y == 0)
			if ( x < 0 )
				A=MILLIPI;
			else
				A = 0;
		else
		{
			if ( Absy <= Absx )
				A = Table16((Absy * 256 )/Absx, &ArctanTable);
			else
				A = HALFMILLIPI - Table16(( Absx << 8 )/Absy, &ArctanTable); 

			if ( x < 0 )
				if ( y > 0 ) // 2nd Quadrant 
					A = MILLIPI - A;
				else // 3rd Quadrant 
					A = MILLIPI + A;
			else
				if ( y < 0 ) // 4th Quadrant 
					A = TWOMILLIPI-A;
	}
	return(A);
} // int16atan2

void InitPorts(void)
{
	// general ports setup
	TRISA = 0b00111111;						// all inputs
	ADCON1 = 0b00000010;					// uses 5V as Vref

	PORTB = 0b11000000;						// all outputs to low, except RB6 & 7 (I2C)!
	TRISB = 0b01000000;						// all servo and LED outputs
	PORTC = 0b01100000;						// all outputs to low, except TxD and CS
	TRISC = 0b10000100;						// RC7, RC2 are inputs
	SSPSTATbits.CKE = true;					// low logic threshold for LISL
	INTCON2bits.NOT_RBPU = true;
} // InitPorts

void GetBatteryVolts(void)
{
	int16 NewBatteryVolts;

	NewBatteryVolts = ADC(ADCBattVoltsChan, ADCVREF5V) >> 3; 
	BatteryVolts = SoftFilter(BatteryVolts, NewBatteryVolts);
	_LowBatt =  (BatteryVolts < (int16) LowVoltThres) & 1;
} // GetBatteryVolts

void SwitchLedsOn(uint8 LEDs)
{
	LedShadow |= LEDs;
	SendLeds();
} // SwitchLedsOn

void SwitchLedsOff(uint8 LEDs)
{
	LedShadow &= ~LEDs;
	SendLeds();
} // SwitchLedsOff

void CheckAlarms(void)
{
	uint8 AlarmOn;

	GetBatteryVolts();
	AlarmOn = _LowBatt || (!_Signal) || (State == Failsafe);

	if ( AlarmOn )
	{
		if ( ( mS[Clock] > mS[AlarmUpdate]) == 0)
		{
			Beeper_ON;
			LedRed_ON;
			mS[AlarmUpdate] = mS[Clock] + 500;
 		}
		else
		{
			LedRed_OFF;
			Beeper_OFF;
			mS[AlarmUpdate] = mS[Clock] + 100;
		}
	}		
	else
		{
			LedRed_OFF;
			Beeper_OFF;
		}
	
} // CheckAlarms

int8 ReadEE(uint8 addr)
{
	int8 b;

	EEADR = addr;
	EECON1bits.EEPGD = false;
	EECON1bits.RD = true;
	b=EEDATA;
	EECON1 = 0;
	return(b);	
} // ReadEE

void WriteEE(uint8 addr, int8 d)
{
	int8 rd;
	
	rd = ReadEE(addr);
	if ( rd != d )						// avoid redundant writes
	{
		EEDATA = d;				
		EEADR = addr;
		EECON1bits.EEPGD = false;
		EECON1bits.WREN = true;

		DisableInterrupts;
		EECON2 = 0x55;
		EECON2 = 0xaa;
		EECON1bits.WR = true;
		while(EECON1bits.WR);
		EnableInterrupts;

		EECON1bits.WREN = false;
	}

} // WriteEE

void ReadParametersEE(void)
{
	int8 *p, c; 
	uint16 addr;

	if( RC[ParamC] > RC_NEUTRAL )
		addr = _EESet2;	
	else
		addr = _EESet1;
	
	for(p = &FirstProgReg; p <= &LastProgReg; p++)
		*p = ReadEE(addr++);

} // ReadParametersEE

void WriteParametersEE(uint8 s)
{
	int8 *p;
	uint8 b;
	uint16 addr;
	
	if( s == 1 )
		addr = _EESet1;	
	else
		addr = _EESet2;

	p = &FirstProgReg; 
	while ( p <= &LastProgReg)
		WriteEE(addr++, *p++);
} // WriteParametersEE

void DoDebugTraces()
{
#ifdef DEBUG_SENSORS

		TxNextLine();
#endif
} // DoDebugTraces



