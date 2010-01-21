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

void DoLEDs(void);
void LinearTest(void);
uint8 ScanI2CBus(void);
void ReceiverTest(void);
void GetCompassParameters(void);
void DoCompassTest(void);
void CompassRun(void);
void CalibrateCompass(void);
void BaroTest(void);
void PowerOutput(int8);
void LEDsAndBuzzer(void);
void GPSTest(void);
void AnalogTest(void);
void ProgramSlaveAddress(uint8);
void ConfigureESCs(void);

void DoLEDs(void)
{
	if ( F.AccelerationsValid  ) LEDYellow_ON; else LEDYellow_OFF;

	if( F.Signal )
	{
		LEDRed_OFF;
		LEDGreen_ON;
	}
	else
	{
		LEDGreen_OFF;
		LEDRed_ON;
	}
} // DoLEDs

void LinearTest(void)
{
	TxString("\r\nAccelerometer test:\r\n");
	TxString("Read once - no averaging\r\n");
	if( F.AccelerationsValid )
	{
		ReadAccelerations();
	
		TxString("\tL->R: \t");
		TxVal32(((int32)Ax.i16*1000+512)/1024, 3, 'G');
		TxString(" (");
		TxVal32((int32)Ax.i16, 0 , ')');
		if ( Abs((Ax.i16)) > 128 )
			TxString(" fault?");
		TxNextLine();

		TxString("\tF->B: \t");	
		TxVal32(((int32)Az.i16*1000+512)/1024, 3, 'G');
		TxString(" (");
		TxVal32((int32)Az.i16, 0 , ')');
		if ( Abs((Az.i16)) > 128 )
			TxString(" fault?");	
		TxNextLine();

		TxString("\tD->U:    \t");
	
		TxVal32(((int32)Ay.i16*1000+512)/1024, 3, 'G');
		TxString(" (");
		TxVal32((int32)Ay.i16 - 1024, 0 , ')');
		if ( ( Ay.i16 < 896 ) || ( Ay.i16 > 1152 ) )
			TxString(" fault?");	
		TxNextLine();
	}
	else
		TxString("\r\n(Acc. not present)\r\n");
} // LinearTest

uint8 ScanI2CBus(void)
{
	int16 s;
	uint8 d;

	d = 0;

	TxString("Sensor Bus\r\n");
	for ( s = 0x10 ; s <= 0xf6 ; s += 2 )
	{
		I2CStart();
		if( SendI2CByte(s) == I2C_ACK )
		{
			d++;
			TxString("\t0x");
			TxValH(s);
			TxNextLine();
		}
		I2CStop();

		Delay1mS(2);
	}

	TxString("\r\nESC Bus\r\n");

	if ( (P[ESCType] == ESCHolger)||(P[ESCType] == ESCX3D)||(P[ESCType] == ESCYGEI2C) )
		for ( s = 0x10 ; s <= 0xf6 ; s += 2 )
		{
			ESCI2CStart();
			if( SendESCI2CByte(s) == I2C_ACK )
			{
				d++;
				TxString("\t0x");
				TxValH(s);
				TxNextLine();
			}
			ESCI2CStop();
	
			Delay1mS(2);
		}
	else
		TxString("\tinactive - I2C ESCs not selected..\r\n");

	TxNextLine();

	return(d);
} // ScanI2CBus

void ReceiverTest(void)
{
	uint8 s;
	uint16 v;

	TxString("\r\nRx: ");
	ShowRxSetup();
	TxString("\r\n");
	
	TxString("\tRAW Rx frame values - neutrals NOT applied\r\n");
	TxString("\tChannel order is: ");
	for ( s = 0; s < RC_CONTROLS; s++)
		TxChar(RxChMnem[RMap[s]-1]);

	if ( F.Signal )
		TxString("\r\nSignal OK ");
	else
		TxString("\r\nSignal FAIL ");	
	TxVal32(mS[Clock] - mS[LastValidRx], 3, 0);
	TxString(" Sec. ago\r\n");
	
	// Be wary as new RC frames are being received as this
	// is being displayed so data may be from overlapping frames

	for ( s = 0; s < RC_CONTROLS ; s++ )
	{
		TxChar(s+'1');
		TxString(": ");
		TxChar(RxChMnem[RMap[s]-1]);
		TxString(":\t 0x");
		v = PPM[s].i16;
		TxValH16(v);
		TxChar(HT);
	    TxVal32(((int32)(v & 0x00ff)*100)/RC_MAXIMUM, 0, '%');

		if( ( v & 0xff00) != (uint16)0x0100 ) 
			TxString(" FAIL");
		TxNextLine();
	}

	// show pause time
	TxString("Gap:\t");
	v = TMR2_TICK*PauseTime;
	TxVal32( v, 3, 0);		
	TxString("mS\r\n");
	TxString("Glitches:\t");
	TxVal32(RCGlitches,0,0);
	TxNextLine();

} // ReceiverTest

