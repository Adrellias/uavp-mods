// EXPERIMENTAL

//#define FAKE_FLIGHT					// For testing Nav on the GROUND!

// Jim - you may wish to try these?
// #define NAV_SUPPRESS_P					// No control proportional to distance from target other than integral
#define ATTITUDE_FF_DIFF			24L		// 0 - 32 max feedforward speeds up roll/pitch recovery on fast stick change
#define NAV_RTH_LOCKOUT				10L		// Roll/Pitch Sum (Angle) above which RTH will not engage; 0 to disable						

#define	ATTITUDE_SUPPRESS_DECAY					// supresses decay to zero angle when roll/pitch is not in fact zero!

#define BARO_HOVER_MAX_ROC_CMPS		50L		// Must be changing altitude at less than this for hover to be detected

#define BARO_DOUBLE_UP_COMP					// Hover has double the amount of up compensation each adjustment

// Moving average of coordinates - Kalman Estimator probably needed
//#define GPSFilter NoFilter
#define GPSFilter SoftFilter				
//#define GPSFilter MediumFilter

#define DAMP_HORIZ_LIMIT 		3L		// equivalent stick units - no larger than 5

#define DAMP_VERT_LIMIT_LOW		-5L		// maximum throttle reduction
#define DAMP_VERT_LIMIT_HIGH	20L		// maximum throttle increase

#define NAV_MAX_ROLL_PITCH 		25L		// *18 Rx stick units ~= degrees max pitch/roll angle
#define NAV_INT_LIMIT			8L		// *3 Suggest similar to DAMP_HORIZ_LIMIT 
#define NAV_DIFF_LIMIT			16L		// *8,3 Less than NAV_MAX_ROLL_PITCH? 

//#define TELEMETRY						// telemetry piggy-backed on GPS Rx

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


#ifndef BATCHMODE

// Tricopter 
// connector K1 = front motor
//           K2 = rear left motor 
//           K3 = rear right motor 
//           K4 = yaw servo output
//#define TRICOPTER						

//#define RX6CH 						// 6ch Receivers

//#define DEBUG_SENSORS					// Debug sensors with UAVPset - no motors

#endif // !BATCHMODE

//________________________________________________________________________________________________

// UAVX Extensions

//#define STATS_INC_GYRO_ACC				// includes tracking of Accs/Gyros but takes time in main loop!

// Timeouts and Update Intervals

#define FAILSAFE_TIMEOUT_MS		1000L 		// mS. hold last "good" settings and then either restore flight or abort
#define ABORT_TIMEOUT_MS		3000L	 	// mS. autonomous RTH with GPS or Descend without. 
#define ABORT_UPDATE_MS			2000L		// mS. retry period for RC Signal and restore Pilot in Control

#define THROTTLE_LOW_DELAY_MS	1000L		// mS. that motor runs at idle after the throttle is closed
#define THROTTLE_UPDATE_MS		3000L		// mS. constant throttle time for hover

#define NAV_ACTIVE_DELAY_MS		10000L		// mS. after throttle exceeds idle that Nav becomes active
#define NAV_RTH_TIMEOUT_MS		30000L		// mS. Descend if no control input when at Origin

#define GPS_TIMEOUT_MS			2000L		// mS.

// Baro

#define BARO_SCRATCHY_BEEPER				// Scratchy beeper noise on hover

// Increase the severity of the filter when averaging barometer pressure readings
// New=(Old*7+New)/8).
// Probably more important with SMD500
//#define BARO_HARD_FILTER

// Accelerometers

#define USE_ACCELEROMETER				// Use the Accelerometer sensor 			

// Gyros

// Adds a delay between gyro neutral acquisition samples (16)
#define GYRO_ERECT_DELAY		1				// x 64 x 100mS 

// Enable "Dynamic mass" compensation Roll and/or Pitch
// Normally disabled for pitch only 
//#define DISABLE_DYNAMIC_MASS_COMP_ROLL
//#define DISABLE_DYNAMIC_MASS_COMP_PITCH

// Altitude Hold

#define BARO_ALT_BAND_CM			200L				// Centimetres

// Throttle reduction/increase limits for Baro Alt Comp

