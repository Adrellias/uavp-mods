
// Alternative mix and limit routine due to Gary Stofer

//#define ALT_MIXANDLIMIT


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

// If you want the LED outputs for test purposes
// To enable output of values via Out(), OutG() and OutSSP()
//#define DEBUGOUT
//#define DEBUGOUTG
//#define DEBUGSSP
//#define DEBUG_RXERRORS

// Only one of the following 3 defines must be activated:
// When using 3 ADXRS300 gyros
#define OPT_ADXRS300

// When using 3 ADXRS150 gyros
//#define OPT_ADXRS150

// When using 1 ADXRS300 and 1 IDG300 gyro
//#define OPT_IDG

// Motor controllers
// Select what speeed controllers to use:
// to use standard PPM pulse
#define ESC_PPM
// to use X3D BL controllers (not yet tested. Info courtesy to Jast :-)
//#define ESC_X3D
// to use Holgers ESCs (tested and confirmed by ufo-hans)
//#define ESC_HOLGER
// to use YGE I2C controllers (for standard YGEs use ESC_PPM)
//#define ESC_YGEI2C

// defined: serial PPM pulse train from receiver
// undefined: standard servo pulses from CH1, 3, 5 and 7
#define RX_PPM

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

// Accelerometer

// Enable this to use the Accelerator sensors
#define USE_ACCSENS

// Barometer

// Increase the severity of the filter on the baromater pressure readings
// may give better altitude hold ( New=(Old*7+New+4)/8) ).
#define BARO_HARD_FILTER

// Make a "scratchy" beeper noise while altitude hold is engaged.
//#define BARO_SCRATCHY_BEEPER

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

#define Version	"3.15m3"

// ==============================================
// == External variables
// ==============================================

extern	shrBank	uns8	IGas;
extern	shrBank	int8 	IRoll,IPitch,IYaw;
extern	shrBank	uns8	IK5,IK6,IK7;

extern	bank2	int8	RE, PE;
extern	bank2	int8	YE;
extern	bank1	int8	REp,PEp;
extern	bank1	int8	YEp;
extern	bank2	int16	YawSum;
extern	bank2	int16	PitchSum, RollSum;
extern	bank2	uns16	RollSamples, PitchSamples;
//extern	bank2	int16	LRSum, FBSum, UDSum;
extern	bank2	int8	LRIntKorr, FBIntKorr;
extern	bank2	uns8	NeutralLR, NeutralFB, NeutralUD;
extern	bank2	int8 	UDSum;

//extern	bank1	int16	LRSumPosi, FBSumPosi;
extern	bank1	int8	NegFact; // general purpose

extern	shrBank	uns8	BlinkCount, BlinkCycle, BaroCount;

extern	bank0	int16	niltemp1;
extern	bank0	int8		Rw,Pw;	// angles
extern  bank1	int8	BatteryVolts; // added by Greg Egan
extern	bank1	int16	niltemp;
int8		nitemp @ niltemp;
				
// Variables for barometric sensor PD-controller
extern	bank0	uns16	BaroBasePressure, BaroBaseTemp;
extern	bank0	uns16	BaroRelTempCorr;
extern	bank1	int8	VBaroComp;
extern  bank0	int8    BaroRelPressure;
extern	bank2	uns8	BaroType, BaroTemp, BaroRestarts;

// Die Reihenfolge dieser Variablen MUSS gewahrt bleiben!!!!
// These variables MUST keep their order!!!

extern	bank1	int8	RollPropFactor; 	// 01
extern	bank1	int8	RollIntFactor;		// 02
extern	bank1	int8	RollDiffFactor;		// 03
extern	bank1	int8 RollLimit;			// 04
extern	bank1	int8	RollIntLimit;		// 05
extern	BaroTempCoeff @RollLimit;

extern	bank1	int8	PitchPropFactor;	 	// 06
extern	bank1	int8	PitchIntFactor;		// 07
extern	bank1	int8	PitchDiffFactor;		// 08
extern	bank1	int8 	PitchLimit;			// 09
extern	bank1	int8	PitchIntLimit;		// 10
extern  BaroThrottleProp @PitchLimit;

extern	bank1	int8	YawPropFactor; 	// 11
extern	bank1	int8	YawIntFactor;		// 12
extern	bank1	int8	YawDiffFactor;		// 13
extern	bank1	int8	YawLimit;			// 14
extern	bank1	int8 	YawIntLimit;		// 15

extern	bank1	int8	ConfigParam;		// 16
extern	bank1	int8 	TimeSlot;			// 17
extern	bank1	int8	LowVoltThres;		// 18

