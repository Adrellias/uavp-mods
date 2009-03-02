#define COMMISSIONING

// EXPERIMENTAL

// reduces the update rate and the additionally the descent rate 
#define SLOW_BARO

// Reinstated vertical acceleration compensation 
#define ACCEL_VUD

// Attempts reconnection to the barometer if there is an I2C error
// I2C comms seems now to be reliable with the BMP085 baro.
//#define BARO_RETRY

// Increase the severity of the filter on the baromater pressure readings
// may give better altitude hold ( New=(Old*7+New+4)/8) ).
#define BARO_HARD_FILTER

// Make a "scratchy" beeper noise while altitude hold is engaged.
#define BARO_SCRATCHY_BEEPER



#include <p18cxxx.h> 
#include <math.h>
#include <delays.h>
#include <timers.h>
#include <usart.h>
#include <capture.h>
#include <adc.h>

// Types

#define true 1
#define false 0

#define MAXINT32 0x7fffffff;
#define	MAXINT16 0x7fff;

typedef unsigned char uns8 ;
typedef signed char int8;
typedef unsigned int uns16;
typedef int int16;
typedef long int32;
typedef unsigned long uint32;
typedef short long int24;
typedef unsigned short long uint24;
typedef float real32;
typedef uns8 boolean;

#define Set(S,b) 		((uns8)(S|=(1<<b)))
#define Clear(S,b) 		((uns8)(S&=(~(1<<b))))
#define IsSet(S,b) 		((uns8)((S>>b)&1))
#define IsClear(S,b) 	((uns8)(!(S>>b)&1))
#define Invert(S,b) 	((uns8)(S^=(1<<b)))

#define Max(i,j) 		((i<j) ? j : ((i>j) ? j : i))
#define Limit(i,l,u) 	((i<l) ? l : ((i>u) ? u : i))
#define DecayBand(i,l,u,d) 	((i<l) ? i+d : ((i>u) ? i-d : i))
#define Decay(i) 		((i<0) ? i+1 : ((i>0) ? i-1 : 0))

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

#define DisableInterrupts (INTCONbits.GIEH=0)
#define EnableInterrupts (INTCONbits.GIEH=1)

#ifndef BATCHMODE
// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
// =             Copyright (c) 2007 Ing. Wolfgang Mahringer              =
// =           Extensively modified 2008-9 by Prof. Greg Egan            =
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

// C-Ufo Header File

// ==============================================
// == Global compiler switches
// ==============================================

// use this to allow debugging with MPLAB's Simulator
// #define SIMU
//
// CAUTION!
// the board version MUST be selected by command line!
// ----------------------------------------------------

//
// if you want the led outputs for test purposes
//
// To enable output of values via Out(), OutG() and OutSSP()
//#define DEBUGOUT
//#define DEBUGOUTG
//#define DEBUGSSP
//#define DEBUG_RXERRORS

// Only one of the following 3 defines must be activated:
// When using 3 ADXRS300 gyros
//#define OPT_ADXRS300

// When using 3 ADXRS150 gyros
#define OPT_ADXRS150

// When using 1 ADXRS300 and 1 IDG300 gyro
//#define OPT_IDG

// Select what speeed controllers to use:
// to use standard PPM pulse
//#define ESC_PPM
// to use X3D BL controllers (not yet tested. Info courtesy to Jast :-)
//#define ESC_X3D
// to use Holgers ESCs (tested and confirmed by ufo-hans)
#define ESC_HOLGER
// to use YGE I2C controllers (for standard YGEs use ESC_PPM)
//#define ESC_YGEI2C

// defined: serial PPM pulse train from receiver
//#define RX_PPM

// defined: Spektrum DSM2 various channel shuffles
#define RX_DSM2

// defined: Interleaved odd channel PPM pulse train from receiver
//#define RX_DEFAULT

// uncomment this to enable Tri-Copter Mixing.
// connector K1 = front motor
//           K2 = rear left motor 
//           K3 = rear right motor 
//           K4 = yaw servo output
// Camera controlling can be used!
//#define TRICOPTER

