// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008, 2009 by Prof. Greg Egan           =
// =   Original V3.15 Copyright (c) 2007, 2008 Ing. Wolfgang Mahringer   =
// =           http://code.google.com/p/uavp-mods/ http://uavp.ch        =
// =======================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "uavx.h"

// Prototypes

extern void InitPorts(void);
extern void InitMisc(void);

void Delay1mS(int16);
void Delay100mSWithOutput(int16);
void DoBeep100mSWithOutput(uint8, uint8);
void DoStartingBeepsWithOutput(uint8);

void CheckAlarms(void);

int16 SlewLimit(int16, int16, int16);
int16 ProcLimit(int16, int16, int16);
int16 DecayX(int16, int16);

void DumpTrace(void);

void InitPorts(void)
{
	// general ports setup
	TRISA = 0b00111111;								// all inputs
	ADCON1 = 0b00000010;							// uses 5V as Vref

	PORTB = 0b11000000;								// all outputs to low, except RB6 & 7 (I2C)!
	TRISB = 0b01000000;								// all servo and LED outputs
	PORTC = 0b01100000;								// all outputs to low, except TxD and CS
	TRISC = 0b10000100;								// RC7, RC2 are inputs

	SSPSTATbits.CKE = true;							// low logic threshold for LISL
	INTCON2bits.NOT_RBPU = false;	// WEAK PULLUPS MUST BE ENABLED OTHERWISE I2C VERSIONS 
									// WITHOUT ESCS INSTALLED WILL PREVENT ANY FURTHER BOOTLOADS
} // InitPorts

void InitMisc(void)
{
	uint8 i;

	State = Starting;				// For trace preconditions

	for ( i = 0; i <= TopTrace; i++)
		Trace[i] = 0;
	
	for ( i = 0; i < FLAG_BYTES ; i++ )
		F.AllFlags[i] = 0;
	F.BeeperInUse = F.GPSTestActive = false; 
	F.RTHAltitudeHold = F.ParametersValid = F.AcquireNewPosition = true;

	ThrNeutral = ThrLow = ThrHigh = MAXINT16;
	IdleThrottle = RC_THRES_STOP;
	InitialThrottle = RC_MAXIMUM;
	ESCMin = OUT_MINIMUM;
	ESCMax = OUT_MAXIMUM;

	GyroMidRoll = GyroMidPitch = GyroMidYaw = RollRate = PitchRate = YawRate = 0;

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
	static int16 NewBatteryVolts;

	NewBatteryVolts = ADC(ADCBattVoltsChan, ADCVREF5V) >> 3; 
	BatteryVolts = SoftFilter(BatteryVolts, NewBatteryVolts);
	F.LowBatt =  (BatteryVolts < (int16)P[LowVoltThres]) & 1;

	F.BeeperInUse = F.LowBatt || F.LostModel;

	if ( F.BeeperInUse )
	{
		if( F.LowBatt ) // repeating beep
			if( ((int16)mS[Clock] & 0x0200) == 0 )
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
				if( ((int16)mS[Clock] & 0x0400) == 0 )
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
		if ( (State == InFlight) && (!F.AcquireNewPosition) && (mS[Clock] > mS[BeeperTimeout]) )
			Beeper_OFF;
	#endif // NAV_ACQUIRE_BEEPER 

} // CheckAlarms

#ifndef USE_LIMIT_MACRO
int16 ProcLimit(int16 i, int16 l, int16 u)
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

int16 SlewLimit(int16 Old, int16 New, int16 Slew)
{
  int16 Low, High;
  
  Low = Old - Slew;
  High = Old + Slew; 
  return(( New < Low ) ? Low : (( New > High ) ? High : New));
} // SlewLimit

void DumpTrace(void)
{
	#ifdef DEBUG_SENSORS
	uint8 t;

	if ( DesiredThrottle > 20 ) 
	{
		for (t=0; t <= TopTrace; t++)
		{
			TxValH16(Trace[t]);
			TxChar(';');
		}
		TxNextLine();
	}

	#endif // DEBUG_SENSORS
} // DumpTrace

void SendUAVXState(void) // 925uS at 16MHz
{
	uint8 b;
	// packet must be shorter than GPS shortest valid packet ($GPGGA)
	// which is ~64 characters - so limit to 48?.
	#ifdef TELEMETRY
	for (b=10;b;b--) 
		SendByte(0x55);
      
	SendByte(0xff); // synchronisation to "jolt" USART

  	SendByte(SOH);

  	TxCheckSum = 0;

	switch ( UAVXCurrPacketTag ) {
	case UAVXFlightPacketTag:
		SendESCByte(UAVXFlightPacketTag);
		SendESCByte(30 + FLAG_BYTES);
		for ( b = 0; b < FLAG_BYTES; b++ )
			SendESCByte(F.AllFlags[b]); 
	
	  	SendESCByte(State);

		SendESCByte(BatteryVolts);
		SendESCWord(0); 						// Battery Current
		SendESCWord(RCGlitches);
		
		SendESCWord(DesiredThrottle);
		SendESCWord(DesiredRoll);
		SendESCWord(DesiredPitch);
		SendESCWord(DesiredYaw);
		SendESCWord(RollRate);
		SendESCWord(PitchRate);
		SendESCWord(YawRate);
		SendESCWord(RollSum);
		SendESCWord(PitchSum);
		SendESCWord(YawSum);
		SendESCWord(LRAcc);
		SendESCWord(FBAcc);
		SendESCWord(DUAcc);
		
		UAVXCurrPacketTag = UAVXNavPacketTag;
		break;

	case UAVXNavPacketTag:
		SendESCByte(UAVXNavPacketTag);
		SendESCByte(20);

		SendESCByte(NavState);
		SendESCByte(FailState);	

		SendESCWord(GPSVel);
		SendESCWord(CurrentRelBaroPressure);
		SendESCWord(GPSHDilute);
		SendESCWord(Heading);
		SendESCWord(GPSRelAltitude);
		SendESCWord(GPSEast);
		SendESCWord(GPSNorth);
		SendESCWord(GPSEastHold);
		SendESCWord(GPSNorthHold);

		UAVXCurrPacketTag = UAVXFlightPacketTag;
		break;

	default:
		UAVXCurrPacketTag = UAVXFlightPacketTag;
		break;		
	}
	
  	SendESCByte(TxCheckSum);

  	SendByte(EOT);

  	SendByte(CR);
	SendByte(LF);  	

	#endif // TELEMETRY
} // SendUAVXState



