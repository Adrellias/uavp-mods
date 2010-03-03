// EXPERIMENTAL

#define INC_JOURNEY					// simple left circuit when engaging RTH
//#define HAVE_CUTOFF_SW				// Ground PortC Bit 0 (Pin 11) for landing cutoff.

#define ATTITUDE_FF_DIFF			24L	// 0 - 32 max feedforward speeds up roll/pitch recovery on fast stick change						

#define	ATTITUDE_ENABLE_DECAY			// enables decay to zero angle when roll/pitch is not in fact zero!
// unfortunately there seems to be a leak which cause the roll/pitch to increase without the decay.

#define NAV_RTH_LOCKOUT				350L	// ~35 units per degree - at least that is for IDG300

// Altitude Hold
#define HOVER_MAX_ROC_CMPS			50L	// Must be changing altitude at less than this for hover to be detected

//#define TEMPORARY_BARO_SCALE		94L	// SMD500 Will be in UAVPSet later inc/dec to make Baro Alt match GPS Alt 
#define TEMPORARY_BARO_SCALE		85L	// BMP085

// Accelerometers
#define DAMP_HORIZ_LIMIT 			3L	// equivalent stick units - no larger than 5

#define DAMP_VERT_LIMIT_LOW			-5L	// maximum throttle reduction
#define DAMP_VERT_LIMIT_HIGH		20L	// maximum throttle increase

// Nav
//#define ATTITUDE_NO_LIMITS				// full stick range is available otherwise it is scaled to Nav sensitivity

#define NAV_MAX_ROLL_PITCH 			25L	// Rx stick units
#define NAV_CONTROL_HEADROOM		10L	// at least this much stick control headroom above Nav control	
#define NAV_DIFF_LIMIT				24L	// Approx double NAV_INT_LIMIT
#define NAV_INT_WINDUP_LIMIT		64L	// ???

#define DEFAULT_TELEMETRY 			0	// 0 None, 1 GPS, 2 Data
// Debugging

//#define FAKE_FLIGHT					// For testing Nav on the GROUND!

//#define TESTS_FULL_BARO				// show pressures and temperatures in Baro test
//#define TESTS_FULL					// increases information displayed in tests but increases code size

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
//    If not, see http://www.gnu.org/licenses/

#ifndef BATCHMODE
//#define TRICOPTER						
//#define RX6CH 						// 6ch Receivers
//#define DEBUG_SENSORS					// Debug sensors with UAVPset - no motors
#endif // !BATCHMODE

//________________________________________________________________________________________________

#define GPS_INC_GROUNDSPEED				// GPS groundspeed is not used for flight but may be of interest

// Timeouts and Update Intervals

#define FAILSAFE_TIMEOUT_MS		1000L 	// mS. hold last "good" settings and then restore flight or abort
#define ABORT_TIMEOUT_MS		3000L	// mS. autonomous RTH with GPS or Descend without. 
#define ABORT_UPDATE_MS			2000L	// mS. retry period for RC Signal and restore Pilot in Control

#define THROTTLE_LOW_DELAY_MS	1000L	// mS. that motor runs at idle after the throttle is closed
#define THROTTLE_UPDATE_MS		3000L	// mS. constant throttle time for hover

#define NAV_ACTIVE_DELAY_MS		10000L	// mS. after throttle exceeds idle that Nav becomes active
#define NAV_RTH_LAND_TIMEOUT_MS	10000L	// mS. Shutdown throttle if descent lasts too long

#define GPS_TIMEOUT_MS			2000L	// mS.

// Altitude Hold

#define ALT_SCRATCHY_BEEPER			// Scratchy beeper noise on hover

// Accelerometers

#define USE_ACCELEROMETER				// Use the Accelerometer sensor 			

// Gyros

// Adds a delay between gyro neutral acquisition samples (16)
#define GYRO_ERECT_DELAY		1		// x 64 x 100mS 

// Enable "Dynamic mass" compensation Roll and/or Pitch
// Normally disabled for pitch only 
//#define DISABLE_DYNAMIC_MASS_COMP_ROLL
//#define DISABLE_DYNAMIC_MASS_COMP_PITCH

// Altitude Hold

#define ALT_BAND_CM				200L	// Cm.

// Throttle reduction/increase limits for Baro Alt Comp

#define MAX_DESCENT_CMPS			-50L 	// Cm./Sec
#define DESCENT_TRANS_CM			1500L	// Cm. Altitude at which final descent starts 
#define LAND_CM						300L	// Cm. deemed to have landed when below this height

#define ALT_LOW_THR_COMP			-7L		// Stick units
#define ALT_HIGH_THR_COMP			30L

#define ALT_INT_WINDUP_LIMIT		4L
#define ALT_INT_LIMIT				4L
#define ALT_DIFF_LIMIT				4L

// the range within which throttle adjustment is proportional to altitude error
#define GPS_ALT_BAND_CM				500L	// Cm.

// Navigation

#define NAV_ACQUIRE_BEEPER

