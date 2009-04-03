// ==============================================
// =    U.A.V.P Brushless UFO Test software     =
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
// =  please visit http://www.uavp.de           =
// =               http://www.mahringer.co.at   =
// ==============================================

// Text page

#pragma codepage=1
#pragma sharedAllocation

#include "pu-test.h"
#include "bits.h"

const char page2 SerHello[] = "\r\nU.A.V.P. TESTSOFTWARE V" Version " Copyright (c) 2007"
							  " Ing. Wolfgang Mahringer\r\n"
							  "U.A.V.P. comes with ABSOLUTELY NO WARRANTY\r\n"
							  "This is FREE SOFTWARE, see GPL license!\r\n\0"

// THE FOLLOWING LINE MUST REMAIN IN THIS FORM, it is important for UAVPset!
/* SerSetup */ 				  "\r\nProfi-Ufo TEST V" Version " ready.\r\n"
							  "Gyro: "
#ifdef OPT_ADXRS
							  "3x ADXRS300\r\n"
#endif
#ifdef OPT_IDG
							  "1x ADXRS300, 1x IDG300\r\n"
#endif
							  "Linear sensor \0"
/* SerLSavail */			  "ONLINE\r\n\0"
/* SerLSnone */				  "not available\r\n\0"

/* SerCompass */			  "Compass \0"
/* SerAlti */   			  "Barometer \0"

/* SerHelp */  				  "\r\nCommands:\r\n"
							  "A....analog voltages\r\n"
							  "B....call Bootloader\r\n"
							  "C....Compass test\r\n"
							  "K....Calibrate Compass sensor\r\n"
							  "I....I2C sensor bus scan\r\n"
							  "L....Linear sensor test\r\n"
							  "H....Barometer sensor test\r\n"
							  "N....negated RX-PPM\r\n"
							  "R....RX test\r\n"
							  "S....Show setup\r\n"
							  "V....Servo test (CAUTION!)\r\n"
#ifdef ESC_YGEI2CESC_YGEI2C_NOT_SUPPORTED
							  "Y....Configure YGE I2C ESCs\r\n"
#endif
							  "1-8.."
/* CAUTION: The following line MUST REMAIN THE LAST in that help string!
            This is for UAVPset to correctly read the available commands! */
							  "Power output test\r\n\0"

/* SerRxTest */				  "\r\nLast RX values received:\r\n\0"
/* SerRxNN */   			  "(no new values)\r\n\0"
/* SerRxRes */				  "received values are \0"
/* SerRxFail */				  "NOT valid!\r\n\0"
/* SerRxOK */ 				  "OK!\r\n\0"

/* SerAnTest */				  "\r\nAnalog channels test:\r\n\0"
/* SerLinTst */				  "\r\nLinear sensor test:\r\n\0"
/* SerLinErr */				  "\r\n(linear sensor not present)\r\n\0"

/* SerSrvRun */				  "\r\nServo test running...\r\n\0"

/* SerI2CRun */				  "\r\nI2C bus scan running, listing slave addresses...\r\n\0"
/* SerI2CCnt */				  " device(s) found\r\n\0"

/* SerMagTst */				  "\r\nCompass device test\r\n\0"

/* SerPowTst */				  "\r\nPower output test\r\n\0"
/* SerPowAux1 */  			  "Aux 1\0"
/* SerPowAux2 */  			  "Aux 2\0"
/* SerPowAux3 */  			  "Aux 3\0"
/* SerPowBlue */  			  "Blue LED\0"
/* SerPowRed */   			  "Red LED\0"
/* SerPowGreen */ 			  "Green LED\0"
/* SerPowYellow */			  "Yellow LED\0"
/* SerPowBeep */  			  "Beeper\0"

/* SerSrvOK */				  "\r\nDone.\r\n\0"
/* SerPrompt */				  "\r\n# \0"
/* SerVolt */				  " Volt\r\n\0"
/* SerFail */				  " FAILED\0"
/* SerGrad */				  " deg\r\n\0"
/* SerI2CFail */			  "I2C comm failed!\r\n\0"
/* SerMS */					  " ms\r\n\0"

/* SerPPMP */				  "\r\nSetting pos. RX PPM\r\n\0"
/* SerPPMN */				  "\r\nSetting neg. RX PPM\r\n\0"

/* SerLinG */				  " G\r\n\0"

/* SerBaroOK */				  "\r\nAir pressure is:\0"
/* SerBaroT */				  "; Temp is:\0"

/* SerCCalib1 */			  "\r\nCalibrate compass sensor. Press any key to continue\r\n\0"
/* SerCCalib2 */			  "\r\n720 deg turn in ca. 30 seconds!\r\nPress any key when done\r\n\0"
/* SerCCalib3 */			  "\r\nCalibration complete\r\n\0"
/* SerCCalibE */			  "\r\nCalibration error!\r\n\0"

/* SerBaroSMD500 */				  "\r\nSMD500\0"
/* SerBaroBMP085 */				  "\r\nBMP085\0"
/* SerBaroComp */				  "; Compensated:\0";

// transmit a fix text from a table
void SendComText(uns8 nitext)
{
	bank1 const char *pch;

	pch = SerHello;

	while( nitext )
	{
		if( *pch == '\0' )
			nitext--;
		pch++;
	}

	while( *pch != '\0' )
	{
		SendComChar(*pch);
		pch++;
	}
}