#define BARO_MAX_ROC_CMPS			500L 		// Centimetres/Sec notional for descent calculations
#define BARO_MAX_DESCENT_CMPS		-50L 		// Centimetres/Sec
#define BARO_FINAL_DESCENT_CMPS		-20L 		// Centimetres/Sec
#define BARO_DESCENT_TRANS_CM		1500L		// Centimetres Altitude at which final descent starts 
#define BARO_FAILSAFE_MIN_ALT_CM	300L		// Centimetres above the origin the motors cut on failsafe descent

#define ALT_LOW_THR_COMP			-7L		// Stick units
#define ALT_HIGH_THR_COMP			30L

// the range within which throttle adjustment is proportional to altitude error
#define GPS_ALT_BAND_CM			500L				// Centimetres

// Navigation

#define NAV_ACQUIRE_BEEPER

#define NAV_MAX_NEUTRAL_RADIUS	3L				// Metres also minimum closing radius
#define NAV_MAX_RADIUS			40L				// Metres

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

//#define NAV_ZERO_INT 					// Zeros the integral when the sign is not the same as the error			
	
#define	NAV_YAW_LIMIT			10L		// yaw slew rate for RTH
#define NAV_MAX_TRIM			20L		// max trim offset for hover hold
#define NAV_CORR_SLEW_LIMIT		1L		// *5L maximum change in roll or pitch correction per GPS update

#define ATTITUDE_HOLD_LIMIT 			8L		// dead zone for roll/pitch stick for position hold
#define ATTITUDE_HOLD_RESET_INTERVAL	25L		// number of impulse cycles before GPS position is re-acquired

#define NAV_MAX_WAYPOINTS		16		// Only WP[0] or Origin used

//#define NAV_PPM_FAILSAFE_RTH			// PPM signal failure causes RTH with Signal sampled periodically

#define METRES_TO_GPS 			5		// each GPS lsb 0.185M => use 200mm but better accuracy for WP needed!

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
#define SOH 1
#define EOT 4
#define HT 		9
#define LF 		10
#define CR 		13
#define NAK 	21
#define ESC 	27
#define true 	1
#define false 	0

#define EarthR 				(real32)(6378140.0)
#define Pi 					(real32)(3.141592654)
#define RecipEarthR 		(real32)(1.5678552E-7)

#define DEGTOM 				(real32)(111319.5431531) 
#define MILLIPI 			3142 
#define CENTIPI 			314 
#define HALFMILLIPI 		1571 
#define QUARTERMILLIPI		785
#define TWOMILLIPI 			6284

#define MILLIRAD 			18 
#define CENTIRAD 			2
#define DEGMILLIRAD 		(real32)(17.453292) 
#define DECIDEGMILLIRAD 	(real32)(1.7453292)
#define MILLIRADDEG 		(real32)(0.05729578)
#define MILLIRADDEG100 		(real32)(5.729578)
#define CENTIRADDEG 		(real32)(0.5729578)
#define DEGRAD 				(real32)(0.017453292)
#define RADDEG 				(real32)(57.2957812)
#define RADDEG1000000 		(real32)(57295781.2)

#define MAXINT32 			0x7fffffff
#define	MAXINT16 			0x7fff

// Additional Types
typedef unsigned char 		uint8 ;
typedef signed char 		int8;
typedef unsigned int 		uint16;
typedef int 				int16;
typedef short long 			int24;
typedef unsigned short long uint24;
typedef long 				int32;
typedef unsigned long 		uint32;
typedef uint8 				boolean;
typedef float 				real32;

typedef union {
	int16 i16;
	uint16 u16;
	struct {
		uint8 low8;
		uint8 high8;
	};
} i16u;

typedef union {
	int32 i32;
	uint32 u32;
	struct {
		uint16 low16;
		uint16 high16;
	};
} i32u;

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

#define ToPercent(n, m) (((n)*100)/m)

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
//#ifdef CLOCK_16MHZ
	#define _ClkOut			(160/4)			// 16.0 MHz Xtal
//#else // CLOCK_40MHZ
	//NOT IMPLEMENTED YET #define _ClkOut	(400L/4)	// 10.0 MHz Xtal * 4 PLL
//#endif

#define _PreScale0			16				// 1 6 TMR0 prescaler 
#define _PreScale1			8				// 1:8 TMR1 prescaler 
#define _PreScale2			16
#define _PostScale2			16

