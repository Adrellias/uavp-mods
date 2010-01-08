// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =                 Copyright (c) 2008 by Prof. Greg Egan               =
// =       Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer     =
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

void StartBaroADC(boolean);
void ReadBaro(boolean);
void GetBaroAltitude(void);
void InitBarometer(void);
void BaroAltitudeHold(void);
void AltitudeHold(void);

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

	F.CompassValid = true;
	return;
CTerror:
	F.CompassValid = false;
	Stats[CompassFailS].i16++;
	F.CompassFailure = true;
	
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

	if( F.CompassValid ) // continuous mode but Compass only updates avery 50mS
	{
		I2CStart();
		F.CompassMissRead = SendI2CByte(COMPASS_I2C_ID+1) != I2C_ACK; 
		Compass.high8 = RecvI2CByte(I2C_ACK);
		Compass.low8 = RecvI2CByte(I2C_NACK);
		I2CStop();

		Temp = ConvertDDegToMPi(Compass.i16) - CompassOffset;
		Heading = Make2Pi((int16) Temp);

		if ( F.CompassMissRead && (State == InFlight) ) Stats[CompassFailS].i16++;	
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

#define BARO_BUFF_SIZE 8	// MUST be 8
static uint8 BaroQHead, BaroQTail;
static int24 BaroQ[BARO_BUFF_SIZE];

void StartBaroADC(boolean ReadPressure)
{
	uint8 TempOrPress;
	if ( ReadPressure )
	{
		TempOrPress = BARO_PRESS;
		mS[BaroUpdate] = mS[Clock] + BARO_PRESS_TIME_MS;
	}
	else
	{
		mS[BaroUpdate] = mS[Clock] + BARO_TEMP_TIME_MS;	
		if ( BaroType == BARO_ID_BMP085 )
			TempOrPress = BARO_TEMP_BMP085;
		else
			TempOrPress = BARO_TEMP_SMD500;
	}

	I2CStart();
	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto SBerror;

	// access control register, start measurement
	if( SendI2CByte(BARO_CTL) != I2C_ACK ) goto SBerror;

	// select 32kHz input, measure temperature
	if( SendI2CByte(TempOrPress) != I2C_ACK ) goto SBerror;
	I2CStop();

	F.BaroAltitudeValid = true;
	return;
SBerror:
	I2CStop();
	F.BaroAltitudeValid = F.Hovering = false; 
	return;
} // StartBaroADC

void ReadBaro(boolean ReadPressure)
{
	static i16u BaroVal;

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

	if ( ReadPressure )
		BaroPress.u16 = BaroVal.u16;
	else
		BaroTemp.u16 = BaroVal.u16;

	return;

RVerror:
	I2CStop();

	F.BaroAltitudeValid = F.Hovering = false;
	if ( State == InFlight ) 
	{
		Stats[BaroFailS].i16++; 
		F.BaroFailure = true;
	}
	return;
} // ReadBaro

#define BARO_SCALE				380L			// dM per click scaled up by 128

void GetBaroAltitude(void)
{ 	// Use sum of 8 samples as the "pressure" to give some noise cancellation	
	static int16 Temp;
	// SMD500 9.5mS (T) 34mS (P)  
	// BMP085 4.5mS (T) 25.5mS (P) OSRS=3
	// Use 50mS => 5 samples per altitude hold update

	if ( mS[Clock] > mS[BaroUpdate] )
	{
		ReadBaro(true);
		StartBaroADC(true);

		BaroQHead = (BaroQHead + 1) & (BARO_BUFF_SIZE -1); // must be 8 entries in the BaroQ
		BaroSum -= BaroQ[BaroQHead];
	
		Temp = (int16)( (int24)BaroPress.u16 - OriginBaroPressure ); 
		BaroSum += Temp;
		BaroQTail = (BaroQTail + 1) & (BARO_BUFF_SIZE -1);
		BaroQ[BaroQTail] = Temp;

		if ( ++BaroSample == 5 ) // time BARO_PRESS_TIME_MS intervals
		{
			// decreasing pressure is increase in altitude
			// negate and rescale to cm altitude
			CurrentRelBaroAltitude = -SRS32((int32)BaroSum * 380, 7);

			CurrentBaroROC = ( CurrentRelBaroAltitude - RelBaroAltitudeP ) * 4;
			CurrentBaroROC = HardFilter(BaroROCP, CurrentBaroROC);
	
			RelBaroAltitudeP = CurrentRelBaroAltitude;
			BaroROCP = CurrentBaroROC;

			if ( State == InFlight )
			{
				if ( CurrentBaroROC > Stats[MaxBaroROCS].i16 )
					Stats[MaxBaroROCS].i16 = CurrentBaroROC;
				else
					if ( CurrentBaroROC < Stats[MinBaroROCS].i16 )
						Stats[MinBaroROCS].i16 = CurrentBaroROC;
				if ( CurrentRelBaroAltitude > Stats[RelBaroAltitudeS].i16 ) 
					Stats[RelBaroAltitudeS].i16 = CurrentRelBaroAltitude;
			}
			F.NewBaroValue = true;
			BaroSample = 0;

			#ifdef DEBUG_SENSORS	
			Trace[TCurrentRelBaroAltitude] = CurrentRelBaroAltitude;
			#endif
		}
	}
}// GetBaroAltitude

void InitBarometer(void)
{	
	int24 BaroAverage;
	uint8 s;

	for ( s = 0; s < BARO_BUFF_SIZE; s ++ ) 
		BaroQ[s] = 0; 
	CurrentRelBaroAltitude = RelBaroAltitudeP = CurrentBaroROC = BaroROCP = 0;
	BaroSum = BaroSample = BaroComp = OriginBaroPressure = 0;
	BaroQTail = 0;  BaroQHead = 1;

	F.NewBaroValue = false;
	F.BaroAltitudeValid = true;

	// Determine baro type
	I2CStart();

	if( SendI2CByte(BARO_I2C_ID) != I2C_ACK ) goto BAerror;
	if( SendI2CByte(BARO_TYPE) != I2C_ACK ) goto BAerror;
	I2CStart();	// restart
	if( SendI2CByte(BARO_I2C_ID+1) != I2C_ACK ) goto BAerror;
	BaroType = RecvI2CByte(I2C_NACK);
	I2CStop();

	StartBaroADC(false);		// get temperature
	while ( mS[Clock] < mS[BaroUpdate] );
	ReadBaro(false);
	// read pressure once to get base value
	StartBaroADC(true);

	if ( !F.BaroAltitudeValid ) goto BAerror;

	BaroAverage = 0;
	for ( s = 32; s ; s-- )
	{
		while ( mS[Clock] < mS[BaroUpdate] );
		ReadBaro(true);
		StartBaroADC(true);
		BaroAverage += (int24)BaroPress.u16;	
	}

	OriginBaroPressure = (int24)(BaroAverage >> 5);

	return;

BAerror:
	F.BaroAltitudeValid = F.Hovering = false;
	Stats[BaroFailS].i16++;
	I2CStop();
} // InitBarometer

void BaroAltitudeHold()
{
	static int16 Corr, DesiredBaroROC, BaroROCE, Temp;

	if ( F.NewBaroValue && F.BaroAltitudeValid )
	{
		F.NewBaroValue = false;

		#ifdef BARO_SCRATCHY_BEEPER
		if ( !F.BeeperInUse ) Beeper_TOG;
		#endif
	
		BE = CurrentRelBaroAltitude - DesiredRelBaroAltitude;

		if ( BE > BARO_ALT_BAND_CM )
		{ // control descent using ROC
			if ( BE > BARO_DESCENT_TRANS_CM )
				DesiredBaroROC = BARO_MAX_DESCENT_CMPS;
			else
				DesiredBaroROC = BARO_FINAL_DESCENT_CMPS;

			BaroROCE = CurrentBaroROC - DesiredBaroROC; // probably need scaling
			Temp = Limit(BaroROCE, -BARO_MAX_ROC_CMPS, BARO_MAX_ROC_CMPS); 
			Corr = -SRS16(Temp * (int16)P[BaroROCCompKp], 8); // (int16)P[BaroROCCompKp]
		}
		else
		{		
			Temp = Limit(BE, -BARO_ALT_BAND_CM, BARO_ALT_BAND_CM); // prevent overflow MAX BaroCompKp = 32	
			Corr = -SRS16(Temp * (int16)P[BaroCompKp], 7);						
		}

		Corr = Limit(Corr, ALT_LOW_THR_COMP, ALT_HIGH_THR_COMP);
		if( Corr > BaroComp )
			#ifdef BARO_DOUBLE_UP_COMP
			BaroComp+=2;
			#else
			BaroComp++;
			#endif // BARO_DOUBLE_UP_COMP
		else
			if( Corr < BaroComp )
				BaroComp--;

		#ifdef BARO_SCRATCHY_BEEPER
		if ( !F.BeeperInUse ) Beeper_TOG;
		#endif
	}

} // BaroAltitudeHold	

void AltitudeHold()
{
	if ( F.RTHAltitudeHold && ( NavState != HoldingStation ) )
	{
		F.Hovering = false;
		BaroAltitudeHold();
	}
	else // holding station
	{
		F.Hovering = !F.ThrottleMoving; 
		
		if( F.Hovering )
		{
			if ( Abs(CurrentBaroROC) < BARO_HOVER_MAX_ROC_CMPS )
				HoverThrottle = HardFilter(HoverThrottle, DesiredThrottle);
			BaroAltitudeHold();
		}
		else	
		{
			DesiredRelBaroAltitude = CurrentRelBaroAltitude;
			BaroComp = Decay1(BaroComp);	
		}
	}

} // AltitudeHold

