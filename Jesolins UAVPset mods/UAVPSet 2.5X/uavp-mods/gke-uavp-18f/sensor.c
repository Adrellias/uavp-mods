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

#include "c-ufo.h"
#include "bits.h"

void I2CDelay(void) 
{
	Delay10TCY();
} // I2CDelay

uint8 I2CWaitClkHi(void)
{
	uint8 s;

	s = 1;
	I2CDelay();
	I2C_CIO=1;								// set SCL to input, output a high
	while( !I2C_SCL )						// wraparound through 255 1.25mS @ 16MHz							
		if( ++s == 0 )			 
			break;	
	I2CDelay();
	return(s);
} // I2CWaitClkHi

void I2CStart(void)
{
	uint8 r;

	I2C_DIO=1;								// set SDA to input, output a high
	r = I2CWaitClkHi();
	I2C_SDA = 0;							// start condition
	I2C_DIO = 0;							// output a low
	I2CDelay();
	I2C_SCL = 0;
	I2C_CIO = 0;							// set SCL to output, output a low
} // I2CStart

void I2CStop(void)
{
	uint8 r;

	I2C_DIO=0;								// set SDA to output
	I2C_SDA = 0;							// output a low
	r = I2CWaitClkHi();

	I2C_DIO=1;								// set SDA to input, output a high, STOP condition
	I2CDelay();								// leave clock high
} // I2CStop 

uint8 SendI2CByte(uint8 d)
{
	int8 s;

	for(s=8; s ; s--)
	{
		if( d & 0x80 )
			I2C_DIO = 1;					// switch to input, line is high
		else
		{
			I2C_SDA = 0;			
			I2C_DIO = 0;					// switch to output, line is low
		}
	
		if(I2CWaitClkHi())
		{ 	
			I2C_SCL = 0;
			I2C_CIO = 0;					// set SCL to output, output a low
			d <<= 1;
		}
		else
			return(I2C_NACK);	
	}

	I2C_DIO = 1;							// set SDA to input
	if(I2CWaitClkHi())
		s = I2C_SDA;
	else
		return(I2C_NACK);	

	I2C_SCL = 0;
	I2C_CIO = 0;							// set SCL to output, output a low

	return(s);
} // SendI2CByte

uint8 RecvI2CByte(uint8 r)
{
	uint8 d;
	int8 s;

	d = 0;
	I2C_DIO = 1;							// set SDA to input, output a high

	for(s=8; s ; s--)
		if( I2CWaitClkHi() )
		{ 
			d <<= 1;
			if( I2C_SDA )
				d |= 1;
			I2C_SCL = 0;
			I2C_CIO = 0;
 		}
		else
			return(0);

	I2C_SDA = r;
	I2C_DIO = 0;
											// set SDA to output
	if( I2CWaitClkHi() )
	{
		I2C_SCL = 0;
		I2C_CIO = 0;						// set SCL to output, output a low
	//	I2CDelay();
	//	I2C_IO = 0;	// set SDA to output
		return(d);
	}
	else
		return(0);	
} // RecvI2CByte


void InitDirection(void)
{
	// 20Hz continuous read with periodic reset.
	#define COMP_OPMODE 0b01110010

	// Set device to Compass mode 
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('G')  != I2C_ACK ) goto CTerror;
	if( SendI2CByte(0x74) != I2C_ACK ) goto CTerror;
	if( SendI2CByte(COMP_OPMODE) != I2C_ACK ) goto CTerror;
	I2CStop();

	I2CStart(); // save operation mode in EEPROM
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('L')  != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(50);

	// use default heading mode (1/10th degrees)

	_UseCompass = true;
	return;
CTerror:
	_UseCompass = false;
	I2CStop();
} // InitDirection