// Parameters for UART port ClockHz/(16*(BaudRate+1))
#define _B9600				104 
#define _B19200				(_ClkOut*100000/(4*19200) - 1)
#define _B38400				26 
#define _B115200			9 
#define _B230400			(_ClkOut*100000/(4*115200) - 1)

#define TMR2_5MS			78				// 1x 5ms + 
#define TMR2_TICK			2				// uSec

// LEDs
#define AUX2M				0x01
#define BlueM				0x02
#define RedM				0x04
#define GreenM				0x08
#define AUX1M				0x10
#define YellowM				0x20
#define AUX3M				0x40
#define BeeperM				0x80

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
#define InTheAir			true	// zzz (PORTCbits.RC0) // micro switch to ground when closed

#define	I2C_ACK				((uint8)(0))
#define	I2C_NACK			((uint8)(1))

#define SPI_CS				PORTCbits.RC5
#define SPI_SDA				PORTCbits.RC4
#define SPI_SCL				PORTCbits.RC3
#define SPI_IO				TRISCbits.TRISC4

#define	RD_SPI				1
#define WR_SPI				0
#define DSEL_LISL  			1
#define SEL_LISL  			0

// ADC Channels
#define ADCPORTCONFIG 		0b00001010 // AAAAA
#define ADCBattVoltsChan 	0 
#define NonIDGADCRollChan 	1
#define NonIDGADCPitchChan 	2
#define IDGADCRollChan 		2
#define IDGADCPitchChan 	1
#define ADCVRefChan 		3 
#define ADCYawChan			4
#define ADCVREF5V 			0
#define ADCVREF3V3 			1

// RC

#define RxFilter					MediumFilterU
//#define RxFilter					SoftFilterU
//#define RxFilter					NoFilter

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

#ifdef RX6CH 
	#define RC_CONTROLS 5			
#else
	#define RC_CONTROLS CONTROLS
#endif //RX6CH

// ESC
#define OUT_MINIMUM			1					// Required for PPM timing loops
#define OUT_NEUTRAL			((150 * _ClkOut/(2*_PreScale1))&0xFF)    //   0%
#define OUT_MAXIMUM			240
#define OUT_HOLGER_MAXIMUM	225
#define OUT_YGEI2C_MAXIMUM	240
#define OUT_X3D_MAXIMUM		200 

// Compass sensor
#define COMPASS_I2C_ID		0x42				// I2C slave address
#define COMPASS_MAXDEV		30					// maximum yaw compensation of compass heading 
#define COMPASS_MIDDLE		10					// yaw stick neutral dead zone
#define COMPASS_TIME_MS		50					// 20Hz

// Baro (altimeter) sensor
#define BARO_I2C_ID			0xee
#define BARO_TEMP_BMP085	0x2e
#define BARO_TEMP_SMD500	0x6e
#define BARO_PRESS			0xf4
#define BARO_CTL			0xf4				// OSRS=3 for BMP085 25.5mS, SMD500 34mS				
#define BARO_ADC_MSB		0xf6
#define BARO_ADC_LSB		0xf7
#define BARO_TYPE			0xd0
//#define BARO_ID_SMD500	??
#define BARO_ID_BMP085		((uint8)(0x55))

#define BARO_TEMP_TIME_MS		10
//#define BMP085_PRESS_TIME_MS 	26
//#define SMD500_PRESS_TIME_MS 	34
#define BARO_PRESS_TIME_MS	50	

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

// Prototypes

// accel.c
extern void SendCommand(int8);
extern uint8 ReadLISL(uint8);
extern uint8 ReadLISLNext(void);
extern void WriteLISL(uint8, uint8);
extern void IsLISLActive(void);
extern void InitLISL(void);
extern void ReadAccelerations(void);
extern void GetNeutralAccelerations(void);

// adc.c
extern int16 ADC(uint8, uint8);
extern void InitADC(void);

// autonomous.c
extern void Navigate(int16, int16);
extern void SetDesiredAltitude(int16);
extern void Descend(void);
extern void AcquireHoldPosition(void);
extern void NavGainSchedule(int16);
extern void DoNavigation(void);
extern void FakeFlight(void); 
extern void DoPPMFailsafe(void);
extern void InitNavigation(void);

// compass_altimeter.c
extern void InitCompass(void);
extern void InitHeading(void);
extern void GetHeading(void);
extern void StartBaroADC(boolean);
extern void ReadBaro(boolean);
extern void GetBaroAltitude(void);
extern void InitBarometer(void);
extern void BaroAltitudeHold(void);
extern void AltitudeHold(void);

