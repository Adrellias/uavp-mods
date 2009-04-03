// ==============================================
// =    U.A.V.P Brushless UFO Test-Software     =
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

// Utilities and subroutines

#pragma codepage=1

#include "pu-test.h"
#include "bits.h"

// Math Library
#include "mymath16.h"


// read accu voltage using 8 bit A/D conversion
// Bit _LowBatt is set if voltage is below threshold
uns16 GetAnalogVal(uns8 nich)
{
	uns16 niltemp;

	ADFM = 1;					// select 10 bit mode
	nich <<= 3;
	ADCON0 = 0b.10.000.0.0.1 + nich;
	AcqTime();
	niltemp.high8 = ADRESH;
	niltemp.low8 = ADRESL;

	return(niltemp);
}



void AnalogTest(void)
{

// UBatt
	nilgval = GetAnalogVal(0);	
		
	nilgval *= 46;
	nilgval += 9;	// round up
	nilgval /= 17;	// resolution is 0,01 Volt

	SendComChar('V');
	SendComChar('b');
	SendComChar(':');
	
	SendComValUL(NKS2 + LEN5);	// print millivolts from nilgval
	SendComText(_SerVolt);

// URoll
#ifdef OPT_ADXRS
	nilgval = GetAnalogVal(1);	
#endif
#ifdef OPT_IDG
	nilgval = GetAnalogVal(2);	
#endif
	nilgval *= 49;
	nilgval += 5;	// round up
	nilgval /= 10;	// resolution is 0,001 Volt

	SendComChar('V');
	SendComChar('r');
	SendComChar(':');
	
	SendComValUL(NKS3+LEN5);	// print millivolts from nilgval
	SendComText(_SerVolt);

// UNick
#ifdef OPT_ADXRS
	nilgval = GetAnalogVal(2);	
#endif
#ifdef OPT_IDG
	nilgval = GetAnalogVal(1);	
#endif
	nilgval *= 49;
	nilgval += 5;	// round up
	nilgval /= 10;	// resolution is 0,001 Volt

	SendComChar('V');
	SendComChar('p');
	SendComChar(':');
	
	SendComValUL(NKS3+LEN5);	// print millivolts from nilgval
	SendComText(_SerVolt);

// UYaw
	nilgval = GetAnalogVal(4);	
	nilgval *= 49;
	nilgval += 5;	// round up
	nilgval /= 10;	// resolution is 0,001 Volt

	SendComChar('V');
	SendComChar('y');
	SendComChar(':');
	
	SendComValUL(NKS3+LEN5);	// print millivolts from nilgval
	SendComText(_SerVolt);

// Uref
	nilgval = GetAnalogVal(3);	
	nilgval *= 49;
	nilgval += 5;	// round up
	nilgval /= 10;	// resolution is 0,001 Volt

	SendComChar('V');
	SendComChar('f');
	SendComChar(':');
	
	SendComValUL(NKS3+LEN5);	// print millivolts from nilgval
	SendComText(_SerVolt);
}