#define NAV_MAX_NEUTRAL_RADIUS	3L		// Metres also minimum closing radius
#define NAV_MAX_RADIUS			90L		// Metres
#define NAV_PROXIMITY_RADIUS	10L		// Metres

// reads $GPGGA and $GPRMC sentences - all others discarded

#define	GPS_MIN_SATELLITES		6		// preferably > 5 for 3D fix
#define GPS_MIN_FIX				1		// must be 1 or 2 
#define GPS_INITIAL_SENTENCES 	30L		// Number of sentences needed with set HDilute
#define GPS_MIN_HDILUTE			130L	// HDilute * 100
	
#ifdef C90
	#define COMPASS_OFFSET_DEG	90L		// North degrees CW from Front - older compass
#else
	#define COMPASS_OFFSET_DEG	270L	// North degrees CW from Front
#endif // COMPASS_OFFSET_90

#define	NAV_GAIN_THRESHOLD 		40L		// Navigation disabled if Ch7 is less than this
#define NAV_GAIN_6CH			80L		// Low GPS gain for 6ch Rx

#define	NAV_YAW_LIMIT			10L		// yaw slew rate for RTH
#define NAV_MAX_TRIM			20L		// max trim offset for hover hold
#define NAV_CORR_SLEW_LIMIT		1L		// *5L maximum change in roll or pitch correction per GPS update

#define ATTITUDE_HOLD_LIMIT 			8L		// dead zone for roll/pitch stick for position hold
#define ATTITUDE_HOLD_RESET_INTERVAL	25L		// number of impulse cycles before GPS position is re-acquired

//#define NAV_PPM_FAILSAFE_RTH			// PPM signal failure causes RTH with Signal sampled periodically

// Throttle

#define THROTTLE_SLEW_LIMIT		0		// limits the rate at which the throttle can change (=0 no slew limit, 5 OK)
#define THROTTLE_MIDDLE			10  	// throttle stick dead zone for baro 
#define THROTTLE_HOVER			75		// min throttle stick for altitude lock

//________________________________________________________________________________________

#include "UAVXRevision.h"

// 18Fxxx C18 includes

#include <p18cxxx.h> 
#include <math.h>
#include <delays.h>
#include <timers.h>
#include <usart.h>
#include <capture.h>
#include <adc.h>

// Useful Constants
#define NUL 	0
#define SOH 	1
#define EOT 	4
#define ACK		6
#define HT 		9
#define LF 		10
#define CR 		13
#define NAK 	21
#define ESC 	27
#define true 	1
#define false 	0

#define MILLIPI 			3142 
#define CENTIPI 			314 
#define HALFMILLIPI 		1571 
#define QUARTERMILLIPI		785
#define TWOMILLIPI 			6284

#define MILLIRAD 			18 
#define CENTIRAD 			2

#define MAXINT32 			0x7fffffff
#define	MAXINT16 			0x7fff

// Additional Types
typedef unsigned char 		uint8 ;
typedef signed char 		int8;
typedef unsigned int 		uint16;
typedef int 				int16;
typedef short long 			int24;
//typedef long 			int24;
typedef unsigned short long uint24;
typedef long 				int32;
typedef unsigned long 		uint32;
typedef uint8 				boolean;
typedef float 				real32;

typedef union {
	int16 i16;
	uint16 u16;
	struct {
		uint8 b0;
		uint8 b1;
	};
	struct {
		uint8 i0;
		uint8 i1;
	};
} i16u;

typedef union {
	int24 i24;
	uint24 u24;
	struct {
		uint8 b0;
		uint8 b1;
		uint8 b2;
	};
} i24u;

typedef union {
	int32 i32;
	uint32 u32;
	struct {
		uint16 w0;
		uint16 w1;
	};
	struct {
		uint8 b0;
		uint8 b1;
		uint8 b2;
		uint8 b3;
	};
} i32u;

typedef struct {
	uint8 Head, Tail;
	uint8 B[128];
	} uint8Q;	

typedef struct {
	uint8 Head, Tail;
	int16 B[8];
	} int16Q;	

typedef struct {
	uint8 Head, Tail;
	int32 B[128];
	} int32Q;	

// Macros
#define Set(S,b) 			((uint8)(S|=(1<<b)))
#define Clear(S,b) 			((uint8)(S&=(~(1<<b))))
#define IsSet(S,b) 			((uint8)((S>>b)&1))
#define IsClear(S,b) 		((uint8)(!(S>>b)&1))
#define Invert(S,b) 		((uint8)(S^=(1<<b)))

#define Abs(i)				((i<0) ? -i : i)
#define Sign(i)				((i<0) ? -1 : 1)

#define Max(i,j) 			((i<j) ? j : i)
#define Min(i,j) 			((i<j) ? i : j )
#define Decay1(i) 			((i < 0) ? i+1 : ((i > 0) ? i-1 : 0))

#define USE_LIMIT_MACRO
#ifdef USE_LIMIT_MACRO
	#define Limit(i,l,u) 	((i < l) ? l : ((i > u) ? u : i))
