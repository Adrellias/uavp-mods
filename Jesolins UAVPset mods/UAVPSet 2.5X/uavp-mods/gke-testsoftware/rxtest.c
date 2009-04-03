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

// Receiver signal test

#pragma codepage=1
#pragma sharedAllocation

#include "pu-test.h"
#include "bits.h"

// Math Library
#include "mymath16.h"


// output all the signal values and the validity of signal
void ReceiverTest(void)
{
	uns8 nii;
	uns16 *p;

	p=&CurrK1;
	for( nii=1; nii <= 7; nii++ )
	{
		SendComChar(nii+'0');
		SendComChar(':');
		nilgval = *p;
		SendComChar('0');
		SendComChar('x');
		SendComValH(nilgval.high8);
		SendComValH(nilgval.low8);
		if( (nilgval.high8 < 1) || (nilgval.high8 > 1) ) 
		{
			SendComText(_SerFail);
		}
		SendComCRLF();
		p++;
	}
// show pause time
	SendComChar('P');
	SendComChar(':');
	nilgval = 2*PauseTime;
 	nilgval += (uns16)TMR2_5MS * 64;	// 78 * 16*16/4 us
	SendComValUL(NKS3+LEN5);	// print from nilgval
	SendComText(_SerMS);
}

void TogglePPMPolarity(void)
{
	NegativePPM ^= 1;	// toggle bit
	if( NegativePPM )
		SendComText(_SerPPMN);
	else
		SendComText(_SerPPMP);

	CurrK1 =
	CurrK2 =
	CurrK3 =
	CurrK4 =
	CurrK5 =
	CurrK6 =
	CurrK7 = 0xFFFF;
	PauseTime = 0;
	_NewValues = 0;
	_NoSignal = 1;
}
