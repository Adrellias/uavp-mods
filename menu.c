// ===============================================================================================
// =                              UAVXArm Quadrocopter Controller                                =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                           http://code.google.com/p/uavp-mods/                               =
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

void ShowPrompt(void);
void ShowRxSetup(void);
void ShowSetup(boolean);
uint8 MakeUpper(uint8);
void ProcessCommand(void);

const uint8 SerHello[] = "UAVXArm "
                         " Copyright 2008 G.K. Egan & 2007 W. Mahringer\r\n"
                         "This is FREE SOFTWARE and comes with ABSOLUTELY NO WARRANTY "
                         "see http://www.gnu.org/licenses/!\r\n";
const uint8 SerHelp[] = "\r\nCommands:\r\n"
                        "A..Attitude test\r\n"
                        "C..Compass test\r\n"
                        "D..Load default parameter set\r\n"
                        "G..GPS test\r\n"
                        "H..Barometer/Rangefinder test\r\n"
                        "I..I2C bus scan\r\n"
                        "K..Calibrate compass scan\r\n"
//    "M..Modify parameters\r\n"
                        "P..Rx test\r\n"
                        "S..Setup\r\n"
                        "T..All LEDs and buzzer test\r\n"
                        "V..Battery test\r\n"
                        "X..Flight stats\r\n"
                        "Y..Program YGE I2C ESC\r\n"
                        "1-8..Individual LED/buzzer test\r\n"; // last line must be in this form for UAVPSet

const uint8 RxChMnem[] = "TAERG12";

uint8 MakeUpper(uint8 ch) {
    if ( ( ch >='a') && ( ch <='z' ) )
        ch = (ch - 'a') + 'A';

    return (ch);
} // MakeUpper

void ShowPrompt(void) {
    TxString("\r\n>");
} // ShowPrompt

void ShowRxSetup(void) {
    if ( F.UsingSerialPPM )
        if ( PPMPosPolarity[P[TxRxType]] )
            TxString("Serial PPM frame (Positive Polarity)");
        else
            TxString("Serial PPM frame (Negative Polarity)");
    else
        TxString("Odd Rx Channels PPM");
} // ShowRxSetup