// control.c
extern void GyroCompensation(void);
extern void LimitRollSum(void);
extern void LimitPitchSum(void);
extern void LimitYawSum(void);
extern void GetGyroValues(void);
extern void ErectGyros(void);
extern void InertialDamping(void);
extern void CalcGyroRates(void);
extern void DoControl(void);

extern void UpdateControls(void);
extern void CaptureTrims(void);
extern void StopMotors(void);
extern void CheckThrottleMoved(void);
extern void LightsAndSirens(void);
extern void InitControl(void);

// eeprom.c
extern int8 ReadEE(uint8);
extern void WriteEE(uint8, int8);

// irq.c
extern void ReceivingGPSOnly(uint8);
void DoRxPolarity(void);
extern void MapRC(void);
extern void InitTimersAndInterrupts(void);
extern void ReceivingGPSOnly(uint8);

// gps.c
extern int16 ConvertGPSToM(int16);
extern int16 ConvertMToGPS(int16);
extern void UpdateField(void);
extern int16 ConvertInt(uint8, uint8);
extern int32 ConvertLatLonM(uint8, uint8);
extern int32 ConvertUTime(uint8, uint8);
extern void ParseGPRMCSentence(void);
extern void ParseGPGGASentence(void);
extern void SetGPSOrigin(void);
extern void ParseGPSSentence(void);
extern void ResetGPSOrigin(void);
extern void InitGPS(void);
extern void UpdateGPS(void);

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

// leds.c
extern void SendLEDs(void);
extern void LEDsOn(uint8);
extern void LEDsOff(uint8);
extern void LEDGame(void);

// math.c
extern int16 SRS16(int16, uint8);
extern int32 SRS32(int32, uint8);
extern int16 Make2Pi(int16);
extern int16 MakePi(int16);
extern int16 Table16(int16, const int16 *);
extern int16 int16sin(int16);
extern int16 int16cos(int16);
extern int16 int16atan2(int16, int16);
extern int16 int16sqrt(int16);
extern int32 int32sqrt(int32);

// menu.c
extern void ShowPrompt(void);
extern void ShowRxSetup(void);
extern void ShowSetup(uint8);
extern void ProcessCommand(void);

// outputs.c
extern uint8 SaturInt(int16);
extern void DoMix(int16 CurrThrottle);
extern void CheckDemand(int16 CurrThrottle);
extern void MixAndLimitMotors(void);
extern void MixAndLimitCam(void);
extern void OutSignals(void);
extern void InitI2CESCs(void);

// params.c
extern void ReadParametersEE(void);
extern void WriteParametersEE(uint8);
extern void UseDefaultParameters(void);
extern void UpdateParamSetChoice(void);
extern void InitParameters(void);

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
extern uint8 RxNumU(void);
extern int8 RxNumS(void);
extern void TxVal32(int32, int8, uint8);
extern void SendByte(uint8);
extern void SendESCByte(uint8);
extern void SendWord(int16);
extern void SendESCWord(int16);
extern void SendPacket(uint8, uint8, uint8 *, boolean);

// stats.c
extern void ZeroStats(void);
extern void ReadStatsEE(void);
extern void WriteStatsEE(void);
extern void ShowStats(void);

// utils.c
extern void InitPorts(void);
extern void InitMisc(void);

extern void Delay1mS(int16);
extern void Delay100mSWithOutput(int16);
extern void DoBeep100mSWithOutput(uint8, uint8);
extern void DoStartingBeepsWithOutput(uint8);

extern int16 SlewLimit(int16, int16, int16);
extern int16 ProcLimit(int16, int16, int16);
extern int16 DecayX(int16, int16);

extern void CheckAlarms(void);

extern void DumpTrace(void);
extern void SendUAVXState(void);

// bootl18f.asm
extern void BootStart(void);

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

// Menu strings
extern const rom uint8 SerHello[];
extern const rom uint8 SerSetup[];
extern const rom uint8 SerPrompt[];

// External Variables

enum { Clock, UpdateTimeout, RCSignalTimeout, BeeperTimeout, ThrottleIdleTimeout, FailsafeTimeout, 
      AbortTimeout, RTHTimeout, LastValidRx, LastGPS, AltHoldUpdate, GPSTimeout, NavActiveTime, ThrottleUpdate, VerticalDampingUpdate, BaroUpdate, CompassUpdate};
	