#else
	#define Limit			ProcLimit
#endif

// To speed up NMEA sentence processing 
// must have a positive argument
#define ConvertDDegToMPi(d) (((int32)d * 3574L)>>11)
#define ConvertMPiToDDeg(d) (((int32)d * 2048L)/3574L)

#define ToPercent(n, m) (((n)*100L)/m)

// Simple filters using weighted averaging
#define VerySoftFilter(O,N) 	(SRS16((O)+(N)*3, 2))
#define SoftFilter(O,N) 		(SRS16((O)+(N), 1))
#define MediumFilter(O,N) 		(SRS16((O)*3+(N), 2))
#define HardFilter(O,N) 		(SRS16((O)*7+(N), 3))

// Unsigned
#define VerySoftFilterU(O,N)	(((O)+(N)*3+2)>>2)
#define SoftFilterU(O,N) 		(((O)+(N)+1)>>1)
#define MediumFilterU(O,N) 		(((O)*3+(N)+2)>>2)
#define HardFilterU(O,N) 		(((O)*7+(N)+4)>>3)

#define NoFilter(O,N)			(N)

#define DisableInterrupts 	(INTCONbits.GIEH=0)
#define EnableInterrupts 	(INTCONbits.GIEH=1)
#define InterruptsEnabled 	(INTCONbits.GIEH)

// Clock
#ifdef CLOCK_16MHZ
	#define	TMR0_1MS		0
#else // CLOCK_40MHZ
	#define	TMR0_1MS		(65536-640) // actually 1.0248mS to clear PWM
#endif // CLOCK_16MHZ

#define _PreScale0		16				// 1 6 TMR0 prescaler 
#define _PreScale1		8				// 1:8 TMR1 prescaler 
#define _PreScale2		16
#define _PostScale2		16

// Parameters for UART port ClockHz/(16*(BaudRate+1))
#ifdef CLOCK_16MHZ
#define _B9600			104
#define _B38400			26 
#else // CLOCK_40MHZ
#define _B9600			65
#define _B38400			65
#endif // CLOCK_16MHZ

// This is messy - trial and error to determine worst case interrupt latency!
#ifdef CLOCK_16MHZ
	#define INT_LATENCY		(uint16)(256 - 35) // x 4uS
	#define FastWriteTimer0(t) TMR0L=(uint8)t
	#define GetTimer0		Timer0.u16=(uint16)TMR0L
#else // CLOCK_40MHZ
	#define INT_LATENCY		(uint16)(65536 - 35) // x 1.6uS
	#define FastWriteTimer0(t) Timer0.u16=t;TMR0H=Timer0.b1;TMR0L=Timer0.b0
	#define GetTimer0		{Timer0.b0=TMR0L;Timer0.b1=TMR0H;}	
#endif // CLOCK_16MHZ

// LEDs
#define AUX2M			0x01
#define BlueM			0x02
#define RedM			0x04
#define GreenM			0x08
#define AUX1M			0x10
#define YellowM			0x20
#define AUX3M			0x40
#define BeeperM			0x80

#define ALL_LEDS_ON		LEDsOn(BlueM|RedM|GreenM|YellowM)
#define AUX_LEDS_ON		LEDsOn(AUX1M|AUX2M|AUX3M)

#define ALL_LEDS_OFF	LEDsOff(BlueM|RedM|GreenM|YellowM)
#define AUX_LEDS_OFF	LEDsOff(AUX1M|AUX2M|AUX3M)

#define ALL_LEDS_ARE_OFF	( (LEDShadow&(BlueM|RedM|GreenM|YellowM))== 0 )

#define LEDRed_ON		LEDsOn(RedM)
#define LEDBlue_ON		LEDsOn(BlueM)
#define LEDGreen_ON		LEDsOn(GreenM)
#define LEDYellow_ON	LEDsOn(YellowM) 
#define LEDAUX1_ON		LEDsOn(AUX1M)
#define LEDAUX2_ON		LEDsOn(AUX2M)
#define LEDAUX3_ON		LEDsOn(AUX3M)
#define LEDRed_OFF		LEDsOff(RedM)
#define LEDBlue_OFF		LEDsOff(BlueM)
#define LEDGreen_OFF	LEDsOff(GreenM)
#define LEDYellow_OFF	LEDsOff(YellowM)
#define LEDYellow_TOG	if( (LEDShadow&YellowM) == 0 ) LEDsOn(YellowM); else LEDsOff(YellowM)
#define LEDRed_TOG		if( (LEDShadow&RedM) == 0 ) LEDsOn(RedM); else LEDsOff(RedM)
#define LEDBlue_TOG		if( (LEDShadow&BlueM) == 0 ) LEDsOn(BlueM); else LEDsOff(BlueM)
#define LEDGreen_TOG	if( (LEDShadow&GreenM) == 0 ) LEDsOn(GreenM); else LEDsOff(GreenM)
#define Beeper_OFF		LEDsOff(BeeperM)
#define Beeper_ON		LEDsOn(BeeperM)
#define Beeper_TOG		if( (LEDShadow&BeeperM) == 0 ) LEDsOn(BeeperM); else LEDsOff(BeeperM)

