// EXPERIMENTAL

// Simulates a FAKE GPS estimating position from control inputs and outputs to 
// Hyperterm lines of the form: 
// 294      75n     33e    -> r= 33        p= 0 hov rth
// compass   uav posn	       controls		state - in this case hovering and returning home
// This permits quadrocopter to be "flown on the bench". Motors WILL run but this is NOT flight code.
//#define FAKE_GPS

// Navigation

// The "Ls" are important
#define COMPASS_OFFSET_DEG		90L		// North degrees CW from Front
#define MAX_ANGLE 				20L		// Rx stick units ~= degrees
#define CLOSING_RADIUS			20L		// closing radius in metres 

// CAUTION GPS will always exert corrective control 
#define OVERRIDE_HOVER_CONDITION
#define MAX_CONTROL_CHANGE 		10		// new hold point if the roll/pitch stick change more

// JIM reduce or make this zero if you get overshoots
#define NavIntLimit 			4		// integral term for windy conditions! 	

// Accelerometer

// Enable vertical acclerometer compensation of vertical velocity 
//#define ENABLE_VERTICAL_VELOCITY_DAMPING

// Gyros


// Barometer

// Reduces the update rate and the additionally the descent rate 
#define ENABLE_NEW_ALT_HOLD

// Modifications which have been adopted are included BELOW.

#ifndef BATCHMODE
// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =     Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer       =
// =                          http://uavp.ch                             =
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

// ==============================================
// == Global compiler switches
// ==============================================

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
// Normally ENABLED
#define USE_ACCELEROMETER

// Gyros

// Enable "Dynamic mass" compensation Roll and/or Pitch
// Normally enabled for Roll only 
#define ENABLE_DYNAMIC_MASS_COMP_ROLL
#define ENABLE_DYNAMIC_MASS_COMP_PITCH

// Navigation

#define GPS_USE_RMC

// Number of good GPS sentences required to establish origin 
#define INITIAL_GPS_SENTENCES 	30	
	
// Misc

// Loads a "representative" parameter set into EEPROM
//#define INIT_PARAMS

// =====================================
// end of user-configurable section!
// =====================================

// Bit Definitions

// Bit definitions

// when changing, see OutSignal() in outputs.c
#ifdef ESC_PPM
#define	PulseFront		0
#define	PulseLeft		1
#define	PulseRight		2
#define	PulseBack		3
#endif

#if defined ESC_X3D || defined ESC_HOLGER || defined ESC_YGEI2C
// Wolfgang's SW I2C ESC
#define	 ESC_SDA		PORTBbits.RB1
#define	 ESC_SCL		PORTBbits.RB2
#define	 ESC_DIO		TRISBbits.TRISB1
#define	 ESC_CIO		TRISBbits.TRISB2
#endif

#define	PulseCamRoll	4
#define	PulseCamPitch	5

#define ALL_PULSE_ON	PORTB |= 0b00001111
#define ALL_OUTPUTS_OFF	PORTB &= 0b11110000
#define ALL_OUTPUTS		(PORTB & 0b00001111)
#define CAM_PULSE_ON	PORTB |= 0b00110000;

#define Armed		(PORTAbits.RA4)

#define SPI_CS		PORTCbits.RC5
#define SPI_SDA		PORTCbits.RC4
#define SPI_SCL		PORTCbits.RC3
#define SPI_IO		TRISCbits.TRISC4

#define	RD_SPI	1
#define WR_SPI	0

#define DSEL_LISL  1
#define SEL_LISL  0

// the sensor bus lines
#define I2C_SDA			PORTBbits.RB6
#define I2C_DIO			TRISBbits.TRISB6
#define I2C_SCL			PORTBbits.RB7
#define I2C_CIO			TRISBbits.TRISB7

#define	I2C_ACK		((uint8)(0))
#define	I2C_NACK	((uint8)(1))

// The LEDs and the beeper
#define ON	1
#define OFF	0

#define MODELLOSTTIMER		20 	/*in 0,2sec until first beep 
								after lost xmit signal */
#define MODELLOSTTIMERINT	2 	/* in 0,2 sec units
								interval beep when active */
