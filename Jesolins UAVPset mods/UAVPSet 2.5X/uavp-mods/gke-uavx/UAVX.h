
#define COMMISSIONING
//#define RAW_BOOT
//#define BOOTONLY

#define LEGACY_CTL

#define DEBUG_SENSORS

#ifndef LEGACY_CTL
	#define THROTTLE_ROC

//	#define USE_GPS
//	#define GPS_NMEA
//	#define DUMMY_GPS
#endif // !LEGACY_CTL

#ifndef BATCHMODE
// =======================================================================
// =                                 UAVX                                =
// =                         Quadrocopter Control                        =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =     Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer       =
// =                          http://www.uavp.org                        =
// =======================================================================
//
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

#include <p18cxxx.h> 
#include <math.h>
#include <delays.h>
#include <timers.h>
#include <usart.h>
#include <capture.h>

#undef BATCHMODE

// if you want the led outputs for test purposes
#define NOLEDGAME

// Only one of the following 3 defines must be activated:
// When using 3 ADXRS300 gyros
//#define OPT_ADXRS300

// When using 3 ADXRS150 gyros
//#define OPT_ADXRS150

// When using 1 ADXRS300 and 1 IDG300 gyro
#define OPT_IDG

// Select what speeed controllers to use:
// to use standard PPM pulse
#define ESC_PWM
// to use X3D BL controllers (not yet tested. Info courtesy to Jast :-)
//#define ESC_X3D
// to use Holgers ESCs (tested and confirmed by ufo-hans)
//#define ESC_HOLGER
// to use YGE I2C controllers (for standard YGEs use ESC_PWM)
//#define ESC_YGEI2C

// defined: serial PPM pulse train from receiver
// RX_DEFAULT: standard servo pulses from CH1, 3, 5 and 7
#define RX_DEFAULT
//#define RX_PPM

// uncomment this to enable Tri-Copter Mixing.
// connector K1 = front motor
//           K2 = rear left motor 
//           K3 = rear right motor 
//           K4 = yaw servo output
// Camera controlling can be used!
//#define TRICOPTER

// Debugging output of the 4 motors on RS232
// When throttling up, every cycle  4 numbers are output
// on the com port:
// <front>;<rear>;<left>;<right> CR LF
// these are the throttle value to the 4 speed controllers
// log them with a terminal proggie to make nice charts :-)
// Reg. 26 ( CamFactor in UAVPSet) controls if motors should run or not
// bit 0 = front, 1 = rear, 2 = left, 3 = right
// if a bit is 1 motor is always stopped, only comport output
// if a bit is 0 motor is running as normal.
// so you can test individual axis easily!
// Reg. 26 bit 4: 1=serial data output on
// NOTE: enable DEBUG_MOTORS only on 3.1 boards (black PCB)!
//#define DEBUG_MOTORS

// special mode for sensor data output (with UAVPset)
// #define DEBUG_SENSORS

#endif // !BATCHMODE

// Includes GPS functionality and limited autonomous capability
//#define USE_GPS
// Use standard NMEA protocol otherwise Trimble TSIP protocol
//#define GPS_NMEA

// 18F2520 PIC can be run at 32MHz with an 8MHz XTAL. At 16MHz there is no 
// camera control to reduce the complexity/reliability of output pulse
// generation. Comment out if using 32MHz
#define CLOCK_16MHZ

// =====================================
// end of user-configurable section!
// =====================================

// this enables common code for all ADXRS gyros
// leave this untouched!
#if defined OPT_ADXRS300 || defined OPT_ADXRS150
#define OPT_ADXRS
#endif

// Misc Constants

#define NUL 0
#define SOH 1
#define ETX 3
#define EOT 4
#define ENQ 5
#define ACK 6
#define HT 9
#define LF 10
#define CR 13
#define DLE 16
#define NAK 21
#define ESC 27

#define EarthR (real32)(6378140.0)
#define Pi (real32)(3.141592654)
#define RecipEarthR (real32)(1.5678552E-7)

#define DEGTOM (real32)(111319.5431531) 
#define MILLIPI 3142 
#define CENTIPI 314 
#define HALFMILLIPI 1571 
#define TWOMILLIPI 6284