// Bit definitions
#define Armed			(PORTAbits.RA4)

#ifdef HAVE_CUTOFF_SW
#define InTheAir		(PORTCbits.RC0) // normally open micro switch to ground
#else
#define InTheAir		true	 
#endif // HAVE_CUTOFF_SW


#define	I2C_ACK			((uint8)(0))
#define	I2C_NACK		((uint8)(1))

#define SPI_CS			PORTCbits.RC5
#define SPI_SDA			PORTCbits.RC4
#define SPI_SCL			PORTCbits.RC3
#define SPI_IO			TRISCbits.TRISC4

#define	RD_SPI			1
#define WR_SPI			0
#define DSEL_LISL  		1
#define SEL_LISL  		0

// RC

#define CONTROLS 			7
#define MAX_CONTROLS 		12 	// maximum Rx channels

#define RxFilter			MediumFilterU
//#define RxFilter			SoftFilterU
//#define RxFilter			NoFilter

#define	RC_GOOD_BUCKET_MAX	20
#define RC_GOOD_RATIO		4

#define RC_MINIMUM			0
#define RC_MAXIMUM			238
#define RC_NEUTRAL			((RC_MAXIMUM-RC_MINIMUM+1)/2)

#define RC_MAX_ROLL_PITCH	(170)	

#define RC_THRES_STOP		((6L*RC_MAXIMUM)/100)		
#define RC_THRES_START		((10L*RC_MAXIMUM)/100)		

#define RC_FRAME_TIMEOUT_MS 	25
#define RC_SIGNAL_TIMEOUT_MS 	(5L*RC_FRAME_TIMEOUT_MS)
#define RC_THR_MAX 			RC_MAXIMUM

#define MAX_ROLL_PITCH		RC_NEUTRAL	// Rx stick units - rely on Tx Rate/Exp

#ifdef RX6CH 
	#define RC_CONTROLS 5			
#else
	#define RC_CONTROLS CONTROLS
#endif //RX6CH

// ESC
#define OUT_MINIMUM			1			// Required for PPM timing loops
#define OUT_MAXIMUM			200			// to reduce Rx capture and servo pulse output interaction
#define OUT_NEUTRAL			105			// 1.503mS @ 105 16MHz
#define OUT_HOLGER_MAXIMUM	225
#define OUT_YGEI2C_MAXIMUM	240
#define OUT_X3D_MAXIMUM		200

// Compass sensor
#define COMPASS_I2C_ID		0x42		// I2C slave address
#define COMPASS_MAXDEV		30			// maximum yaw compensation of compass heading 
#define COMPASS_MIDDLE		10			// yaw stick neutral dead zone
#define COMPASS_TIME_MS		50			// 20Hz

// EEPROM

#define PARAMS_ADDR_EE		0			// code assumes zero!
#define MAX_PARAMETERS	64		// parameters in EEPROM start at zero

#define STATS_ADDR_EE	 	( PARAMS_ADDR_EE + (MAX_PARAMETERS *2) )
#define MAX_STATS			32

// uses second Page of EEPROM
#define NAV_ADDR_EE			256L
#define WP_NO_ADDR_EE		(NAV_ADDR_EE + 9)	// number of waypoints 		
#define WP_ADDR_EE			(NAV_ADDR_EE + 24)	// 24 is ArduPilot misc nav params
#define WAYPOINT_REC_SIZE 	11					// 4 (Lat) + 4 (Lon) + 2 (Alt) + 1 Byte Loiter
#define NAV_MAX_WAYPOINTS	((256 - 24)/WAYPOINT_REC_SIZE) 

// Sanity checks

// check the Rx and PPM ranges
#if OUT_MINIMUM >= OUT_MAXIMUM
#error OUT_MINIMUM < OUT_MAXIMUM!
#endif
#if (OUT_MAXIMUM < OUT_NEUTRAL)
#error OUT_MAXIMUM < OUT_NEUTRAL !
#endif

#if RC_MINIMUM >= RC_MAXIMUM
#error RC_MINIMUM < RC_MAXIMUM!
#endif
#if (RC_MAXIMUM < RC_NEUTRAL)
#error RC_MAXIMUM < RC_NEUTRAL !
#endif

// end of sanity checks

//______________________________________________________________________________________________

// main.c