static uint8 CP[9];

void GetCompassParameters(void)
{
	uint8 r;

	#define COMP_OPMODE 0b01110000	// standby mode to reliably read EEPROM

	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('G')  != I2C_ACK ) goto CTerror;
	if( SendI2CByte(0x74) != I2C_ACK ) goto CTerror;
	if( SendI2CByte(COMP_OPMODE) != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(COMPASS_TIME_MS);

	for (r = 0; r <= 8; r++)
	{
		CP[r] = 0xff;

		Delay1mS(10);

		I2CStart();
		if ( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
		if ( SendI2CByte('r')  != I2C_ACK ) goto CTerror;
		if ( SendI2CByte(r)  != I2C_ACK ) goto CTerror;
		I2CStop();

		Delay1mS(10);

		I2CStart();
		if( SendI2CByte(COMPASS_I2C_ID+1) != I2C_ACK ) goto CTerror;
		CP[r] = RecvI2CByte(I2C_NACK);
		I2CStop();
	}

	Delay1mS(7);

	return;

CTerror:
	I2CStop();
	TxString("FAIL\r\n");

} // GetCompassParamters

void DoCompassTest(void)
{
	uint16 v, prev;
	int16 Temp;
	i16u Compass;

	TxString("\r\nCompass test\r\n");

	#define COMP_OPMODE 0b01110000	// standby mode to reliably read EEPROM

	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('G')  != I2C_ACK ) goto CTerror;
	if( SendI2CByte(0x74) != I2C_ACK ) goto CTerror;
	if( SendI2CByte(COMP_OPMODE) != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(1);

	I2CStart(); // Do Set/Reset now		
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('O')  != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(7);

	GetCompassParameters();

	InitCompass();

	TxString("Registers\r\n");
	TxString("0:\tI2C"); 
	TxString("\t 0x"); TxValH(CP[0]); 
	if ( CP[0] != 0x42 ) 
		TxString("\t Error expected 0x42 for HMC6352");
	TxNextLine();

	Temp = (CP[1]*256)|CP[2];
	TxString("1:2:\tXOffset\t"); 
	TxVal32((int32)Temp, 0, 0); 
	TxNextLine(); 

	Temp = (CP[3]*256)|CP[4];
	TxString("3:4:\tYOffset\t"); 
	TxVal32((int32)Temp, 0, 0); 
	TxNextLine(); 

	TxString("5:\tDelay\t"); 
	TxVal32((int32)CP[5], 0, 0); 
	TxNextLine(); 

	TxString("6:\tNSum\t"); TxVal32((int32)CP[6], 0, 0);
	if ( CP[6] !=16 ) 
		TxString("\t Error expected 16");
	TxNextLine(); 

	TxString("7:\tSW Ver\t"); 
	TxString(" 0x"); TxValH(CP[7]); 
	TxNextLine(); 

	TxString("8:\tOpMode:");
	switch ( ( CP[8] >> 5 ) & 0x03 ) {
		case 0: TxString("  1Hz"); break;
		case 1: TxString("  5Hz"); break;
		case 2: TxString("  10Hz"); break;
		case 3: TxString("  20Hz"); break;
		}
 
	if ( CP[8] & 0x10 ) TxString(" S/R"); 

	switch ( CP[8] & 0x03 ) {
		case 0: TxString(" Standby"); break;
		case 1: TxString(" Query"); break;
		case 2: TxString(" Continuous"); break;
		case 3: TxString(" Not-allowed"); break;
		}
	TxNextLine(); 

	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID+1) != I2C_ACK ) goto CTerror;
	Compass.high8 = RecvI2CByte(I2C_ACK);
	Compass.low8 = RecvI2CByte(I2C_NACK);
	I2CStop();

	TxVal32((int32)Compass.u16, 1, 0);
	TxString(" deg \tCorrected ");

	Temp = Compass.u16 - (COMPASS_OFFSET_DEG - (int16)P[NavMagVar])*10;
	if ( F.UsingXMode )
		Temp -= 450;
	while (Temp < 0) Temp +=3600;
	while (Temp >= 3600) Temp -=3600;
	TxVal32((int32)Temp, 1, 0);
	TxString(" deg\r\n");
		
	return;