#define LedYellow	LED6
#define LedGreen	LED4
#define	LedBlue		LED2
#define LedRed		LED3
#define LedAUX1		LED5
#define LedAUX2		LED1
#define LedAUX3		LED7

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
#define LedAUX1_ON		SwitchLedsOn(LedAUX1)
#define LedAUX2_ON		SwitchLedsOn(LedAUX2)
#define LedAUX3_ON		SwitchLedsOn(LedAUX3)
#define LedRed_OFF		SwitchLedsOff(LedRed)
#define LedBlue_OFF		SwitchLedsOff(LedBlue)
#define LedGreen_OFF	SwitchLedsOff(LedGreen)
#define LedYellow_OFF	SwitchLedsOff(LedYellow)
#define LedRed_TOG		if( (LedShadow&LedRed) == 0 ) SwitchLedsOn(LedRed); else SwitchLedsOff(LedRed)
#define LedBlue_TOG		if( (LedShadow&LedBlue) == 0 ) SwitchLedsOn(LedBlue); else SwitchLedsOff(LedBlue)
#define LedGreen_TOG	if( (LedShadow&LedGreen) == 0 ) SwitchLedsOn(LedGreen); else SwitchLedsOff(LedGreen)
#define Beeper_OFF		SwitchLedsOff(Beeper)
#define Beeper_ON		SwitchLedsOn(Beeper)
#define Beeper_TOG		if( (LedShadow&Beeper) == 0 ) SwitchLedsOn(Beeper); else SwitchLedsOff(Beeper);

// compass sensor
#define COMPASS_I2C_ID	0x42	/* I2C slave address */
#define COMPASS_MAXDEV	30		/* maximum yaw compensation of compass heading */
#define COMPASS_MAX		240		/* means 360 degrees */
#define COMPASS_INVAL	(COMPASS_MAX+15)	/* 15*4 cycles to settle */
#define COMPASS_MIDDLE	10		/* yaw stick neutral dead zone */

#define COMPASS_TIME	50	/* 20Hz */

// baro (altimeter) sensor
#define BARO_I2C_ID			0xee
#define BARO_TEMP_BMP085	0x2e
#define BARO_TEMP_SMD500	0x6e
#define BARO_PRESS			0xf4
#define BARO_CTL			0xf4
#define BARO_ADC_MSB		0xf6
#define BARO_ADC_LSB		0xf7
#define BARO_TYPE			0xd0
//#define BARO_ID_SMD500		??
#define BARO_ID_BMP085		((uint8)(0x55))

#define BARO_TEMP_TIME	10
#define BARO_PRESS_TIME 35

#define THR_DOWNCOUNT	255		/* 128 PID-cycles (=3 sec) until current throttle is fixed */
#define THR_MIDDLE		10  	/* throttle stick dead zone for baro */
#define THR_HOVER		75		/* min throttle stick for alti lock */

#define	_Signal		Flags[0]	/* if no valid signal is received */
#define	_Flying			Flags[1]	/* UFO is flying */
#define	_NewValues		Flags[2]	/* new RX channel values sampled */
#define _FirstTimeout	Flags[3]	/* is 1 after first 9ms TO expired */

#define _LowBatt		Flags[4]	/* if Batt voltage is low */
#define _UseLISL		Flags[5]	/* 1 if LISL Sensor is used */
#define	_UseCompass		Flags[6]	/* 1 if compass sensor is enabled */
#define _CompassMisRead Flags[7]
#define _UseBaro		Flags[8]	/* 1 if baro sensor active */
#define _BaroTempRun	Flags[9]	/* 1 if baro temp a/d conversion is running */
#define _BaroRestart	Flags[10] 	/* Baro restart required */
#define _OutToggle		Flags[11]	/* cam servos only evers 2nd output pulse */								
#define _UseCh7Trigger	Flags[12]	/* 1: don't use Ch7 */
									/* 0: use Ch7 as Cam Roll trim */
#define _ReceivingGPS 	Flags[16]
#define _GPSValid 		Flags[17]
#define _LostModel		Flags[18]
#define _Hovering		Flags[19]
#define _HoldingStation Flags[20]
#define _NavComputed 	Flags[21]


