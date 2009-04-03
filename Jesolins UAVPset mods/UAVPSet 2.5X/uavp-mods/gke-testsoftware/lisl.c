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
// =  please visit http://www.uavp.de           =
// =               http://www.mahringer.co.at   =
// ==============================================

// The LISL controller routines

//#pragma codepage=0
#pragma codepage=2
#include "pu-test.h"
#include "bits.h"

#pragma sharedAllocation

bank2 uns8	nii;


// send an address byte (niaddr) to linear sensor
// read the answer and return it
uns8 ReadLISL(uns8 niaddr)
{

//	SendCommand(niaddr);
	LISL_SDA = 1;	// very important!! really!! LIS3L likes it
	LISL_IO = 0;	// SDA is output
	LISL_SCL = 0;
	LISL_CS = 0;	// CS to 0
	for( nii = 0; nii < 8; nii++ )
	{
		LISL_SCL = 0;
		if( niaddr & 0x80 )
			LISL_SDA = 1;
		else
			LISL_SDA = 0;
		niaddr <<= 1;
		LISL_SCL = 1;
	}


	LISL_IO = 1;	// SDA is input
	niaddr = 0;
	for( nii = 0; nii < 8; nii++ )
	{
		LISL_SCL = 0;
		niaddr <<= 1;
		if( LISL_SDA == 1 )
			niaddr |= 1;	// set LSB
		LISL_SCL = 1;
	}
	LISL_CS = 1;
	return(niaddr);
}

// only needed for CC5X version 3.3 and earlier
page2 void WriteLISL(uns8, uns8);

// send an address byte (niaddr) to lienar sensor
// and write data byte (nidata)
void WriteLISL(uns8 niaddr, uns8 nidata)
{

	LISL_IO = 0;	// SDA is output
	LISL_SCL = 0;
	LISL_CS = 0;	// CS to 0
	for( nii = 0; nii < 8; nii++ )
	{
		LISL_SCL = 0;
		if( niaddr & 0x80 )
			LISL_SDA = 1;
		else
			LISL_SDA = 0;
		niaddr <<= 1;
		LISL_SCL = 1;
	}

	for( nii = 0; nii < 8; nii++ )
	{
		LISL_SCL = 0;
		if( nidata & 0x80 )
			LISL_SDA = 1;
		else
			LISL_SDA = 0;
		nidata <<= 1;
		LISL_SCL = 1;
	}
	LISL_CS = 1;
	LISL_IO = 1;	// IO is input (to allow RS232 reception)
}

// put the base setup to linear sensor
// enable all axes, setup resolution
// setup parachute options
void IsLISLactive(void)
{

	LISL_CS = 1;
	WriteLISL(LISL_CTRLREG_2, 0b.01001010); // enable 3-wire, BDU=1, +/-2g

	nii = ReadLISL(LISL_WHOAMI + LISL_READ);
	if( nii == 0x3A )	// a LIS03L sensor is there!
	{
		WriteLISL(LISL_CTRLREG_1, 0b.11010111); // startup, enable all axis
		WriteLISL(LISL_CTRLREG_3, 0b.00000000);
		WriteLISL(LISL_FF_CFG,    0b.01001000); // Y-axis is height
		WriteLISL(LISL_FF_THS_L,  0b.00000000);
		WriteLISL(LISL_FF_THS_H,  0b.11111100); // -0,5g threshold
		WriteLISL(LISL_FF_DUR,    255);
		WriteLISL(LISL_DD_CFG,    0b.00000000);
		_UseLISL = 1;
	}
}


void LinearTest(void)
{

	nii = ReadLISL(LISL_STATUS + LISL_READ);
	SendComChar('S');
	SendComChar(':');
	SendComChar('0');
	SendComChar('x');
	SendComValH(nii);
	SendComCRLF();

	nilgval.high8 = (int)ReadLISL(LISL_OUTX_H + LISL_READ);
	nilgval.low8  = (int)ReadLISL(LISL_OUTX_L + LISL_READ);
	SendComChar('X');
	SendComChar(':');
	SendComValUL(NKS3+LEN5+VZ);
	SendComText(_SerLinG);

	nilgval.high8 = (int)ReadLISL(LISL_OUTZ_H + LISL_READ);
	nilgval.low8  = (int)ReadLISL(LISL_OUTZ_L + LISL_READ);
	SendComChar('Y');
	SendComChar(':');
	SendComValUL(NKS3+LEN5+VZ);
	SendComText(_SerLinG);

	nilgval.high8 = (int)ReadLISL(LISL_OUTY_H + LISL_READ);
	nilgval.low8  = (int)ReadLISL(LISL_OUTY_L + LISL_READ);
	SendComChar('Z');
	SendComChar(':');
	SendComValUL(NKS3+LEN5+VZ);
	SendComText(_SerLinG);
	
}


