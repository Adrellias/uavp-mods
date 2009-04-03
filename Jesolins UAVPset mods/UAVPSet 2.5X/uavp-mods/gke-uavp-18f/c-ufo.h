// EXPERIMENTAL

// Accelerometer

// Enable vertical acclerometer compensation of vertical velocity 
#define ENABLE_VERTICAL_VELOCITY_DAMPING

// Gyros

// Enable "Dynamic mass" compensation Roll and/or Pitch
// Normally enabled for Roll only 
#define ENABLE_DYNAMIC_MASS_COMP_ROLL
#define ENABLE_DYNAMIC_MASS_COMP_PITCH

// Barometer

// Reduces the update rate and the additionally the descent rate 
//#define ENABLE_NEW_ALT_HOLD

// Modifications which have been adopted are included BELOW.

#ifndef BATCHMODE
// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =     Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer       =
// =                          http://www.uavp.org                        =
// =======================================================================

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

#define CLOCK_16MHZ

// Only one of the following 3 defines must be activated:
// When using 3 ADXRS300 gyros
//#define OPT_ADXRS300

// When using 3 ADXRS150 gyros
//#define OPT_ADXRS150

// When using 1 ADXRS300 and 1 IDG300 gyro
#define OPT_IDG

// Select what speeed controllers to use:
// to use standard PPM pulse
#define ESC_PPM
// to use X3D BL controllers (not yet tested. Info courtesy to Jast :-)
//#define ESC_X3D
// to use Holgers ESCs (tested and confirmed by ufo-hans)
// #define ESC_HOLGER
// to use YGE I2C controllers (for standard YGEs use ESC_PPM)
//#define ESC_YGEI2C

// defined: serial PPM pulse train from receiver
//#define RX_PPM

// defined: Spektrum DSM2 various channel shuffles
//#define RX_DSM2

// defined: Interleaved odd channel PPM pulse train from receiver
#define RX_DEFAULT

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

// special mode for sensor data output (with UAVPset)
//#define DEBUG_SENSORS

#endif // !BATCHMODE

// Adopted extensions or major modifications to original version

// Baro

// Attempts reconnection to the barometer if there is an I2C error
// I2C comms seems now to be reliable with the BMP085 baro.
//#define BARO_RETRY

// Increase the severity of the filter on the barometer pressure readings
// may give better altitude hold ( New=(Old*7+New+4)/8) ).
#define BARO_HARD_FILTER

// Make a "scratchy" beeper noise while altitude hold is engaged.
// A better method is to use the auxilliary LEDs - see Manual.
#define BARO_SCRATCHY_BEEPER

// Accelerometers

// Enable this to use the Accelerometer sensor 
// Usually ENABLED
#define USE_ACCELEROMETER

// Gyros


// Misc

// Loads a "representative" parameter set into EEPROM
//#define INIT_PARAMS

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
// == Additional declarations etc for C18 port
// ==============================================

#include <p18cxxx.h> 
#include <math.h>
#include <delays.h>
#include <timers.h>
#include <usart.h>
#include <capture.h>
#include <adc.h>

// Types

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
#define true 1
#define false 0

#define MAXINT32 0x7fffffff;
#define	MAXINT16 0x7fff;

typedef unsigned char uint8 ;
typedef signed char int8;
typedef unsigned int uint16;
typedef int int16;
typedef short long int24;
typedef unsigned short long uint24;
typedef long int32;
typedef unsigned long uint32;

#define Set(S,b) 		((uint8)(S|=(1<<b)))
#define Clear(S,b) 		((uint8)(S&=(~(1<<b))))
#define IsSet(S,b) 		((uint8)((S>>b)&1))
#define IsClear(S,b) 	((uint8)(!(S>>b)&1))
#define Invert(S,b) 	((uint8)(S^=(1<<b)))

#define Abs(i)			((i<0) ? -i : i)	

#define Max(i,j) 		((i<j) ? j : i)
#define Min(i,j) 		((i<j) ? i : j )
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
#define InterruptsEnabled (INTCONbits.GIEH)

// ADC Channels
#define ADCPORTCONFIG 0b00001010 // AAAAA
#define ADCBattVoltsChan 	0 