// Mask Bits of ConfigParam
#define FlyCrossMode 	IsSet(ConfigParam,0)
#define FutabaMode		IsSet(ConfigParam,1)
#define IntegralTest	IsSet(ConfigParam,2)
#define DoubleRate		IsSet(ConfigParam,3)
#define NegativePPM		IsSet(ConfigParam,4)
#define CompassTest		IsSet(ConfigParam,5)

// LISL-Register mapping
#define	LISL_WHOAMI		(0x0f)
#define	LISL_OFFSET_X	(0x16)
#define	LISL_OFFSET_Y	(0x17)
#define	LISL_OFFSET_Z	(0x18)
#define	LISL_GAIN_X		(0x19)
#define	LISL_GAIN_Y		(0x1A)
#define	LISL_GAIN_Z		(0x1B)
#define	LISL_CTRLREG_1	(0x20)
#define	LISL_CTRLREG_2	(0x21)
#define	LISL_CTRLREG_3	(0x22)
#define	LISL_STATUS		(0x27)
#define LISL_OUTX_L		(0x28)
#define LISL_OUTX_H		(0x29)
#define LISL_OUTY_L		(0x2A)
#define LISL_OUTY_H		(0x2B)
#define LISL_OUTZ_L		(0x2C)
#define LISL_OUTZ_H		(0x2D)
#define LISL_FF_CFG		(0x30)
#define LISL_FF_SRC		(0x31)
#define LISL_FF_ACK		(0x32)
#define LISL_FF_THS_L	(0x34)
#define LISL_FF_THS_H	(0x35)
#define LISL_FF_DUR		(0x36)
#define LISL_DD_CFG		(0x38)
#define LISL_INCR_ADDR	(0x40)
#define LISL_READ		(0x80)

// this enables common code for all ADXRS gyros
// leave this untouched!
#if defined OPT_ADXRS300 || defined OPT_ADXRS150
#define OPT_ADXRS
#endif

// this enables common code for debugging purposes
#if defined DEBUGOUT || defined DEBUGOUTG || defined DEBUGSSP
#define DEBUG
#endif

#define Version	"1.0m3"

// 18Fxxx C18 includes

#include <p18cxxx.h> 
#include <math.h>
#include <delays.h>
#include <timers.h>
#include <usart.h>
#include <capture.h>
#include <adc.h>

// Additional Types

typedef unsigned char uint8 ;
typedef signed char int8;
typedef unsigned int uint16;
typedef int int16;
typedef short long int24;
typedef unsigned short long uint24;
typedef long int32;
typedef unsigned long uint32;
typedef uint8 boolean;
typedef float real32;

// Useful Constants

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
#define DECIDEGMILLIRAD (real32)(1.7453292)
#define MILLIRADDEG (real32)(0.05729578)
#define MILLIRADDEG100 (real32)(5.729578)
#define CENTIRADDEG (real32)(0.5729578)
#define DEGRAD (real32)(0.017453292)
#define RADDEG (real32)(57.2957812)
#define RADDEG1000000 (real32)(57295781.2)

#define MAXINT32 0x7fffffff;
#define	MAXINT16 0x7fff;

// Macros

#define Set(S,b) 		((uint8)(S|=(1<<b)))
#define Clear(S,b) 		((uint8)(S&=(~(1<<b))))
#define IsSet(S,b) 		((uint8)((S>>b)&1))
#define IsClear(S,b) 	((uint8)(!(S>>b)&1))
#define Invert(S,b) 	((uint8)(S^=(1<<b)))

#define Abs(i)			((i<0) ? -i : i)
#define Sign(i)			((i<0) ? -1 : 1)

#define Max(i,j) 		((i<j) ? j : i)
#define Min(i,j) 		((i<j) ? i : j )
#define Limit(i,l,u) 	((i<l) ? l : ((i>u) ? u : i))
#define DecayBand(i,l,u,d) 	((i<l) ? i+d : ((i>u) ? i-d : i))
#define Decay(i) 		((i<0) ? i+1 : ((i>0) ? i-1 : 0))

