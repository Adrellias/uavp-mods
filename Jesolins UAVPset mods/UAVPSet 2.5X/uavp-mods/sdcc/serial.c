// ==============================================
// =      U.A.V.P Brushless UFO Controller      =
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
// =  please visit http://www.uavp.org          =
// =               http://www.mahringer.co.at   =
// ==============================================

// Serial support (RS232 option)

// this is required on CC5X V3.3
typedef char CHAR;

#pragma codepage=3
#pragma sharedAllocation

#include "c-ufo.h"
#include "bits.h"

// Math Library
#include "mymath16.h"

// data strings

const char page2 SerHello[] = "\r\nU.A.V.P. V" Version " (c) 2007"
							  " Ing. Wolfgang Mahringer\r\n"
							  "This is FREE SOFTWARE, see GPL license!\r\n";

const char page2 SerSetup[] = "\r\nProfi-Ufo V" Version " ready.\r\n"
							  "Gyro: "
#ifdef OPT_ADXRS300
							  "3x ADXRS300\r\n"
#endif
#ifdef OPT_ADXRS150
							  "3x ADXRS150\r\n"
#endif
#ifdef OPT_IDG
							  "1x ADXRS300, 1x IDG300\r\n"
#endif
							  "Linear sensors ";
const char page2 SerLSavail[]="ONLINE\r\n";
const char page2 SerLSnone[]= "not available\r\n";
const char page2 SerBaro[]=   "Baro sensor ";
const char page2 SerChannel[]="Channel mode: Throttle Ch";
const char page2 SerFM_Fut[]= "3";
const char page2 SerFM_Grp[]= "1";

#ifdef BOARD_3_1
const char page2 SerCompass[]="Compass sensor ";
#endif

const char page2 SerHelp[]  = "\r\nCommands:\r\n"
					 		  "L...List param\r\n"
							  "M...Modify param\r\n"
							  "S...Show setup\r\n"
							  "N...Neutral values\r\n"
							  "R...Show receiver channels\r\n"
							  "B...start Boot-Loader\r\n";
const char page2 SerReg1[]  = "\r\nRegister ";
const char page2 SerReg2[]  = " = ";
const char page2 SerPrompt[]= "\r\n>";
// THE FOLLOWING LINE NOT TO BE CHANGED, it is important for UAVPset
const char page2 SerList[]  = "\r\nParameter list for set #";
const char page2 SerSelSet[]= "\r\nSelected parameter set: ";

const char page2 SerNeutralR[]="\r\nNeutral Roll:";
const char page2 SerNeutralN[]=" Nick:";
const char page2 SerNeutralY[]=" Yaw:";

const char page2 SerRecvCh[]=  "\r\nT:";

// transmit a fix text from a table
void SendComText(const char *pch)
{
	while( *pch != '\0' )
	{
		SendComChar(*pch);
		pch++;
	}
}

void ShowPrompt(void)
{
	SendComText(SerPrompt);
}

// send a character to the serial port
void SendComChar(char W)
{
	while( TXIF == 0 ) ;	// wait for transmit ready
	TXREG = W;		// put new char
	// register W must be retained on exit!!!!
}

static uns8 nival;
static char ch;

// converts an unsigned byte to decimal and send it
void SendComValU(uns8 W)
{
	nival = W;

	W = nival / 100;
	SendComChar(W+'0');
	nival %= 100;		// Einsparpotential: Modulo als Mathlib

	W = nival / 10;
	SendComChar(W+'0');
	nival %= 10;

	SendComChar(nival+'0');
}

// converts a nibble to HEX and sends it
void SendComNibble(uns8 W)
{
	nival = W + '0';
	if( nival > '9' )
		nival += 7;		// A to F
	SendComChar(nival);
}

// converts an unsigned byte to HEX and sends it
void SendComValH(uns8 W)
{
	uns8 nival2;

	nival2 = W;
	SendComNibble(nival2 >> 4);
	SendComNibble(nival2 & 0x0F);
}

// converts a signed byte to decimal and send it
// because of dumb compiler nival must be declared as unsigned :-(
void SendComValS(uns8 W)
{
	nival = W;
	if( (int)nival < 0 )
	{
		SendComChar('-');	// send sign
		nival = -(int)nival;
	}
	else
		SendComChar('+');	// send sign

	SendComValU(nival);
}

// if a character is in the buffer
// return it. Else return the NUL character
char RecvComChar(void)
{
	
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
			W = RCREG;	// get the character
			SendComChar(W);	// echo it
			return(W);		// and return it
		}
	}
	return( '\0' );	// nothing in buffer
}


// enter an unsigned number 00 to 99
uns8 RecvComNumU(void)
{

	nival = 0;
	do
	{
		ch = RecvComChar();
	}
	while( (ch < '0') || (ch > '9') );
	nival = ch - '0';
	nival *= 10;
	do
	{
		ch = RecvComChar();
	}
	while( (ch < '0') || (ch > '9') );
	nival += ch - '0';
	return(nival);
}


