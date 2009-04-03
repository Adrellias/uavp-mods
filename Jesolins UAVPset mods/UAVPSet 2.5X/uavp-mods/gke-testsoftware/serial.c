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

// Serial support (RS232 option)

#pragma codepage=0
#pragma sharedAllocation

#include "pu-test.h"
#include "bits.h"
// Math Library
#include "mymath16.h"

bank1 uns16	nilgval;

// data strings

void SendComCRLF(void)
{
	SendComChar(0x0D);
	SendComChar(0x0A);
}

void ShowPrompt(void)
{
	SendComText(_SerPrompt);
}

// send a character to the serial port
void SendComChar(char W)
{
	while( TXIF == 0 ) ;	// wait for transmit ready
	TXREG = W;		// put new char
}


// converts an unsigned byte to decimal and send it
void SendComValU(uns8 nival)
{

	W = nival / 100;
	SendComChar(W +'0');
	nival %= 100;

	W = nival / 10;
	SendComChar(W+'0');
	nival %= 10;

	SendComChar(nival+'0');
}


// converts a nibble to HEX and sends it
void SendComNibble(uns8 nival)
{
	nival += '0';
	if( nival > '9' )
		nival += 7;		// A to F
	SendComChar(nival);
}

// converts an unsigned byte to HEX and sends it
void SendComValH(uns8 nihex)
{
	SendComNibble(nihex >> 4);
	SendComNibble(nihex & 0x0F);
}

/*
// converts a signed byte to decimal and send it
void SendComValS(int nival)
{
	if( nival < 0 )
	{
		SendComChar('-');	// send sign
		nival = -nival;
	}
	else
		SendComChar('+');	// send sign

	SendComValU((uns8)nival);
}
*/
// converts an unsigned long to decimal and send it
void SendComValUL(uns8 niformat)
{
//	bank1 char nidigit;
	bank1 bit ninull=1;

	if( niformat & VZ )
	{
		if( (long)nilgval >= 0 )
			SendComChar('+');
		else
		{
			SendComChar('-');
			(long)nilgval = -(long)nilgval;
		}
	}
	
	W = nilgval / 10000;
	if( (W == 0) && (ninull == 1) && 
		((niformat & NKSMASK) < NKS4) )
	{
		if( (niformat & LENMASK) >= LEN5 )
			SendComChar(' ');
	}
	else
	{
		SendComChar(W+'0');
		ninull = 0;
	}
	nilgval %= 10000;

	if( (niformat & NKSMASK) == NKS4 )
		SendComChar('.');

	W = nilgval / 1000;
	if( (W == 0) && (ninull == 1)  && 
		((niformat & NKSMASK) < NKS3) )
	{
		if( (niformat & LENMASK) >= LEN4 )
			SendComChar(' ');
	}
	else
	{
		SendComChar(W+'0');
		ninull = 0;
	}
	nilgval %= 1000;

	if( (niformat & NKSMASK) == NKS3 )
		SendComChar('.');

	W = nilgval / 100;
	if( (W == 0) && (ninull == 1) && 
		((niformat & NKSMASK) < NKS2) )
	{
		if( (niformat & LENMASK) >= LEN3 )
			SendComChar(' ');
	}
	else
	{
		SendComChar(W+'0');
		ninull = 0;
	}
	nilgval %= 100;

	if( (niformat & NKSMASK) == NKS2 )
		SendComChar('.');

	W = nilgval / 10;
	if( (W == 0) && (ninull == 1) && 
		((niformat & NKSMASK) < NKS1)  )
	{
		if( (niformat & LENMASK) >= LEN2 )
			SendComChar(' ');
	}
	else
	{
		SendComChar(W+'0');
		ninull = 0;
	}
	nilgval %= 10;

	if( (niformat & NKSMASK) == NKS1 )
		SendComChar('.');

	SendComChar(nilgval+'0');
}

// if a character is in the buffer
// return it. Else return the NUL character
char RecvComChar(void)
{
	char chread;

	if( RCIF )	// a character is waiting in the buffer
	{
		if( OERR || FERR )	// overrun or framing error?
		{
			CREN = 0;	// diable, then re-enable port to
			CREN = 1;	// reset OERR and FERR bit
			W = RCREG;	// dummy read
		}
		else
		{
			chread = RCREG;	// get the character
			SendComChar(chread);	// echo it
			return(chread);		// and return it
		}
	}
	return( '\0' );	// nothing in buffer
}

