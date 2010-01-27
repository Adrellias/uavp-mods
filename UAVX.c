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

//#ifdef CLOCK_40MHZ
//#pragma	config OSC=HSPLL, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC
//#else
#pragma	config OSC=HS, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC  
//#endif

#include "uavx.h"

// Global Variables

#pragma udata clocks
uint24	mS[CompassUpdate+1];
#pragma udata

// Interrupt related 
#pragma udata access isrvars
uint8 	SHADOWB, MF, MB, ML, MR, MT, ME; // motor/servo outputs
uint8	State, FailState;
i16u 	PPM[MAX_CONTROLS];
int8 	PPM_Index;
int24 	PrevEdge, CurrEdge;
i16u 	Width;
int16 	PauseTime;
uint8 	GPSRxState;
uint8 	ll, tt, gps_ch;
uint8 	RxCheckSum, GPSCheckSumChar, GPSTxCheckSum;
uint8	ESCMin, ESCMax;
#pragma udata

int16 	RC[CONTROLS];
int8	SignalCount;
uint16	RCGlitches;
boolean	FirstPass; 

const rom uint8 RxChMnem[] = "TAERG12";
int8 	RMap[CONTROLS];

#pragma udata gpsbuff
struct {
	uint8 s[GPSRXBUFFLENGTH];
	uint8 length;
	} NMEA;
#pragma udata

const rom uint8 NMEATag[6] = {"GPGGA"};

uint8	ParamSet;
boolean ParametersChanged;

// Control
int16	RE, PE, YE, HE;					// gyro rate error	
int16	REp, PEp, YEp, HEp;				// previous error for derivative
int16	RollSum, PitchSum, YawSum;		// integral 	
int16	RollRate, PitchRate, YawRate;
int16	RollTrim, PitchTrim, YawTrim;
int16	HoldYaw;
int16	RollIntLimit256, PitchIntLimit256, YawIntLimit256;
int16	GyroMidRoll, GyroMidPitch, GyroMidYaw;
int16	HoverThrottle, DesiredThrottle, IdleThrottle, InitialThrottle;
int16	DesiredRoll, DesiredPitch, DesiredYaw, DesiredHeading, DesiredCamPitchTrim, Heading;
int16	DesiredRollP, DesiredPitchP;
int16	CurrMaxRollPitch;

i16u 	Compass;

#pragma udata accs
i16u	Ax, Ay, Az;
int8	LRIntCorr, FBIntCorr;
int16	Rl,Pl,Yl;						// PID output values
int8	NeutralLR, NeutralFB, NeutralDU;
int16	DUVel, LRVel, FBVel, DUAcc, LRAcc, FBAcc, DUComp, LRComp, FBComp;
#pragma udata

int16 	NavClosingRadius, NavNeutralRadius, NavCloseToNeutralRadius, CompassOffset;

uint8 	NavState;
int16 	NavSensitivity;
int16 	AltSum, AE;

int16	ThrLow, ThrHigh, ThrNeutral;

// Variables for barometric sensor PD-controller
int24	OriginBaroPressure, OriginBaroTemperature, BaroSum;
int24	DesiredRelBaroAltitude, RelBaroAltitude, RelBaroAltitudeP, BE;
int16	BaroROC, BaroROCP;
i16u	BaroPress, BaroTemp;
int8	BaroSample;
int16	BaroComp, BaroTempComp;
uint8	BaroType;

uint8	LEDShadow;		// shadow register

uint8	MCamRoll,MCamPitch;
int16	Motor[NoOfMotors];
boolean	ESCI2CFail[NoOfMotors];

#pragma udata stats
int24 	MaxRelBaroAltitudeS, MaxGPSAltitudeS;
i16u 	Stats[MaxStats];
#pragma udata

int16	Trace[TopTrace+1];
Flags 	F;
uint8	LEDCycles;
int16	AttitudeHoldResetCount;	
int8	BatteryVolts;

#pragma udata txbuffer
uint8 TxCheckSum;
uint8Q TxQ;
#pragma udata

uint8 UAVXCurrPacketTag;

#pragma udata params
int8 P[MAX_PARAMETERS];
#pragma udata

