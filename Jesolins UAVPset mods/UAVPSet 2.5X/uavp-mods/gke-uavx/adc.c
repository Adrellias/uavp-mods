// =======================================================================
// =                                 UAVX                                =
// =                         Quadrocopter Control                        =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =     Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer       =
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

#include "UAVX.h"
#include <adc.h>

// Prototypes
int16 ADC(uint8, uint8);
void InitADC(void);

int16 ADC(uint8 Channel, uint8 VRef)
{
	int16 Result;
	uint8 d;

	ADCON1bits.VCFG0 = VRef;
	SetChanADC(Channel<<3);		// using automatic acq
	ConvertADC();  
	while (BusyADC()){};

	Result=ReadADC();

	return (Result); 

} // ADC

void InitADC()
{
 OpenADC(ADC_FOSC_32 & 
          ADC_RIGHT_JUST &
          ADC_12_TAD,
		  ADC_CH0 &
		  ADC_INT_OFF &
          ADC_VREFPLUS_VDD &
          ADC_VREFMINUS_VSS,	  
          ADCPORTCONFIG);
} // InitADC