enum RCControls {ThrottleC, RollC, PitchC, YawC, RTHC, CamPitchC, NavGainC}; 
#define CONTROLS (NavGainC+1)
#define MAX_CONTROLS 12 // maximum Rx channels

enum WaitGPSStates { WaitGPSSentinel, WaitNMEATag, WaitGPSBody, WaitGPSCheckSum};
enum FlightStates { Starting, Landing, Landed, InFlight};
enum FailStates { Waiting, Aborting, Returning, Terminated };

enum ESCTypes { ESCPPM, ESCHolger, ESCX3D, ESCYGEI2C };
enum GyroTypes { ADXRS300, ADXRS150, IDG300};
enum TxRxTypes { FutabaCh3, FutabaCh2, FutabaDM8, JRPPM, JRDM9, JRDXS12, 
				DX7AR7000, DX7AR6200, FutabaCh3_6_7, DX7AR6000, GraupnerMX16s, CustomTxRx };

enum TraceTags {THE, TCurrentRelBaroAltitude,
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

enum MotorTags {Front=0, Back, Right, Left}; // order is important for X3D & Holger ESCs
#define NoOfMotors 		4

extern uint24 mS[];

extern uint8	State, FailState;
extern uint8 	SHADOWB, MF, MB, ML, MR, MT, ME; // motor and servo outputs
extern i16u 	PPM[];
extern int8 	PPM_Index;
extern int24 	PrevEdge, CurrEdge;
extern i16u 	Width;
extern int16	PauseTime; // for tests
extern uint8 	GPSRxState;
extern uint8 	ll, tt, gps_ch;
extern uint8 	RxCheckSum, GPSCheckSumChar, GPSTxCheckSum;
extern uint8	ESCMin, ESCMax;

extern int16	RC[];
extern int8 	RMap[CONTROLS];
extern int8		SignalCount;
extern uint16	RCGlitches;
extern boolean	FirstPass;

#define MAXTAGINDEX 	4
#define GPSRXBUFFLENGTH 80
extern struct {
		uint8 	s[GPSRXBUFFLENGTH];
		uint8 	length;
	} NMEA;

extern const rom uint8 NMEATag[];

extern uint8 	CurrentParamSet;
extern boolean 	ParametersChanged;

extern int16	RE, PE, YE, HE;
extern int16	REp,PEp,YEp, HEp;
extern int16	PitchSum, RollSum, YawSum;
extern int16	RollRate, PitchRate, YawRate;
extern int16	RollTrim, PitchTrim, YawTrim;
extern int16	HoldYaw;
extern int16	RollIntLimit256, PitchIntLimit256, YawIntLimit256;
extern int16	GyroMidRoll, GyroMidPitch, GyroMidYaw;
extern int16	HoverThrottle, DesiredThrottle, IdleThrottle, InitialThrottle;
extern int16	DesiredRoll, DesiredPitch, DesiredYaw, DesiredHeading, DesiredCamPitchTrim, Heading;
extern int16	DesiredRollP, DesiredPitchP;
extern int16	CurrMaxRollPitch;
extern i16u		Ax, Ay, Az;
extern int8		LRIntCorr, FBIntCorr;
extern int8		NeutralLR, NeutralFB, NeutralDU;
extern int16	DUVel, LRVel, FBVel, DUAcc, LRAcc, FBAcc, DUComp, LRComp, FBComp;

// GPS
extern int16 	GPSLongitudeCorrection;
extern uint8 	GPSNoOfSats;
extern uint8 	GPSFix;
extern int16 	GPSHDilute;
extern int16 	GPSNorth, GPSEast, GPSNorthHold, GPSEastHold, GPSNorthP, GPSEastP, GPSVel;
extern int16 	GPSRelAltitude;
extern int16 	ValidGPSSentences;

extern int16 	NavClosingRadius, NavNeutralRadius, NavCloseToNeutralRadius, CompassOffset;

enum NavStates { HoldingStation, ReturningHome, AtHome, Descending, Navigating, Terminating };
extern uint8 	NavState;
extern int16 	NavSensitivity;
extern int16 	AltSum, AE;

// Waypoints

typedef  struct {
	int16 N, E, A;
} WayPoint;

extern WayPoint WP[];

// Failsafes
extern int16	ThrLow, ThrHigh, ThrNeutral;
			
// Variables for barometric sensor PD-controller
extern int24	OriginBaroPressure, BaroSum;
extern int16	DesiredRelBaroAltitude, CurrentRelBaroAltitude, RelBaroAltitudeP;
extern int16	CurrentBaroROC, BaroROCP, BE;
extern i16u		BaroTemp, BaroPress;
extern int8		BaroSample;
extern int16	BaroComp;
extern uint8	BaroType;

extern uint8	MCamRoll,MCamPitch;
extern int16	Motor[NoOfMotors];
extern boolean	ESCI2CFail[NoOfMotors];
extern int16	Rl,Pl,Yl;		// PID output values

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
		ReturnHome:1,
		Proximity:1,
		CloseProximity:1,