// mask giving common variables across parameter sets
const rom int8	ComParms[]={
	0,0,0,1,0,0,0,0,1,0,
	0,0,0,0,0,1,1,1,0,1,
	1,1,1,1,1,0,0,1,0,0,
	1,1,0,1,1,1,1,0,0,1,
	0,1,1,1,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0
	};

const rom int8 DefaultParams[] = {
	-24, 			// RollKp, 			01
	-14, 			// RollKi,			02
	75, 			// RollKd,			03
	0, 				// HorizDampKp,		04c 
	3, 				// RollIntLimit,	05
	-24, 			// PitchKp,			06
	-14, 			// PitchKi,			07
	75, 			// PitchKd,			08
	4, 				// BaroCompKp,		09c
	3, 				// PitchIntLimit,	10
	
	-30, 			// YawKp, 			11
	-20, 			// YawKi,			12
	0, 				// YawKd,			13
	30, 			// YawLimit,		14
	2, 				// YawIntLimit,		15
	0, 				// ConfigBits,		16c
	4, 				// TimeSlots,		17c
	48, 			// LowVoltThres,	18c
	0, 				// CamRollKp,		19
	45, 			// PercentHoverThr,	20c 
	
	-1, 			// VertDampKp,		21c
	0, 				// MiddleDU,		22c
	10, 			// PercentIdleThr,	23c
	0, 				// MiddleLR,		24c
	0, 				// MiddleFB,		25c
	0, 				// CamPitchKp,		26
	24, 			// CompassKp,		27
	10, 			// BaroCompKd,		28c
	30, 			// NavRadius,		29
	8, 				// NavKi,			30 

	15, 			// NavAltKp,		31c
	15, 			// NavAltKi,		32c
	20, 			// NavRTHAlt,		33
	0, 				// NavMagVar,		34c
	ADXRS300, 		// GyroType,		35c
	ESCPPM, 		// ESCType,			36c
	DX7AR7000, 		// TxRxType			37c
	2,				// NeutralRadius	38
	30,				// PercentNavSens6Ch	39
	0,				// CamRollTrim,		40c

	-16,				// NavKd			41
	1,				// VertDampDecay    42c
	1,				// HorizDampDecay	43c
	0,				// BaroScale		44c				

	0,				// 45 - 64 unused currently
	0,
	0,	
	0,
	0,
	0,

	0,
	0,
	0,
	0,
	0,
	0,
	0,	
	0,
	0,
	0,

	0,
	0,
	0,
	0					
	};

// Reference Internal Quadrocopter Channel Order
// 1 Throttle
// 2 Aileron
// 3 Elevator
// 4 Rudder
// 5 Gear
// 6 Aux1
// 7 Aux2

const rom uint8 Map[CustomTxRx+1][CONTROLS] = {
	{ 3,1,2,4,5,6,7 }, 	// Futaba Thr 3 Throttle
	{ 2,1,4,3,5,6,7 },	// Futaba Thr 2 Throttle
	{ 5,3,2,1,6,4,7 },	// Futaba 9C Spektrum DM8/AR7000
	{ 1,2,3,4,5,6,7 },	// JR XP8103/PPM
	{ 7,1,4,6,3,5,2 },	// JR 9XII Spektrum DM9 ?

	{ 6,1,4,7,3,2,5 },	// JR DXS12 
	{ 6,1,4,7,3,2,5 },	// Spektrum DX7/AR7000
	{ 5,1,4,6,3,2,7 },	// Spektrum DX7/AR6200

	{ 3,1,2,4,5,7,6 }, 	// Futaba Thr 3 Sw 6/7
	{ 1,2,3,4,5,6,7 },	// Spektrum DX7/AR6000
	{ 1,2,3,4,5,6,7 },	// Graupner MX16S

	{ 1,2,3,4,5,6,7 }	// Custom
	};

// Rx signalling polarity used only for serial PPM frames usually
// by tapping internal Rx circuitry.
const rom boolean PPMPosPolarity[CustomTxRx+1] =
	{
		false, 	// Futaba Ch3 Throttle
		false,	// Futaba Ch2 Throttle
		true,	// Futaba 9C Spektrum DM8/AR7000
		true,	// JR XP8103/PPM
		true,	// JR 9XII Spektrum DM9/AR7000

		true,	// JR DXS12
		true,	// Spektrum DX7/AR7000
		true,	// Spektrum DX7/AR6200
		false,	// Futaba Thr 3 Sw 6/7
		true,	// Spektrum DX7/AR6000
		true,	// Graupner MX16S
		true	// custom Tx/Rx combination
	};