#define MILLIRAD 18 
#define CENTIRAD 2
#define DEGMILLIRAD (real32)(17.453292) 
#define MILLIRADDEG (real32)(0.05729578)
#define MILLIRADDEG100 (real32)(5.729578)
#define CENTIRADDEG (real32)(0.5729578)
#define DEGRAD (real32)(0.017453292)
#define RADDEG (real32)(57.2957812)
#define RADDEG1000000 (real32)(57295781.2)

#define KTTODMPS 5.15 

// Types

#define true 1
#define false 0

#define MAXINT32 0x7fffffff;
#define	MAXINT16 0x7fff;

typedef unsigned char uint8 ;
typedef signed char int8;
typedef unsigned int uint16;
typedef int int16;
typedef long int32;
typedef unsigned long uint32;
typedef short long int24;
typedef unsigned short long uint24;
typedef float real32;
typedef uint8 boolean;

// ------------------------------------------------------------------------------------

// Macros - could be turned into functions if space gets tight

#define USE_MACROS
#define Set(S,b) 		((uint8)(S|=(1<<b)))
#define Clear(S,b) 		((uint8)(S&=(~(1<<b))))
#define IsSet(S,b) 		((uint8)((S>>b)&1))
#define IsClear(S,b) 	((uint8)(!(S>>b)&1))
#define Invert(S,b) 	((uint8)(S^=(1<<b)))

#define Upper8(i) 		(i>>8)
#define Lower8(i) 		(i&0xff)
#define Abs(a) 			(a<0 ? -a: a)
#define Sense(a) 		(a<0 ? -1 : (a>0 ? 1 : 1))
 	
#define Max(i,j) 		((i<j) ? j : i)
#define Min(i,j) 		((i<j) ? i : j)
#define Limit(i,l,u) 	((i<l) ? l : ((i>u) ? u : i))
#define DecayBand(i,l,u,d) 	((i<l) ? i+d : ((i>u) ? i-d : i))
#define Decay(i) 		((i<0) ? i+1 : ((i>0) ? i-1 : 0))

#define DisableInterrupts (INTCONbits.GIEH=0)
#define EnableInterrupts (INTCONbits.GIEH=1)

// Simple filters using weighted averaging

#ifdef SUPPRESSFILTERS
  #define VerySoftFilter(O,N)		(N)
  #define SoftFilter(O,N) 			(N)
  #define MediumFilter(O,N) 			(N)
  #define AccelerometerFilter(O,N) 	(N)
#else
  #define VerySoftFilter(O,N) 		(SRS16(O+N*3+2, 2))
  #define SoftFilter(O,N) 			(SRS16(O+N+1, 1))
  #define MediumFilter(O,N) 		(SRS16(O*3+N+2, 2))
  #define HardFilter(O,N) 			(SRS16(O*7+N+4, 3))
#endif
#define NoFilter(O,N)				(N)

#define	PERCENT(v,m)	((v*m+50)/100)

// ------------------------------------------------------------------------------------

// ADC Channels
#define ADCBattVoltsChan 	0 

#ifdef OPT_ADXRS
	#define ADCRollChan 	1
	#define ADCPitchChan 	2
#else // OPT_IDG
	#define ADCRollChan 	2
	#define ADCPitchChan 	1
#endif // OPT_ADXRS

#define ADCVRefChan 			3 
#define ADCYawChan			4

// ------------------------------------------------------------------------------------

#define	Switch			PORTAbits.RA4

// The LEDs and the beeper

#define ON	1
#define OFF	0

#define LedYellow		LED6
#define LedGreen		LED4
#define	LedBlue			LED2
#define LedRed			LED3
#define LedAUX1			LED5
#define LedAUX2			LED1
#define LedAUX3			LED7

#define LED1	0x01	/* Aux2 */
#define LED2	0x02	/* blue */
#define LED3	0x04	/* red */ 
#define LED4	0x08	/* green */
#define LED5	0x10	/* Aux1 */
#define LED6	0x20	/* yellow */
#define LED7	0x40	/* Aux3 */
#define Beeper	0x80

