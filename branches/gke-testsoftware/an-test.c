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


// Utilities and subroutines

#include "pu-test.h"
#include "bits.h"


void AnalogTest(void)
{
	// UBatt
	nilgval = ADC(0, ADCVREF5V);	
		
	nilgval *= 46;
	nilgval += 9;	// round up
	nilgval /= 17;	// resolution is 0,01 Volt

	SendComChar('V');
	SendComChar('b');
	SendComChar(':');
	
	SendComValUL(NKS2 + LEN5);	// print millivolts from nilgval
	SendComText(SerVolt);

// URoll
#ifdef OPT_ADXRS
	nilgval = ADC(1, ADCVREF5V);	
#endif
#ifdef OPT_IDG
	nilgval = ADC(2, ADCVREF5V);	
#endif
	nilgval *= 49;
	nilgval += 5;	// round up
	nilgval /= 10;	// resolution is 0,001 Volt

	SendComChar('V');
	SendComChar('r');
	SendComChar(':');
	
	SendComValUL(NKS3+LEN5);	// print millivolts from nilgval
	SendComText(SerVolt);

// UNick
#ifdef OPT_ADXRS
	nilgval = ADC(2, ADCVREF5V);	
#endif
#ifdef OPT_IDG
	nilgval = ADC(1, ADCVREF5V);	
#endif
	nilgval *= 49;
	nilgval += 5;	// round up
	nilgval /= 10;	// resolution is 0,001 Volt

	SendComChar('V');
	SendComChar('p');
	SendComChar(':');
	
	SendComValUL(NKS3+LEN5);	// print millivolts from nilgval
	SendComText(SerVolt);

// UYaw
	nilgval = ADC(4, ADCVREF5V);	
	nilgval *= 49;
	nilgval += 5;	// round up
	nilgval /= 10;	// resolution is 0,001 Volt

	SendComChar('V');
	SendComChar('y');
	SendComChar(':');
	
	SendComValUL(NKS3+LEN5);	// print millivolts from nilgval
	SendComText(SerVolt);

// Uref
	nilgval = ADC(3, ADCVREF5V);	
	nilgval *= 49;
	nilgval += 5;	// round up
	nilgval /= 10;	// resolution is 0,001 Volt

	SendComChar('V');
	SendComChar('f');
	SendComChar(':');
	
	SendComValUL(NKS3+LEN5);	// print millivolts from nilgval
	SendComText(SerVolt);

}
