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

// Prototypes

void InitDirection(void);
void GetDirection(void);

// Defines

#define COMPASS_TIME	50	/* 20Hz */
#define BARO_TEMP_TIME	10
#define BARO_PRESS_TIME 35

#define COMPASS_I2C_ID		(0x42)
#define	COMPASS_OFFSET 		MILLIPI				/* orientation of compass relative to "front" - positive */

#define BARO_I2C_ID			0xee
#define BARO_TEMP_BMP085	0x2e
#define BARO_TEMP_SMD500	0x6e
#define BARO_PRESS			0xf4
#define BARO_CTL			0xf4
#define BARO_ADC_MSB		0xf6
#define BARO_ADC_LSB		0xf7
#define BARO_TYPE			0xd0
//#define BARO_ID_SMD500		??
#define BARO_ID_BMP085		0x55

// Variables
#pragma udata compassvars
int16 	CompassHeading;							// raw compass value
int16	DesiredHeading;							// desired heading (240 = 360 deg)
#pragma udata

void InitDirection(void)
{ 	
	CompassHeading = 0;
	HE = 0;
	mS[CompassUpdate] = mS[Clock] + COMPASS_TIME;

	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto IDerror;
							
	_UseCompass = true;
	return;

IDerror:
	I2CStop();
	_UseCompass = false;
	return;
} // InitDirection

void GetDirection(void)
{
	int16 Temp;

	if( _UseCompass )
		if ( mS[Clock] > mS[CompassUpdate] ) 
		{
			// compass is in continous mode 
			mS[CompassUpdate] = mS[Clock] + COMPASS_TIME;

			I2CStart();
			if( SendI2CByte(COMPASS_I2C_ID+1) == I2C_ACK ) 
			{
				CompassHeading = ((int24)((RecvI2CByte(I2C_ACK)<<8) | RecvI2CByte(I2C_NACK))
								* (int24) MILLIPI ) / (int24)1800 + COMPASS_OFFSET;
				I2CStop();

				if ( CompassHeading >= TWOMILLIPI )
					CompassHeading -= TWOMILLIPI;
	
				// positive means quadrocopter is left of heading
				Temp = CompassHeading - DesiredHeading;
				if( Temp <= -MILLIPI )
					Temp +=  MILLIPI;
				else
					if( Temp >   MILLIPI )
						Temp -=  MILLIPI;
				
				HE = SoftFilter(HE, Temp);
			}
			else
			{
				I2CStop();
				HE = 0;
			}
		}
	else
		HE = 0;
} // GetDirection

// --------------------------------------------------------------------------------

// Prototypes

void InitBarometer(void);
int16 AltitudeCompensation(uint16, uint16);
void StartBaroAcq(uint8);
uint16 ReadBaro(void);
void GetBaroAltitude(void);

// Variables

#pragma udata barovars
uint8	BaroTemp;
uint16	OriginBaroTemp;
int16	OriginBaroAltitude, CurrBaroAltitude, CurrBaroTemp;
#pragma udata

uint16 ReadBaro(void)
{ 	// ugly goto hurdles but clearer perhaps
	uint16 BaroVal, Temp;

	// Possible I2C protocol error - split read of ADC
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto RBerror;
	if( SendI2CByte(BARO_ADC_MSB) != I2C_ACK ) goto RBerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto RBerror;
	Temp = RecvI2CByte(I2C_NACK) << 8;
	I2CStop();
		
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto RBerror;
	if( SendI2CByte(BARO_ADC_LSB) != I2C_ACK ) goto RBerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto RBerror;
	BaroVal = Temp | RecvI2CByte(I2C_NACK);
	I2CStop();
		
	_UseBaro = true;
	return(BaroVal);

RBerror:
	I2CStop();
	return(0);

} // ReadBaro

void StartBaroAcq(uint8 addr)
{ // ugly goto hurdles but clearer perhaps

	_BaroAcqTemp = (addr == BaroTemp);
	if ( _BaroAcqTemp )
		mS[BaroUpdate] = mS[Clock] + BARO_TEMP_TIME;
	else
		mS[BaroUpdate] = mS[Clock] + BARO_PRESS_TIME;

	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto BAerror;
	if( SendI2CByte(BARO_CTL) != I2C_ACK )  goto BAerror; 	
	if( SendI2CByte(addr) != I2C_ACK )  goto BAerror;
	I2CStop();

	_UseBaro = true;
	return;
	 		
BAerror:
	I2CStop();
	_UseBaro = false;
} // StartBaroAcq

int16 AltitudeCompensation(uint16 P, uint16 T)
{	// Altitudes are relative to Origin altitude
	// Full conversion to altitude very computationally expensive
	// Units for SMD500 are approximately 0.3M/LSB or 1foot/LSB
	int24 TC;

	TC = SRS24((T - OriginBaroTemp) * BaroTempCoeff + 16, 5);
	
	return (OriginBaroAltitude - (P + TC));	
} // AltitudeCompensation

void InitBarometer(void)
{ 
	uint8 s;
	uint8 BaroType;
	uint24 AvTemperature, AvPressure;

	OriginBaroAltitude = CurrBaroAltitude = CurrBaroTemp = 0;
	AvTemperature = AvPressure = 0;

	// Determine baro type
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto IBerror;
	if( SendI2CByte(BARO_TYPE) != I2C_ACK ) goto IBerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto IBerror;
	BaroType = RecvI2CByte(I2C_NACK);
	I2CStop();

	if ( BaroType == BARO_ID_BMP085 )
		BaroTemp = BARO_TEMP_BMP085;
	else
		BaroTemp = BARO_TEMP_SMD500;

	AvTemperature = AvPressure = 0;
	for ( s = 32; s; s-- )
	{
		StartBaroAcq(BaroTemp);
		while ( mS[Clock] <= mS[BaroUpdate] ) {};
		AvTemperature += ReadBaro();
	
		StartBaroAcq(BARO_PRESS);
		while ( mS[Clock] <= mS[BaroUpdate] ) {};
		AvPressure += ReadBaro();	
	}
	// OriginBaroAltitude initially 0
	OriginBaroAltitude = -AltitudeCompensation(AvPressure >> 5, CurrBaroTemp);
	OriginBaroTemp = AvTemperature >> 5;
	StartBaroAcq(BaroTemp);						// overlap baro

	return;

IBerror:

	BaroTemp = BARO_TEMP_SMD500;
} // InitBarometer

void GetBaroAltitude(void)
{
	uint16 BaroVal;
	
	if ( _UseBaro )
	{
		BaroVal = ReadBaro();
		if ( mS[Clock] >= mS[BaroUpdate] )
			if( _BaroAcqTemp )
			{ 	// temperature
				StartBaroAcq(BARO_PRESS);			// start pressure for next call
				CurrBaroTemp = BaroVal;
			}
			else
			{
				StartBaroAcq(BaroTemp);				// start temperature for next call
				CurrBaroAltitude = SoftFilter(CurrBaroAltitude, AltitudeCompensation(BaroVal, CurrBaroTemp));
			}
	}
} // GetBaroAltitude

	
