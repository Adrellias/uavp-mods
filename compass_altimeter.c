// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008, 2009 by Prof. Greg Egan           =
// =   Original V3.15 Copyright (c) 2007, 2008 Ing. Wolfgang Mahringer   =
// =           http://code.google.com/p/uavp-mods/ http://uavp.ch        =
// =======================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "uavx.h"

// Prototypes

void InitCompass(void);
void InitHeading(void);
void GetHeading(void);

void StartBaroADC(void);
void ReadBaro(void);
void GetBaroPressure(void);
void InitBarometer(void);
void CheckForHover(void);
void BaroPressureHold(int16);

//_____________________________________________________________________________________

// Compass

void InitCompass(void)
{
	// 20Hz continuous read with periodic reset.
	#ifdef SUPPRESS_COMPASS_SR
		#define COMP_OPMODE 0b01100010
	#else
		#define COMP_OPMODE 0b01110010
	#endif // SUPPRESS_COMPASS_SR

	// Set device to Compass mode 
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('G')  != I2C_ACK ) goto CTerror;
	if( SendI2CByte(0x74) != I2C_ACK ) goto CTerror;
	if( SendI2CByte(COMP_OPMODE) != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(1);

	I2CStart(); // save operation mode in EEPROM
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('L')  != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(1);

	I2CStart(); // Do Bridge Offset Set/Reset now
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('O')  != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(50);

	// use default heading mode (1/10th degrees)

	F[CompassValid] = true;
	return;
CTerror:
	F[CompassValid] = false;
	Stats[CompassFailS].i16++;
	
	I2CStop();
} // InitCompass


void InitHeading(void)
{
	GetHeading();
	DesiredHeading = Heading;
	HEp = 0;
} // InitHeading

void GetHeading(void)
{
	static i16u Compass;
	static int32 Temp;

	if( F[CompassValid]  ) // continuous mode but Compass only updates avery 50mS
	{
		I2CStart();
		F[CompassMissRead] = SendI2CByte(COMPASS_I2C_ID+1) != I2C_ACK; 
		Compass.high8 = RecvI2CByte(I2C_ACK);
		Compass.low8 = RecvI2CByte(I2C_NACK);
		I2CStop();

		//Temp = (int32)((int32)Compass * MILLIPI)/1800L - COMPASS_OFFSET;
		Temp = ConvertDDegToMPi(Compass.i16) - CompassOffset;
		Heading = Make2Pi((int16) Temp);
		if ( F[CompassMissRead] && (State == InFlight) ) Stats[CompassFailS].i16++;	
	}
	else
		Heading = 0;
} // GetHeading

//_____________________________________________________________________________________

// Barometer

#ifdef BARO_HARD_FILTER
	#define BaroFilter HardFilter
#else
	#define BaroFilter MediumFilter
#endif // BARO_HARD_FILTER

void StartBaroADC(void)
{
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto SBerror;

	// access control register, start measurement
	if( SendI2CByte(BARO_CTL) != I2C_ACK ) goto SBerror;

	// select 32kHz input, measure temperature
	if( SendI2CByte(BARO_PRESS) != I2C_ACK ) goto SBerror;
	I2CStop();

	if ( BaroType == BARO_ID_BMP085 )
		mS[BaroUpdate] = mS[Clock] + BMP085_PRESS_TIME_MS;
	else
		mS[BaroUpdate] = mS[Clock] + SMD500_PRESS_TIME_MS;

	F[BaroAltitudeValid] = true;
	return;
SBerror:
	I2CStop();
	F[BaroAltitudeValid] = F[Hovering] = false; 
	return;
} // StartBaroADC

void ReadBaro(void)
{
	// Possible I2C protocol error - split read of ADC
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto RVerror;
	if( SendI2CByte(BARO_ADC_MSB) != I2C_ACK ) goto RVerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto RVerror;
	BaroVal.high8 = RecvI2CByte(I2C_NACK);
	I2CStop();
			
	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto RVerror;
	if( SendI2CByte(BARO_ADC_LSB) != I2C_ACK ) goto RVerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto RVerror;
	BaroVal.low8 = RecvI2CByte(I2C_NACK);
		I2CStop();

	StartBaroADC(); // overlap next acquisition

	return;

RVerror:
	I2CStop();

	F[BaroAltitudeValid] = F[Hovering] = false;
	if ( State == InFlight ) Stats[BaroFailS].i16++; 
	return;
} // ReadBaro