void ShowSetup(boolean h) {
    int8 i;

    TxNextLine();
    if ( h )
        ParamSet = 1;

    TxString(SerHello);

    UpdateRTC();
    i = 0;
    while ( RTCString[i] != NULL )
        TxChar(RTCString[i++]);
    TxNextLine();

    TxString("Clock: 92MHz LPC1768 (mbed)\r\n");

    TxString("Aircraft: ");
    switch ( UAVXAirframe ) {
        case QuadAF:
            TxString("QUADROCOPTER\r\n");
            break;
        case TriAF:
            TxString("TRICOPTER\r\n");
            break;
        case VTAF:
            TxString("VTCOPTER\r\n");
            break;
        case HeliAF:
            TxString("HELICOPTER\r\n");
            break;
        case ElevAF:
            TxString("FLYING WING\r\n");
            break;
        case AilAF:
            TxString("AILERON\r\n");
            break;
        default:
            TxString("UNKNOWN\r\n");
    }

    if ( F.CompassValid ) {
        TxString("Compass: ");
        ShowCompassType();
        TxString(" ( offset  ");
        TxVal32((int16)P[CompassOffsetQtr] * 90,0,0);
        TxString("deg. )\r\n");
    }

    TxString("Baro: ");
    ShowBaroType();

    TxString("Accelerometers: ");
    ShowAccType();
    TxNextLine();

    TxString("Gyros: ");
    ShowGyroType();
    TxNextLine();

    TxString("Motor ESCs: ");
    switch ( P[ESCType] ) {
        case ESCPPM:
            TxString("PPM ");
            break;
        case ESCHolger:
            TxString("Holger I2C {");
            break;
        case ESCX3D:
            TxString("X-3D I2C {");
            break;
        case ESCYGEI2C:
            TxString("YGE I2C {");
            break;
    }

    if ( P[ESCType] != ESCPPM ) {
        for ( i = 0; i < NoOfI2CESCOutputs; i++ )
            if ( ESCI2CFail[i] )
                TxString(" Fail");
            else
                TxString(" OK");
        TxString(" }");
    }
    TxNextLine();
    
    if ( F.HaveBatterySensor )
         TxString("External Volt/Amp Sensor Fitted\r\n");
#ifdef RX6CH
    TxString("6 CHANNEL VERSION - 5 ACTIVE CHANNELS ONLY\r\n");
#endif // RX6CH

    TxString("Tx/Rx: ");

    switch ( P[TxRxType] ) {
        case FutabaCh3:
            TxString("Futaba Th 3 {");
            break;
        case FutabaCh2:
            TxString("Futaba Th 2 {");
            break;
        case FutabaDM8:
            TxString("Futaba DM8 & AR7000 {");
            break;
        case JRPPM:
            TxString("JR PPM {");
            break;
        case JRDM9:
            TxString("JR DM9 & AR7000{");
            break;
        case JRDXS12:
            TxString("JR DSX12 & AR7000 {");
            break;
        case DX7AR7000:
            TxString("Spektrum DX7 & AR7000 {");
            break;
        case DX7AR6200:
            TxString("Spektrum DX7 & AR6200 {");
            break;
        case CustomTxRx:
            TxString("Custom {");
            break;
        case FutabaCh3_6_7:
            TxString("Futaba Th 2 Swap 6&7 {");
            break;
        case DX7AR6000:
            TxString("Spektrum DX7 & AR6000 {");
            break;
        case DX6iAR6200:
            TxString("Spektrum DX6i & AR6200 {");
            break;
        case FutabaCh3_R617FS:
            TxString("Futaba Th 3 & R617FS {");
            break;
        case GraupnerMX16s:
            TxString("Graupner MX16s {");
            break;
        case DX7aAR7000:
            TxString("Spektrum DX7a & AR7000 {");
            break;
        case FrSkyDJT_D8R:
            TxString("FrSky DJT & D8R-SP Composite {");
            break;
        case ExternalDecoder:
            TxString("External Decoder {");
            break;
        case UnknownTxRx:
            TxString("UNKNOWN {");
            break;
        default:
            ;
    }  // switch

    if ( F.UsingSerialPPM )
        ShowRxSetup();
    else
        if ( P[TxRxType] != UnknownTxRx ) {
            for ( i = 0; i < RC_CONTROLS; i++)
                TxChar(RxChMnem[RMap[i]]);

            TxString("} connect {");

            for ( i = 0; i < RC_CONTROLS; i+=2) {
                TxChar(RxChMnem[RMap[i]]);
                TxChar(' ');
            }
        }
    TxChar('}');
    if (( P[TxRxType] == DX7AR6200 ) || ( P[TxRxType] == DX6iAR6200))
        TxString(" Mix Rudder to Aux1/Flaps ");
    if ( F.UsingTxMode2 )
        TxString(" Tx Mode 2");
    else
        TxString(" Tx Mode 1");
    TxNextLine();

    TxString("Selected parameter set: "); // must be exactly this string as UAVPSet expects it
    TxChar('0' + ParamSet);
    TxNextLine();

#ifdef MULTICOPTER
    TxString("Forward Flight: ");
    TxVal32((int16)Orientation * 75L, 1, 0);
    TxString("deg CW from K1 motor(s)\r\n");
#endif // MULTICOPTER

    if ( F.UsingAngleControl )
        TxString("\tSticks control roll/pitch angle directly\r\n");
    else
        TxString("\tSticks control rate of change of roll/pitch angle\r\n");

    if ( F.UsingRTHAutoDescend )
        TxString("\tAuto descend ENABLED\r\n");
    else
        TxString("\tAuto descend disabled\r\n");

    if ( F.AllowTurnToWP )
        TxString("\tTurn toward Way Point\r\n");
    else
        TxString("\tHold heading\r\n");

    if ( F.AllowNavAltitudeHold )
        TxString("\tAllow Nav altitude hold\r\n");
    else
        TxString("\tWARNING - Manual Nav altitude hold\r\n");

    if ( !F.SDCardValid )
        TxString("\tSD Card NOT loaded - DISCONNECT power and load card if required.\r\n");

    TxString("\r\nALARM (if any):\r\n");
    if ( P[TxRxType] == UnknownTxRx )
        TxString("\tTx/Rx TYPE not set\r\n");
#ifdef TESTING
    TxString("\tTEST VERSION - No Motors\r\n");
#endif // TESTING

    if ( !F.ParametersValid )
        TxString("\tINVALID flight parameters (PID)!\r\n");

    if ( !F.BaroAltitudeValid )
        TxString("\tBarometer OFFLINE\r\n");
    if ( BaroRetries >= BARO_INIT_RETRIES )
        TxString("\tBaro Init: FAILED\r\n");

    if ( !F.RangefinderAltitudeValid )
        TxString("\tRangefinder OFFLINE\r\n");

    if ( F.GyroFailure )
        TxString("\tGyro FAILURE\r\n");

    if ( !F.AccelerationsValid )
        TxString("\tAccelerometers OFFLINE\r\n");

    if ( !F.CompassValid )
        TxString("\tCompass OFFLINE\r\n");

    if ( !F.Signal )
        TxString("\tBad EPAs or Tx switched off?\r\n");
    if ( Armed && FirstPass )
        TxString("\tUAVX is armed - DISARM!\r\n");

    if ( F.Navigate || F.ReturnHome )
        TxString("\tNavigate/RTH is selected - DESELECT!\r\n");

    if ( InitialThrottle >= RC_THRES_START )
        TxString("\tThrottle may be open - CLOSE!\r\n");
        
    if ( !F.UsingLEDDriver )
        TxString("\tPCA Line Driver OFFLINE - no BUZZER warnings!\r\n");
        
    ShowPrompt();
} // ShowSetup

