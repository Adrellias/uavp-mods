// ===============================================================================================
// =                                UAVX Quadrocopter Controller                                 =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                     http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// ===============================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

#include "uavx.h"

void ShowPrompt(void);
void ShowRxSetup(void);
void ShowSetup(boolean);
void ProcessCommand(void);

#pragma idata menu1


#ifdef TESTING
const rom uint8 SerHello[] = "UAVX TEST " Version 
#else
const rom uint8 SerHello[] = "UAVX " Version 							 
#endif // TESTING

 							  " Copyright 2008 G.K. Egan & 2007 W. Mahringer\r\n"
							  "This is FREE SOFTWARE and comes with ABSOLUTELY NO WARRANTY "
							  "see http://www.gnu.org/licenses/!\r\n";

#pragma idata

#pragma idata menuhelp
const rom uint8 SerHelp[] = "\r\nCommands:\r\n"
	#ifdef TESTING
	"A..Accelerometer test\r\n"
	#endif // TESTING
	"B..Load UAVX hex file\r\n"
	#ifdef TESTING
	"C..Compass test\r\n"
	#endif // TESTING
	"D..Load default parameter set\r\n"
	#ifdef TESTING
	"E..Exercise PWM\r\n"
	"G..Gyro test\r\n"
	"H..Barometer/Rangefinder test\r\n"
	"I..I2C bus scan\r\n"
	"K..Calibrate Compass\r\n"
//	"M..Modify parameters\r\n"
	"P..Rx test\r\n"
	#endif // TESTING
	"S..Setup\r\n"
	#ifdef TESTING
	"T..All LEDs and buzzer test\r\n"
	#endif // TESTING
	"V..Battery test\r\n"
	"X..Flight stats\r\n"
	#ifdef TESTING
	"Y..Program YGE I2C ESC\r\n"
	#endif // TESTING
	"1-8..Individual LED/buzzer test\r\n"; // last line must be in this form for UAVPSet
#pragma idata

const rom uint8 RxChMnem[] = "TAERG12";

void ShowPrompt(void)
{
	TxString("\r\n>");
} // ShowPrompt

void ShowRxSetup(void)
{
	if ( F.UsingSerialPPM )
		if ( PPMPosPolarity[P[TxRxType]] )
			TxString("Serial PPM frame (Positive Polarity)");
		else
			TxString("Serial PPM frame (Negative Polarity)");
	else
		TxString("Odd Rx Channels PPM");
} // ShowRxSetup

