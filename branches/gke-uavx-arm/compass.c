// ===============================================================================================
// =                              UAVXArm Quadrocopter Controller                                =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                     http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// ===============================================================================================

//    This is part of UAVXArm.

//    UAVXArm is free software: you can redistribute it and/or modify it under the terms of the GNU
//    General Public License as published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.

//    UAVXArm is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.
//    If not, see http://www.gnu.org/licenses/

#include "UAVXArm.h"

// Local magnetic declination not included
// http://www.ngdc.noaa.gov/geomagmodels/Declination.jsp


void ReadCompass(void);
void GetHeading(void);
void CalibrateCompass(void);
void ShowCompassType(void);
void DoCompassTest(void);
void InitCompass(void);

MagStruct Mag[3] = {{ 0,0 },{ 0,0 },{ 0,0 }};
real32 MagDeviation, CompassOffset;
real32 MagHeading, Heading, Headingp, FakeHeading;
real32 HeadingSin, HeadingCos;
uint8 CompassType;

void ReadCompass(void) {
    switch ( CompassType ) {
        case HMC5843:
            ReadHMC5843();
            break;
        case HMC6352:
            ReadHMC6352();
            break;
        default:
            Heading = 0;
            break;
    } // switch

} // ReadCompass

void CalibrateCompass(void) {
    switch ( CompassType ) {
        case HMC5843:
            CalibrateHMC5843();
            break;
        case HMC6352:
            CalibrateHMC6352();
            break;
        default:
            break;
    } // switch
} // CalibrateCompass

void ShowCompassType(void) {
    switch ( CompassType ) {
        case HMC5843:
            TxString("HMC5843");
            break;
        case HMC6352:
            TxString("HMC6352");
            break;
        default:
            break;
    }
} // ShowCompassType

void DoCompassTest(void) {
    switch ( CompassType ) {
        case HMC5843:
            DoHMC5843Test();
            break;
        case HMC6352:
            DoHMC6352Test();
            break;
        default:
            TxString("\r\nCompass test\r\nCompass not detected?\r\n");
            break;
    } // switch
} // DoCompassTest

void GetHeading(void) {

    const real32 CompassA = COMPASS_UPDATE_S / ( 1.0 / ( TWOPI * COMPASS_FREQ ) + COMPASS_UPDATE_S );

    ReadCompass();

    Heading = Make2Pi( MagHeading - MagDeviation - CompassOffset );
    if ( fabs(Heading - Headingp ) > PI )
        Headingp = Heading;

    Heading = LPFilter(Heading, Headingp, CompassA, COMPASS_UPDATE_S);
    Headingp = Heading;

#ifdef SIMULATE
#if ( defined AILERON | defined ELEVON )
    if ( State == InFlight )
        FakeHeading -= FakeDesiredRoll/5 + FakeDesiredYaw/5;
#else
    if ( State == InFlight ) {
        if ( Abs(FakeDesiredYaw) > 5 )
            FakeHeading -= FakeDesiredYaw/5;
    }

    FakeHeading = Make2Pi((int16)FakeHeading);
    Heading = FakeHeading;
#endif // AILERON | ELEVON
#endif // SIMULATE 
} // GetHeading

void InitCompass(void) {
    if ( IsHMC5843Active() )
        CompassType = HMC5843;
    else
        if ( HMC6352Active() )
            CompassType = HMC6352;
        else {
            CompassType = NoCompass;
            F.CompassValid = false;
        }

    switch ( CompassType ) {
        case HMC5843:
            InitHMC5843();
            break;
        case HMC6352:
            InitHMC6352();
            break;
        default:
            MagHeading = 0;
    } // switch

    ReadCompass();
    mS[CompassUpdate] = mSClock();
    Heading = Headingp = Make2Pi( MagHeading - MagDeviation - CompassOffset );

} // InitCompass

//________________________________________________________________________________________

// HMC5843 3 Axis Magnetometer

void ReadHMC5843(void);
void GetHMC5843Parameters(void);
void DoHMC5843Test(void);
void CalibrateHMC5843(void);
void InitHMC5843(void);
boolean HMC5843Active(void);

