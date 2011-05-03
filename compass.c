// ===============================================================================================
// =                                UAVX Quadrocopter Controller                                 =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                     http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// ===============================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

// Compass 100KHz I2C

#include "uavx.h"

int16 GetCompass(void);
void GetHeading(void);
int16 MinimumTurn(int16);
void GetCompassParameters(void);
void DoCompassTest(void);
void CalibrateCompass(void);
void InitHeading(void);
void InitCompass(void);

i24u 	Compass;
i32u 	HeadingValF;
int16 	MagHeading;

void GetHeading(void)
{
	static int16 HeadingChange;

	if( F.CompassValid ) // continuous mode but Compass only updates avery 50mS
	{
		MagHeading = GetCompass();
		Heading = Make2Pi(MagHeading - CompassOffset);

		HeadingChange = Abs( Heading - HeadingValF.iw1 );
	    if ( HeadingChange > MILLIPI )// wrap 0 -> TwoPI
	        HeadingValF.iw1 = Heading;
	    else
	        if (( HeadingChange > COMPASS_MAX_SLEW ) && ( State == InFlight )) { // Sanity check - discard reading
	     		Heading = SlewLimit(HeadingValF.iw1, Heading, COMPASS_MAX_SLEW);    
	            Stats[CompassFailS]++;
	        }
			
	    LPFilter16(&Heading, &HeadingValF, YawFilterA);
	}
	
	#ifdef SIMULATE
		#if ( defined AILERON | defined ELEVON )
			if ( State == InFlight )
				FakeHeading -= FakeDesiredRoll/5 + FakeDesiredYaw/5;
		#else
			if ( State == InFlight )
			{
				if ( Abs(FakeDesiredYaw) > 5 )
					FakeHeading -= FakeDesiredYaw/5;
			}
	
		FakeHeading = Make2Pi((int16)FakeHeading);
		Heading = FakeHeading;
		#endif // AILERON | ELEVON

	#endif // SIMULATE

} // GetHeading

int16 MinimumTurn(int16 A ) {

    static int16 AbsA;

    AbsA = Abs(A);
    if ( AbsA > MILLIPI )
        A = ( AbsA - TWOMILLIPI ) * Sign(A);

    //DirectionSelected = fabs(A) > THIRDPI; // avoid dithering around reciprocal heading
    //DirectionSense = Sign(A);

    return ( A );

} // MinimumTurn

void InitHeading(void)
{

	MagHeading = GetCompass();
	Heading = Make2Pi( MagHeading - CompassOffset );
	HeadingValF.iw1 = Heading; 

	#ifdef SIMULATE
		FakeHeading = Heading = 0;
	#endif // SIMULATE
	DesiredHeading = Heading;

} // InitHeading

#ifdef HMC5843_DO_NOT_USE_FOR_PIC_VERSION

//________________________________________________________________________________________

// HMC5843 3 Axis Magnetometer

#define HMC5843_ID      0x3C        // 0x1E 9DOF

int16 Mag[3];

int16 GetCompass(void) {

    static char b[6];
    static i16u X, Y, Z;
    static uint8 r;
    static int16 mx, my;
    static int16 CRoll, SRoll, CPitch, SPitch;
	static int16 CompassVal;

    I2CStart();
    r = WriteI2CByte(HMC5843_ID);
    r = WriteI2CByte(0x03); // point to data
    I2CStop();

	I2CStart();	
	if( WriteI2CByte(HMC5843_ID+1) != I2C_ACK ) goto SGerror;
	r = ReadI2CString(b, 6);
	I2CStop();

    X.b1 = b[0]; X.b0 = b[1];
    Y.b1 = b[2]; Y.b0 = b[3];
    Z.b1 = b[4]; Z.b0 = b[5];

    Mag[LR] = X.i16;     // Y axis (internal sensor x axis)
    Mag[FB] = -Y.i16;    // X axis (internal sensor y axis)
    Mag[DU] = -Z.i16;    // Z axis

	// UNFORTUNATELY THE ANGLE ESTIMATES ARE NOT VALID FOR THE PIC VERSION
	// AS THEY ARE NOT BOUNDED BY +/- Pi
    CRoll = int16cos(Angle[Roll]);
    SRoll = int16sin(Angle[Roll]);
    CPitch = int16cos(Angle[Pitch]);
    SPitch = int16sin(Angle[Pitch]);

    // Tilt compensated Magnetic field X:
    mx = Mag[0] * CPitch + Mag[1] * SRoll * SPitch + Mag[2] * CRoll * SPitch;

    // Tilt compensated Magnetic field Y:
    my =  Mag[1] * CRoll - Mag[2] * SRoll;

    // Magnetic Heading
    CompassVal = int32atan2( -my, mx );

    F.CompassValid = true;
    return ( CompassVal );

SGerror:
	F.CompassValid = false;
	return ( 0 );

} // GetCompass

