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
//    If not, see http://www.gnu.org/licenses/.

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

#define BaroFilter NoFilter
#define BaroROCFilter MediumFilter

#define BARO_BUFF_SIZE 8	// MUST be 8
#pragma udata baroq
static int16Q BaroQ;
#pragma udata

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

#define BARO_BMP085_TEMP_COEFF		62L 	
#define BARO_SMD500_TEMP_COEFF		81L

void GetBaroAltitude(void)
{ 	// Use sum of 8 samples as the "pressure" to give some noise cancellation	
	static int24 Temp, RelPressSample, CompBaroPress;
	static int32 RelTemp;
	// SMD500 9.5mS (T) 34mS (P)  
	// BMP085 4.5mS (T) 25.5mS (P) OSRS=3
	// Use 50mS => 5 samples per altitude hold update

	if ( mS[Clock] >= mS[BaroUpdate] ) // 5 pressure readings and 1 temperature reading
	{
		BaroSample++;
		if ( BaroSample < 6)		
		{
			ReadBaro(true);
			if ( BaroSample == 5 )
				StartBaroADC(false);
			else
				StartBaroADC(true);
	
			BaroQ.Head = (BaroQ.Head + 1) & (BARO_BUFF_SIZE -1); // must be 8 entries in the BaroQ
			BaroSum -= (int24)BaroQ.B[BaroQ.Head];
		
			RelPressSample = (int24)((int24)BaroPress.u16 - OriginBaroPressure); 
			BaroSum += RelPressSample;
			BaroQ.Tail = (BaroQ.Tail + 1) & (BARO_BUFF_SIZE -1);
			BaroQ.B[BaroQ.Tail] = (int16)RelPressSample;
		}
		else
		{
			ReadBaro(false);
			RelTemp = (int32)BaroTemp.u16 - OriginBaroTemperature;
			if ( BaroType == BARO_ID_BMP085 )
				BaroTempComp = SRS32(RelTemp * BARO_BMP085_TEMP_COEFF, 4);
			else
				BaroTempComp = SRS32(RelTemp * BARO_SMD500_TEMP_COEFF, 2);
			StartBaroADC(true);
		}

		if ( BaroSample == 6 ) // time BARO_PRESS_TIME_MS intervals
		{
			// decreasing pressure is increase in altitude
			// negate and rescale to cm altitude

			Temp = (int24)BaroSum + BaroTempComp;
			CompBaroPress = BaroFilter(CompBaroPress, Temp);
			RelBaroAltitude = -SRS32((int32)CompBaroPress * (int16)P[BaroScale], 5);

			BaroROC = ( RelBaroAltitude - RelBaroAltitudeP ) * 4;
			BaroROC = BaroROCFilter(BaroROCP, BaroROC);
	
			RelBaroAltitudeP = RelBaroAltitude;
			BaroROCP = BaroROC;

			if ( State == InFlight )
			{
				if ( BaroROC > Stats[MaxBaroROCS].i16 )
					Stats[MaxBaroROCS].i16 = BaroROC;
				else
					if ( BaroROC < Stats[MinBaroROCS].i16 )
						Stats[MinBaroROCS].i16 = BaroROC;
				if ( RelBaroAltitude > MaxRelBaroAltitudeS ) 
				{ 
					MaxRelBaroAltitudeS = RelBaroAltitude;
					Stats[RelBaroPressureS].i16 = CompBaroPress;
				}
			}
			F.NewBaroValue = true;
			BaroSample = 0;

			#ifdef DEBUG_SENSORS	
			Trace[TRelBaroAltitude] = RelBaroAltitude/10L;
			#endif
		}
	}
}// GetBaroAltitude

void InitBarometer(void)
{	
	int24 BaroAverage;
	uint8 s;

	for ( s = 0; s < BARO_BUFF_SIZE; s ++ ) 
		BaroQ.B[s] = 0; 
	RelBaroAltitude = RelBaroAltitudeP = BaroROC = BaroROCP = 0;
	BaroSum = BaroSample = BaroComp = OriginBaroPressure = BaroDiffSum = 0;
	BaroQ.Tail = 0;  BaroQ.Head = 1;

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

	while ( mS[Clock] < mS[BaroUpdate] ); // wait until outstanding pressure read completes
	ReadBaro(true);

	StartBaroADC(false);		// get temperature
	while ( mS[Clock] < mS[BaroUpdate] );
	ReadBaro(false);
	OriginBaroTemperature = (int24)BaroTemp.u16;
	BaroTempComp = 0;
	StartBaroADC(true);

	OriginBaroPressure = (int24)(BaroAverage >> 5);

	return;

BAerror:
	F.BaroAltitudeValid = F.Hovering = false;
	Stats[BaroFailS].i16++;
	I2CStop();
} // InitBarometer

void BaroAltitudeHold()
{
	static int16 NewBaroComp, LimBE, BaroP, BaroI, BaroD;
	static int24 Temp;

	if ( F.NewBaroValue && F.BaroAltitudeValid )
	{
		F.NewBaroValue = false;

		#ifdef BARO_SCRATCHY_BEEPER
		if ( !F.BeeperInUse ) Beeper_TOG;
		#endif
	
		BE = RelBaroAltitude - DesiredRelBaroAltitude;
		LimBE = Limit(BE, -BARO_ALT_BAND_CM, BARO_ALT_BAND_CM);

		NewBaroComp = -SRS16(LimBE * (int16)P[BaroCompKp], 7);
		
		if ( BE < BARO_ALT_BAND_CM )
			BaroDiffSum = Decay1(BaroDiffSum);
		else
			if ( (( BE < BARO_DESCENT_TRANS_CM )&&( BaroROC < BARO_FINAL_DESCENT_CMPS )) ||
				 (BaroROC < BARO_MAX_DESCENT_CMPS) )
				BaroDiffSum++;
		BaroDiffSum = Limit(BaroDiffSum, 0, 6); // trial number!  
	
		NewBaroComp += BaroDiffSum;
		
		BaroComp = SlewLimit(BaroComp, NewBaroComp, 2);
		BaroComp = Limit(BaroComp, ALT_LOW_THR_COMP, ALT_HIGH_THR_COMP);

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
			if ( Abs(BaroROC) < BARO_HOVER_MAX_ROC_CMPS )
				HoverThrottle = HardFilter(HoverThrottle, DesiredThrottle);
			BaroAltitudeHold();
		}
		else	
		{
			DesiredRelBaroAltitude = RelBaroAltitude;
			BaroComp = Decay1(BaroComp);	
		}
	}

} // AltitudeHold