void ProcessCommand(void) {
    static int8  p;
    static uint8 ch;
    static int8 d;

    if ( 1 ) {//!Armed.read() ) {
        ch = PollRxChar();
        if ( ch != NUL   ) {
            ch = MakeUpper(ch);

            switch ( ch ) {
                case 'D':
                    UseDefaultParameters();
                    InitParameters();
                    ShowPrompt();
                    break;
                case 'L'  :    // List parameters
                    TxString("\r\nParameter list for set #");    // do not change (UAVPset!)
                    TxChar('0' + ParamSet);
                    ReadParameters();
                    for ( p = 0 ; p < MAX_PARAMETERS; p++ ) {
                        TxString("\r\nRegister ");
                        TxValU((uint8)(p+1));
                        TxString(" = ");
                        TxValS(P[p]);
                    }
                    ShowPrompt();
                    break;
                case 'M'  : // modify parameters
                    // no reprogramming in flight!!!!!!!!!!!!!!!
                    LEDBlue_ON;
                    TxString("\r\nRegister ");
                    p = (uint16)(RxNumU()-1);
                    // Attempts to block use of old versions of UAVPSet not compatible with UAVX
                    // assumes parameters are written sequentially from 0..(MAX_PARAMETERS-1)

                    TxString(" = ");
                    d = RxNumS();
                    if ( p < MAX_PARAMETERS ) {
                        // Keep RAM based set up to date.
                        if ( ParamSet == (uint8)1 ) {
                            WritePX(p, d);
                            if ( DefaultParams[p][1] )
                                WritePX(MAX_PARAMETERS + p, d);
                        } else {
                            if ( !DefaultParams[p][1] )
                                WritePX(MAX_PARAMETERS + p, d);
                        }
                        ParametersChanged = true;
                    }
                    if ( p < (MAX_PARAMETERS-1) )
                        F.ParametersValid = false;
                    else
                        if ( p == (MAX_PARAMETERS-1) ) {
                            WritePXImagefile();
                            F.ParametersValid = true;     // ALL parameters must be written
                        }
                    LEDBlue_OFF;
                    ShowPrompt();
                    break;
                case 'N' :    // neutral values
                    GetNeutralAccelerations();
                    TxString("\r\nNeutral    R:");
                    TxValS(NewAccNeutral[LR]);

                    TxString("    P:");
                    TxValS(NewAccNeutral[BF]);
                    
                    TxString("   V:");
                    TxValS(NewAccNeutral[UD]);
                    ShowPrompt();
                    break;
                case 'Z' : // set Paramset
                    p = RxNumU();
                    if ( p != (int8)ParamSet ) {
                        ParamSet = p;
                        ParametersChanged = true;
                        ReadParameters();
                    }
                    break;
                case 'W' :    // comms with UAVXNav utility NOT UAVPSet
                    UAVXNavCommand();
                    //ShowPrompt();
                    break;
                case 'R':    // receiver values
                    TxString("\r\nT:");
                    TxValU(ToPercent(RC[ThrottleRC], RC_MAXIMUM));
                    TxString(",R:");
                    TxValS(ToPercent(((RC[RollRC]- RC_NEUTRAL) * 2L), RC_MAXIMUM));
                    TxString(",P:");
                    TxValS(ToPercent(((RC[PitchRC]- RC_NEUTRAL) * 2L), RC_MAXIMUM));
                    TxString(",Y:");
                    TxValS(ToPercent(((RC[YawRC]- RC_NEUTRAL) * 2L), RC_MAXIMUM));
                    TxString(",5:");
                    TxValU(ToPercent(RC[RTHRC], RC_MAXIMUM));
                    TxString(",6:");
                    TxValS(ToPercent(((RC[CamPitchRC] - RC_NEUTRAL) * 2L), RC_MAXIMUM));
                    TxString(",7:");
                    TxValU(ToPercent(RC[NavGainRC], RC_MAXIMUM));
                    ShowPrompt();
                    break;
                case 'S' :    // show status
                    ShowSetup(false);
                    break;
                case 'X' :    // flight stats
                    ShowStats();
                    ShowPrompt();
                    break;
                case 'A' :    // linear sensor
                    AttitudeTest();
                    ShowPrompt();
                    break;
                case 'G':    // GPS
                    GPSTest();
                    ShowPrompt();
                    break;
                case 'H':    // barometer
                    BaroTest();
                    ShowPrompt();
                    break;
                case 'I':
                    TxString("\r\nI2C devices ...\r\n");
                    TxVal32(ScanI2CBus(),0,0);
                    TxString(" device(s) found\r\n");
                    ShowPrompt();
                    break;
                case 'P'  :    // Receiver test
                    ReceiverTest();
                    ShowPrompt();
                    break;
                case 'C'  :    // Compass test
                    DoCompassTest();
                    ShowPrompt();
                    break;

                case 'K'  :    //Calibrate compass
                    CalibrateCompass();
                    ShowPrompt();
                    break;

                case 'Y':    // configure YGE30i EScs
                    ConfigureESCs();
                    ShowPrompt();
                    break;
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                    TxString("\r\nOutput test\r\n");
                    TxChar(ch);
                    TxChar(':');
                    switch ( ch ) {
                        case '1':
                            TxString("Aux2");
                            break;
                        case '2':
                            TxString("Blue");
                            break;
                        case '3':
                            TxString("Red");
                            break;
                        case '4':
                            TxString("Green");
                            break;
                        case '5':
                            TxString("Aux1");
                            break;
                        case '6':
                            TxString("Yellow");
                            break;
                        case '7':
                            TxString("Aux3");
                            break;
                        case '8':
                            TxString("Beeper");
                            break;
                    }
                    TxNextLine();
                    PowerOutput(ch-'1');
                    ShowPrompt();
                    break;
                case 'T':
                    LEDsAndBuzzer();
                    ShowPrompt();
                    break;

                case 'V' :    // Battery test
                    BatteryTest();
                    ShowPrompt();
                    break;
                case '?'  :  // help
                    TxString(SerHelp);
                    ShowPrompt();
                    break;
                default:
                    break;
            }
        }
    }
} // ProcessCommand