CTerror:
	I2CStop();
	TxString("FAIL\r\n");

} // DoCompassTest

/*
void CompassRun(void)
{
	int16 i, r, Temp;
	static i16u Compass;	

	for (i = 0; i < 32000; i++)
	{
		I2CStart();
		F.CompassMissRead |= SendI2CByte(COMPASS_I2C_ID+1) != I2C_ACK; 
		Compass.high8 = RecvI2CByte(I2C_ACK);
		Compass.low8 = RecvI2CByte(I2C_NACK);
		I2CStop();
		
		TxVal32((int32) mS[Clock],3,' ');

		TxVal32((int32) Compass.i16,1,' ');
		TxNextLine();
	}
} // CompassRun
*/

void CalibrateCompass(void)
{	// calibrate the compass by rotating the ufo through 720 deg smoothly
	TxString("\r\nCalib. compass - Press any key (x) to continue\r\n");	
	while( PollRxChar() != 'x' ); // UAVPSet uses 'x' for any key button

	I2CStart(); // Do Set/Reset now		
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CCerror;
	if( SendI2CByte('O')  != I2C_ACK ) goto CCerror;
	I2CStop();

	Delay1mS(7);

	// set Compass device to Calibration mode 
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CCerror;
	if( SendI2CByte('C')  != I2C_ACK ) goto CCerror;
	I2CStop();

	TxString("\r\nRotate horizontally 720 deg in ~30 sec. - Press any key (x) to FINISH\r\n");
	while( PollRxChar() != 'x' );

	// set Compass device to End-Calibration mode 
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CCerror;
	if( SendI2CByte('E')  != I2C_ACK ) goto CCerror;
	I2CStop();

	TxString("\r\nCalibration complete\r\n");

	Delay1mS(COMPASS_TIME_MS);

	InitCompass();

	return;
CCerror:
	I2CStop();
	TxString("Calibration FAILED\r\n");

} // CalibrateCompass

void BaroTest(void)
{
	TxString("\r\nBarometer test\r\n");
	if ( !F.BaroAltitudeValid ) goto BAerror;

	if ( BaroType == BARO_ID_BMP085 )
		TxString("Type:\tBMP085\r\n");
	else
		TxString("Type:\tSMD500\r\n");
	
	while ( mS[Clock] <= mS[BaroUpdate] );
	ReadBaro();
	CurrentRelBaroPressure = (int24)BaroVal.u16 - OriginBaroPressure;		
	TxString("Origin: \t");
	TxVal32((int32)OriginBaroPressure,0,0);
	TxString("\t Rel.: \t");	
	TxVal32((int32)CurrentRelBaroPressure, 0, 0);
		
	TxNextLine();

	return;
BAerror:
	I2CStop();
	TxString("FAIL\r\n");
} // BaroTest

void PowerOutput(int8 d)
{
	int8 s;
	uint8 m;

	m = 1 << d;
	for( s=0; s < 10; s++ )	// 10 flashes (count MUST be even!)
	{
		LEDShadow ^= m;
		SendLEDs();
		Delay1mS(50);
	}		
} // PowerOutput