static int16 BaroAverage;

void GetBaroPressure(void)
{	
	static int16 Temp;
	// SMD500 9.5mS (T) 34mS (P)  
	// BMP085 4.5mS (T) 25.5mS (P) OSRS=3, 7.5mS OSRS=1

	if ( mS[Clock] > mS[BaroUpdate] )
	{
		ReadBaro();
		BaroAverage += (int16)( (int24)BaroVal.u16 - OriginBaroPressure );
		BaroSample++;
		if ( BaroSample == 8 )
		{
			CurrentRelBaroPressure = BaroFilter(CurrentRelBaroPressure, SRS16( BaroAverage, 3) );
			if ( State == InFlight )
			{
				Temp = Abs(CurrentRelBaroPressure);
				if ( Temp > Stats[RelBaroPressureS].i16 ) 
					Stats[RelBaroPressureS].i16 = Temp;
			}
			BaroSample = BaroAverage = 0;
			F[NewBaroValue] = true;
		}

		#ifdef DEBUG_SENSORS	
		Trace[TCurrentRelBaroPressure] = CurrentRelBaroPressure;
		#endif
	}
}// GetBaroPressure

void InitBarometer(void)
{	
	int24 BaroAv;
	uint8 s;
	uint8 r;

	BaroComp = BaroAverage = 0;

	// Determine baro type
	I2CStart();

	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto BAerror;
	if( SendI2CByte(BARO_TYPE) != I2C_ACK ) goto BAerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto BAerror;
	BaroType = RecvI2CByte(I2C_NACK);
	I2CStop();

	// read pressure once to get base value
	StartBaroADC();
	if ( !F[BaroAltitudeValid] ) goto BAerror;

	BaroAv = 0;
	for ( s = 32; s ; s-- )
	{
		while ( mS[Clock] < mS[BaroUpdate] );
		ReadBaro();
		BaroAv += (int24)BaroVal.u16;	
	}
	
	OriginBaroPressure = (int24)(BaroAv >> 5);
	CurrentRelBaroPressure = BEp = 0;
	F[NewBaroValue] = false;
	BaroSample = 0;

	F[BaroAltitudeValid] = true;

	return;

BAerror:
	F[BaroAltitudeValid] = F[Hovering] = false;
	Stats[BaroFailS].i16++;
	I2CStop();
} // InitBarometer

void CheckForHover(void)
{
	CheckThrottleMoved();
	
	if( F[ThrottleMoving] )	// while moving throttle stick
	{
		F[Hovering] = false;
		DesiredRelBaroPressure = CurrentRelBaroPressure;
		BaroComp = BE = BEp = 0;	
	}
	else
		F[Hovering] = F[BaroAltitudeValid];

	if ( F[Hovering] )
		BaroPressureHold(DesiredRelBaroPressure);

} // CheckForHover

void BaroPressureHold(int16 DesiredRelBaroPressure)
{	// decreasing pressure is increasing altitude
	static int16 Temp, Delta;

	if ( F[NewBaroValue] && F[BaroAltitudeValid] )
	{
		F[NewBaroValue] = false;

		#ifdef BARO_SCRATCHY_BEEPER
		Beeper_TOG;
		#endif
	
		BE = CurrentRelBaroPressure - DesiredRelBaroPressure;		
		BE = Limit(BE, -5, 10); 
			
		// strictly this is acting more like an integrator 
		// bumping VBaroComp up and down proportional to the error?	
		Temp = SRS16(BE * (int16)P[BaroCompKp], 4);
		if( BaroComp > Temp )
			BaroComp--;
		else
			if( BaroComp < Temp )
				BaroComp++; // climb
					
		// Differential	
		Delta = Limit(BE - BEp , -5, 8);	
		BaroComp += SRS16(Delta * (int16)P[BaroCompKd], 2);
		
		BaroComp = Limit(BaroComp, BARO_LOW_THR_COMP, BARO_HIGH_THR_COMP);

		if ( !(  F[RTHAltitudeHold] && ( NavState == ReturningHome ) ) )
		{
			Temp = DesiredThrottle + BaroComp;
			HoverThrottle = HardFilter(HoverThrottle, Temp);
		}
	
		BEp = BE;
	
		#ifdef BARO_SCRATCHY_BEEPER
		Beeper_TOG;
		#endif
	}

} // BaroPressureHold	