/*
static bank0 uns8 i;
static bank0 char chread;

// enter an unsigned number 00 to 99
uns8 RecvComNumU(void)
{

	i = 0;
	do
	{
		chread = RecvComChar();
	}
	while( (chread < '0') || (chread > '9') );
	i = chread - '0';
	do
	{
		chread = RecvComChar();
	}
	while( (chread < '0') || (chread > '9') );
	i *= 10;
	i += chread - '0';
	return(i);
}

// enter a signed number -99 to 99 (always 2 digits)!
int RecvComNumS(void)
{
	i = 0;

	_NegIn = 0;
	do
	{
		chread = RecvComChar();
	}
	while( ((chread < '0') || (chread > '9')) &&
           (chread != '-') );
	if( chread == '-' )
	{
		_NegIn = 1;
		do
		{
			chread = RecvComChar();
		}
		while( (chread < '0') || (chread > '9') );
	}
	i = chread - '0';
	do
	{
		chread = RecvComChar();
	}
	while( (chread < '0') || (chread > '9') );
	i *= 10;
	i += chread - '0';
	if( _NegIn )
		i = -i;
	return(i);
}
*/

// send the current configuration setup to serial port
void ShowSetup(uns8 W)
{

	if( W )
		SendComText(_SerHello);

	SendComText(_SerSetup);	// send hello message
	if( _UseLISL )
		SendComText(_SerLSavail);
	else
		SendComText(_SerLSnone);


// check for compass device
	SendComText(_SerCompass);	// send hello message
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) == I2C_ACK ) 
		SendComText(_SerLSavail);
	else
		SendComText(_SerLSnone);
	I2CStop();

// check for altimeter device
	SendComText(_SerAlti);	// send hello message
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) == I2C_ACK ) 
		SendComText(_SerLSavail);
	else
		SendComText(_SerLSnone);
	I2CStop();

	ShowPrompt();
}

// if a command is waiting, read and process it.
// Do NOT call this routine while in flight!
void ProcessComCommand(void)
{
    int size1 *p;
	char chcmd;

	chcmd = RecvComChar();
	if( chcmd.6 )	// 0x40..0x7F, a character
		chcmd.5=0;	// convert to uppercase

	switch( chcmd )
	{
		case '\0' : break;
		case 'R'  :	// Receiver test
			SendComText(_SerRxTest);
			if( _NewValues )
			{
				ReceiverTest();
				_NewValues = 0;
				SendComText(_SerRxRes);
				if( _NoSignal )
					SendComText(_SerRxFail);
				else
					SendComText(_SerRxOK);
			}
			else
				SendComText(_SerRxNN);
			ShowPrompt();
			break;
		case 'A' :	// analog test
			SendComText(_SerAnTest);
			AnalogTest();
			ShowPrompt();
			break;
		case 'L' :	// linear sensor
			SendComText(_SerLinTst);
			if( _UseLISL )
				LinearTest();
			else
				SendComText(_SerLinErr);
			ShowPrompt();
			break;
		case 'I':
			SendComText(_SerI2CRun);
//			i = ScanI2CBus();
			SendComValU(ScanI2CBus());
			SendComText(_SerI2CCnt);
			ShowPrompt();
			break;
		case 'C':
			SendComText(_SerMagTst);
			CompassTest();
			ShowPrompt();
			break;
		case 'K':
			SendComText(_SerCCalib1);
			CalibrateCompass();
			ShowPrompt();
			break;
		case 'H':	// barometer
			ReadEEdata();
			BaroTest();
			ShowPrompt();
			break;	
		case 'S' :	// show status
			ShowSetup(0);
//			ShowPrompt();
			break;
		case 'V' :	// servo test
			SendComText(_SerSrvRun);
			TestServos();
			SendComText(_SerSrvOK);
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

			SendComText(_SerPowTst);
			SendComChar(chcmd);
			SendComChar(':');
			switch( chcmd )
			{
				case '1': SendComText(_SerPowAux2);  break;
				case '2': SendComText(_SerPowBlue);  break;
				case '3': SendComText(_SerPowRed);   break;
				case '4': SendComText(_SerPowGreen); break;
				case '5': SendComText(_SerPowAux1);  break;
				case '6': SendComText(_SerPowYellow);break;
				case '7': SendComText(_SerPowAux3);  break;
				case '8': SendComText(_SerPowBeep);  break;
			}
			SendComCRLF();
			PowerOutput(chcmd-'1');
			ShowPrompt();
			break;

		case 'N':	// toggle PPM polarity
			TogglePPMPolarity();
			ShowPrompt();
			break;

#ifdef ESC_YGEI2C_NOT_SUPPORTED
		case 'Y':	// configure YGE30i EScs
			ConfigureESCs();
			ShowPrompt();
			break;
#endif
		case 'B':	// call bootloader
			SendComCRLF();
#asm
			movlw	0x1f
			movwf	PCLATH
			dw	0x2F00
#endasm
			BootStart();	// never comes back!
		case '?'  : // help
			SendComText(_SerHelp);
			ShowPrompt();
	}
}