// PCB mounted 45 deg rotated, "VORNE" points between FL and FR
// special version for Willi Pirngruber :-)
//#define MOUNT_45

// Debugging output of the 4 motors on RS232
// When throttling up, every cycle  4 numbers are output
// on the com port:
// <front>;<rear>;<left>;<right> CR LF
// these are the throttle value to the 4 speed controllers
// log them with a terminal proggie to make nice charts :-)
// Reg. 26 controls if motors should run or not
// bit 0 = front, 1 = rear, 2 = left, 3 = right
// if a bit is 1 motor is always stopped, only comport output
// if a bit is 0 motor is running as normal.
// so you can test individual axis easily!
// Reg. 26 bit 4: 1=serial data output on
// NOTE: enable DEBUG_MOTORS only on 3.1 boards (black PCB)!
//#define DEBUG_MOTORS

// special mode for sensor data output (with UAVPset)
//#define DEBUG_SENSORS

// Switched Roll and Pitch channels for Conrad mc800 transmitter
//#define EXCHROLLNICK

// internal test switch...DO NOT USE FOR REAL UFO'S!
//#define INTTEST

#endif // !BATCHMODE

// Enable this to use the Accelerator sensors
#define USE_ACCSENS

// =====================================
// end of user-configurable section!
// =====================================

// this enables common code for all ADXRS gyros
// leave this untouched!
#if defined OPT_ADXRS300 || defined OPT_ADXRS150
#define OPT_ADXRS
#endif

// this enables common code for debugging purposes
#if defined DEBUGOUT || defined DEBUGOUTG || defined DEBUGSSP
#define DEBUG
#endif

#define Version	"3.15m3_18f"

// ==============================================
// == External variables
// ==============================================

extern	 uns8	IGas;
extern	 int8 	IRoll,IPitch,IYaw;
extern	 uns8	IK5,IK6,IK7;

extern	 int8	RE, PE;
extern	 int8	YE;
extern	 int8	REp,PEp;
extern	 int8	YEp;
extern	 int16	YawSum;
extern	 int16	PitchSum, RollSum;
extern	 uns16	RollSamples, PitchSamples;
extern	 int8	LRIntKorr, FBIntKorr;
extern	 uns8	NeutralLR, NeutralFB, NeutralUD;
extern	 int8 	UDSum;
extern	 uns8	BlinkCount, BlinkCycle, BaroCount;
extern	 int8	Rw,Pw;	// angles
extern   int8	BatteryVolts; 
				
// Variables for barometric sensor PD-controller
extern	 uns16	BaroBasePressure, BaroBaseTemp;
extern	 uns16	BaroRelTempCorr;
extern	 int8	VBaroComp;
extern   int8    BaroRelPressure;
extern	 uns8	BaroType, BaroTemp, BaroRestarts;

// Die Reihenfolge dieser Variablen MUSS gewahrt bleiben!!!!
// These variables MUST keep their order!!!

extern	 int8	RollPropFactor; 	// 01
extern	 int8	RollIntFactor;		// 02
extern	 int8	RollDiffFactor;		// 03
extern	 int8 	BaroTempCoeff;		// 04
extern	 int8	RollIntLimit;		// 05
extern	 int8	PitchPropFactor;	// 06
extern	 int8	PitchIntFactor;		// 07
extern	 int8	PitchDiffFactor;	// 08
extern	 int8 	BaroThrottleProp;	// 09
extern	 int8	PitchIntLimit;		// 10
extern	 int8	YawPropFactor; 		// 11
extern	 int8	YawIntFactor;		// 12
extern	 int8	YawDiffFactor;		// 13
extern	 int8	YawLimit;			// 14
extern	 int8 	YawIntLimit;		// 15
extern	 int8	ConfigParam;		// 16
extern	 int8 	TimeSlot;			// 17
extern	 int8	LowVoltThres;		// 18
extern	 int8	CamRollFactor;		// 19
extern	 int8	LinFBIntFactor;		// 20 free
extern	 int8	LinUDIntFactor;		// 21
extern	 int8 	MiddleUD;			// 22
extern	 int8	MotorLowRun;		// 23
extern	 int8	MiddleLR;			// 24
extern	 int8	MiddleFB;			// 25
extern	 int8	CamPitchFactor;		// 26
extern	 int8	CompassFactor;		// 27
extern	 int8	BaroThrottleDiff;	// 28

