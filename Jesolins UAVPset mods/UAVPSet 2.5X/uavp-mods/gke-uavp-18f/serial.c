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

// USART routines

#include "c-ufo.h"
#include "bits.h"

void TxText(const uint8 *pch)
{
	while( *pch != '\0' )
		TxChar(*pch++);

} // TxText

void TxString(const rom uint8 *pch)
{
	while( *pch != '\0' )
		TxChar(*pch++);

} // TxString

void TxChar(uint8 ch)
{
	while( !PIR1bits.TXIF ) ;	// wait for transmit ready
	TXREG = ch;		// put new char
} // TxChar

void TxValU(uint8 v)
{	
	TxChar((v/100) + '0');
	v %= 100;	

	TxChar((v/10) + '0');
	v %= 10;

	TxChar(v + '0');
} // TxValU

void TxValS(int8 v)
{
	if( v < 0 )
	{
		TxChar('-');	// send sign
		v = -v;
	}
	else
		TxChar('+');	// send sign

	TxValU(v);
} // TxValS

void TxNextLine(void)
{
	TxChar(CR);
	TxChar(LF);
} // TxNextLine

void TxNibble(uint8 v)
{
	if ( v > 9)
		TxChar('A' + v - 10);
	else
		TxChar('0' + v);
} // TxNibble

void TxValH(uint8 v)
{
	TxNibble(v >> 4);
	TxNibble(v & 0x0f);
} // TxValH

void TxValH16(uint16 v)
{
	TxValH(v>>8);
	TxValH(v);
} // TxValH16

uint8 RxChar(void)
{
	uint8 ch;
	
	if( PIR1bits.RCIF )	// a character is waiting in the buffer
	{
		if( RCSTAbits.OERR || RCSTAbits.FERR )	// overrun or framing error?
		{
			RCSTAbits.CREN = false;	// disable, then re-enable port to
			RCSTAbits.CREN = true;	// reset OERR and FERR bit
			ch = RCREG;				// dummy read
		}
		else
		{
			ch = RCREG;				// get the character
			TxChar(ch);				// echo it
			return(ch);				// and return it
		}
	}
	return( NUL );					// nothing in buffer
} // RxChar


// enter an uintigned number 00 to 99
uint8 RxNumU(void)
{
	uint8 ch;
	uint8 n;

	n = 0;
	do
	{
		ch = RxChar();
	}
	while( (ch < '0') || (ch > '9') );
	n = ch - '0';
	n *= 10;
	do
	{
		ch = RxChar();
	}
	while( (ch < '0') || (ch > '9') );
	n += ch - '0';
	return(n);
} // RxNumU


// enter a signed number -99 to 99 (always 2 digits)!
int8 RxNumS(void)
{
	uint8 ch;
	int8 n;

	n = 0;

	_NegIn = false;
	do
	{
		ch = RxChar();
	}
	while( ((ch < '0') || (ch > '9')) &&
           (ch != '-') );
	if( ch == '-' )
	{
		_NegIn = true;
		do
		{
			ch = RxChar();
		}
		while( (ch < '0') || (ch > '9') );
	}
	n = ch - '0';
	n *= 10;

	do
	{
		ch = RxChar();
	}
	while( (ch < '0') || (ch > '9') );
	n += ch - '0';
	if( _NegIn )
		n = -n;
	return(n);
} // RxNumS


#ifdef TEST_SOFTWARE

void TxVal32(int32 V, int8 dp, uint8 Separator)
{
	uint8 S[12];
	int8 c, Rem, zeros;
	int32 NewV, i;
	 
	if (V<0)
	{
		TxChar('-');
	    V=-V;
	}
	else
		TxChar(' ');
	
	c=0;
	do
	{
	    NewV=V/10;
	    Rem=V-(NewV*10);
	    S[c++]=Rem + '0';
	    V=NewV;
	}
	while (V>0);
	  
	if ((c<(dp+1)) && (dp>0))
	{
	    TxChar('0');
	    TxChar('.');
	} 

	zeros = (int8)dp-c-1;
	if ( zeros >= 0 ) 
		for (i=zeros; i>=0; i--)
			TxChar('0');

	do
	{
	    c--;
	    TxChar(S[c]);
	    if ((c==dp)&&(c>0)) 
	      TxChar('.');
	}
	while (c>0);

	if ( Separator != NUL )
		TxChar(Separator);
} // TxVal32

#endif // TEST_SOFTWARE

