// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =     Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer       =
// =                          http://www.uavp.org                        =
// =======================================================================

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

// Menu for Test Software

#include "c-ufo.h"
#include "bits.h"

#pragma idata menuhello
const uint8  SerHello[] = "\r\nProfi-Ufo TEST V" Version "\r\nCopyright (c) 2007-9"
							  " G.K. Egan & W. Mahringer\r\n"
							  "Comes with ABSOLUTELY NO WARRANTY\r\n"
							  "This is FREE SOFTWARE, see GPL license!\r\n";
#pragma idata
#pragma idata menusetup
// THE FOLLOWING LINE MUST REMAIN IN THIS FORM, it is important for UAVPset!
const uint8  SerSetup[] = "\r\nProfi-Ufo TEST V" Version " ready.\r\n"

#ifdef DEBUG_SENSORS
	"Debug: Sensors\r\n"
#endif
#ifdef DEBUG_MOTORS
	"Debug: Motors\r\n"
#endif

#ifdef RX_DEFAULT
	"Rx: PPM Odd Channels\r\n"
#endif
#ifdef RX_PPM
	"Rx: PPM Composite\r\n"
#endif
#ifdef RX_DSM2
	"Rx: PPM DSM2\r\n"
#endif

#ifdef ESC_PPM
	"ESC: PPM\r\n"
#endif
#ifdef ESC_YGEI2C
	"ESC: YGE I2C\r\n"
#endif
#ifdef ESC_HOLGER
	"ESC: Holger I2C\r\n"
#endif

	"Gyros: "
#ifdef OPT_ADXRS300
	"ADXRS300 (Roll/Pitch/Yaw)\r\n"
#endif
#ifdef OPT_ADXRS150
	"ADXRS150 (Roll/Pitch/Yaw)\r\n"
#endif
#ifdef OPT_IDG
	"ADXRS300 (Yaw), IDG300 (Roll/Pitch)\r\n"
#endif
	"Accelerometers ";
#pragma idata

#pragma idata menuhelp
const uint8 SerHelp[] = "\r\nCommands:\r\n"
	"A..Analog ch.\r\n"
	"B..Boot\r\n"
	"C..Compass test\r\n"
	"K..Calib. Compass\r\n"
	#ifdef USE_GPS
	"G..GPS test\r\n"
	#endif
	"H..Baro. test\r\n"
	"I..I2C bus scan\r\n"
	"L..Linear test\r\n"
	"N..Negate Rx-PPM\r\n"
	"R..RX test\r\n"
	"S..Setup\r\n"
	#ifdef ESC_YGEI2C
	"Y..Prog. YGE\r\n"
	#endif
	"1-8..Power output test\r\n"; // last line must be in this form for UAVPSet

void ShowPrompt(void)
{
	TxString("\r\n# ");
} // ShowPrompt

// send the current configuration setup to serial port
void ShowSetup(uint8 h)
{
	if( h )
		TxText(SerHello);

	TxText(SerSetup);	// send hello message
	if( _UseLISL )
		TxString("ONLINE\r\n\0");
	else
		TxString("not available\r\n");

	// check for compass device
	TxString("Compass ");	// send hello message
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) == I2C_ACK ) 
		TxString("ONLINE\r\n");
	else
		TxString("not available\r\n");
	I2CStop();

	// check for altimeter device
	TxString("Barometer ");	// send hello message
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) == I2C_ACK ) 
		TxString("ONLINE\r\n");
	else
		TxString("not available\r\n");
	I2CStop();

	ShowPrompt();
} // ShowSetup

// if a command is waiting, read and process it.
// Do NOT call this routine while in flight!
void ProcessComCommand(void)
{
    int8 *p;
	uint8 i, ch;

	ch = PollRxChar();

	if ( ch != NUL )
	{
		if( islower(ch))
			ch=toupper(ch);
	
		switch( ch )
		{
			case NUL : break;
			case 'R'  :	// Receiver test
				ReceiverTest();
				ShowPrompt();
				break;
			case 'A' :	// analog test
				AnalogTest();
				ShowPrompt();
				break;
			#ifdef USE_GPS
			case 'G' : // GPS test
				GPSTest();
				ShowPrompt();
				break;
			#endif // USE_GPS			
			case 'L' :	// linear sensor
				LinearTest();
				ShowPrompt();
				break;
			case 'I':
				TxString("\r\nI2C devices ...\r\n");
				TxVal32(ScanI2CBus(),0,0);
				TxString(" device(s) found\r\n");
				ShowPrompt();
				break;
			case 'C':
				DoCompassTest();
				ShowPrompt();
				break;
			case 'K':
				CalibrateCompass();
				ShowPrompt();
				break;
			case 'H':	// barometer
				BaroTest();
				ShowPrompt();
				break;	
			case 'S' :	// show status
				ShowSetup(0);
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
	
			case 'N':	// toggle PPM polarity
				TogglePPMPolarity();
				ShowPrompt();
				break;

		#ifdef ESC_YGEI2C
		case 'Y':	// configure YGE30i EScs
			ConfigureESCs();
			ShowPrompt();
			break;
		#endif // ESC_YGEI2C

			case 'B':	// call bootloader
				TxNextLine();
				BootStart();
			case '?'  : // help
				TxText(SerHelp);
				ShowPrompt();
		}
	}
} // ProcessComCommand

