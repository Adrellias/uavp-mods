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


// Serial support (RS232 option)

#include "c-ufo.h"
#include "bits.h"

// data strings

void TxNextLine(void)
{
	TxChar(0x0D);
	TxChar(0x0A);
} // TxNextLine

void ShowPrompt(void)
{
	TxText(SerPrompt);
} // ShowPrompt

// transmit a fix text from a table
void TxText(const uint8 *pch)
{
	while( *pch != '\0' )
	{
		TxChar(*pch);
		pch++;
	}
} // TxText

// send a character to the serial port
void TxChar(uint8 ch)
{
	while( PIR1bits.TXIF == 0 ) ;	// wait for transmit ready
	TXREG = ch;		// put new uint8
} // TxChar 


// converts an unsigned byte to decimal and send it
void TxValU(uint8 nival)
{
	uint16 i;

	i = nival / 100;
	TxChar(i +'0');
	nival %= 100;

	i = nival / 10;
	TxChar(i+'0');
	nival %= 10;

	TxChar(nival+'0');
} // TxValU


// converts a nibble to HEX and sends it
void TxNibble(uint8 nival)
{
	nival += '0';
	if( nival > '9' )
		nival += 7;		// A to F
	TxChar(nival);
}  // TxNibble

// converts an unsigned byte to HEX and sends it
void TxValH(uint8 nihex)
{
	TxNibble(nihex >> 4);
	TxNibble(nihex & 0x0F);
} // TxValH

// converts an unsigned long to decimal and send it
void TxValUL(uint8 niformat)
{
	int24 Temp, v;
	uint8 ninull=1;

	v = val;

	if( niformat & VZ )
	{
		if( v >= 0 )
			TxChar('+');
		else
		{
			TxChar('-');
			v = -v;
		}
	}
	
	Temp = v / 10000;
	if( (Temp == 0) && (ninull == 1) && 
		((niformat & NKSMASK) < NKS4) )
	{
		if( (niformat & LENMASK) >= LEN5 )
			TxChar(' ');
	}
	else
	{
		TxChar(Temp+'0');
		ninull = 0;
	}
	v %= 10000;

	if( (niformat & NKSMASK) == NKS4 )
		TxChar('.');

	Temp = v / 1000;
	if( (Temp == 0) && (ninull == 1)  && 
		((niformat & NKSMASK) < NKS3) )
	{
		if( (niformat & LENMASK) >= LEN4 )
			TxChar(' ');
	}
	else
	{
		TxChar(Temp+'0');
		ninull = 0;
	}
	v %= 1000;

	if( (niformat & NKSMASK) == NKS3 )
		TxChar('.');

	Temp = v / 100;
	if( (Temp == 0) && (ninull == 1) && 
		((niformat & NKSMASK) < NKS2) )
	{
		if( (niformat & LENMASK) >= LEN3 )
			TxChar(' ');
	}
	else
	{
		TxChar(Temp+'0');
		ninull = 0;
	}
	v %= 100;

	if( (niformat & NKSMASK) == NKS2 )
		TxChar('.');

	Temp = v / 10;
	if( (Temp == 0) && (ninull == 1) && 
		((niformat & NKSMASK) < NKS1)  )
	{
		if( (niformat & LENMASK) >= LEN2 )
			TxChar(' ');
	}
	else
	{
		TxChar(Temp+'0');
		ninull = 0;
	}
	v %= 10;

	if( (niformat & NKSMASK) == NKS1 )
		TxChar('.');

	TxChar(v+'0');
} // TxValUL

// if a character is in the buffer
// return it. Else return the NUL character
uint8 RxChar(void)
{
	uint8 chread;

	if( PIR1bits.RCIF )	// a character is waiting in the buffer
	{
		if( RCSTAbits.OERR || RCSTAbits.FERR )	// overrun or framing error?
		{
			RCSTAbits.CREN = 0;	// diable, then re-enable port to
			RCSTAbits.CREN = 1;	// reset OERR and FERR bit
			W = RCREG;	// dummy read
		}
		else
		{
			chread = RCREG;	// get the character
			TxChar(chread);	// echo it
			return(chread);		// and return it
		}
	}
	return( '\0' );	// nothing in buffer
} // RxChar

// send the current configuration setup to serial port
void ShowSetup(uint8 h)
{

	if( h )
		TxText(SerHello);

	TxText(SerSetup);	// send hello message
	if( _UseLISL )
		TxText(SerLSavail);
	else
		TxText(SerLSnone);


// check for compass device
	TxText(SerCompass);	// send hello message
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) == I2C_ACK ) 
		TxText(SerLSavail);
	else
		TxText(SerLSnone);
	I2CStop();

// check for altimeter device
	TxText(SerAlti);	// send hello message
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) == I2C_ACK ) 
		TxText(SerLSavail);
	else
		TxText(SerLSnone);
	I2CStop();

	ShowPrompt();
} // ShowSetup

// if a command is waiting, read and process it.
// Do NOT call this routine while in flight!
void ProcessComCommand(void)
{
    int8 *p;
	uint8 i, chcmd;

	chcmd = RxChar();
	if( islower(chcmd))							// check lower case
		chcmd=toupper(chcmd);

	switch( chcmd )
	{
		case '\0' : break;
		case 'R'  :	// Receiver test
			TxText(SerRxTest);
			if( _NewValues )
			{
				ReceiverTest();
				_NewValues = 0;

				if( _NoSignal )
					TxText(SerFail);
				else
					TxText(SerOK);
			}
			else
				TxText(SerRxNN);
			ShowPrompt();
			break;
		case 'A' :	// analog test
			TxText(SerAnTest);
			AnalogTest();
			ShowPrompt();
			break;
		case 'L' :	// linear sensor
			TxText(SerLinTst);
			if( _UseLISL )
				LinearTest();
			else
				TxText(SerLinErr);
			ShowPrompt();
			break;
		case 'I':
			TxText(SerI2CRun);
			i = ScanI2CBus();
			TxValU(ScanI2CBus());
			TxText(SerI2CCnt);
			ShowPrompt();
			break;
		case 'C':
			TxText(SerMagTst);
			DoCompassTest();
			ShowPrompt();
			break;
		case 'K':
			TxText(SerCCalib1);
			CalibrateCompass();
			ShowPrompt();
			break;
		case 'H':	// barometer
			while( !RxChar() )
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

			TxText(SerPowTst);
			TxChar(chcmd);
			TxChar(':');
			switch( chcmd )
			{
				case '1': TxText(SerPowAux2);  break;
				case '2': TxText(SerPowBlue);  break;
				case '3': TxText(SerPowRed);   break;
				case '4': TxText(SerPowGreen); break;
				case '5': TxText(SerPowAux1);  break;
				case '6': TxText(SerPowYellow);break;
				case '7': TxText(SerPowAux3);  break;
				case '8': TxText(SerPowBeep);  break;
			}
			TxNextLine();
			PowerOutput(chcmd-'1');
			ShowPrompt();
			break;

		case 'N':	// toggle PPM polarity
			TogglePPMPolarity();
			ShowPrompt();
			break;

		case 'B':	// call bootloader
			TxNextLine();
			BootStart();	// never comes back!
		case '?'  : // help
			TxText(SerHelp);
			ShowPrompt();
	}
} // ProcessComCommand

