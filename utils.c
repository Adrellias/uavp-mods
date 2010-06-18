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

extern void InitPorts(void);
extern void InitMisc(void);
void Delay1mS(int16);
void Delay100mSWithOutput(int16);
void DoBeep100mSWithOutput(uint8, uint8);
void DoStartingBeepsWithOutput(uint8);
void CheckAlarms(void);
int32 SlewLimit(int32, int32, int32);
int32 ProcLimit(int32, int32, int32);
int16 DecayX(int16, int16);

int8 BatteryVolts;
int16 BatteryVoltsADC, BatteryCurrentADC, BatteryVoltsLimitADC, BatteryCurrentADCEstimated, BatteryChargeUsedmAH;
int32 BatteryChargeADC, BatteryCurrent;

void InitPorts(void)
{
	// general ports setup
	TRISA = 0b00111111;								// all inputs
	ADCON1 = 0b00000010;							// uses 5V as Vref

	#ifdef UAVX_HW
		#ifdef UAVX_HW_RX_PARALLEL
			PORTB = 0b00000000;						// bottom 4 bits motor/servos, top Rx signals
			TRISB = 0b1110000;
// zzz ??? interrupts on change		
		#else
			PORTB = 0b00000000;						// all outputs - motos/servos LEDs, buzzer
			TRISB = 0b00000000;					
		#endif // UAVX_HW_RX_PARALLEL
	#else // UAVX_HW
		PORTB = 0b11000000;							// all outputs to low, except RB6 & 7 (I2C)!
		TRISB = 0b01000000;							// all servo and LED outputs
	#endif // UAVX_HW

	// RC0 Pin 11 currently unused 
	PORTC = 0b01100000;								// all outputs to low, except TxD and CS
	#ifdef HAVE_CUTOFF_SW
		TRISC = 0b10000101;							// RC7, RC2, RC0 are inputs
	#else
		TRISC = 0b10000100;							// RC7, RC2 are inputs
	#endif // HAVE_CUTOFF_SW

	SSPSTATbits.CKE = true;							// low logic threshold for LISL
	INTCON2bits.NOT_RBPU = false;	// WEAK PULLUPS MUST BE ENABLED OTHERWISE I2C VERSIONS 
									// WITHOUT ESCS INSTALLED WILL PREVENT ANY FURTHER BOOTLOADS
} // InitPorts

void InitMisc(void)
{
	uint8 i;

	State = Starting;				// For trace preconditions
	ServoToggle = false;
	
	for ( i = 0; i < FLAG_BYTES ; i++ )
		F.AllFlags[i] = 0;
	F.BeeperInUse = false; 
	F.NavAltitudeHold = F.ParametersValid = F.AcquireNewPosition = true;

	#ifdef SIMULATE
	F.Simulation = true;
	#endif // SIMULATE

	BatteryChargeADC = 0;

	ThrNeutral = ThrLow = ThrHigh = MAXINT16;
	IdleThrottle = ((10L*OUT_MAXIMUM)/100);
	InitialThrottle = RC_MAXIMUM;
	ESCMin = OUT_MINIMUM;
	ESCMax = OUT_MAXIMUM;

	RollRate = PitchRate = YawRate = 0;
	DesiredRollP = DesiredPitchP = 0;
	RollTrim = PitchTrim = YawTrim = 0;	

	LEDShadow = 0;
    ALL_LEDS_OFF;
	LEDRed_ON;
	Beeper_OFF;
} // InitMisc

void Delay1mS(int16 d)
{ 
	int16 i;
	boolean T0IntEn;

	T0IntEn = INTCONbits.TMR0IE;	// not protected?
	INTCONbits.TMR0IE = false;

	// if d is 1 then delay can be less than 1mS due to 	
	for (i = d; i ; i--)
	{						// compromises ClockMilliSec;
		while ( !INTCONbits.TMR0IF ) {};
		INTCONbits.TMR0IF = 0;
		FastWriteTimer0(TMR0_1MS);
	}

	INTCONbits.TMR0IE = T0IntEn;

} // Delay1mS