extern	bank1	int8	LinLRIntFactor;		// 19 free
extern	bank1	int8	LinFBIntFactor;		// 20 free
extern	bank1	int8	LinUDIntFactor;		// 21
extern	bank1	int8 	MiddleUD;			// 22
extern	bank1	int8	MotorLowRun;		// 23
extern	bank1	int8	MiddleLR;			// 24
extern	bank1	int8	MiddleFB;			// 25
extern	bank1	int8	CamPitchFactor;		// 26
extern	CamRollFactor @LinLRIntFactor;
extern	bank1	int8	CompassFactor;		// 27
extern	bank1	int8	BaroThrottleDiff;	// 28

// these 2 dummy registers (they do not occupy any RAM location)
// are here for defining the first and the last programmable 
// register in a set

int FirstProgReg @RollPropFactor;
int	LastProgReg @BaroThrottleDiff;

// end of "order-block"

extern	bank1	uns8	MFront,MLeft,MRight,MBack;	// output channels
extern	bank1	uns8	MCamRoll,MCamPitch;
extern	bank1	int16	Ml, Mr, Mf, Mb;
extern	bank1	int16	Rl,Pl,Yl;	// PID output values
extern	bank1	int16	Rp,Pp,Yp,Vud;


extern	shrBank	uns8	Flags;
extern	bank0	uns8	Flags2;
extern	shrBank	uns8	RecFlags;	// Interrupt save registers for FSR

extern	shrBank	uns8	IntegralCount;

// measured neutral gyro values
// current stick neutral values
extern	bank2	int8	RollNeutral, PitchNeutral, YawNeutral;
extern	bank2	uns8	ThrNeutral;
extern	bank0	uns16	ThrDownCount;

extern	bank2	uns16	MidRoll, MidPitch, MidYaw;

extern	shrBank	uns8	LedShadow;	// shadow register
extern	bank2	uns16	AbsDirection;	// wanted heading (240 = 360 deg)
extern	shrBank	int8		CurDeviation;	// deviation from correct heading

#define _ClkOut		(160/4)	/* 16.0 MHz quartz */
#define _PreScale0	16	/* 1:16 TMR0 prescaler */
#define _PreScale1	8	/* 1:8 TMR1 prescaler */
#define _PreScale2	16
#define _PostScale2	16

// wegen dem dummen Compiler muss man händisch rechnen :-(

#define DELAY_MS(ms)	for( W=ms; W!=0; W--){T0IF=0;while(T0IF == 0);}

//#define TMR2_9MS	(9000*_ClkOut/(10*_PreScale2*_PostScale2))
//#define TMR2_9MS	141	/* 2x 9ms = 18ms pause time */
// modified for Spectrum DX6 and DX7
#define TMR2_5MS	78	/* 1x 5ms +  */
#define TMR2_14MS	234	/* 1x 15ms = 20ms pause time */

#ifdef ESC_PPM
#define	_Minimum	1
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

// Sanity checks - please leave them as they are!

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

extern	page0	void BootStart(void);

extern	page0	void OutSignals(void);
extern	page0	void GetGyroValues(void);
extern	page0	void CalcGyroValues(void);
extern	page1	void CheckBattery(void);
extern	page1	void CheckAlarms(void);
extern	page1	void UpdateBlinkCount(void);
extern	page3	void SendComValH(uns8);
extern	page3	void SendComChar(char);
extern	page3	void ShowSetup(uns8);
extern	page3	void ProcessComCommand(void);
extern	page3	void SendComValU(uns8);
extern	page3	void SendComValS(uns8);
extern	page1	void GetEvenValues(void);
extern	page2	void ReadEEdata(void);
extern	page2	void DoProgMode(void);
extern	page1	void InitArrays(void);
extern  page1	void PID(void);
extern	page1	void Out(uns8);
extern	page1	void OutG(uns8);
extern	page1	void LimitRollSum(void);
extern	page1	void LimitPitchSum(void);
extern	page1	void LimitYawSum(void);
extern	page1	void AddUpLRArr(uns8);
extern	page1	void AddUpFBArr(uns8);
extern	page1	void AcqTime(void);
extern	page1	void MixAndLimit(void);
extern	page0	void MixAndLimitCam(void);
extern	page1	void Delay100mSWithOutput(uns8);

extern	page1	void SendLeds(void);
extern	page1	void SwitchLedsOn(uns8);
extern	page1	void SwitchLedsOff(uns8);

extern	page2	void CheckLISL(void);
extern	page2	void IsLISLactive(void);
extern 	page2	uns8 ReadLISL(uns8);
extern 	page2	uns8 ReadLISLNext(void);
extern	page2	void OutSSP(uns8);
extern	page3	void InitDirection(void);
extern	page3	void GetDirection(void);
extern	page3	void InitAltimeter(void);
extern	page3	void ComputeBaroComp(void);
//extern	page3	uns8 StartBaroADC(uns8);

extern	page0	uns8 Sin(void);
extern	page0	uns8 Cos(void);
extern	page0	uns8 Arctan(uns8);

extern	page2	void MatrixCompensate(void);

// End of c-ufo.h