#define ALL_LEDS_ON		SwitchLedsOn(LedBlue|LedRed|LedGreen|LedYellow)
#define AUX_LEDS_ON		SwitchLedsOn(LedAUX1|LedAUX2|LedAUX3)

#define ALL_LEDS_OFF	SwitchLedsOff(LedBlue|LedRed|LedGreen|LedYellow)
#define AUX_LEDS_OFF	SwitchLedsOff(LedAUX1|LedAUX2|LedAUX3)

#define ARE_ALL_LEDS_OFF if((LedShadow&(LedBlue|LedRed|LedGreen|LedYellow))==0)

#define LedRed_ON		SwitchLedsOn(LedRed)
#define LedBlue_ON		SwitchLedsOn(LedBlue)
#define LedGreen_ON		SwitchLedsOn(LedGreen)
#define LedYellow_ON	SwitchLedsOn(LedYellow)
#define LedAUX1_ON		SwitchLedsOn(LedAUX1);
#define LedAUX2_ON		SwitchLedsOn(LedAUX2);
#define LedAUX3_ON		SwitchLedsOn(LedAUX3);
#define LedRed_OFF		SwitchLedsOff(LedRed)
#define LedBlue_OFF		SwitchLedsOff(LedBlue)
#define LedGreen_OFF	SwitchLedsOff(LedGreen)
#define LedYellow_OFF	SwitchLedsOff(LedYellow)
#define LedRed_TOG		if( (LedShadow&LedRed) == 0 ) SwitchLedsOn(LedRed); else SwitchLedsOff(LedRed)
#define LedBlue_TOG		if( (LedShadow&LedBlue) == 0 ) SwitchLedsOn(LedBlue); else SwitchLedsOff(LedBlue)
#define Beeper_OFF		SwitchLedsOff(Beeper)
#define Beeper_ON		SwitchLedsOn(Beeper)
#define Beeper_TOG		if( (LedShadow&Beeper) == 0 ) SwitchLedsOn(Beeper); else SwitchLedsOff(Beeper)

//------------------------------------------------------------------------------------

#define	_Signal			Flags[0]			/*if no valid signal is received*/
#define _NewValues		Flags[1]			/*new RX channel values sampled*/
#define _LowBatt		Flags[2]
#define	_UseLISL		Flags[3]	
#define	_UseCompass		Flags[4]	
#define _UseBaro		Flags[5]	
#define _BaroAcqTemp	Flags[6]	
#define _OutToggle		Flags[7]			/*cam servos only every 2nd output pulse*/									
#define _UseCh7Trigger	Flags[8]			/*1: don't use Ch7 0: use Ch7 as Cam Roll trim*/
#define _GPSValid		Flags[9]
#define _LostModel		Flags[10]
#define _Armed			Flags[11]							
#define _MotorsEnabled	Flags[12]
#define _HoldingStation	Flags[13]
#define _Hold			Flags[14]

// Mask Bits of ConfigParam
#define FlyCrossMode 	IsSet(ConfigParam,0)
#define FutabaMode		IsSet(ConfigParam,1)
#define IntegralTest	IsSet(ConfigParam,2)
#define DoubleRate		IsSet(ConfigParam,3)	
#define NegativePPM		IsSet(ConfigParam,4)
#define CompassTest		IsSet(ConfigParam,5)

// ------------------------------------------------------------------------------------

// Navigation

#define RETURN_ALT	50

// Attitude

// Max attitude angles in attitude angle units for now 500 -> ~45 deg
#define MAX_ROLL	200	
#define	MAX_PITCH	200

// compass sensor
#define COMPASS_MAXDEV		30					/* maximum yaw compensation of compass heading */
#define COMPASS_MAX		240					/* means 360 degrees */
#define COMPASS_INVAL		(COMPASS_MAX+15)	/* 15*4 cycles to settle */
#define COMPASS_MIDDLE	10					/* yaw stick neutral dead zone */

// Timer constants in MilliSec

#define THROTTLE_TIMEOUT		2000
#define ROC_INTERVAL			500		
#define FAILSAFE_TIMEOUT		1500
#define FAILSAFE_CANCEL			1000

#define GPS_TIMEOUT				3000

#define AUTONOMOUS_TIMEOUT		5000
#define AUTONOMOUS_CANCEL		10000