void ReadHMC5843(void) {
    static char b[6];
    static i16u X, Y, Z;
    static uint8 r;
    static real32 mx, my;
    static real32 CRoll, SRoll, CPitch, SPitch;

    I2CCOMPASS.start();
    r = I2CCOMPASS.write(HMC5843_WR);
    r = I2CCOMPASS.write(0x03); // point to data
    I2CCOMPASS.stop();

    I2CCOMPASS.blockread(HMC5843_RD, b, 6);

    X.b1 = b[0];
    X.b0 = b[1];
    Y.b1 = b[2];
    Y.b0  =b[3];
    Z.b1 = b[4];
    Z.b0 = b[5];

    if ( F.Using9DOF ) { // SparkFun/QuadroUFO 9DOF Breakout pins  front edge components up
        Mag[BF].V = X.i16;
        Mag[LR].V = -Y.i16;
        Mag[UD].V = -Z.i16;
    } else { // SparkFun Magnetometer Breakout pins  right edge components up
        Mag[BF].V = -X.i16;
        Mag[LR].V = Y.i16;
        Mag[UD].V = -Z.i16;
    }
    DebugPin = true;
    CRoll = cos(Angle[Roll]);
    SRoll = sin(Angle[Roll]);
    CPitch = cos(Angle[Pitch]);
    SPitch = sin(Angle[Pitch]);

    mx = (Mag[BF].V-Mag[BF].Offset) * CPitch + (Mag[LR].V-Mag[LR].Offset) * SRoll * SPitch + (Mag[UD].V-Mag[UD].Offset) * CRoll * SPitch;
    my =  (Mag[LR].V-Mag[LR].Offset) * CRoll - (Mag[UD].V-Mag[UD].Offset) * SRoll;

    // Magnetic Heading
    MagHeading = MakePi(atan2( -my, mx ));
    DebugPin = false;
    F.CompassValid = true;
    return;

} // ReadHMC5843

void CalibrateHMC5843(void) {

} // DoHMC5843Test

void DoHMC5843Test(void) {
    TxString("\r\nCompass test (HMC5843)\r\n\r\n");

    ReadHMC5843();

    TxString("Mag:\t");
    TxVal32(Mag[LR].V, 0, HT);
    TxVal32(Mag[BF].V, 0, HT);
    TxVal32(Mag[UD].V, 0, HT);
    TxNextLine();
    TxNextLine();

    TxVal32(MagHeading * RADDEG * 10.0, 1, 0);
    TxString(" deg (Magnetic)\r\n");

    Heading = Headingp = Make2Pi( MagHeading - MagDeviation - CompassOffset );
    TxVal32(Heading * RADDEG * 10.0, 1, 0);
    TxString(" deg (True)\r\n");
} // DoHMC5843Test

void InitHMC5843(void) {
    static uint8 r;

    I2CCOMPASS.start();
    r = I2CCOMPASS.write(HMC5843_WR);
    r = I2CCOMPASS.write(0x02);
    r = I2CCOMPASS.write(0x00);   // Set continuous mode (default to 10Hz)
    I2CCOMPASS.stop();

    Delay1mS(50);

} // InitHMC5843Magnetometer

boolean IsHMC5843Active(void) {

    F.CompassValid = I2CCOMPASSAddressResponds( HMC5843_ID );

    if ( F.CompassValid )
        TrackMinI2CRate(400000);

    return ( F.CompassValid );

} // IsHMC5843Active

//________________________________________________________________________________________

// HMC6352 Compass

void ReadHMC6352(void);
uint8 WriteByteHMC6352(uint8);
void GetHMC6352Parameters(void);
void DoHMC6352Test(void);
void CalibrateHMC6352(void);
void InitHMC6352(void);
boolean IsHMC6352Active(void);

void ReadHMC6352(void) {
    static i16u v;

    I2CCOMPASS.start();
    F.CompassMissRead = I2CCOMPASS.write(HMC6352_RD) != I2C_ACK;
    v.b1 = I2CCOMPASS.read(I2C_ACK);
    v.b0 = I2CCOMPASS.read(I2C_NACK);
    I2CCOMPASS.stop();

    MagHeading = Make2Pi( ((real32)v.i16 * PI) / 1800.0 - CompassOffset ); // Radians
} // ReadHMC6352