void Delay100mSWithOutput(int16 dur)
{  // Motor and servo pulses are still output every 10ms
	int16 i;
	uint8 j;
	boolean T0IntEn;

	T0IntEn = INTCONbits.TMR0IE;	// not protected?
	INTCONbits.TMR0IE = false;

	for( i = 0; i < dur*10; i++)
		{
			for (j = 8; j ; j--)
			{
				while ( !INTCONbits.TMR0IF ) {};
				INTCONbits.TMR0IF = 0;
				FastWriteTimer0(TMR0_1MS);
			}
			OutSignals(); // 1-2 ms Duration
			if( PIR1bits.RCIF )
			{
				INTCONbits.TMR0IE = T0IntEn;
				return;
			}
		}
	INTCONbits.TMR0IE = T0IntEn;
} // Delay100mSWithOutput

void DoBeep100mSWithOutput(uint8 t, uint8 d)
{
	Beeper_ON;
	Delay100mSWithOutput(t);
	Beeper_OFF;
	Delay100mSWithOutput(d);
} // DoBeep100mSWithOutput

void DoStartingBeepsWithOutput(uint8 b)
{
	uint8 i;

	for ( i = 0; i < b; i++ )
		DoBeep100mSWithOutput(2, 8);

	DoBeep100mSWithOutput(8,0);
} // DoStartingBeeps

void CheckAlarms(void)
{
	static int16 Temp;

	//No spare ADC channels yet. Temp = ADC(ADCBattCurrentChan);
	BatteryCurrentADC  = CurrThrottle * THROTTLE_CURRENT_SCALE; // Mock Sensor

	BatteryChargeADC += BatteryCurrentADC * (mSClock() - mS[LastBattery]);
	mS[LastBattery] = mSClock();
	BatteryChargeUsedmAH = CURRENT_SENSOR_MAX * (BatteryChargeADC/3686400L); // 1024*1000*3600

	Temp = ADC(ADCBattVoltsChan);
	BatteryVoltsADC  = SoftFilter(BatteryVoltsADC, Temp);
	F.LowBatt = (BatteryVoltsADC < BatteryVoltsLimitADC ) & 1;

	F.BeeperInUse = F.LowBatt || F.LostModel;

	if ( F.BeeperInUse )
	{
		if( F.LowBatt ) // repeating beep
			if( ((int16)mSClock() & 0x0200) == 0 )
			{
				Beeper_ON;
				LEDRed_ON;
			}
			else
			{
				Beeper_OFF;
				LEDRed_OFF;
			}	
		else
			if ( F.LostModel ) // 2 beeps with interval
				if( ((int16)mSClock() & 0x0400) == 0 )
				{
					Beeper_ON;
					LEDRed_ON;
				}
				else
				{
					Beeper_OFF;
					LEDRed_OFF;
				}	
			else
				{
					Beeper_OFF;
					LEDRed_OFF;
				}
	}	
	#ifdef NAV_ACQUIRE_BEEPER
	else
		if ( (State == InFlight) && (!F.AcquireNewPosition) && (mSClock() > mS[BeeperTimeout]) )
			Beeper_OFF;
	#endif // NAV_ACQUIRE_BEEPER 

} // CheckAlarms

#ifndef USE_LIMIT_MACRO
int32 ProcLimit(int32 i, int32 l, int32 u)
{
	return ((i<l) ? l : ((i>u) ? u : i));	
} // ProcLimit
#endif // USE_LIMIT_MACRO

int16 DecayX(int16 i, int16 d)
{
	if ( i < 0 )
	{
		i += d;
		if ( i >0 )
			i = 0;
	}
	else
	if ( i > 0 )
	{
		i -= d;
		if ( i < 0 )
			i = 0;
	}
	return (i);
} // DecayX

int32 SlewLimit(int32 Old, int32 New, int32 Slew)
{
  int32 Low, High;
  
  Low = Old - Slew;
  High = Old + Slew; 
  return(( New < Low ) ? Low : (( New > High ) ? High : New));
} // SlewLimit