// Serial I/O

#define _ClkOut		(160/4)								// 16.0 MHz quartz

#define _B9600		(_ClkOut*100000/(4*9600) - 1)
#define _B19200		(_ClkOut*100000/(4*19200) - 1)
#define _B38400		(_ClkOut*100000/(4*38400) - 1)
#define _B115200	(_ClkOut*104000/(4*115200) - 1)
#define _B230400	(_ClkOut*100000/(4*115200) - 1)

#define RXBUFFMASK 127 

// Filter selection

#define GyroFilter SoftFilter
#define AccelerometerFilter SoftFilter
#define	RCFilter NoFilter

// RC

#define RC_MIN_CHANNELS	7

#define RC_MINIMUM	0
#define RC_MAXIMUM	238
#define RC_NEUTRAL	((RC_MAXIMUM-RC_MINIMUM+1)/2)

#define RC_THRES_STOP	((5*RC_MAXIMUM+50)/100)  /* % */
#define RC_THRES_START	((10*RC_MAXIMUM+50)/100) /* % */

#define RC_FRAME_TIMEOUT 25
#define RC_SIGNAL_TIMEOUT (5*RC_FRAME_TIMEOUT)
#define RC_THR_MAX 		RC_MAXIMUM

// Motors

#ifdef ESC_PWM
#define	OUT_MINIMUM 	1				/* must be at least 1 */									
#define OUT_MAXIMUM 	165				/* 1.974mS - must be less than 2mS */
#endif

#ifdef ESC_X3D
#define OUT_MINIMUM		0
#define OUT_MAXIMUM		255
#endif

#ifdef ESC_HOLGER
#define OUT_MINIMUM		0
#define OUT_MAXIMUM		225	/* ESC demanded */
#endif

#ifdef ESC_YGEI2C
#define OUT_MINIMUM		0
#define OUT_MAXIMUM		240	/* ESC demanded */
#endif

#define OUT_NEUTRAL  ((OUT_MAXIMUM-OUT_MINIMUM+1)/2)

// ------------------------------------------------------------------------------------

// Accelerometers
// LIS3LV02DQ Inertial Sensor (Accelerometer)
//  Ax=> Y + right
//	Ay=> Z + up
//	Az=> X + back

#define ACCSIGN_X	(1)
#define ACCSIGN_Y	(-1)
#define ACCSIGN_Z	(-1)

// Gyros

// The following is to attempt to unify the coordinate system
// to Right Hand Cartesian
//	+X forward
//	+Y right
//	+Z down

//	+P roll right
//	+Q pitch up
//	+R yaw clockwise

// Controls 
//  Aileron + roll right
//	Elevator + pitch up
//	Rudder + yaw anti-clockwise


// IDG300
// 3.3V Reference +-500 Deg/Sec
// 0.4882815 Deg/Sec/LSB 
//	+ roll right
//	+ pitch down
//	+ yaw clockwise

// ADRSX300 Yaw
// 5V Reference +-300 Deg/Sec
// 0.2926875 Deg/Sec/LSB 
//	+ roll left
//	+ pitch down
//	+ yaw clockwise

// ADXRS150
// 5V Reference +-150 Deg/Sec
// 0.146484375 Deg/Sec/LSB

// Gyro 

#define ADCPORTCONFIG 0b00001010 // AAAAA
#ifdef OPT_IDG
	#define MAXDEGSEC_PITCHROLL 	500
	#define ADCEXTVREF_PITCHROLL 	1
	#define	GYROSIGN_ROLL 			(1)
	#define	GYROSIGN_PITCH 			(-1)
#else
#ifdef OPT_ADXRS150
	#define MAXDEGSEC_PITCHROLL 	150
	#define ADCEXTVREF_PITCHROLL 	0
	#define	GYROSIGN_ROLL 			(-1)
	#define	GYROSIGN_PITCH 			(-1)
#else // OPT_ADXRS300
	#define MAXDEGSEC_PITCHROLL 	300
	#define ADCEXTVREF_PITCH 		0
	#define	GYROSIGN_ROLL 			(-1)
	#define	GYROSIGN_PITCH 			(-1)
#endif
#endif