uint8 WriteByteHMC6352(uint8 d) {
    I2CCOMPASS.start();
    if ( I2CCOMPASS.write(HMC6352_WR) != I2C_ACK ) goto WError;
    if ( I2CCOMPASS.write(d) != I2C_ACK ) goto WError;
    I2CCOMPASS.stop();

    return( I2C_ACK );
WError:
    I2CCOMPASS.stop();
    return ( I2C_NACK );
} // WriteByteHMC6352

char CP[9];

#define TEST_COMP_OPMODE 0x70    // standby mode to reliably read EEPROM

void GetHMC6352Parameters(void) {
    uint8 r;

    I2CCOMPASS.start();
    if ( I2CCOMPASS.write(HMC6352_WR) != I2C_ACK ) goto CTerror;
    if ( I2CCOMPASS.write('G')  != I2C_ACK ) goto CTerror;
    if ( I2CCOMPASS.write(0x74) != I2C_ACK ) goto CTerror;
    if ( I2CCOMPASS.write(TEST_COMP_OPMODE) != I2C_ACK ) goto CTerror;
    I2CCOMPASS.stop();

    Delay1mS(20);

    for (r = 0; r <= (uint8)8; r++) { // must have this timing - not block read!

        I2CCOMPASS.start();
        if ( I2CCOMPASS.write(HMC6352_WR) != I2C_ACK ) goto CTerror;
        if ( I2CCOMPASS.write('r')  != I2C_ACK ) goto CTerror;
        if ( I2CCOMPASS.write(r)  != I2C_ACK ) goto CTerror;
        I2CCOMPASS.stop();

        Delay1mS(10);

        I2CCOMPASS.start();
        if ( I2CCOMPASS.write(HMC6352_RD) != I2C_ACK ) goto CTerror;
        CP[r] = I2CCOMPASS.read(I2C_NACK);
        I2CCOMPASS.stop();

        Delay1mS(10);
    }

    return;

CTerror:
    I2CCOMPASS.stop();
    TxString("FAIL\r\n");

} // GetHMC6352Parameters

void DoHMC6352Test(void) {
    static real32 Temp;

    TxString("\r\nCompass test (HMC6352)\r\n");

    I2CCOMPASS.start();
    if ( I2CCOMPASS.write(HMC6352_WR) != I2C_ACK ) goto CTerror;
    if ( I2CCOMPASS.write('G')  != I2C_ACK ) goto CTerror;
    if ( I2CCOMPASS.write(0x74) != I2C_ACK ) goto CTerror;
    if ( I2CCOMPASS.write(TEST_COMP_OPMODE) != I2C_ACK ) goto CTerror;
    I2CCOMPASS.stop();

    Delay1mS(1);

    //  I2CCOMPASS.start(); // Do Set/Reset now
    if ( WriteByteHMC6352('O')  != I2C_ACK ) goto CTerror;

    Delay1mS(7);

    GetHMC6352Parameters();

    TxString("\r\nRegisters\r\n");
    TxString("\t0:\tI2C");
    TxString("\t 0x");
    TxValH(CP[0]);
    if ( CP[0] != (uint8)0x42 )
        TxString("\t Error expected 0x42 for HMC6352");
    TxNextLine();

    Temp = (CP[1]*256)|CP[2];
    TxString("\t1:2:\tXOffset\t");
    TxVal32((int32)Temp, 0, 0);
    TxNextLine();

    Temp = (CP[3]*256)|CP[4];
    TxString("\t3:4:\tYOffset\t");
    TxVal32((int32)Temp, 0, 0);
    TxNextLine();

    TxString("\t5:\tDelay\t");
    TxVal32((int32)CP[5], 0, 0);
    TxNextLine();

    TxString("\t6:\tNSum\t");
    TxVal32((int32)CP[6], 0, 0);
    TxNextLine();

    TxString("\t7:\tSW Ver\t");
    TxString(" 0x");
    TxValH(CP[7]);
    TxNextLine();

    TxString("\t8:\tOpMode:");
    switch ( ( CP[8] >> 5 ) & 0x03 ) {
        case 0:
            TxString("  1Hz");
            break;
        case 1:
            TxString("  5Hz");
            break;
        case 2:
            TxString("  10Hz");
            break;
        case 3:
            TxString("  20Hz");
            break;
    }

    if ( CP[8] & 0x10 ) TxString(" S/R");

    switch ( CP[8] & 0x03 ) {
        case 0:
            TxString(" Standby");
            break;
        case 1:
            TxString(" Query");
            break;
        case 2:
            TxString(" Continuous");
            break;
        case 3:
            TxString(" Not-allowed");
            break;
    }
    TxNextLine();

    InitCompass();
    if ( !F.CompassValid ) goto CTerror;

    Delay1mS(50);

    ReadHMC6352();
    if ( F.CompassMissRead ) goto CTerror;

    TxNextLine();
    TxVal32(MagHeading * RADDEG * 10.0, 1, 0);
    TxString(" deg (Magnetic)\r\n");
    Heading = Headingp = Make2Pi( MagHeading - MagDeviation - CompassOffset );
    TxVal32(Heading * RADDEG * 10.0, 1, 0);
    TxString(" deg (True)\r\n");

    return;
CTerror:
    I2CCOMPASS.stop();
    TxString("FAIL\r\n");
} // DoHMC6352Test

