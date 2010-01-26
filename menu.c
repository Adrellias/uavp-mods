// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =                 Copyright (c) 2008 by Prof. Greg Egan               =
// =       Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer     =
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

void ShowPrompt(void);
void ShowRxSetup(void);
void ShowSetup(uint8);
void ProcessCommand(void);

#pragma idata menu1
const rom uint8 SerHello[] = "\r\nUAVX " Version " Copyright 2008 G.K. Egan & 2007 W. Mahringer\r\n"
							  "This is FREE SOFTWARE and comes with ABSOLUTELY NO WARRANTY\r\n"
							  "see http://www.gnu.org/licenses/!\r\n";

#pragma idata

#pragma idata menuhelp
const rom uint8 SerHelp[] = "\r\nCommands:\r\n"
	"A..Accelerometer test\r\n"
	"B..Load UAVX hex file\r\n"
	"C..Compass test\r\n"
	"D..Load default parameter set\r\n"
	"G..GPS test (Use HyperTerm)\r\n"
	"H..Barometer test\r\n"
	"I..I2C bus scan\r\n"
	"K..Calibrate Compass\r\n"
//	"M..Modify parameters\r\n"
	"P..Rx test\r\n"
	"S..Setup\r\n"
	"T..All LEDs and buzzer test\r\n"
	"V..Analog input test\r\n"
	"X..Flight stats\r\n"
	"Y..Program YGE I2C ESC\r\n"
//	"Z..Continuously display compass(Use HyperTerm)\r\n"
	"1-8..Individual LED/buzzer test\r\n"; // last line must be in this form for UAVPSet
#pragma idata

void ShowPrompt(void)
{
	TxString("\r\n>");
} // ShowPrompt

void ShowRxSetup(void)
{
	if ( F.UsingSerialPPM )
		if ( PPMPosPolarity[TxRxType] )
			TxString("Serial PPM frame (Positive Polarity)");
		else
			TxString("Serial PPM frame (Negative Polarity)");
	else
		TxString("Odd Rx Channels PPM");
} // ShowRxSetup

void ShowSetup(uint8 h)
{
	uint8 i;

	if( h )
	{
		TxString(SerHello);
		ParamSet = 1;	
	}

	TxString("\r\nUAVX V" Version " ready.\r\nAccelerometers ");

	if( F.AccelerationsValid )
		TxString("ONLINE\r\n");
	else
		TxString("not available\r\n");

	TxString("Compass ");
	if( F.CompassValid )
	{
		TxString("ONLINE (");
		TxVal32(COMPASS_OFFSET_DEG,0,0);
		TxString("deg. offset)\r\n");
	}
	else
		TxString("not available\r\n");

	TxString("Baro ");
	if ( F.BaroAltitudeValid )
		if ( BaroType == BARO_ID_BMP085 )
			TxString("BMP085 ONLINE\r\n");
		else
			TxString("SMD500 ONLINE\r\n");
	else
		TxString("not available\r\n");

	if ( F.UsingXMode )
		TxString("Orientation: X\r\n");
	else
		TxString("Orientation: +\r\n");
	switch ( P[GyroType] ) {
	case ADXRS300:TxString("Pitch/Roll Gyros: ADXRS610/300 or MLX90609\r\n"); break;
	case ADXRS150:TxString("Pitch/Roll Gyros: ADXRS613/150\r\n"); break;
	case IDG300:TxString("Pitch/Roll Gyros: IDG300\r\n"); break;
	}
	
	TxString("Motor ESCs: ");	
	switch ( P[ESCType] ) {
	case ESCPPM:TxString("PPM "); break;
	case ESCHolger:TxString("Holger I2C {"); break;
	case ESCX3D:TxString("X-3D I2C {"); break;
	case ESCYGEI2C:TxString("YGE I2C {"); break;
	}

	if ( P[ESCType] != ESCPPM )
	{
		for ( i = 0; i < NoOfMotors; i++ )
			if ( ESCI2CFail[i] )
				TxString(" Fail");
			else
				TxString(" OK");
		TxString(" }");
	}
	TxNextLine();	
	
	#ifdef RX6CH
	TxString("6 CHANNEL VERSION - 5 ACTIVE CHANNELS ONLY\r\n");
	#endif // RX6CH
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
	case GraupnerMX16s: TxString("Graupner MX16s {"); break;
	}
	
	if ( F.UsingSerialPPM )
		ShowRxSetup();
	else
	{	
		for ( i = 0; i < RC_CONTROLS; i++)
			TxChar(RxChMnem[RMap[i]-1]);
		
		TxString("} connect {");
		
		for ( i = 0; i < RC_CONTROLS; i+=2)
		{
			TxChar(RxChMnem[RMap[i]-1]);
			TxChar(' ');
		}
	}
	TxChar('}');
	if ( P[TxRxType] == DX7AR6200 )
		TxString(" Mix Rudder to Aux1 ");
	if ( F.UsingTxMode2 )
		TxString(" Tx Mode 2");
	else
		TxString(" Tx Mode 1");
	TxNextLine();

	TxString("Selected parameter set: ");
	TxChar('0' + ParamSet);	
	TxNextLine();

	TxString("\r\nRTH: \r\n");
	if ( F.UsingGPSAlt )
		TxString("\tGPS is altitude source\r\n");
	else
		TxString("\tBaro is altitude source\r\n");
	if ( F.UsingRTHAutoDescend )
		TxString("\tAuto descend ENABLED\r\n");
	else
		TxString("\tAuto descend disabled\r\n");

	if ( F.RTHAltitudeHold )
		TxString("\tAltitude hold enabled\r\n");
	else
		TxString("\tAltitude hold DISABLED\r\n");
	if ( F.TurnToHome )
		TxString("\tTurn towards home\r\n");
	else
		TxString("\tHold heading\r\n");

	TxString("\r\nALARM (if any):\r\n");
	if ( !F.Signal )
		TxString("\tRC signal invalid - bad EPAs or Tx may be switched off?\r\n");
	if ( Armed && FirstPass ) 
		TxString("\tUAVX is armed - disarm!\r\n");
	if ( F.ReturnHome )
		TxString("\tRTH is selected - deselect!\r\n");
	if ( InitialThrottle >= RC_THRES_START )
		TxString("\tThrottle is open - close throttle!\r\n");
	
	ShowPrompt();
} // ShowSetup