#define ADCEXTVREF_YAW 		0
#define	GYROSIGN_YAW 		(1)
#define ADCVREF5V 			0

// ------------------------------------------------------------------------------------

// Sanity checks - please leave them as they are!

// check the PPM RC values
#if OUT_MINIMUM >= OUT_MAXIMUM
#error RC_MINIMUM < OUT_MAXIMUM!
#endif
#if RC_THRES_START <= RC_THRES_STOP
#error RC_THRES_START <= RC_THRES_STOP!
#endif

// Check the PWM Motor values
#if (OUT_MAXIMUM < OUT_NEUTRAL)
#error OUT_MAXIMUM < OUT_NEUTRAL !
#endif

// for Drive saturation
#if (OUT_MAXIMUM > 255)
#error OUT_MAXIMUM > 255 (uint8) !
#endif

#if (OUT_MAXIMUM < 0)
#error OUT_MAXIMUM < 0 (uint8) !
#endif

#if (OUT_MAXIMUM > 255)
#error OUT_MAXIMUM > 255 (uint8) !
#endif

#if (OUT_MINIMUM < 0)
#error OUT_MINIMUM < 0 (uint8) !
#endif

// check gyro model
#if defined OPT_ADXRS150 + defined OPT_ADXRS300 + defined OPT_IDG != 1
#error Define only ONE out of OPT_ADXRS150 OPT_ADXRS300 OPT_IDG
#endif

// check ESC model
#if defined ESC_PWM + defined ESC_X3D + defined ESC_HOLGER + defined ESC_YGEI2C != 1
#error Define only ONE out of ESC_PWM ESC_X3D ESC_HOLGER ESC_YGEI2C
#endif

// check RX model
#if defined RX_DEFAULT + defined RX_PPM + defined RX_DSM2 != 1
#error Define only ONE out of RX_DEFAULT RX_PPM RX_DSM2
#endif

// check debug settings
#if defined DEBUG_MOTORS + defined DEBUG_SENSORS > 1
#error Define only ONE or NONE out of DEBUG_MOTORS DEBUG_SENSORS
#endif
// end of sanity checks
#if defined DEBUG_MOTORS + defined DEBUG_SENSORS > 0
#define DEBUG
#endif

// ------------------------------------------------------------------------------------

// I2C

#define	I2C_ACK		0
#define	I2C_NACK	1

// ------------------------------------------------------------------------------------

#define Version	"EXP-18F2520"

// Prototypes

// ADC.c
extern int16 ADC(uint8, uint8);
extern void InitADC(void);

// autonomous.c
extern void AcquireSatellites(void);
extern void DoAutonomous(void);
extern void ResetTimeOuts(void);
extern uint8 RCLinkRestored(int24);
extern void HoldStation(void);

// compass_altimeter.c
extern void InitDirection(void);
extern void GetDirection(void);
extern void InitBarometer(void);
extern void GetBaroAltitude(void);

// control.c
extern void InitAttitude(void);
extern void UpdateControls(void);
extern void DoControl(void);
extern void InitControl(void);
extern void AltitudeHold(int16);

// gps.c
extern void InitGPS(void);
extern void ParseGPSSentence(void);
extern void UpdateGPS(void);
extern void GPSAltitudeHold(int16);
extern void GPSHeadingHold(int16);

// i2c.c
extern void I2CStart(void);
extern void I2CStop(void);
extern uint8 SendI2CByte(uint8);
extern uint8 RecvI2CByte(uint8);

// irq.c
extern void InitTimersAndInterrupts(void);
extern uint24 mSClock(void);
extern void MapRC(void);

// outputs.c
extern void InitOutputs(void);
extern void OutSignals(void);
extern void CheckFrameOverrun(void);
extern void DoPWMFrame(void);

// serial.c
extern void ProcessCommand(void);
extern void ShowSetup(uint8);
extern uint8 RxChar(void);
extern uint8 PollRxChar(void);
extern int16 RxNumS(void);
extern uint8 RxNumU(void);
extern void TxValH16(uint16);
extern void TxValH(uint8);
extern void TxChar(uint8);
extern void TxNextLine(void);
extern void TxValU(uint8);
extern void TxValS(int8);