// these 2 dummy registers (they do not occupy any RAM location)
// are here for defining the first and the last programmable 
// register in a set

#define FirstProgReg RollPropFactor
#define	LastProgReg BaroThrottleDiff


// end of "order-block"

extern	 uns8	MFront,MLeft,MRight,MBack;	// output channels
extern	 uns8	MCamRoll,MCamPitch;
extern	 int16	Ml, Mr, Mf, Mb;
extern	 int16	Rl,Pl,Yl;	// PID output values
extern	 int16	Rp,Pp,Yp,Vud;


extern	 uns8	Flags[8];
extern	 uns8	Flags2[8];
extern	 uns8	RecFlags;	// Interrupt save registers for FSR

extern	 uns8	IntegralCount;

// measured neutral gyro values
// current stick neutral values
extern	 int8	RollNeutral, PitchNeutral, YawNeutral;
extern	 uns8	ThrNeutral;
extern	 uns16	ThrDownCount;
extern	uns8 mSTick;
extern	 uns16	MidRoll, MidPitch, MidYaw;

extern	 uns8	LedShadow;	// shadow register
extern	 uns16	AbsDirection;	// wanted heading (240 = 360 deg)
extern	 int8		CurDeviation;	// deviation from correct heading

#define _ClkOut		(160/4)	/* 16.0 MHz quartz */
#define _PreScale0	16	/* 1:16 TMR0 prescaler */
#define _PreScale1	8	/* 1:8 TMR1 prescaler */
#define _PreScale2	16
#define _PostScale2	16

// wegen dem dummen Compiler muss man h�ndisch rechnen :-(

#define DELAY_MS(ms)	for( mSTick=ms; mSTick!=0; mSTick--){INTCONbits.TMR0IF=0; while(INTCONbits.TMR0IF == 0);}

//#define TMR2_9MS	(9000*_ClkOut/(10*_PreScale2*_PostScale2))
//#define TMR2_9MS	141	/* 2x 9ms = 18ms pause time */
// modified for Spectrum DX6 and DX7
#define TMR2_5MS	78	/* 1x 5ms +  */
#define TMR2_14MS	234	/* 1x 15ms = 20ms pause time */


//                    RX impuls times in 10-microseconds units
//                    vvv   ACHTUNG: Auf numerischen �berlauf achten!
#ifdef ESC_PPM
#define	_Minimum	((105* _ClkOut/(2*_PreScale1))&0xFF)	/*-100% */
#define _Maximum	240					/* reduced from 255 */
#endif
#ifdef ESC_X3D
#define _Minimum	0
#define _Maximum	255
#endif
#ifdef ESC_HOLGER
#define _Minimum	0
#define _Maximum	225	/* ESC demanded */
#endif
#ifdef ESC_YGEI2C
#define _Minimum	0
#define _Maximum	240	/* ESC demanded */
#endif

#define _Neutral	((150* _ClkOut/(2*_PreScale1))&0xFF)    /*   0% */
#define _ThresStop	((113* _ClkOut/(2*_PreScale1))&0xFF)	/*-90% ab hier Stopp! */
#define _ThresStart	((116* _ClkOut/(2*_PreScale1))&0xFF)	/*-85% ab hier Start! */
#define _ProgMode	((160* _ClkOut/(2*_PreScale1))&0xFF)	/*+75% */
#define _ProgUp		((150* _ClkOut/(2*_PreScale1))&0xFF)	/*+60% */
#define _ProgDown	((130* _ClkOut/(2*_PreScale1))&0xFF)	/*-60% */