// To speed up NMEA sentence porocessing 
// must have a positive argument
#define ConvertDDegToMPi(d) (((int32)d * 3574L)>>11)

// Simple filters using weighted averaging
#ifdef SUPPRESSFILTERS
  #define VerySoftFilter(O,N)		(N)
  #define SoftFilter(O,N) 			(N)
  #define MediumFilter(O,N) 			(N)
  #define AccelerometerFilter(O,N) 	(N)
#else
  #define VerySoftFilter(O,N) 		(SRS16(O+N*3, 2))
  #define SoftFilter(O,N) 			(SRS16(O+N, 1))
  #define MediumFilter(O,N) 		(SRS16(O*3+N, 2))
  #define HardFilter(O,N) 			(SRS16(O*7+N, 3))
#endif
#define NoFilter(O,N)				(N)

#define DisableInterrupts (INTCONbits.GIEH=0)
#define EnableInterrupts (INTCONbits.GIEH=1)
#define InterruptsEnabled (INTCONbits.GIEH)

// Constants 

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

//#ifdef CLOCK_16MHZ
#define _ClkOut		(160/4)	/* 16.0 MHz Xtal */
//#else // CLOCK_40MHZ
//NOT IMPLEMENTED YET #define _ClkOut		(400/4)	/* 10.0 MHz Xtal * 4 PLL */
//#endif

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



#define MAXDROPOUT	400L	// 400 x 16 x 7ms = 40sec. dropout allowable
#define GPSDROPOUT	20L		// 2sec.

// Parameters for UART port

// ClockHz/(16*(BaudRate+1))

#define _B9600		104 
#define _B19200		(_ClkOut*100000/(4*19200) - 1)
#define _B38400		26 
#define _B115200	(_ClkOut*104000/(4*115200) - 1)
#define _B230400	(_ClkOut*100000/(4*115200) - 1)

// EEPROM parameter set addresses

#define _EESet1	0		// first set starts at address 0x00
#define _EESet2	0x20	// second set starts at address 0x20

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

// Prototypes

// accel.c
extern void AccelerationCompensation(void);
extern void IsLISLactive(void);
extern void InitLISL(void);
extern uint8 ReadLISL(uint8);
extern uint8 ReadLISLNext(void);
extern void ReadAccelerations(void);

// adc.c
extern int16 ADC(uint8, uint8);
extern void InitADC(void);

// autonomous.c
extern void AcquireSatellites(void);
extern void Navigate(int16, int16);
extern void InitNavigation(void);
extern void Descend(void);

// compass_altimeter.c
extern void InitDirection(void);
extern void GetDirection(void);
extern void InitBarometer(void);
extern void StartBaroAcq(uint8);
extern uint16 ReadBaro(void);
extern void GetBaroAltitude(void);

// irq.c
extern void InitTimersAndInterrupts(void);
extern void ReceivingGPSOnly(uint8);

// pid.c
extern void GetNeutralAccelerations(void);
extern void GetGyroValues(void);
extern void CalcGyroValues(void);
extern void PID(void);
extern void LimitRollSum(void);
extern void LimitPitchSum(void);
extern void LimitYawSum(void);

// gps.c
extern void InitGPS(void);
extern void PollGPS(void);
extern void ParseGPSSentence(void);
extern void UpdateGPS(void);

// i2c.c
extern void I2CStart(void);
extern void I2CStop(void);
extern uint8 SendI2CByte(uint8);
extern uint8 RecvI2CByte(uint8);

// outputs.c
extern void MixAndLimitMotors(void);
extern void MixAndLimitCam(void);
extern void OutSignals(void);

// sensors.c
extern void InitDirection(void);
extern void GetDirection(void);
extern void InitBarometer(void);
extern void ComputeBaroComp(void);
extern uint8 ReadValueFromBaro(void);
extern uint8 StartBaroADC(uint8);