void CalibrateCompass(void) {

} // CalibrateCompass

void DoCompassTest(void) {
	int32 Temp;

    TxString("\r\nCompass test (HMC5843)\r\n\r\n");

    GetHeading();

    TxString("Mag:\t");
    TxVal32(Mag[LR], 0, HT);
    TxVal32(Mag[FB], 0, HT);
    TxVal32(Mag[DU], 0, HT);
    TxNextLine();
    TxNextLine();

    TxVal32(ConvertMPiToDDeg(MagHeading), 1, 0);
    TxString(" deg (Compass)\r\n");
    TxVal32(ConvertMPiToDDeg(Heading), 1, 0);
    TxString(" deg (True)\r\n");
} // DoCompassTest

void InitCompass(void) {
    uint8 r;

    I2CStart();
    F.CompassValid = !(WriteI2CByte(HMC5843_ID) != I2C_ACK);
    I2CStop();

	Delay1mS(COMPASS_TIME_MS);

	if ( F.CompassValid ) {

	    I2CStart();
	    r = WriteI2CByte(HMC5843_ID);
	    r = WriteI2CByte(0x02);
	    r = WriteI2CByte(0x00);   // Set continuous mode (default to 10Hz)
	    I2CStop();

		Delay1mS(COMPASS_TIME_MS);
	}
} // InitCompass

#else

//________________________________________________________________________________________

// HMC6352 Compass

#define HMC6352_ID             0x42

int16 GetCompass(void)
{
	static i16u CompassVal;

/// slew limit setting

	I2CStart();
	F.CompassMissRead = WriteI2CByte(HMC6352_ID+1) != I2C_ACK; 
	CompassVal.b1 = ReadI2CByte(I2C_ACK);
	CompassVal.b0 = ReadI2CByte(I2C_NACK);
	I2CStop();

	return ( ConvertDDegToMPi( CompassVal.i16 ) );
} // GetCompass

static uint8 CP[9];

#ifdef TESTING

#define TEST_COMP_OPMODE 0b01110000	// standby mode to reliably read EEPROM