// spi.c
extern void IsLISLactive(void);
extern void ReadAccelerations(void);

// utils.c
extern int24 SRS24(int24, uint8);
extern int16 SRS16(int16, uint8);
extern void CheckAlarms(void);
extern void ReadParametersEE(void);
extern void WriteParametersEE(uint8);
extern 	void WriteEE(uint8, int8);
extern 	int8 ReadEE(uint8);
extern void SendLeds(void);
extern 	void SwitchLedsOn(uint8);
extern void SwitchLedsOff(uint8);
extern void LEDGame(void);
extern void DoDebugTraces(void);
extern void Delay100mSec(uint8);
extern void InitPorts(void);
extern int16 int16atan2(int16, int16);
extern int16 int16sin(int16);
extern int16 int16cos(int16);

// Variables
									
enum States {Initialising, Landed, Flying, Hold, Failsafe, Autonomous};
enum {Clock,  UpdateTimeout, RCSignalTimeout, AlarmUpdate, ThrottleClosed, ROCUpdate, FailsafeTimeout, 
      AutonomousTimeout, GPSTimeout, BaroUpdate, CompassUpdate};	
enum RCControls {ThrottleC, RollC, PitchC, YawC, ParamC, CamTiltC, CamRollC}; 
#define CONTROLS  (CamRollC+1)

extern uint8	State;									// Quadrocopter current State

extern uint24 mS[CompassUpdate+1];

extern uint16 DummyGPSTimeout;

// RC
extern const 	uint8	Map[2][CONTROLS];
extern int16	RC[CONTROLS];
							
// PWM
extern uint8 	SHADOWB, MF, MB, ML, MR, MT, ME;
extern uint8 	volatile  EmitPWMFrame;

// Gyros
extern int24	RollAngle, PitchAngle, YawAngle;		// PID integral (angle)

// Acceleration Corrections
extern int16	Ax, Ay, Az;								// LISL sensor accelerations								
extern int16	NeutralLR, NeutralFB, NeutralUD;		// LISL scaled neutral values
extern int16	Vud;					

// PID
extern int24	Rl, Pl, Yl;								// PID output values to motors
extern int16	YE, HE;							
extern int16	DesiredThrottle, HoverThrottle, DesiredAltitude;
extern int16	DesiredRoll, DesiredPitch;
extern int16 	DesiredRollRate, DesiredPitchRate;
extern int16	DesiredYaw, DesiredYawRate;
		
// Altitude
extern int16	OriginBaroAltitude, CurrAltitude, CurrBaroTemp;
extern int16	SumAE;
extern int16	Valt;

// Compass
extern int16	AbsDirection;							// desired heading (240 = 360 deg)
extern int16	CompassHeading;						
extern int16	CurrDeviation;							// deviation from correct heading

// GPS
extern boolean 	GPSSentenceReceived; 
extern int16 	GPSNorth, GPSEast, GPSNorthHold, GPSEastHold;
extern int16 	GPSHeading, GPSAltitude, GPSOriginAltitude, GPSGroundSpeed;

// Serial I/O
extern uint8 	RxCheckSum, RxHead, RxTail;
extern uint8 	RxBuff[RXBUFFMASK+1];

// Misc
extern uint8	Flags[32];										
extern uint8	LedShadow;								// shadow register

#ifdef DEBUG_SENSORS

extern int16    CompassReading;
extern int16	RE, PE, YE, BE;					
extern int16	SumRE, SumPE, SumYE, SumBE;
extern int16	RollRate, PitchRate, YawRate;			
extern uint8	MCamPitch, MCamRoll;

#endif // DEBUG_SENSORS

extern int8	CompassKp;
extern int8	BaroTempCoeff;
extern int8	ConfigParam;
extern int8	RollKp;
extern int8	AltitudeKp;
extern int8	AltitudeKd;
extern int8	CamKi;
extern int8	MotorLowRun;
extern int8	LowVoltThres;

#define FirstProgReg RollKp
#define	LastProgReg AltitudeKd

// EEPROM parameter set addresses
#define _EESet1	0		// first set starts at address 0
#define _EESet2	32	// second set starts at address 32

// UAVX.h