		UsingGPSAlt:1,
		UsingRTHAutoDescend:1,
		BaroAltitudeValid:1,

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
		GPSOriginValid:1,

		NavComputed:1,
		CheckThrottleMoved:1,
		RTHAltitudeHold:1,	// stick programmed
		TurnToHome:1,		// stick programmed
		UsingSerialPPM:1,
		UsingTxMode2:1,
		UsingXMode:1,
		NewBaroValue:1,
		BeeperInUse:1, 
		AcquireNewPosition:1, 
		GPSTestActive:1;

		};
} Flags;

extern Flags F;

extern uint8	LEDCycles;		// for hover light display
extern int16	AttitudeHoldResetCount;	
extern int8	BatteryVolts; 
extern uint8	LEDShadow;		// shadow register

enum Statistics { GPSAltitudeS, RelBaroAltitudeS, MinBaroROCS, MaxBaroROCS, GPSVelS, RollRateS, PitchRateS, YawRateS,
				LRAccS, FBAccS,DUAccS, GyroMidRollS, GyroMidPitchS, GyroMidYawS, 
				AccFailS, CompassFailS, BaroFailS, GPSInvalidS, GPSSentencesS, MinHDiluteS, MaxHDiluteS,
				RCGlitchesS, MaxStats};
extern i16u Stats[];

enum PacketTags {UnknownPacketTag, LevPacketTag, NavPacketTag, MicropilotPacketTag, WayPacketTag, 
                 AirframePacketTag, NavUpdatePacketTag, BasicPacketTag, RestartPacketTag, TrimblePacketTag, 
                 MessagePacketTag, EnvironmentPacketTag, BeaconPacketTag, UAVXFlightPacketTag, UAVXNavPacketTag};
#define TX_BUFF_MASK	127
extern uint8 TxHead, TxTail;
extern uint8 TxCheckSum;
extern uint8 TxBuf[];

extern uint8 UAVXCurrPacketTag;

extern int16	Trace[];

// Principal quadrocopter parameters

// parameters start at address zero in EEPROM

#define MAX_PARAMETERS	64		// parameters in EEPROM start at zero
enum Params {
	RollKp, 			// 01
	RollKi,				// 02
	RollKd,				// 03
	HorizDampKp,		// 04c
	RollIntLimit,		// 05
	PitchKp,			// 06
	PitchKi,			// 07
	PitchKd,			// 08
	BaroCompKp,			// 09c
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
	BaroROCCompKp,			// 28c was BaroCompKd
	NavRadius,			// 29
	NavKi,				// 30
	
	GPSAltKp,			// 31
	GPSAltKi,			// 32
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
	HorizDampDecay		// 43c		
	// 44 - 64 unused currently
	};

#define FlyXMode 			0
#define FlyXModeMask 		0x01

#define UseRTHDescend 		1
#define	UseRTHDescendMask	0x02

#define TxMode2 			2
#define TxMode2Mask 		0x04

#define RxSerialPPM 		3
#define RxSerialPPMMask		0x08 

#define UseGPSAlt 			5
#define	UseGPSAltMask		0x20

#define STATS_ADDR_EE	 	( MAX_PARAMETERS *2 )
extern int8 P[];
extern const rom int8 ComParms[];
extern const rom int8 DefaultParams[];
extern const rom uint8 Map[CustomTxRx+1][CONTROLS];
extern const rom uint8 RxChMnem[];
extern const rom uint8 ESCLimits [];
extern const rom boolean PPMPosPolarity[];