// if a command is waiting, read and process it.
// Do NOT call this routine while in flight!
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
				ch=toupper(ch);
			
			switch( ch )
			{
			case 'A' :	// linear sensor
				LinearTest();
				ShowPrompt();
				break;
			case 'B':	// call bootloader
				{ // arming switch must be OFF to call bootloader!!!
					DisableInterrupts;
					BootStart();		// never comes back!
				}
			case 'C':
				DoCompassTest();
				ShowPrompt();
				break;
			case 'D':
				UseDefaultParameters();
				ShowPrompt();
				break;
			case 'G' : // GPS test
				GPSTest();
				ShowPrompt();
				break;			
			case 'H':	// barometer
				BaroTest();
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
						if( ParamSet == 1 )
						{
							WriteEE(p, d);
							if ( ComParms[p] )
								WriteEE(MAX_PARAMETERS + p, d);
						}
						else
						{
							if ( !ComParms[p] )
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
				TxValS(NeutralLR);
		
				TxString("    P:");
				TxValS(NeutralFB);
		
				TxString("   V:");	
				TxValS(NeutralDU);
				ShowPrompt();
				break;
			case 'P'  :	// Receiver test			
				ReceiverTest();
				ShowPrompt();
				break;
			case 'R':	// receiver values
				TxString("\r\nT:");TxValU(ToPercent(RC[ThrottleC], RC_MAXIMUM));
				TxString(",R:");TxValS(ToPercent((RC[RollC]- RC_NEUTRAL) * 2, RC_MAXIMUM));
				TxString(",P:");TxValS(ToPercent((RC[PitchC]- RC_NEUTRAL) * 2, RC_MAXIMUM));
				TxString(",Y:");TxValS(ToPercent((RC[YawC]- RC_NEUTRAL) * 2, RC_MAXIMUM));
				TxString(",5:");TxValU(ToPercent(RC[RTHC], RC_MAXIMUM));
				TxString(",6:");TxValS(ToPercent((RC[CamPitchC] - RC_NEUTRAL) * 2, RC_MAXIMUM));
				TxString(",7:");TxValU(ToPercent(RC[NavGainC], RC_MAXIMUM));
				ShowPrompt();
				break;
			case 'S' :	// show status
				ShowSetup(0);
				break;
			case 'V' :	// analog test
				AnalogTest();
				ShowPrompt();
				break;
			case 'X' :	// analog test
				ShowStats();
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
				TxChar(ch);
				TxChar(':');
				switch( ch )
				{
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
				ShowPrompt();
				break;
			case 'T':
				LEDsAndBuzzer();
				ShowPrompt();
				break;
			case 'Z': // Do compass run of 32K readings
				// CompassRun();
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