void LEDsAndBuzzer(void)
{
	uint8 s, m, mask, LEDSave;

	LEDSave = LEDShadow;
	LEDShadow  = 0;
	SendLEDs();	

	TxString("\r\nOutput test\r\n");
	mask = 1;
	for ( m = 1; m <= 8; m++ )		
	{
		TxChar(HT);
		TxChar(m+'0');
		TxString(":8 ");
		switch( m ) {
		case 1: TxString("Aux2   "); break;
		case 2: TxString("Blue   "); break;
		case 3: TxString("Red    "); break;
		case 4: TxString("Green  "); break;
		case 5: TxString("Aux1   "); break;
		case 6: TxString("Yellow "); break;
		case 7: TxString("Aux3   "); break;
		case 8: TxString("Beeper "); break;
		}
		TxString("\tPress any key (x) to continue\r\n");	
		while( PollRxChar() != 'x' ); // UAVPSet uses 'x' for any key button

		for( s = 0; s < 10; s++ )	// 10 flashes (count MUST be even!)
		{
			LEDShadow ^= mask;
			SendLEDs();
			Delay1mS(50);
		}
		mask <<= 1;
	}
	LEDShadow  = LEDSave;
	SendLEDs();	
	TxString("Test Finished\r\n");		
} // PowerOutput

void GPSTest(void)
{
	uint8 ch; 

	TxString("\r\nGPS test\r\n");
	TxString("Monitors GPS input at 9.6Kb - units GPS lsb (~0.185M) and degrees\r\n");
	TxString("ARM the quadrocopter to connect the GPS\r\n");
	TxString("Set Baud Rate to 9.6Kb - you will get trash on the screen until you do this \r\n");
	TxString("DISARM to terminate the test \r\n");

	ReceivingGPSOnly(true);
	
	while ( !Armed )
	{
		Delay1mS(500);
		TxChar('.');
	}

	F.GPSTestActive = true;

	while ( Armed )
	{
		DoLEDs();

		UpdateGPS();	
		if ( F.GPSValid )
		{
			GetHeading();

			if ( F.CompassValid )
				TxVal32((int32)ConvertMPiToDDeg(Heading), 1, 'D');
			else
				TxString("___");
			if ( F.CompassMissRead )
				TxChar('?');
			else
				TxChar(' ');
			
			TxString(" fx=");
			TxVal32(GPSFix, 0, ' ');
	
			TxString("s=");
			TxVal32(GPSNoOfSats, 0, ' ');
	
			TxString("hd=");
			TxVal32(GPSHDilute, 2, ' ');
	
			TxString("ra=");
			TxVal32(GPSRelAltitude, 1, 'M');

			TxString(" re=");
			TxVal32(((int32)GPSEast * 1855L)/1000L , 1,'M'); 

			TxString(" rn=");
			TxVal32(((int32)GPSNorth * 1855L)/1000L, 1,'M');

			TxNextLine();
		}	
		F.GPSValid = false;	
	}

	TxString("GPS Test TERMINATED \r\n");
	TxString("Set Baud Rate to 38.4Kb \r\n");
	ReceivingGPSOnly(false);
 	F.GPSTestActive = false;
	
} // GPSTest