#define FLAG_BYTES 6
typedef union {
	uint8 AllFlags[FLAG_BYTES];
	struct { // Order of these flags subject to change
		uint8
		Signal:1,
		RCFrameOK:1, 
		LostModel:1,
		Failsafe:1,

		LowBatt:1,
		BaroFailure:1,
		AccFailure:1,
		CompassFailure:1,
		GPSFailure:1,

		AttitudeHold:1,
		ThrottleMoving:1,
		Hovering:1,
		Navigate:1,
		ReturnHome:1,
		Proximity:1,
		CloseProximity:1,

		UsingGPSAlt:1,
		UsingRTHAutoDescend:1,
		BaroAltitudeValid:1,
		RangefinderAltitudeValid:1,
		UsingRangefinderAlt:1,

		// internal flags not really useful externally
		ParametersValid:1,

		RCNewValues:1,
		NewCommands:1,

		AccelerationsValid:1,
		CompassValid:1,
		CompassMissRead:1,
		GyrosErected:1,

		ReceivingGPS:1,
		GPSSentenceReceived:1,
		GPSValid:1,
		NavValid:1,

		NavComputed:1,
		CheckThrottleMoved:1,
		MotorsArmed:1,
		NavAltitudeHold:1,	// stick programmed
		TurnToWP:1,			// stick programmed
		WayPointsChanged:1,
		UsingSerialPPM:1,
		UsingTxMode2:1,
		UsingXMode:1,
		UsingTelemetry:1,
		NewBaroValue:1,
		BeeperInUse:1, 
		AcquireNewPosition:1, 
		GPSTestActive:1;

		};
} Flags;

enum FlightStates { Starting, Landing, Landed, InFlight};
extern Flags F;
extern near uint8 State;

// accel.c

extern void SendCommand(int8);
extern uint8 ReadLISL(uint8);
extern uint8 ReadLISLNext(void);
extern void WriteLISL(uint8, uint8);
extern void IsLISLActive(void);
extern void InitLISL(void);
extern void ReadAccelerations(void);
extern void GetNeutralAccelerations(void);

extern i16u Ax, Ay, Az;
extern int8 LRIntCorr, FBIntCorr;
extern int8 NeutralLR, NeutralFB, NeutralDU;
extern int16 DUVel, LRVel, FBVel, DUAcc, LRAcc, FBAcc, DUComp, LRComp, FBComp;
//______________________________________________________________________________________________

// adc.c

extern int16 ADC(uint8);
extern void InitADC(void);

// ADC Channels
#define ADCPORTCONFIG 		0b00001010 // AAAAA
#define ADCBattVoltsChan 	0 
#define NonIDGADCRollChan 	1
#define NonIDGADCPitchChan 	2
#define IDGADCRollChan 		2
#define IDGADCPitchChan 	1
#define ADCAltChan 			3 	// Altitude
#define ADCYawChan			4

//______________________________________________________________________________________________

// autonomous.c

extern void Navigate(int24, int24);
extern void SetDesiredAltitude(int16);
extern void DoFailsafeLanding(void);
extern void AcquireHoldPosition(void);
extern void NavGainSchedule(int16);
extern void DoNavigation(void);
extern void FakeFlight(void); 
extern void DoPPMFailsafe(void);
extern void WriteWayPointEE(uint8, int32, int32, int16, uint8);
extern void LoadNavBlockEE(void);
extern void GetWayPointEE(uint8);
extern void InitNavigation(void);

typedef struct { int32 E, N; int16 A; uint8 L; } WayPoint;

enum NavStates { HoldingStation, AtHome, Descending, Touchdown, Navigating, Loitering,
	Terminating };
enum FailStates { Waiting, Aborting, Returning, Terminated };

extern near uint8 FailState;
extern WayPoint WP;
extern uint8 CurrWP;
extern int8 NoOfWayPoints;
extern int16 NavClosingRadius, NavNeutralRadius, NavCloseToNeutralRadius, NavProximityRadius; 
extern int16 CompassOffset, NavRTHTimeoutmS;
extern uint8 NavState;
extern int16 NavSensitivity, RollPitchMax;
extern int32 NavRTHTimeout;
extern int16 AltSum, AE;

//______________________________________________________________________________________________

// baro.c

extern void StartBaroADC(boolean);
extern void ReadBaro(boolean);
extern void GetBaroAltitude(void);
extern void InitBarometer(void);

#define BARO_ID_BMP085		((uint8)(0x55))

extern int24 OriginBaroPressure, OriginBaroTemperature, BaroSum, CompBaroPress;
extern int24 RelBaroAltitude, RelBaroAltitudeP;
extern int16 BaroROC, BaroROCP;
extern i16u	BaroPress, BaroTemp;
extern int8	BaroSample;
extern int16 BaroTempComp;
extern uint8 BaroType;

//______________________________________________________________________________________________

// compass.c

extern void InitCompass(void);
extern void InitHeading(void);
extern void GetHeading(void);

extern i16u Compass;

//______________________________________________________________________________________________

// control.c

extern void DoAltitudeHold(int24, int16);
extern void AltitudeHold(void);

extern void LimitRollSum(void);
extern void LimitPitchSum(void);
extern void LimitYawSum(void);
extern void InertialDamping(void);
extern void DoControl(void);

extern void UpdateControls(void);
extern void CaptureTrims(void);
extern void StopMotors(void);
extern void CheckThrottleMoved(void);
extern void LightsAndSirens(void);
extern void InitControl(void);

