// =================================================================================================
// =                                  UAVX Quadrocopter Controller                                 =
// =                             Copyright (c) 2008 by Prof. Greg Egan                             =
// =                   Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                       http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// =================================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even 
//    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//    General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

#include "uavx.h"

// Prototypes

int16 ADC(uint8, uint8);
void InitADC(void);

int16 ADC(uint8 Channel, uint8 VRef)
{
	ADCON1bits.VCFG0 = VRef;
	SetChanADC(Channel<<3);		// using automatic acq
	ConvertADC();  
	while (BusyADC()){};

	return ( ReadADC() ); 
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