void GetCompassParameters(void)
{
	#ifndef CLOCK_40MHZ

	uint8 r;

	I2CStart();
	if( WriteI2CByte(HMC6352_ID) != I2C_ACK ) goto CTerror;
	if( WriteI2CByte('G')  != I2C_ACK ) goto CTerror;
	if( WriteI2CByte(0x74) != I2C_ACK ) goto CTerror;
	if( WriteI2CByte(TEST_COMP_OPMODE) != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(COMPASS_TIME_MS);

	for (r = 0; r <= (uint8)8; r++)
	{
		CP[r] = 0xff;

		Delay1mS(10);

		I2CStart();
		if ( WriteI2CByte(HMC6352_ID) != I2C_ACK ) goto CTerror;
		if ( WriteI2CByte('r')  != I2C_ACK ) goto CTerror;
		if ( WriteI2CByte(r)  != I2C_ACK ) goto CTerror;
		I2CStop();

		Delay1mS(10);

		I2CStart();
		if( WriteI2CByte(HMC6352_ID+1) != I2C_ACK ) goto CTerror;
		CP[r] = ReadI2CByte(I2C_NACK);
		I2CStop();
	}

	Delay1mS(7);

	return;

CTerror:
	I2CStop();
	TxString("FAIL\r\n");

	#endif // CLOCK_40MHZ
} // GetCompassParameters

void DoCompassTest(void)
{
	uint16 v, prev;
	int16 Temp;
	uint8 i;

	TxString("\r\nCompass test (HMC6352)\r\n");

	I2CStart();
	if( WriteI2CByte(HMC6352_ID) != I2C_ACK ) goto CTerror;
	if( WriteI2CByte('G')  != I2C_ACK ) goto CTerror;
	if( WriteI2CByte(0x74) != I2C_ACK ) goto CTerror;
	if( WriteI2CByte(TEST_COMP_OPMODE) != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(1);

	I2CStart(); // Do Set/Reset now		
	if( WriteI2CByte(HMC6352_ID) != I2C_ACK ) goto CTerror;
	if( WriteI2CByte('O')  != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(7);

	#ifndef CLOCK_40MHZ

	GetCompassParameters();

	TxString("Registers\r\n");
	TxString("0:\tI2C"); 
	TxString("\t 0x"); TxValH(CP[0]); 
	if ( CP[0] != (uint8)0x42 ) 
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

	#endif // CLOCK_40MHZ

	InitCompass();
	if ( !F.CompassValid ) goto CTerror;

	Delay1mS(COMPASS_TIME_MS);

	for ( i = 0; i < 200; i++) 
    	GetHeading();

	if ( F.CompassMissRead ) goto CTerror;

    TxVal32(ConvertMPiToDDeg(MagHeading), 1, 0);
    TxString(" deg (Compass)\r\n");
    TxVal32(ConvertMPiToDDeg(Heading), 1, 0);
    TxString(" deg (True)\r\n");
	
	return;
CTerror:
	I2CStop();
	TxString("FAIL\r\n");
} // DoCompassTest

void CalibrateCompass(void)
{	// calibrate the compass by rotating the ufo through 720 deg smoothly
	TxString("\r\nCalib. compass - Press the CONTINUE button (x) to continue\r\n");	
	while( PollRxChar() != 'x' ); // UAVPSet uses 'x' for CONTINUE button

	I2CStart(); // Do Set/Reset now		
	if( WriteI2CByte(HMC6352_ID) != I2C_ACK ) goto CCerror;
	if( WriteI2CByte('O')  != I2C_ACK ) goto CCerror;
	I2CStop();

	Delay1mS(7);

	// set Compass device to Calibration mode 
	I2CStart();
	if( WriteI2CByte(HMC6352_ID) != I2C_ACK ) goto CCerror;
	if( WriteI2CByte('C')  != I2C_ACK ) goto CCerror;
	I2CStop();

	TxString("\r\nRotate horizontally 720 deg in ~30 sec. - Press the CONTINUE button (x) to FINISH\r\n");
	while( PollRxChar() != 'x' );

	// set Compass device to End-Calibration mode 
	I2CStart();
	if( WriteI2CByte(HMC6352_ID) != I2C_ACK ) goto CCerror;
	if( WriteI2CByte('E')  != I2C_ACK ) goto CCerror;
	I2CStop();

	TxString("\r\nCalibration complete\r\n");

	Delay1mS(COMPASS_TIME_MS);

	InitCompass();

	return;

CCerror:
	TxString("Calibration FAILED\r\n");
} // CalibrateCompass

#endif // TESTING

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
	if( WriteI2CByte(HMC6352_ID) != I2C_ACK ) goto CTerror;
	if( WriteI2CByte('G')  != I2C_ACK ) goto CTerror;
	if( WriteI2CByte(0x74) != I2C_ACK ) goto CTerror;
	if( WriteI2CByte(COMP_OPMODE) != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(1);

	I2CStart(); // save operation mode in EEPROM
	if( WriteI2CByte(HMC6352_ID) != I2C_ACK ) goto CTerror;
	if( WriteI2CByte('L')  != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(1);

	I2CStart(); // Do Bridge Offset Set/Reset now
	if( WriteI2CByte(HMC6352_ID) != I2C_ACK ) goto CTerror;
	if( WriteI2CByte('O')  != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(COMPASS_TIME_MS);

	// use default heading mode (1/10th degrees)

	F.CompassValid = true;
	return;
CTerror:
	F.CompassValid = false;
	Stats[CompassFailS]++;
	F.CompassFailure = true;
	
	I2CStop();
} // InitCompass


#endif // HMC6352