void AnalogTest(void)
{
	int32 v;

	TxString("\r\nAnalog ch. test\r\n");

	// Roll
	if ( P[GyroType] == IDG300 )
		v = ((int24)ADC(IDGADCRollChan, ADCVREF5V) * 50 + 5)/10; // resolution is 0,001 Volt
	else
		v = ((int24)ADC(NonIDGADCRollChan, ADCVREF5V) * 50 + 5)/10; 
	//TxVal32(ADCRollChan, 0, ' ');
	TxString("Roll: \t"); 
	TxVal32(v, 3, 'V');
	if ( P[GyroType] == IDG300 )
	{
		if ( ( v < 750 ) || ( v > 1750 ) ) 
			TxString(" gyro NC or fault?");
	}
	else
	{
		if ( ( v < 2000 ) || ( v > 3000 ) )
			TxString(" gyro NC or fault?");
	}
	TxNextLine();

	// Pitch
	if ( P[GyroType] == IDG300 )
		v = ((int24)ADC(IDGADCPitchChan, ADCVREF5V) * 50 + 5)/10; 
	else
		v = ((int24)ADC(NonIDGADCPitchChan, ADCVREF5V) * 50 + 5)/10; 
	//TxVal32(ADCPitchChan, 0, ' ');
	TxString("Pitch:\t");		
	TxVal32(v, 3, 'V');
	if ( P[GyroType] == IDG300 )
	{
		if ( ( v < 750 ) || ( v > 1750 ) ) 
			TxString(" gyro NC or fault?");
	}
	else
	{
		if ( ( v < 2000 ) || ( v > 3000 ) )
			TxString(" gyro NC or fault?");
	}	
	TxNextLine();

	// Yaw
	v = ((int24)ADC(ADCYawChan, ADCVREF5V) * 50 + 5)/10; // resolution is 0,001 Volt
	//TxVal32(ADCYawChan, 0, ' ');
	TxString("Yaw:  \t");
	TxVal32(v, 3, 'V');
	if ( ( v < 2000 ) || ( v > 3000 ) )
		TxString(" gyro NC or fault?");	
	TxNextLine();
	TxNextLine();

	// Battery
	v = ((int24)ADC(ADCBattVoltsChan, ADCVREF5V) * 46 + 9)/17; // resolution is 0,01 Volt
	//TxVal32(ADCBattVoltsChan, 0, ' ');
	TxString("Batt:\t");
	TxVal32(v, 2, 'V');
	if ( v < 900 )
		TxString(" ** LIPO ALARM < 9V ** ");
	else	
	if ( v < 950 )
		TxString(" ** LOW < 9.5V ** ");
	TxNextLine();

	// VRef
	v = ((int24)ADC(ADCVRefChan, ADCVREF5V) * 50 + 5)/10; // resolution is 0,001 Volt
	//TxVal32(ADCVRefChan, 0, ' ');	
	TxString("Ref:  \t");	
	TxVal32(v, 3, 'V');
	if ( ( v < 3000 ) || ( v > 4000 ) )
		TxString(" fault?");	
	TxNextLine();	
} // AnalogTest

void ProgramSlaveAddress(uint8 addr)
{
	static uint8 s;

	for (s = 0x10 ; s < 0xf0 ; s += 2 )
	{
		ESCI2CStart();
		if( SendESCI2CByte(s) == I2C_ACK )
			if( s == addr )
			{	// ESC is already programmed OK
				ESCI2CStop();
				TxString("\tESC at SLA 0x");
				TxValH(addr);
				TxString(" is already programmed OK\r\n");
				return;
			}
			else
			{
				if( SendESCI2CByte(0x87) == I2C_ACK ) // select register 0x07
					if( SendESCI2CByte( addr ) == I2C_ACK ) // new slave address
					{
						ESCI2CStop();
						TxString("\tESC at SLA 0x");
						TxValH(s);
						TxString(" reprogrammed to SLA 0x");
						TxValH(addr);
						TxNextLine();
						return;
					}
			}
		ESCI2CStop();
	}
	TxString("\tESC at SLA 0x");
	TxValH(addr);
	TxString(" no response - check cabling and pullup resistors!\r\n");
} // ProgramSlaveAddress

boolean CheckESCBus(void)
{
	return ( true );
} // CheckESCBus

void ConfigureESCs(void)
{
	uint8 m, s;

	if ( P[ESCType] == ESCYGEI2C )		
	{
		TxString("\r\nProgram YGE ESCs\r\n");
		for ( m = 0 ; m < NoOfMotors ; m++ )
		{
			TxString("Connect ONLY ");
			switch( m )
			{
				case 0 : TxString("Front"); break;
				case 1 : TxString("Back");  break;
				case 2 : TxString("Right"); break;
				case 3 : TxString("Left");  break;
			}
			TxString(" ESC, then press any key \r\n");
			while( PollRxChar() != 'x' ); // UAVPSet uses 'x' for any key button
		//	TxString("\r\n");
			ProgramSlaveAddress( 0x62 + ( m*2 ));
		}
		TxString("\r\nConnect ALL ESCs and power-cycle the Quadrocopter\r\n");
	}
	else
		TxString("\r\nYGEI2C not selected as ESC?\r\n");
} // ConfigureESCs