void ShowSetup(boolean h)
{
	int8 i, NoOfChannels;

	TxNextLine();
	if( h )
		ParamSet = 1;	

	TxString(SerHello);

	#ifdef EXPERIMENTAL
		TxString("WARNING - EXPERIMENTAL\r\n");
	#endif // EXPERIMENTAL

	#ifdef CLOCK_16MHZ
		TxString("Clock: 16MHz\r\n");
	#else // CLOCK_40MHZ
		TxString("Clock: 40MHz\r\n");
	#endif // CLOCK_16MHZ

	TxString("Aircraft: ");
	switch ( UAVXAirframe ) {
		case QuadAF: TxString("QUAD"); break;
		case TriAF: TxString("TRI"); break;
		case VAF: TxString("VT"); break;
		case Y6AF: TxString("Y6"); break;
		case HeliAF: TxString("HELI"); break;
		case ElevAF: TxString("FLYING WING"); break;
		case AilAF: TxString("AILERON"); break;
		default: TxString("Unknown");
	}
	TxNextLine();

	if( F.CompassValid )
	{
		TxString("Compass Offset: ");
		TxVal32((int16)P[CompassOffsetQtr] * 90,0,0);
		TxString("deg.\r\n");
	}

	TxString("Baro: ");
	ShowBaroType();

	#ifdef MULTICOPTER
		TxString("Forward Flight: ");
		TxVal32((int16)Orientation * 75L, 1, 0);
		TxString("deg CW from K1 motor(s)\r\n");
	#endif // MULTICOPTER

	TxString("Accelerometers: ");
	ShowAccType();	
	if ( F.AccelerationsValid )
		TxString(" ONLINE\r\n");
	else
		TxString(" FAILED\r\n");
	
	TxString("Roll/Pitch Gyros: ");
	ShowGyroType();	
	TxNextLine();

	TxString("Motor ESCs: ");	
	switch ( P[ESCType] ) {
	case ESCPPM:TxString("PPM "); break;
	case ESCHolger:TxString("Holger I2C {"); break;
	case ESCX3D:TxString("X-3D I2C {"); break;
	case ESCYGEI2C:TxString("YGE I2C {"); break;
	}

	if ( P[ESCType] != ESCPPM )
	{
		for ( i = 0; i < NoOfPWMOutputs; i++ )
			if ( ESCI2CFail[i] )
				TxString(" Fail");
			else
				TxString(" OK");
		TxString(" }");
	}	
	TxNextLine();
	
	#ifdef RX6CH
		TxString("6 CH - 5 ACTIVE CHANNELS\r\n");
	#endif // RX6CH

	#ifndef CLOCK_40MHZ

	TxString("Tx/Rx: ");
	
	switch ( P[TxRxType] ) {
		case FutabaCh3: TxString("Futaba Th 3 {"); break;
		case FutabaCh2: TxString("Futaba Th 2 {"); break;
		case FutabaDM8:TxString("Futaba DM8 & AR7000 {"); break; 
		case JRPPM: TxString("JR PPM {"); break; 
		case JRDM9: TxString("JR DM9 & AR7000{"); break; 
		case JRDXS12: TxString("JR DSX12 & AR7000 {"); break; 
		case DX7AR7000: TxString("Spektrum DX7 & AR7000 {"); break;
		case DX7AR6200: TxString("Spektrum DX7 & AR6200 {"); break;
		case CustomTxRx: TxString("Custom {"); break;
		case FutabaCh3_6_7: TxString("Futaba Th 2 Swap 6&7 {"); break;
		case DX7AR6000:TxString("Spektrum DX7 & AR6000 {"); break;
		case DX6iAR6200: TxString("Spektrum DX6i & AR6200 {"); break;
		case FutabaCh3_R617FS: TxString("Futaba Th 3 & R617FS {"); break; 
		case GraupnerMX16s: TxString("Graupner MX16s {"); break;
		case DX7aAR7000: TxString("Spektrum DX7a & AR7000 {"); break;
		case ExternalDecoder: TxString("External Decoder {"); break;
		default: ;
	}

	#endif // CLOCK_40MHZ
	
	if ( F.UsingSerialPPM )
		ShowRxSetup();
	else
	{	
		for ( i = 0; i < RC_CONTROLS; i++)
			TxChar(RxChMnem[RMap[i]]);
		
		TxString("} connect {");
		
		for ( i = 0; i < RC_CONTROLS; i+=2)
		{
			TxChar(RxChMnem[RMap[i]]);
			TxChar(' ');
		}
	}
	TxChar('}');
	if (( P[TxRxType] == DX7AR6200 ) || ( P[TxRxType] == DX6iAR6200))
		TxString(" Mix Rudder to Aux1/Flaps ");
	if ( F.UsingTxMode2 )
		TxString(" Tx Mode 2");
	else
		TxString(" Tx Mode 1");
	TxNextLine();

	TxString("Selected parameter set: "); // must be exactly this string as UAVPSet expects it
	TxChar('0' + ParamSet);	
	TxNextLine();

	TxString("\r\nNav:\r\n");
	if ( F.UsingRTHAutoDescend )
		TxString("\tAuto descend ENABLED\r\n");
	else
		TxString("\tAuto descend disabled\r\n");

	if ( F.AllowTurnToWP )
		TxString("\tTurn toward way point\r\n");
	else
		TxString("\tHold heading\r\n");

	if ( F.AllowNavAltitudeHold )
		TxString("\tAllow Nav Alt Hold\r\n");
	else
		TxString("\tManual Nav Alt Hold CAUTION\r\n");

	TxString("\r\nALARM (if any):\r\n");
	#ifdef TESTING
		TxString("\tTEST VERSION - No Motors\r\n");
	#endif // TESTING

	if ( !F.ParametersValid )
		TxString("\tINVALID flight paramters (PID)!\r\n");
	
	if ( !F.BaroAltitudeValid )
		TxString("\tBarometer: OFFLINE\r\n");
	if ( BaroRetries >= BARO_INIT_RETRIES )
		TxString("\tBaro Init: FAILED\r\n");

	if ( !F.RangefinderAltitudeValid )
		TxString("\tRangefinder: OFFLINE\r\n");

	if ( F.GyroFailure )
		TxString("\tGyro: FAILURE\r\n");

	if ( !F.AccelerationsValid )
		TxString("\tAccelerometers: OFFLINE\r\n");

	if ( !F.CompassValid )
		TxString("\tCompass: OFFLINE\r\n");

	if ( !F.Signal )
		TxString("\tBad EPAs or Tx switched off?\r\n");
	if ( Armed && FirstPass ) 
		TxString("\tUAVX is armed - DISARM!\r\n");

	if ( F.Navigate || F.ReturnHome )
		TxString("\tNavigate/RTH is selected - DESELECT!\r\n");

	if ( InitialThrottle >= RC_THRES_START )
		TxString("\tThrottle may be open - CLOSE!\r\n");
	
	ShowPrompt();
} // ShowSetup