void GetDirection(void)
{
	// Read direction, convert it to 2 degrees unit
	// and store result in variable AbsDirection.
	// The current heading correction is stored in CurDeviation
	int16 DirVal,  temp;
	uint16 Compass;

	if( _UseCompass  ) // continuous mode but Compass only updates avery 50mS
	{
		// set Compass device to Compass mode 
		I2CStart();		
		Compass = (uint16)RecvI2CByte(I2C_ACK)*256 | RecvI2CByte(I2C_NACK);
		I2CStop();
	
		// DirVal has 1/10th degrees
		// convert to set 360.0 deg = 240 units
		Compass /= 15;
		DirVal = Compass;
	
		// must use pre-decrement, because of dumb compiler
		if( AbsDirection > COMPASS_MAX )
		{
			CurDeviation = 0;
			AbsDirection--;
		}
		else
		{
			// setup desired heading (AbsDirection)
			if( AbsDirection == COMPASS_MAX )	// no heading stored yet
			{
				AbsDirection = DirVal;	// store current heading
				CurDeviation = 0;
			}
			// calc deviation and direction of deviation
			DirVal = AbsDirection - DirVal;
			// handle wraparound
			if( DirVal <= -240/2 ) 
				DirVal +=  240;
			else
				if( DirVal > 240/2 ) 
					DirVal -=  240;
	
			// positive means ufo is left off-heading
			// negative means ufo is right off-heading
	
			DirVal = Limit(DirVal, -20, 20); // limit to give soft reaction
	
			// Empirical found :-)
			// New_CurDev = ((3*Old_CurDev) + DirVal) / 4
			CurDeviation = SRS16((((int16)CurDeviation *3 + DirVal) << 2) 
								* (int16)CompassFactor, 8);
		}
		#ifdef DEBUG_SENSORS
		if( IntegralCount == 0 )
			Trace[TAbsDirection] = DirVal * 4; //AbsDirection; // scale for UAVPSet
		#endif					
	}
	#ifdef DEBUG_SENSORS
	else	// no new value received
		if( IntegralCount == 0 )
			Trace[TAbsDirection] = 0;
	#endif

} // GetDirection

#ifdef BARO_HARD_FILTER
	#define BaroFilter HardFilter
#else
	#define BaroFilter MediumFilter
#endif // BARO_HARD_FILTER

uint8 ReadValueFromBaro(void)
{
	// Possible I2C protocol error - split read of ADC
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto RVerror;
	if( SendI2CByte(BARO_ADC_MSB) != I2C_ACK ) goto RVerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto RVerror;
	BaroVal = (uint16)RecvI2CByte(I2C_NACK) * 256;
	I2CStop();
		
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto RVerror;
	if( SendI2CByte(BARO_ADC_LSB) != I2C_ACK ) goto RVerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto RVerror;
	BaroVal |= RecvI2CByte(I2C_NACK);
	I2CStop();

	return(I2C_NACK);

RVerror:
	I2CStop();
	_UseBaro = false; // read error, disable baro
	#ifdef BARO_RETRY
	_Hovering = false;	
	if ( BaroRestarts < 100 )
	{
		InitBarometer();
		_BaroRestart = true;
		BaroRestarts++;
	}
	#endif
	return(I2C_ACK);
} // ReadValueFromBaro

uint8 StartBaroADC(uint8 TempOrPress)
{
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto SBerror;

	// access control register, start measurement
	if( SendI2CByte(BARO_CTL) != I2C_ACK ) goto SBerror;

	// select 32kHz input, measure temperature
	if( SendI2CByte(TempOrPress) != I2C_ACK ) goto SBerror;
	I2CStop();

	// set or clear bit to signal what A/D is currently running
	_BaroTempRun = (TempOrPress == BaroTemp);
	
	return(I2C_NACK);
SBerror:
	I2CStop();
	return(I2C_ACK);
} // StartBaroADC

// initialize compass sensor
void InitBarometer(void)
{
	// SMD500 9.5mS (T) 34mS (P)  
	// BMP085 4.5mS (T) 25.5mS (P) OSRS=3, 7.5mS OSRS=1
	// Baro is assumed offline unless it responds - no retries!
	uint8 r;

	VBaroComp = BaroCount = 0;

	// Determine baro type
	I2CStart();

	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto BAerror;
	if( SendI2CByte(BARO_TYPE) != I2C_ACK ) goto BAerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto BAerror;
	BaroType = RecvI2CByte(I2C_NACK);
	I2CStop();

	if ( BaroType == BARO_ID_BMP085 )
		BaroTemp = BARO_TEMP_BMP085;
	else
		BaroTemp = BARO_TEMP_SMD500;

	// read temperature once to get base value
	if( !StartBaroADC(BaroTemp) ) goto BAerror;

	Delay1mS(10);
	r = ReadValueFromBaro();
	BaroBaseTemp = BaroVal;	// save start value
		
	// read pressure once to get base value
	if( !StartBaroADC(BARO_PRESS) ) goto BAerror;
	Delay1mS(30);
	r = ReadValueFromBaro();	
	BaroBasePressure = BaroVal;
	
	// set baro device to start temperature conversion
	// before first call to ComputeBaroComp
	if( !StartBaroADC(BaroTemp) ) goto BAerror;	

	_UseBaro = true;

	return;

BAerror:
	_UseBaro = _Hovering = false;
	I2CStop();
} // InitBarometer