extern int16 RC[];
extern int16 RE, PE, YE, HE;					// gyro rate error	
extern int16 REp, PEp, YEp, HEp;				// previous error for derivative
extern int16 Rl,Pl,Yl;							// PID output values
extern int16 RollSum, PitchSum, YawSum;			// integral/angle	
extern int16 RollTrim, PitchTrim, YawTrim;
extern int16 HoldYaw;
extern int16 RollIntLimit256, PitchIntLimit256, YawIntLimit256;
extern int16 HoverThrottle, DesiredThrottle, IdleThrottle, InitialThrottle;
extern int16 DesiredRoll, DesiredPitch, DesiredYaw, DesiredHeading, DesiredCamPitchTrim, Heading;
extern int16 DesiredRollP, DesiredPitchP;
extern int16 CurrMaxRollPitch;
extern int16 ThrLow, ThrHigh, ThrNeutral;
extern int16 AltComp, AltDiffSum, AltDSum, DescentCmpS;
extern int16 AttitudeHoldResetCount;
extern int24 DesiredAltitude;
extern boolean FirstPass;

//______________________________________________________________________________________________

// eeprom.c

extern int8 ReadEE(uint16);
extern int16 Read16EE(uint16);
extern int32 Read32EE(uint16);
extern void WriteEE(uint16, int8);
extern void Write16EE(uint16, int16);
extern void Write32EE(uint16, int32);

//______________________________________________________________________________________________

// gps.c

extern void UpdateField(void);
extern int32 ConvertGPSToM(int32);
extern int32 ConvertMToGPS(int32);
extern int24 ConvertInt(uint8, uint8);
extern int32 ConvertLatLonM(uint8, uint8);
extern int32 ConvertUTime(uint8, uint8);
extern void ParseGPRMCSentence(void);
extern void ParseGPGGASentence(void);
extern void SetGPSOrigin(void);
extern void ParseGPSSentence(void);
extern void InitGPS(void);
extern void UpdateGPS(void);

#define MAXTAGINDEX 		4
#define GPSRXBUFFLENGTH 	80
extern struct {
		uint8 	s[GPSRXBUFFLENGTH];
		uint8 	length;
	} NMEA;

extern const rom uint8 NMEATag[];

extern int32 GPSMissionTime, GPSStartTime;
extern int32 GPSLatitude, GPSLongitude;
extern int32 GPSOriginLatitude, GPSOriginLongitude;
extern int24 GPSAltitude, GPSRelAltitude, GPSOriginAltitude;
extern int24 GPSNorth, GPSEast, GPSNorthP, GPSEastP, GPSNorthHold, GPSEastHold;
extern int16 GPSLongitudeCorrection;
extern int16 GPSVel, GPSROC;
extern uint8 GPSNoOfSats;
extern uint8 GPSFix;
extern int16 GPSHDilute;
extern uint8 nll, cc, lo, hi;
extern boolean EmptyField;
extern int16 ValidGPSSentences;
extern int32 SumGPSRelAltitude, SumCompBaroPress;

//______________________________________________________________________________________________

// gyro.c

extern void CompensateRollPitchGyros(void);
extern void GetRollPitchGyroValues(void);
extern void GetYawGyroValue(void);
extern void ErectGyros(void);
extern void CalcGyroRates(void);

extern int16 GyroMidRoll, GyroMidPitch, GyroMidYaw;
extern int16 RollRate, PitchRate, YawRate;

//______________________________________________________________________________________________

// irq.c

extern void SyncToTimer0AndDisableInterrupts(void);
extern void DoRxPolarity(void);
extern void ReceivingGPSOnly(uint8);
extern void InitTimersAndInterrupts(void);
extern void ReceivingGPSOnly(uint8);

enum { Clock, UpdateTimeout, RCSignalTimeout, BeeperTimeout, ThrottleIdleTimeout, 
	FailsafeTimeout, AbortTimeout, RTHTimeout, LoiterTimeout, LandingTimeout, LastValidRx, LastGPS, 
	GPSTimeout, GPSROCUpdate, ArduPilotUpdate, RangefinderROCUpdate, NavActiveTime, ThrottleUpdate, VerticalDampingUpdate, BaroUpdate, CompassUpdate};

enum WaitGPSStates { WaitGPSSentinel, WaitNMEATag, WaitGPSBody, WaitGPSCheckSum};

extern uint24 mS[];
extern int16 RC[];

extern near i16u PPM[];
extern near int8 PPM_Index;
extern near int24 PrevEdge, CurrEdge;
extern near i16u Width, Timer0;
extern near int24 PauseTime; // for tests
extern near uint8 GPSRxState;
extern near uint8 ll, tt, gps_ch;
extern near uint8 RxCheckSum, GPSCheckSumChar, GPSTxCheckSum;
extern int8	SignalCount;
extern uint16 RCGlitches;

//______________________________________________________________________________________________

// i2c.c