// Must be in thesame order as 
const rom uint8 ESCLimits [] = { OUT_MAXIMUM, OUT_HOLGER_MAXIMUM, OUT_X3D_MAXIMUM, OUT_YGEI2C_MAXIMUM };

void main(void)
{
	static int16	Temp;
	static uint8	b;

	DisableInterrupts;

	InitMisc();
	ReadStatsEE();

	InitPorts();

	OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
				USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_HIGH, _B38400);

	InitADC();		
	InitTimersAndInterrupts();
	InitParameters();

	StopMotors();
	INTCONbits.PEIE = true;	
	INTCONbits.TMR0IE = true; 
	EnableInterrupts;

	InitLISL();
	InitCompass();
	InitGPS();
	InitNavigation();
	InitBarometer();

	ShowSetup(1);

	FirstPass = true;
	
	while( true )
	{
		StopMotors();
		ReceivingGPSOnly(false);
		EnableInterrupts;

		LightsAndSirens();				// Check for Rx Signal, Disarmed on power up, Throttle closed
	
		State = Starting;
		F.MotorsArmed = true;

		#ifdef FAKE_FLIGHT 

		FakeFlight();

		#else

		while ( Armed )
		{ // no command processing while the Quadrocopter is armed
	
			ReceivingGPSOnly(true); 

			UpdateGPS();
			UpdateControls();

			if ( F.Signal && ( FailState != Terminated ) && ( FailState != Returning ) )
			{
				switch ( State  ) {
				case Starting:	// this state executed once only after arming

					LEDYellow_OFF;
					ZeroStats();
					InitControl();
					CaptureTrims();

					InitHeading();
					InitGPS();
					InitNavigation();

					DesiredThrottle = 0;
					ErectGyros();			// DO NOT MOVE AIRCRAFT!
					InitBarometer(); 		// as late as possible to allow warmup
					DoStartingBeepsWithOutput(3);

					State = Landed;
					break;
				case Landed:
					if ( DesiredThrottle < IdleThrottle  )
						SetGPSOrigin();
					else
					{
						InitHeading();						
						LEDCycles = 1;
						mS[NavActiveTime] = mS[Clock] + NAV_ACTIVE_DELAY_MS;
						Stats[RCGlitchesS].i16 = RCGlitches; // start of flight
						State = InFlight;	
					}
						
					break;
				case Landing:
					if ( DesiredThrottle > IdleThrottle )
						State = InFlight;
					else
						if ( mS[Clock] < mS[ThrottleIdleTimeout] )
							DesiredThrottle = IdleThrottle;
						else
						{		
							Stats[RCGlitchesS].i16 = RCGlitches - Stats[RCGlitchesS].i16;	
							WriteStatsEE();
							State = Landed;
						}
					break;
				case InFlight:
					DoNavigation();
					LEDGame();
					if ( DesiredThrottle < IdleThrottle )
					{
						mS[ThrottleIdleTimeout] = mS[Clock] + THROTTLE_LOW_DELAY_MS;
						State = Landing;
					}
					break;

				} // Switch State
				F.LostModel = false;
				mS[FailsafeTimeout] = mS[Clock] + FAILSAFE_TIMEOUT_MS;
				FailState = Waiting;
			}
			else
				DoPPMFailsafe();

			GetGyroValues();				// First gyro read
			GetHeading();
			CheckThrottleMoved();
			GetBaroAltitude();
			AltitudeHold();
	
			while ( mS[Clock] < mS[UpdateTimeout] ) {}; // cycle sync. point
			mS[UpdateTimeout] = mS[Clock] + P[TimeSlots];

			GetGyroValues();				// Second gyro read
			DoControl();

			MixAndLimitMotors();
			MixAndLimitCam();
			OutSignals();

			CheckAlarms();
			DumpTrace();
		
		} // flight while armed
		#endif // FAKE_FLIGHT
	}
} // main