#ifdef OPT_ADXRS
	#define ADCRollChan 	1
	#define ADCPitchChan 	2
#else // OPT_IDG
	#define ADCRollChan 	2
	#define ADCPitchChan 	1
#endif // OPT_ADXRS

#define ADCVRefChan 		3 
#define ADCYawChan			4

#define ADCVREF5V 			0
#define ADCVREF 			1

// ==============================================
// == External variables
// ==============================================

enum TraceTags {TAbsDirection,TVBaroComp,TBaroRelPressure,				TRollRate,TPitchRate,TYE,				TRollSum,TPitchSum,TYawSum,
				TAx,TAz,TAy,
				TUDSum, TVud,
				TIGas,
				TIRoll, TIPitch, TIYaw,
				TMFront, TMBack, TMLeft, TMRight,
				TMCamRoll, TMCamPitch,
				LastTrace
				};
#define TopTrace TMCamPitch

enum MotorTags {Front, Left, Right, Back};
#define NoOfMotors 4

extern uint8	IGas;
extern int8 	IRoll,IPitch,IYaw;
extern uint8	IK5,IK6,IK7;

extern int16	RE, PE, YE;
extern int16	REp,PEp,YEp;
extern int16	PitchSum, RollSum, YawSum;
extern int16	RollRate, PitchRate;
extern int16	AverageYawRate, YawRate;
extern int16	GyroMidRoll, GyroMidPitch, GyroMidYaw;
extern int16	Ax, Ay, Az;
extern int8		LRIntKorr, FBIntKorr;
extern int8		NeutralLR, NeutralFB, NeutralUD;
extern int16 	UDSum;

// Failsafes
extern uint8	ThrNeutral;
			
// Variables for barometric sensor PD-controller
extern int24	BaroBasePressure, BaroBaseTemp;
extern int16	BaroRelPressure, BaroRelTempCorr;
extern uint16	BaroVal;
extern int16	VBaroComp;
extern uint8	BaroType, BaroTemp, BaroRestarts;

extern uint8	MCamRoll,MCamPitch;
extern int16	Motor[NoOfMotors];
extern int16	Rl,Pl,Yl;	// PID output values
extern int16	Rp,Pp,Yp,Vud;

extern uint8	Flags[16];

extern int16	IntegralCount, ThrDownCount, DropoutCount, LedCount, BlinkCount, BlinkCycle, BaroCount;
extern uint24	RCGlitchCount;
extern int8		Rw,Pw;	// angles
extern int8		BatteryVolts; 

extern uint8	LedShadow;	// shadow register
extern int16	AbsDirection;	// wanted heading (240 = 360 deg)
extern int16	CurDeviation;	// deviation from correct heading

#ifdef DEBUG_SENSORS
extern int16	Trace[LastTrace];
#endif // DEBUG_SENSORS

// Principal quadrocopter parameters - MUST remain in this order
// for block read/write to EEPROM
extern int8	RollPropFactor; 	// 01
extern int8	RollIntFactor;		// 02
extern int8	RollDiffFactor;		// 03
extern int8 BaroTempCoeff;		// 04
extern int8	RollIntLimit;		// 05
extern int8	PitchPropFactor;	// 06
extern int8	PitchIntFactor;		// 07
extern int8	PitchDiffFactor;	// 08
extern int8 BaroThrottleProp;	// 09
extern int8	PitchIntLimit;		// 10
extern int8	YawPropFactor; 		// 11
extern int8	YawIntFactor;		// 12
extern int8	YawDiffFactor;		// 13
extern int8	YawLimit;			// 14
extern int8 YawIntLimit;		// 15
extern int8	ConfigParam;		// 16
extern int8 TimeSlot;		// 17
extern int8	LowVoltThres;		// 18
extern int8	CamRollFactor;		// 19
extern int8	LinFBIntFactor;		// 20 free
extern int8	LinUDIntFactor;		// 21
extern int8 MiddleUD;			// 22
extern int8	MotorLowRun;		// 23
extern int8	MiddleLR;			// 24
extern int8	MiddleFB;			// 25
extern int8	CamPitchFactor;		// 26
extern int8	CompassFactor;		// 27
extern int8	BaroThrottleDiff;	// 28

// these 2 dummy registers (they do not occupy any RAM location)
// are here for defining the first and the last programmable 
// register in a set