// Sanity checks
//
// please leave them as they are!

// check the PPM RX and motor values
#if _Minimum >= _Maximum
#error _Minimum < _Maximum!
#endif
#if _ThresStart <= _ThresStop
#error _ThresStart <= _ThresStop!
#endif
#if (_Maximum < _Neutral)
#error Maximum < _Neutral !
#endif

// check gyro model
#if defined OPT_ADXRS150 + defined OPT_ADXRS300 + defined OPT_IDG != 1
#error Define only ONE out of OPT_ADXRS150 OPT_ADXRS300 OPT_IDG
#endif

// check ESC model
#if defined ESC_PPM + defined ESC_X3D + defined ESC_HOLGER + defined ESC_YGEI2C != 1
#error Define only ONE out of ESC_PPM ESC_X3D ESC_HOLGER ESC_YGEI2C
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

#define MAXDROPOUT	200	// max 200x 20ms = 4sec. dropout allowable

// Counter for flashing Low-Power LEDs
#define BLINK_LIMIT 100	// should be a number dividable by 4!
#define BLINK_CYCLES 8

// Parameters for UART port

#define _B9600		(_ClkOut*100000/(4*9600) - 1)
#define _B19200		(_ClkOut*100000/(4*19200) - 1)
#define _B38400		(_ClkOut*100000/(4*38400) - 1)
#define _B115200	(_ClkOut*104000/(4*115200) - 1)
#define _B230400	(_ClkOut*100000/(4*115200) - 1)

// EEPROM parameter set addresses

#define _EESet1	0		// first set starts at address 0x00
#define _EESet2	0x20	// second set starts at address 0x20

// Prototypes

extern	 void BootStart(void);

extern	 void OutSignals(void);
extern	 void GetGyroValues(void);
extern	 void CalcGyroValues(void);
extern	 void CheckBattery(void);
extern	 void CheckAlarms(void);
extern	 void UpdateBlinkCount(void);
extern	 void SendComText(const char *);
extern	 void SendComValH(uns8);
extern	 void SendComChar(char);
extern	 void ShowSetup(uns8);
extern	 void ProcessComCommand(void);
extern	 void SendComValU(uns8);
extern	 void SendComValS(uns8);
extern	 void SendComValH16(uns16);
extern	 void GetEvenValues(void);
extern	 void ReadParametersEE(void);
extern	 void WriteParametersEE(uns8);
extern	 void WriteEE(uns8, int8);
extern	 void ReadParametersEE(void);
extern	 int8 ReadEE(uns8);
extern	 void DoProgMode(void);
extern	 void InitArrays(void);
extern   void PID(void);
extern	 void Out(uns8);
extern	 void OutG(uns8);
extern	 void LimitRollSum(void);
extern	 void LimitPitchSum(void);
extern	 void LimitYawSum(void);
extern	 void AddUpLRArr(uns8);
extern	 void AddUpFBArr(uns8);
extern	 void AcqTime(void);
extern	 void MixAndLimit(void);
extern	 void MixAndLimitCam(void);
extern	 void Delay100mSWithOutput(uns8);

extern	 void SendLeds(void);
extern	 void SwitchLedsOn(uns8);
extern	 void SwitchLedsOff(uns8);

extern	 void CheckLISL(void);
extern	 void IsLISLactive(void);
extern 	 uns8 ReadLISL(uns8);
extern 	 uns8 ReadLISLNext(void);
extern	 void OutSSP(uns8);
extern	 void InitDirection(void);
extern	 void GetDirection(void);
extern	 void InitAltimeter(void);
extern	 void ComputeBaroComp(void);
//extern	 uns8 StartBaroADC(uns8);

extern	 uns8 Sin(void);
extern	 uns8 Cos(void);
extern	 uns8 Arctan(uns8);

extern	void nop2(void);
extern	int16 SRS16(int16, uns8);

extern	 void MatrixCompensate(void);

// End of c-ufo.h

