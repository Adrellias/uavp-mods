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

int16 ADC(uint8);
void InitADC(void);

#pragma udata adcq
SensorStruct ADCVal[(ADC_TOP_CHANNEL+1)];
#pragma udata
uint8 ADCChannel;

int16 ADC(uint8 ADCChannel)
{ // all ADC reads use 5V reference
	static int16 v;

	DisableInterrupts; // make atomic
		v = ADCVal[ADCChannel].v.b2_1; // rescale by 256
	EnableInterrupts;

	return ( v ); 
} // ADC

void InitADC()
{
	static uint8 i, c;

	OpenADC(ADC_FOSC_32 & 
          ADC_RIGHT_JUST &
          ADC_20_TAD,		// 12 TAD?
		  ADC_CH0 &
		  ADC_INT_OFF &
          ADC_VREFPLUS_VDD &
          ADC_VREFMINUS_VSS,	  
          ADCPORTCONFIG);

	for ( i = 0; i <= ADC_TOP_CHANNEL; i++)
	{
		ADCVal[i].v.i24 = 512L * 256L;
		ADCVal[i].a = 0;
		ADCVal[i].dt = ADC_TOP_CHANNEL;			// mS - channels are processed cyclically on each mS clock tick
		ADCVal[i].f = ADC_ATT_FREQ;				// Hz
	}
	
	ADCVal[ADCBattVoltsChan].f = ADC_BATT_FREQ;		
	ADCVal[ADCAltChan].f = ADC_ALT_FREQ;		
	ADCVal[ADCYawChan].f = ADC_YAW_FREQ;
	
	for ( i = 0; i <= ADC_TOP_CHANNEL; i++ )
		ADCVal[i].a	 = ( (int24) ADCVal[i].dt * 256L) / ( 1000L / ( 6L * (int24) ADCVal[i].f ) + (int24) ADCVal[i].dt );

	ADCChannel = 0;
	SetChanADC(	ADCChannel);					// using automatic acq
	ConvertADC();

} // InitADC