void CalibrateHMC6352(void) {   // calibrate the compass by rotating the ufo through 720 deg smoothly
    TxString("\r\nCalib. compass - Press CONTINUE button (x) to Start\r\n");
    while ( PollRxChar() != 'x' ); // UAVPSet uses 'x' for CONTINUE button

    // Do Set/Reset now
    if ( WriteByteHMC6352('O') != I2C_ACK ) goto CCerror;

    Delay1mS(7);

    // set Compass device to Calibration mode
    if ( WriteByteHMC6352('C') != I2C_ACK ) goto CCerror;

    TxString("\r\nRotate horizontally 720 deg in ~30 sec. - Press CONTINUE button (x) to Finish\r\n");
    while ( PollRxChar() != 'x' );

    // set Compass device to End-Calibration mode
    if ( WriteByteHMC6352('E') != I2C_ACK ) goto CCerror;

    TxString("\r\nCalibration complete\r\n");

    Delay1mS(50);

    InitCompass();

    return;

CCerror:
    TxString("Calibration FAILED\r\n");
} // CalibrateHMC6352

void InitHMC6352(void) {

    // 20Hz continuous read with periodic reset.
#ifdef SUPPRESS_COMPASS_SR
#define COMP_OPMODE 0x62
#else
#define COMP_OPMODE 0x72
#endif // SUPPRESS_COMPASS_SR

    // Set device to Compass mode
    I2CCOMPASS.start();
    if ( I2CCOMPASS.write(HMC6352_WR) != I2C_ACK ) goto CTerror;
    if ( I2CCOMPASS.write('G')  != I2C_ACK ) goto CTerror;
    if ( I2CCOMPASS.write(0x74) != I2C_ACK ) goto CTerror;
    if ( I2CCOMPASS.write(COMP_OPMODE) != I2C_ACK ) goto CTerror;
    I2CCOMPASS.stop();

    Delay1mS(1);

    // save operation mode in Flash
    if ( WriteByteHMC6352('L') != I2C_ACK ) goto CTerror;

    Delay1mS(1);

    // Do Bridge Offset Set/Reset now
    if ( WriteByteHMC6352('O') != I2C_ACK ) goto CTerror;

    Delay1mS(50);

    F.CompassValid = true;

    return;
CTerror:
    F.CompassValid = false;
    Stats[CompassFailS]++;
    F.CompassFailure = true;

    I2CCOMPASS.stop();
} // InitHMC6352

boolean HMC6352Active(void) {

    F.CompassValid = I2CCOMPASSAddressResponds( HMC6352_ID );

    if ( F.CompassValid )
        TrackMinI2CRate(100000);

    return ( F.CompassValid );

} // HMC6352Active