void ComputeBaroComp(void)
{
	int16 OldBaroRelPressure, Temp, Delta;

	BaroCount++;

	if( _UseBaro )
		// ~10ms for Temperature and 40ms for Pressure at TimeStep = 2 - UGLY
#ifdef ENABLE_NEW_ALT_HOLD
		if (((BaroCount >= 8) && _BaroTempRun) || ((BaroCount >= 32 ) && !_BaroTempRun))
#else
		if (((BaroCount >= 2) && _BaroTempRun) || ((BaroCount >= 8 ) && !_BaroTempRun))
#endif // ENABLE_NEW_ALT_HOLD	
		{
			BaroCount = 0;
			if ( ReadValueFromBaro() == I2C_NACK) 	// returns niltemp as value		
				if( _BaroTempRun )
				{
					StartBaroADC(BARO_PRESS); // next is pressure - overlap
					if( ThrDownCount )
						BaroBaseTemp = BaroVal; // current read value
					else 
						BaroRelTempCorr = BaroVal - BaroBaseTemp;
	
				}
				else
				{	// current measurement was "pressure"
					StartBaroADC(BaroTemp);	// next is temp - overlap

					if( ThrDownCount )	// while moving throttle stick
					{
						BaroBasePressure = BaroVal;	
						_Hovering = false;
						VBaroComp = 0;	
					}
					else
					{
						#ifdef BARO_SCRATCHY_BEEPER
						Beeper_TOG;
						#endif

						if ( !_Hovering )
						{
							BaroRelPressure = 0;
							_Hovering = true;
						}
	
#ifdef ENABLE_NEW_ALT_HOLD
// EXPERIMENTAL ALTITUDE HOLD
						// while holding altitude
						BaroVal -= BaroBasePressure;
						// BaroVal has -400..+400 approx
						BaroVal += SRS16((int16)BaroRelTempCorr * (int16)BaroTempCoeff + 16, 5);

						OldBaroRelPressure = BaroRelPressure;	// remember old value for delta
	

						BaroRelPressure = BaroFilter(BaroRelPressure, BaroVal);
	
						Delta = BaroRelPressure - OldBaroRelPressure;

						BaroRelPressure = Limit(BaroRelPressure, -2, 8); // was: +10 and -5
		
						// strictly this is acting more like an integrator 
						// bumping VBaroComp up and down proportional to the error?	
						Temp = BaroRelPressure * (int16)BaroThrottleProp;
						if( VBaroComp > Temp )
							VBaroComp--;
						else
							if( VBaroComp < Temp )
								VBaroComp++; // climb

						if( VBaroComp > Temp )
							VBaroComp--;
				
						// Differential		
						VBaroComp += Limit(Delta, -5, 8) * (int16)BaroThrottleDiff;
	
						VBaroComp = Limit(VBaroComp, -5, 15);

#else

						// while holding altitude
						BaroVal -= BaroBasePressure;
						// BaroVal has -400..+400 approx
						BaroVal += SRS16((int16)BaroRelTempCorr * (int16)BaroTempCoeff + 16, 5);

						OldBaroRelPressure = BaroRelPressure;	// remember old value for delta
	
						BaroRelPressure = BaroFilter(BaroRelPressure, BaroVal);
	
						Delta = BaroRelPressure - OldBaroRelPressure;	// subtract new height to get delta

						BaroRelPressure = Limit(BaroRelPressure, -2, 8); // was: +10 and -5
		
						// strictly this is acting more like an integrator 
						// bumping VBaroComp up and down proportional to the error?
		
						Temp = BaroRelPressure * (int16)BaroThrottleProp;
						if( VBaroComp > Temp )
							VBaroComp--;
						else
							if( VBaroComp < Temp )
								VBaroComp++; // climb

						if( VBaroComp > Temp )
							VBaroComp--;
						else
							if( VBaroComp < Temp )
								VBaroComp++; // climb
		
						// Differentialanteil		
						VBaroComp += Limit(Delta, -8, 8) * (int16)BaroThrottleDiff;
	
						VBaroComp = Limit(VBaroComp, -5, 15);
#endif // ENABLE_NEW_ALT_HOLD
	
						#ifdef BARO_SCRATCHY_BEEPER
						Beeper_TOG;
						#endif
					}
				}
		}

	#ifdef DEBUG_SENSORS	
	if( IntegralCount == 0 )
		if ( _UseBaro )
		{
			Trace[TVBaroComp] = VBaroComp << 4; // scale for UAVPSet
			Trace[TBaroRelPressure] = BaroRelPressure << 4;
		}
		else	// baro sensor not active
		{
			Trace[TVBaroComp] = 0;
			Trace[TBaroRelPressure] = 0;
		}
	#endif
} // ComputeBaroComp	