extern boolean I2CWaitClkHi(void);
extern void I2CStart(void);
extern void I2CStop(void);
extern uint8 SendI2CByte(uint8);
extern uint8 RecvI2CByte(uint8);

extern boolean ESCWaitClkHi(void);
extern void ESCI2CStart(void);
extern void ESCI2CStop(void);
extern uint8 SendESCI2CByte(uint8);

//______________________________________________________________________________________________

// leds.c

extern void SendLEDs(void);
extern void LEDsOn(uint8);
extern void LEDsOff(uint8);
extern void LEDGame(void);

extern uint8 LEDShadow;		// shadow register
extern uint8 LEDCycles;		// for hover light display

//______________________________________________________________________________________________

// math.c

extern int16 SRS16(int16, uint8);
extern int32 SRS32(int32, uint8);
extern int16 Make2Pi(int16);
extern int16 MakePi(int16);
extern int16 Table16(int16, const int16 *);
extern int16 int16sin(int16);
extern int16 int16cos(int16);
extern int16 int32atan2(int32, int32);
extern int16 int16sqrt(int16);
extern int32 int32sqrt(int32);

//______________________________________________________________________________________________

// menu.c

extern void ShowPrompt(void);
extern void ShowRxSetup(void);
extern void ShowSetup(uint8);
extern void ProcessCommand(void);

extern const rom uint8 SerHello[];
extern const rom uint8 SerSetup[];
extern const rom uint8 SerPrompt[];

extern const rom uint8 RxChMnem[];

//______________________________________________________________________________________________

// outputs.c

extern uint8 SaturInt(int16);
extern void DoMix(int16 CurrThrottle);
extern void CheckDemand(int16 CurrThrottle);
extern void MixAndLimitMotors(void);
extern void MixAndLimitCam(void);
extern void OutSignals(void);
extern void InitI2CESCs(void);

enum MotorTags {Front=0, Back, Right, Left}; // order is important for X3D & Holger ESCs
#define NoOfMotors 		4

extern int16 Motor[NoOfMotors];
extern boolean ESCI2CFail[NoOfMotors];
extern near uint8 SHADOWB, MF, MB, ML, MR, MT, ME;
extern near uint8 ESCMin, ESCMax;
extern near boolean ServoToggle;

//______________________________________________________________________________________________

// params.c

extern void MapRC(void);
extern void ReadParametersEE(void);
extern void WriteParametersEE(uint8);
extern void UseDefaultParameters(void);
extern void UpdateParamSetChoice(void);
extern void InitParameters(void);

enum TxRxTypes { 
	FutabaCh3, FutabaCh2, FutabaDM8, JRPPM, JRDM9, JRDXS12, 
	DX7AR7000, DX7AR6200, FutabaCh3_6_7, DX7AR6000, GraupnerMX16s, CustomTxRx };
enum RCControls {ThrottleC, RollC, PitchC, YawC, RTHC, CamPitchC, NavGainC}; 
enum ESCTypes { ESCPPM, ESCHolger, ESCX3D, ESCYGEI2C };
enum GyroTypes { ADXRS300, ADXRS150, IDG300};

enum Params { // MAX 64
	RollKp, 			// 01
	RollKi,				// 02
	RollKd,				// 03
	HorizDampKp,		// 04c
	RollIntLimit,		// 05
	PitchKp,			// 06
	PitchKi,			// 07
	PitchKd,			// 08
	AltKp,				// 09c
	PitchIntLimit,		// 10
	
	YawKp, 				// 11
	YawKi,				// 12
	YawKd,				// 13
	YawLimit,			// 14
	YawIntLimit,		// 15
	ConfigBits,			// 16c
	TimeSlots,			// 17c
	LowVoltThres,		// 18c
	CamRollKp,			// 19
	PercentHoverThr,	// 20c 
	
	VertDampKp,			// 21c
	MiddleDU,			// 22c
	PercentIdleThr,		// 23c
	MiddleLR,			// 24c
	MiddleFB,			// 25c
	CamPitchKp,			// 26
	CompassKp,			// 27
	AltKi,				// 28c 
	NavRadius,			// 29
	NavKi,				// 30
	
	unused1,			// 31
	unused2,			// 32
	NavRTHAlt,			// 33
	NavMagVar,			// 34c
	GyroType,			// 35c
	ESCType,			// 36c
	TxRxType,			// 37c
	NeutralRadius,		// 38
	PercentNavSens6Ch,	// 39
	CamRollTrim,		// 40c
	NavKd,				// 41
	VertDampDecay,		// 42c
	HorizDampDecay,		// 43c
	BaroScale,			// 44c
	TelemetryType,		// 45c
	MaxDescentRateDmpS,	// 46
	DescentDelayS,		// 47c
	NavIntLimit,		// 48
	AltIntLimit,		// 49
	GravComp,			// 50
	CompSteps			// 51		
	// 51 - 64 unused currently
	};

#define FlyXMode 			0
#define FlyXModeMask 		0x01

#define UseRTHDescend 		1
#define	UseRTHDescendMask	0x02