#define FirstProgReg RollPropFactor
#define	LastProgReg BaroThrottleDiff

// end of "order-block"

#ifdef CLOCK_16MHZ
#define _ClkOut		(160/4)	/* 16.0 MHz Xtal */
#else // CLOCK_40MHZ
NOT IMPLEMENTED YET #define _ClkOut		(400/4)	/* 10.0 MHz Xtal * 4 PLL */
#endif

#define _PreScale0	16	/* 1:16 TMR0 prescaler */
#define _PreScale1	8	/* 1:8 TMR1 prescaler */
#define _PreScale2	16
#define _PostScale2	16

#define TMR2_5MS	78	/* 1x 5ms +  */
#define TMR2_14MS	234	/* 1x 15ms = 20ms pause time */

#ifdef ESC_PPM
#define	_Minimum	1 /*((105* _ClkOut/(2*_PreScale1))&0xFF)*/
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

// Sanity checks

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

extern void BootStart(void);

extern void ShowSetup(uint8);
extern void ProcessComCommand(void);
extern void TxNextLine(void);
extern void ShowPrompt(void);
extern void TxText(const uint8 *);
extern void TxString(const rom uint8 *);
extern void TxChar(uint8);
extern void TxValH(uint8);
extern void TxValU(uint8);
extern void TxValS(int8);
extern void TxValH16(uint16);
extern void TxVal32(int32, int8, uint8);
extern void TxNextLine(void);
extern uint8 RxChar(void);
extern uint8 RxNumU(void);
extern int8 RxNumS(void);

extern void ReadParametersEE(void);
extern void WriteParametersEE(int8);
extern void WriteEE(uint8, int8);
extern void ReadParametersEE(void);
extern int8 ReadEE(uint8);

extern void I2CStart(void);
extern void I2CStop(void);extern uint8 SendI2CByte(uint8);
extern uint8 RecvI2CByte(uint8);

extern void OutSSP(uint8);

extern void AccelerationCompensation(void);
extern void IsLISLactive(void);
extern 	uint8 ReadLISL(uint8);
extern 	uint8 ReadLISLNext(void);
extern void ReadAccelerations(void);

extern void InitDirection(void);
extern void GetDirection(void);
extern void InitBarometer(void);
extern void ComputeBaroComp(void);
extern uint8 ReadValueFromBaro(void);
extern uint8 StartBaroADC(uint8);

extern void GetNeutralAccelerations(void);
extern void GetGyroValues(void);
extern void CalcGyroValues(void);
extern void PID(void);
extern void LimitRollSum(void);
extern void LimitPitchSum(void);
extern void LimitYawSum(void);

extern void Out(uint8);
extern void OutG(uint8);
extern void MixAndLimit(void);
extern void MixAndLimitCam(void);
extern void OutSignals(void);

extern void SendLeds(void);
extern void SwitchLedsOn(uint8);
extern void SwitchLedsOff(uint8);
extern void DoPIDDisplays(void);
extern void LedGame(void);

extern void InitADC(void);
extern int16 ADC(uint8, uint8);

extern void InitArrays(void);
extern void InitPorts(void);
extern void Delay100mSWithOutput(int16);
extern void Delay1mS(int16);
extern void UpdateBlinkCount(void);
extern void CheckAlarms(void);
extern int16 SRS16(int16, uint8);

#ifdef DEBUG_SENSORS
extern void DumpTrace(void);
#endif

#ifdef TEST_SOFTWARE

extern int16	TestTimeSlot;
extern uint16	PauseTime;
extern int16 	NewK1, NewK2, NewK3, NewK4, NewK5, NewK6, NewK7;

// Menu strings

extern const uint8  SerHello[];
extern const uint8  SerSetup[];
extern const uint8 SerPrompt[];

extern void AnalogTest(void);extern void DoCompassTest(void);
extern void CalibrateCompass(void);
extern void PowerOutput(int8);
extern void ReceiverTest(void);
extern void TogglePPMPolarity(void);
extern void BaroTest(void);
extern void LinearTest(void);
extern uint8 ScanI2CBus(void);
extern int16 int32sqrt(int32);

#endif // TEST_SOFTWARE

// End of c-ufo.h