// serial.c
extern void ProcessCommand(void);
extern void ShowSetup(uint8);
extern uint8 RxChar(void);
extern uint8 PollRxChar(void);
extern int8 RxNumS(void);
extern uint8 RxNumU(void);
extern void TxValH16(uint16);
extern void TxValH(uint8);
extern void TxVal32(int32, int8, uint8);
extern void TxChar(uint8);
extern void TxNextLine(void);
extern void TxValU(uint8);
extern void TxValS(int8);
extern void TxString(const rom uint8 *);

// spi.c
extern void IsLISLactive(void);
extern void WaitLISLReady(void);
extern void ReadAccelerations(void);

// utils.c
extern int32 SRS32(int32, uint8);
extern int16 SRS16(int16, uint8);
extern void CheckAlarms(void);
extern void ReadParametersEE(void);
extern void WriteParametersEE(uint8);
extern void InitParams(void);
extern 	void WriteEE(uint8, int8);
extern 	int8 ReadEE(uint8);

extern void SendLeds(void);
extern void SwitchLedsOn(uint8);
extern void SwitchLedsOff(uint8);
extern void LEDGame(void);

extern void DoDebugTraces(void);
extern void Delay100mSWithOutput(int16);
extern void Delay1mS(int16);
extern void InitPorts(void);
extern int16 Table16(int16, const int16 *);
extern int16 Make2Pi(int16);
extern int16 int16atan2(int16, int16);
extern int16 int16sin(int16);
extern int16 int16cos(int16);

extern int16 ConvertMToGPS(int16);
extern int16 ConvertGPSToM(int16);

extern void InitArrays(void);

#ifdef DEBUG_SENSORS
extern void DumpTrace(void);
#endif

// bootl18f.asm
extern void BootStart(void);

#ifdef TEST_SOFTWARE

extern int16	TestTimeSlot;
extern uint16	PauseTime;
extern int16 	NewK1, NewK2, NewK3, NewK4, NewK5, NewK6, NewK7;

// Menu strings

extern const rom uint8  SerHello[];
extern const rom uint8  SerSetup[];
extern const rom uint8 SerPrompt[];

extern void AnalogTest(void);extern void DoCompassTest(void);
extern void GPSTest(void);
extern void CalibrateCompass(void);
extern void PowerOutput(int8);
extern void ReceiverTest(void);
extern void TogglePPMPolarity(void);
extern void BaroTest(void);
extern void LinearTest(void);
extern uint8 ScanI2CBus(void);
extern int16 int16sqrt(int16);

#endif // TEST_SOFTWARE


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
extern int16	RollRate, PitchRate, YawRate;
extern int16	GyroMidRoll, GyroMidPitch, GyroMidYaw;
extern	int16	DesiredThrottle, DesiredRoll, DesiredPitch, DesiredYaw, CompassHeading;
extern int16	Ax, Ay, Az;
extern int8		LRIntKorr, FBIntKorr;
extern int8		NeutralLR, NeutralFB, NeutralUD;
extern int16 	UDSum;

// GPS
#ifdef GPS_USE_RMC
extern uint8 GPSMode;
extern int16 GPSGroundSpeed, GPSMagVariation, GPSHeading;
#else
extern uint8 GPSNoOfSats;
extern uint8 GPSFix;
extern int16 GPSHDilute;
#endif // GPS_USE_RMC
extern int16 	GPSNorth, GPSEast, GPSNorthHold, GPSEastHold;

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
extern int16	Vud;

extern uint8	Flags[32];

extern int16	IntegralCount, ThrDownCount, GPSCount, DropoutCount, LedCount, BaroCount;
extern int16	FakeGPSCount;
extern uint32	BlinkCount;
extern uint24	RCGlitchCount;
extern int8		Rw,Pw;	// angles
extern int8		BatteryVolts; 

extern uint8	LedShadow;	// shadow register
extern int16	AbsDirection;	// wanted heading (240 = 360 deg)
extern int16	CurDeviation;	// deviation from correct heading

#define RXBUFFMASK	63L
extern uint8 RxCheckSum, RxHead, RxTail;
extern uint8 RxBuff[RXBUFFMASK+1];

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
extern int8 TimeSlot;			// 17
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

// End of c-ufo.h

