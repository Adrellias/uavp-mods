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
int16 SlewLimit(int16, int16, int16);
int32 ProcLimit(int32, int32, int32);
int16 DecayX(int16, int16);
void DumpTrace(void);
void SendUAVXState(void);

int16 Trace[TopTrace+1];
uint8 UAVXCurrPacketTag;
int8 BatteryVolts;

void InitPorts(void)
{
	// general ports setup
	TRISA = 0b00111111;								// all inputs
	ADCON1 = 0b00000010;							// uses 5V as Vref

	PORTB = 0b11000000;								// all outputs to low, except RB6 & 7 (I2C)!
	TRISB = 0b01000000;								// all servo and LED outputs

	// RC0 Pin 11 currently unused 
	PORTC = 0b01100000;								// all outputs to low, except TxD and CS
	#ifdef HAVE_CUTOFF_SW
	TRISC = 0b10000101;								// RC7, RC2, RC0 are inputs
	#else
	TRISC = 0b10000100;								// RC7, RC2 are inputs
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

	for ( i = 0; i <= TopTrace; i++)
		Trace[i] = 0;
	
	for ( i = 0; i < FLAG_BYTES ; i++ )
		F.AllFlags[i] = 0;
	F.BeeperInUse = F.GPSTestActive = false; 
	F.NavAltitudeHold = F.ParametersValid = F.AcquireNewPosition = true;

	ThrNeutral = ThrLow = ThrHigh = MAXINT16;
	IdleThrottle = RC_THRES_STOP;
	InitialThrottle = RC_MAXIMUM;
	ESCMin = OUT_MINIMUM;
	ESCMax = OUT_MAXIMUM;

	GyroMidRoll = GyroMidPitch = GyroMidYaw = RollRate = PitchRate = YawRate = 0;
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
	static int16 NewBatteryVolts;

	NewBatteryVolts = ADC(ADCBattVoltsChan) >> 3; 
	BatteryVolts = SoftFilter(BatteryVolts, NewBatteryVolts);
	F.LowBatt = (BatteryVolts < (int16)P[LowVoltThres]) & 1;

	F.BeeperInUse = F.LowBatt || F.LostModel;

	if ( F.BeeperInUse )
	{
		if( F.LowBatt ) // repeating beep
			if( ((int16)mS[Clock] & 0x0200) == 0 )
			{
				Stats[BatteryS] = true;
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
	i32u Temp;

	// packet must be shorter than GPS shortest valid packet ($GPGGA)
	// which is ~64 characters - so limit to 48?.

	for (b=10;b;b--) 
		SendByte(0x55);
	      
	SendByte(0xff); // synchronisation to "jolt" USART
	
	SendByte(SOH);
	
	TxCheckSum = 0;
	
	switch ( UAVXCurrPacketTag ) {
	case UAVXFlightPacketTag:
		SendESCByte(UAVXFlightPacketTag);
		SendESCByte(32 + FLAG_BYTES);
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
		SendESCByte(30);
	
		SendESCByte(NavState);
		SendESCByte(FailState);
		SendESCByte(GPSNoOfSats);
		SendESCByte(GPSFix);

		SendESCByte(CurrWP);	
		SendESCByte(0); // padding

		SendESCWord(GPSVel);
		Temp.i32 = RelBaroAltitude;
		SendESCWord(Temp.w0);
		SendESCWord(Temp.w1);

		SendESCWord(BaroROC); // cm/S 
		SendESCWord(GPSHDilute);
		SendESCWord(Heading);

		Temp.i32 = GPSRelAltitude; // cm
		SendESCWord(Temp.w0);
		SendESCWord(Temp.w1);

		Temp.i32 = GPSLongitude; // 5 decimal minute units
		SendESCWord(Temp.w0);
		SendESCWord(Temp.w1);
	
		Temp.i32 = GPSLatitude;
		SendESCWord(Temp.w0);
		SendESCWord(Temp.w1);
	
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
	
} // SendUAVXState

#ifdef ARDUPILOT_TELEMETRY

void ArduPilotTelemetry(void)
{
	/*      
	Definitions of the low rate telemetry (1Hz):
    LAT: Latitude
    LON: Longitude
    SPD: Speed over ground from GPS
    CRT: Climb Rate in M/S
    ALT: Altitude in meters
    ALH: The altitude is trying to hold
    CRS: Course over ground in degrees.
    BER: Bearing is the heading you want to go
    WPN: Waypoint number, where WP0 is home.
    DST: Distance from Waypoint
    BTV: Battery Voltage.
    RSP: Roll setpoint used to debug, (not displayed here).
    
    Definitions of the high rate telemetry (4Hz):
    ASP: Airspeed, right now is the raw data.
    TTH: Throttle in 100% the autopilot is applying.
    RLL: Roll in degrees + is right - is left
    PCH: Pitch in degrees
    SST: Switch Status, used for debugging, but is disabled in the current version.
	*/

	static uint32 timer1=0;
	static uint8 counter;
  
	if ( mS[Clock] > mS[ArduPilotUpdate] )
	{
		mS[ArduPilotUpdate] = mS[Clock] + 1000;	   
//     digitalWrite(13,HIGH); // turns on link GKE?
    if( true )//counter >= POSITION_RATE_OUTPUT)//If to repeat every second.... 
    {


		TxString("!!!");
		TxString("LAT:");
		TxVal32(GPSLatitude, 0,0);
		TxString(",LON:");
		TxVal32(GPSLongitude, 0, 0); //wp_current_lat
      //TxVal32(",WLA:");
      //TxVal32((long)((float)wp_current_lat*(float)t7));
      //TxVal32(",WLO:");
      //TxVal32((long)((float)wp_current_lon*(float)t7));
      
		TxString(",SPD:");
		TxVal32(12, 0, 0);    
		TxString(",CRT:");
		TxVal32(GPSROC, 0, 0);
		TxString(",ALT:");
		TxVal32(100,0,0);
		TxString(",ALH:");
		TxVal32(DesiredAltitude, 2, 0);
		TxString(",MSL:");
		TxVal32(0,0,0);//alt_MSL);
		TxString(",CRS:");
		TxVal32(Heading, 0, 0);//ground_course);
		TxString(",BER:");
		TxVal32(0, 0, 0);//wp_bearing);
		TxString(",WPN:");
		TxVal32(5,0,0);//This the TO waypoint.
		TxString(",DST:");
		TxVal32(0, 0, 0);//wp_distance);
		TxString(",BTV:");
		TxVal32(BatteryVolts, 0, 0);
		//TxVal32(read_Ch1());
		TxString(",RSP:");
		TxVal32(DesiredRoll, 0, 0);
 /*
     TxString(",GPS:");
     TxVal32(gpsFix);
     TxVal32(read_Ch2());
     TxString(",STT:");
     TxVal32((int)Tx_Switch_Status());
     TxString(",RST:");
     TxVal32((int)wpreset_flag);
     TxString(",WLF:");
     TxVal32((int)wplist_flag);
     TxString(",CWP:");
     TxVal32((int)current_wp);
     TxString(",NWP:");
     TxVal32((int)wp_number);
 */
      TxString(",***\r\n");
      counter=0;
      
      //TxVal32ln(refresh_rate);
 //     refresh_rate=0;
    }
    else
    {
    counter++;
    
    TxString("+++");
/*  TxVal32("ASO:");
		TxVal32((int)air_speed_offset);
		TxVal32(",AN3:");
		TxVal32((int)analog3); */
		TxString(",ASP:");
		TxVal32(GPSVel, 0,0);
		TxString("THH:");
		TxVal32(DesiredThrottle,0,0);
		TxString(",RLL:");
		TxVal32(RollSum,0,0);
		//TxVal32(Roll);
		TxString(",PCH:");
		TxVal32(PitchSum,0,0);
  
		TxString(",STT:");
		TxVal32(F.Navigate,0,0);

		/*
		TxVal32(",");
		TxString("rER:");
		TxVal32((int)roll_set_point);
		TxString(",Mir:");
		TxVal32(max_ir);
		TxVal32(",");
		TxString("CH1:");
		TxVal32(read_Ch1()/16);
		TxVal32(",");
		TxString("CH2:");
		TxVal32(read_Ch2()/16);
		TxString(",PSET:");
		TxVal32(DesiredPitch);
		*/
		TxString(",***\r\n");
    }
  //  timer1=millis(); 
 //   digitalWrite(13,LOW);
  } 
} // ArduPilotTelemetry

#endif // ARDUPILOT_TELEMETRY