// enter a signed number -99 to 99 (always 2 digits)!
int RecvComNumS(void)
{
	nival = 0;

	_NegIn = 0;
	do
	{
		ch = RecvComChar();
	}
	while( ((ch < '0') || (ch > '9')) &&
           (ch != '-') );
	if( ch == '-' )
	{
		_NegIn = 1;
		do
		{
			ch = RecvComChar();
		}
		while( (ch < '0') || (ch > '9') );
	}
	nival = ch - '0';
	nival *= 10;

	do
	{
		ch = RecvComChar();
	}
	while( (ch < '0') || (ch > '9') );
	nival += ch - '0';
	if( _NegIn )
		nival = -nival;
	return(nival);
}

// send the current configuration setup to serial port
void ShowSetup(uns8 W)
{
	if( W )
	{
		SendComText(SerHello);
		IK5 = _Minimum;	
	}

	SendComText(SerSetup);	// send hello message
	if( _UseLISL )
		SendComText(SerLSavail);
	else
		SendComText(SerLSnone);

#ifdef BOARD_3_1
	SendComText(SerCompass);
	if( _UseCompass )
		SendComText(SerLSavail);
	else
		SendComText(SerLSnone);

	SendComText(SerBaro);
	if( _UseBaro )
		SendComText(SerLSavail);
	else
		SendComText(SerLSnone);
#endif

	ReadEEdata();
	SendComText(SerChannel);
	if( FutabaMode )
		SendComText(SerFM_Fut);
	else
		SendComText(SerFM_Grp);

	SendComText(SerSelSet);
	if( IK5 > _Neutral )
		SendComChar('2');
	else
		SendComChar('1');
	
	ShowPrompt();
}

void ProgRegister(void)
{
	EEPGD = 0;
	WREN = 1;		// enable eeprom writes
	GIE = 0;
	EECON2 = 0x55;	// fix prog sequence (see 16F628A datasheet)
	EECON2 = 0xAA;
	WR = 1;			// start write cycle
	GIE = 1;
	while( WR == 1 );	// wait to complete
	WREN = 0;	// disable EEPROM write
}

long nila1@nilarg1;

// if a command is waiting, read and process it.
// Do NOT call this routine while in flight!
void ProcessComCommand(void)
{
    int size1 *p;
	uns8 nireg;
	
	nireg = RecvComChar();
	if( nireg.6 )	// 0x40..0x7F, a character
		nireg.5=0;
	switch( nireg )
	{
		case '\0' : break;
		case 'L'  :	// List parameters
			SendComText(SerList);	// must send it (UAVPset!)
			if( IK5 > _Neutral )
				SendComChar('2');
			else
				SendComChar('1');
			ReadEEdata();
			nireg = 1;
			for(p = &FirstProgReg; p <= &LastProgReg; p++)
			{
				SendComText(SerReg1);
				SendComValU(nireg);
				SendComText(SerReg2);
				SendComValS(*p);
				nireg++;
			}
			ShowPrompt();
			break;
		case 'M'  : // modify parameters
			LedBlue_ON;
			SendComText(SerReg1);
			nireg = RecvComNumU();
			nireg--;
			SendComText(SerReg2);	// = 
			EEDATA = RecvComNumS();
			if( IK5 > _Neutral )
				nireg += _EESet2;
			EEADR = nireg;
// prog values into data flash
			ProgRegister();

// if config register on set #1 is progged,
// write through the transmitter config bits to set #2
			if( nireg == 15 /* = &ConfigParam - &FirstProgReg */ )
			{
				nival &= 0x12;	// read the programmed value
					// mask only bits _FutabaMode and _NegativePPM
				EEADR += _EESet2;	// goto set #2
// da gehts no
				RD = 1;
// da niimer
				EEDATA &= 0xED;
				EEDATA |= nival;

				ProgRegister();	// write to set#2 config reg
			}
			LedBlue_OFF;
			ShowPrompt();
			break;
		case 'S' :	// show status
			ShowSetup(0);
			break;
		case 'N' :	// neutral values
			SendComText(SerNeutralR);
			SendComValS(NeutralLR);

			SendComText(SerNeutralN);
			SendComValS(NeutralFB);

			SendComText(SerNeutralY);
			Tp -= 1024;		// subtract 1g (vertical sensor)
			SendComValS(NeutralUD);
			ShowPrompt();
			break;
		case 'R':	// receiver values
			SendComText(SerRecvCh);
			SendComValU(IGas);
			SendComChar(',');
			SendComChar('R');
			SendComChar(':');
			SendComValS(IRoll);
			SendComChar(',');
			SendComChar('N');
			SendComChar(':');
			SendComValS(INick);
			SendComChar(',');
			SendComChar('Y');
			SendComChar(':');
			SendComValS(ITurn);
			SendComChar(',');
			SendComChar('5');
			SendComChar(':');
			SendComValU(IK5);
			SendComChar(',');
			SendComChar('6');
			SendComChar(':');
			SendComValU(IK6);
			SendComChar(',');
			SendComChar('7');
			SendComChar(':');
			SendComValU(IK7);
			ShowPrompt();
			break;

		case 'B':	// call bootloader
#asm
			movlw	0x1f
			movwf	PCLATH
			dw	0x2F00
#endasm
//			BootStart();	// never comes back!
		
#ifndef TESTOUT	
		case 'T':
			RE = 10;
			NE = 20;
			Rw = 30;
			Nw = 40;
			MatrixCompensate();
			ShowPrompt();
			break;
#endif

		case '?'  : // help
			SendComText(SerHelp);
			ShowPrompt();
	}
}