#define TxMode2 			2
#define TxMode2Mask 		0x04

#define RxSerialPPM 		3
#define RxSerialPPMMask		0x08 

// bit 4 is pulse polarity for 3.15

#define UseGPSAlt 			5
#define	UseGPSAltMask		0x20

// bit 6 is throttle channel for 3.15

// bit 7 unusable in UAVPSet

extern const rom int8 ComParms[];
extern const rom int8 DefaultParams[];
extern const rom uint8 Map[CustomTxRx+1][CONTROLS];
extern const rom uint8 ESCLimits [];
extern const rom boolean PPMPosPolarity[];

extern uint8 ParamSet;
extern boolean ParametersChanged;
extern int8 P[];
extern int8 RMap[];

//__________________________________________________________________________________________

// rangefinder.c

extern void GetRangefinderAltitude(void);
extern void InitRangefinder(void);

extern int16 RangefinderAltitude, RangefinderROC;

//__________________________________________________________________________________________

// serial.c

extern void TxString(const rom uint8 *);
extern void TxChar(uint8);
extern void TxValU(uint8);
extern void TxValS(int8);
extern void TxNextLine(void);
extern void TxNibble(uint8);
extern void TxValH(uint8);
extern void TxValH16(uint16);
extern uint8 PollRxChar(void);
extern uint8 RxChar(void);
extern uint8 RxNumU(void);
extern int8 RxNumS(void);
extern void TxVal32(int32, int8, uint8);
extern void SendByte(uint8);
extern void SendESCByte(uint8);
extern void SendWord(int16);
extern void SendESCWord(int16);
extern void SendPacket(uint8, uint8, uint8 *, boolean);

#define TX_BUFF_MASK	127
extern uint8 	TxCheckSum;
extern uint8Q 	TxQ;

//______________________________________________________________________________________________

// stats.c

extern void ZeroStats(void);
extern void ReadStatsEE(void);
extern void WriteStatsEE(void);
extern void ShowStats(void);

enum Statistics { 
	GPSAltitudeS, RelBaroAltitudeS, RelBaroPressureS, GPSBaroScaleS, MinBaroROCS, MaxBaroROCS, GPSVelS,  
	AccFailS, CompassFailS, BaroFailS, GPSInvalidS, GPSSentencesS, NavValidS, 
	MinHDiluteS, MaxHDiluteS, RCGlitchesS}; // NO MORE THAN 16 or 32 bytes

extern int24 MaxRelBaroAltitudeS, MaxGPSAltitudeS;
extern int16 Stats[];
//______________________________________________________________________________________________

// utils.c

extern void InitPorts(void);
extern void InitMisc(void);
extern void Delay1mS(int16);
extern void Delay100mSWithOutput(int16);
extern void DoBeep100mSWithOutput(uint8, uint8);
extern void DoStartingBeepsWithOutput(uint8);
extern int16 SlewLimit(int16, int16, int16);
extern int32 ProcLimit(int32, int32, int32);
extern int16 DecayX(int16, int16);
extern void CheckAlarms(void);
extern void DumpTrace(void);
extern void SendUAVXState(void);
extern void ArduPilotTelemetry(void);

enum TraceTags {
	THE, TRelBaroAltitude,
	TRollRate,TPitchRate,TYE,
	TRollSum,TPitchSum,TYawSum,
	TAx,TAz,TAy,
	TLRIntCorr, TFBIntCorr,
	TDesiredThrottle,
	TDesiredRoll, TDesiredPitch, TDesiredYaw,
	TMFront, TMBack, TMLeft, TMRight,
	TMCamRoll, TMCamPitch
	};
#define TopTrace TMCamPitch

enum PacketTags {UnknownPacketTag, LevPacketTag, NavPacketTag, MicropilotPacketTag, WayPacketTag, 
	AirframePacketTag, NavUpdatePacketTag, BasicPacketTag, RestartPacketTag, TrimblePacketTag, 
	MessagePacketTag, EnvironmentPacketTag, BeaconPacketTag, UAVXFlightPacketTag, 
	UAVXNavPacketTag};

enum TelemetryTypes { NoTelemetry, GPSTelemetry, DataTelemetry };

extern int16 Trace[];
extern int8 BatteryVolts;
extern uint8 UAVXCurrPacketTag;

//______________________________________________________________________________________________

// bootl18f.asm

extern void BootStart(void);

//______________________________________________________________________________________________

// tests.c

extern void DoLEDs(void);
extern void LinearTest(void);
extern uint8 ScanI2CBus(void);
extern void ReceiverTest(void);
extern void GetCompassParameters(void);
extern void DoCompassTest(void);
extern void CompassRun(void);
extern void CalibrateCompass(void);
extern void BaroTest(void);
extern void PowerOutput(int8);
extern void LEDsAndBuzzer(void);
extern void GPSTest(void);
extern void AnalogTest(void);
extern void ProgramSlaveAddress(uint8);
extern void ConfigureESCs(void);