void ProcessCommand(void)
{
	static int8  p;
	static uint8 ch;
	static uint16 addrbase, curraddr;
	static int8 d;

	if ( !Armed )
	{
		ch = PollRxChar();
		if ( ch != NUL   )
		{
			if( islower(ch))							// check lower case
				ch = toupper(ch);
			
			switch( ch )
			{
			case 'B':	// call bootloader
				{ // arming switch must be OFF to call bootloader!!!
					DisableInterrupts;
					BootStart();		// never comes back!
				}
			case 'D':
				UseDefaultParameters();
				ShowPrompt();
				break;
			case 'E':
				ExercisePWM();
				ShowPrompt();
				break;
			case 'L'  :	// List parameters
				TxString("\r\nParameter list for set #");	// do not change (UAVPset!)
				TxChar('0' + ParamSet);
				ReadParametersEE();
				for(p = 0; p < MAX_PARAMETERS; p++)
				{
					TxString("\r\nRegister ");
					TxValU((uint8)(p+1));
					TxString(" = ");
					TxValS(P[p]);
				}
				ShowPrompt();
				break;
			case 'M'  : // modify parameters
				// no reprogramming in flight!!!!!!!!!!!!!!!
				LEDBlue_ON;
				TxString("\r\nRegister ");
				p = (uint16)(RxNumU()-1);
				// Attempts to block use of old versions of UAVPSet not compatible with UAVX
				// assumes parameters are written sequentially from 0..(MAX_PARAMETERS-1)
				if ( p < (MAX_PARAMETERS-1) )
					F.ParametersValid = false;
				else
					if ( p == (MAX_PARAMETERS-1) )
						F.ParametersValid = true; 	// ALL parameters must be written 
				TxString(" = ");
				d = RxNumS();
				if ( p < MAX_PARAMETERS )
				{
					// Keep RAM based set up to date.
					if( ParamSet == (uint8)1 )
					{
						WriteEE(p, d);
						if ( DefaultParams[p][1] )
							WriteEE(MAX_PARAMETERS + p, d);
					}
					else
					{
						if ( !DefaultParams[p][1] )
							WriteEE(MAX_PARAMETERS + p, d);
					}
					ParametersChanged = true;
				}
				LEDBlue_OFF;
				ShowPrompt();
				break;
			case 'N' :	// neutral values
				GetNeutralAccelerations();
				TxString("\r\nNeutral    R:");
				TxValS(AccNeutral[LR]);
		
				TxString("    P:");
				TxValS(AccNeutral[FB]);
		
				TxString("   V:");	
				TxValS(AccNeutral[DU]);
				ShowPrompt();
				break;
			case 'Z' : // set Paramset
				p = RxNumU();
				if ( p != (int8)ParamSet )
				{
					ParamSet = p;
					ParametersChanged = true;
					ReadParametersEE();
				}
				break;
			case 'W' :	// comms with UAVXNav utility NOT UAVPSet
				UAVXNavCommand();
				//ShowPrompt();
				break;
			case 'R':	// receiver values
				TxString("\r\nT:");TxValU(ToPercent(RC[ThrottleC], RC_MAXIMUM));
				TxString(",R:");TxValS(ToPercent(((RC[RollC]- RC_NEUTRAL) * 2L), RC_MAXIMUM));
				TxString(",P:");TxValS(ToPercent(((RC[PitchC]- RC_NEUTRAL) * 2L), RC_MAXIMUM));
				TxString(",Y:");TxValS(ToPercent(((RC[YawC]- RC_NEUTRAL) * 2L), RC_MAXIMUM));
				TxString(",5:");TxValU(ToPercent(RC[RTHC], RC_MAXIMUM));
				TxString(",6:");TxValS(ToPercent(((RC[CamPitchC] - RC_NEUTRAL) * 2L), RC_MAXIMUM));
				TxString(",7:");TxValU(ToPercent(RC[NavGainC], RC_MAXIMUM));
				ShowPrompt();
				break;
			case 'S' :	// show status
				ShowSetup(false);
				break;
			case 'X' :	// flight stats
				ShowStats();
				ShowPrompt();
				break;

			#ifdef TESTING
			case 'A' :	// linear sensor
				AccelerometerTest();
				ShowPrompt();
				break;
			case 'C':
				DoCompassTest();
				ShowPrompt();
				break;		
			case 'H':	// barometer
				BaroTest();
				ShowPrompt();
				break;
			case 'G':	// gyro
				GyroTest();
				ShowPrompt();
				break;
			case 'I':
				TxString("\r\nI2C devices ...\r\n");
				TxVal32(ScanI2CBus(),0,0);
				TxString(" device(s) found\r\n");
				ShowPrompt();
				break;	
			case 'K':
				CalibrateCompass();
				ShowPrompt();
				break;			
			case 'P'  :	// Receiver test			
				ReceiverTest();
				ShowPrompt();
				break;

			case 'Y':	// configure YGE30i EScs
				ConfigureESCs();
				ShowPrompt();
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				TxString("\r\nOutput test\r\n");
				#ifndef CLOCK_40MHZ
				TxChar(ch);
				TxChar(':');
				switch( ch ) {
					case '1': TxString("Aux2");  break;
					case '2': TxString("Blue");  break;
					case '3': TxString("Red");   break;
					case '4': TxString("Green"); break;
					case '5': TxString("Aux1");  break;
					case '6': TxString("Yellow");break;
					case '7': TxString("Aux3");  break;
					case '8': TxString("Beeper");  break;
				}
				TxNextLine();
				PowerOutput(ch-'1');
				#else
				TxString("Test deleted - no space\r\n");
				#endif // !CLOCK_40MHZ
				ShowPrompt();
				break;
			case 'T':
				LEDsAndBuzzer();
				ShowPrompt();
				break;
			#endif // TESTING
			case 'V' :	// Battery test
				BatteryTest();
				ShowPrompt();
				break;
			case '?'  :  // help
				TxString(SerHelp);
				ShowPrompt();
				break;
			default: break;
			}
		}
	}
} // ProcessCommand

