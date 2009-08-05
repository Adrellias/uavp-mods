// EXPERIMENTAL

// limits the rate at which the thtottle can change
#define SLEW_LIMIT_THROTTLE

// Navigation

#define NAV_HOLD_LIMIT 			20L		// Dead zone for roll/pitch stick for position hold
#define NAV_HOLD_RESET_INTERVAL	100		// number of impulse cycles before GPS position is re-acquired
#define NAV_ACTIVE_DELAY_S		10L		// Sec. after throttle exceeds idle that Nav becomes active
#define NAV_RTH_TIMEOUT_S		30L		// Sec. Descend if no control input when at Origin

// comment out for normal wind compensation otherwise integral assist is cancelled upon reaching target
#define ZERO_NAVINT

// Accelerometer

// Altitude Hold

// Throttle reduction and increase limits for Baro Alt Comp
#define BARO_LOW_THR_COMP		-5L
#define BARO_HIGH_THR_COMP		20L

// Throttle reduction and increase limits for GPS Alt Comp
#define GPSALT_LOW_THR_COMP		-10L
#define GPSALT_HIGH_THR_COMP	30L

#define BARO_FROM_METRES		5L

// the range within which throttle adjustment is proportional to altitude error
#define GPSALT_BAND				5L		// Metres

// Modifications which have been adopted are included BELOW.

// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008, 2009 by Prof. Greg Egan           =
// =   Original V3.15 Copyright (c) 2007, 2008 Ing. Wolfgang Mahringer   =
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

// Unroll some loops on critical path
#define UNROLL_LOOPS

#ifndef BATCHMODE

// connector K1 = front motor
//           K2 = rear left motor 
//           K3 = rear right motor 
//           K4 = yaw servo output
// uncomment this to enable Tri-Copter Mixing.
//#define TRICOPTER

// uncomment for 6 channel recievers
//#define RX6CH 

// special mode for sensor data output (with UAVPset)
//#define DEBUG_SENSORS

#endif // !BATCHMODE

//________________________________________________________________________________________________

// UAVX Extensions

// Baro

// Make a "scratchy" beeper noise while altitude hold is engaged.
// A better method is to use the auxilliary LEDs - see Manual.
#define BARO_SCRATCHY_BEEPER

// Increase the severity of the filter when averaging barometer pressure readings
// New=(Old*7+New)/8).
// Probably more important with SMD500
//#define BARO_HARD_FILTER

// Accelerometers

// Enable this to use the Accelerometer sensor 
// Normally ENABLED
#define USE_ACCELEROMETER

// Gyros

// Adds a delay between gyro neutral acquisition samples (16)
#define GYRO_ERECT_DELAY		2		// x 16 x 100mS 

// Enable "Dynamic mass" compensation Roll and/or Pitch
// Normally disabled for pitch only 
//#define DISABLE_DYNAMIC_MASS_COMP_ROLL
//#define DISABLE_DYNAMIC_MASS_COMP_PITCH

// Navigation

// reads $GPGGA and $GPRMC sentences - all others discarded

#define	NAV_GAIN_THRESHOLD 		20		// Navigation disabled if Ch7 is less than this
	
#define	MIN_SATELLITES			5		// preferably >5 for 3D fix
#define MIN_FIX					1		// must be 1 or 2 
#define INITIAL_GPS_SENTENCES 	90		// Number of sentences needed with set HDilute
#define MIN_HDILUTE				130L	// HDilute * 100	
#define COMPASS_OFFSET_DEG		270L	// North degrees CW from Front
#define MAX_ANGLE 				25L		// Rx stick units ~= degrees

#define	NAV_YAW_LIMIT			10L		// yaw slew rate for RTH
#define MAX_TRIM				20L		// max trim offset for hover hold

#define FAILSAFE_TIMEOUT_S		1 		// Sec. hold last "good" settings and then either restore flight or abort
#define ABORT_TIMEOUT_S			3	 	// Sec. full flight abort - motors shutdown until reboot 
#define LOW_THROTTLE_DELAY_S	1		// Sec. that motor runs at idle after the throttle is closed
#define THROTTLE_UPDATE_S		3		// Sec. constant throttle time for hover

#define THR_MIDDLE				10  	// throttle stick dead zone for baro 
#define THR_HOVER				75		// min throttle stick for altitude lock

#define GPS_TIMEOUT_S			2		// Sec.

#define MAX_WAYPOINTS			16

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

#define MAXINT32 			0x7fffffff;
#define	MAXINT16 			0x7fff;

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
#define Limit(i,l,u) 		((i<l) ? l : ((i>u) ? u : i))
#define DecayBand(i,l,u,d) 	((i<l) ? i+d : ((i>u) ? i-d : i))
#define Decay(i) 			((i<0) ? i+1 : ((i>0) ? i-1 : 0))

// To speed up NMEA sentence processing 
// must have a positive argument
#define ConvertDDegToMPi(d) (((int32)d * 3574L)>>11)
#define ConvertMPiToDDeg(d) (((int32)d * 2048L)/3574L)

#define ToPercent(n, m) (((n)*100)/m)

// Simple filters using weighted averaging
#ifdef SUPPRESSFILTERS
  #define VerySoftFilter(O,N)		(N)
  #define SoftFilter(O,N) 			(N)
  #define MediumFilter(O,N) 		(N)
  #define AccelerometerFilter(O,N) 	(N)
#else
  #define VerySoftFilter(O,N) 		(SRS16((O)+(N)*3, 2))
  #define SoftFilter(O,N) 			(SRS16((O)+(N), 1))
  #define MediumFilter(O,N) 		(SRS16((O)*3+(N), 2))
  #define HardFilter(O,N) 			(SRS16((O)*7+(N), 3))
#endif
#define NoFilter(O,N)				(N)

#define DisableInterrupts 	(INTCONbits.GIEH=0)
#define EnableInterrupts 	(INTCONbits.GIEH=1)
#define InterruptsEnabled 	(INTCONbits.GIEH)

// Clock
//#ifdef CLOCK_16MHZ
	#define _ClkOut			(160/4)			// 16.0 MHz Xtal
//#else // CLOCK_40MHZ
	//NOT IMPLEMENTED YET #define _ClkOut	(400L/4)	// 10.0 MHz Xtal * 4 PLL
//#endif

#define _PreScale0			16				// 1:16 TMR0 prescaler 
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

// Status 
#define	_Signal				Flags[0]
	
#define	_NewValues			Flags[2]	
#define _FirstTimeout		Flags[3]

#define _LowBatt			Flags[4]	
#define _AccelerationsValid Flags[5]
#define	_CompassValid		Flags[6]	
#define _CompassMissRead 	Flags[7]
#define _BaroAltitudeValid	Flags[8]
	
#define _BaroRestart		Flags[10] 	
#define _OutToggle			Flags[11]									
#define _Failsafe			Flags[12]
#define _GyrosErected		Flags[13]
#define _NewBaroValue		Flags[14]

#define _ReceivingGPS 		Flags[16]
#define _GPSValid 			Flags[17]
#define _LostModel			Flags[18]
#define _ThrottleMoving		Flags[19]
#define _Hovering			Flags[20]
#define _NavComputed 		Flags[21]

#define _RTHAltitudeHold	Flags[24]
#define _ReturnHome			Flags[25]
#define _TurnToHome			Flags[26]
#define _Proximity			Flags[27]

#define _ParametersInvalid	Flags[30]
#define _GPSTestActive		Flags[31]

// LEDs
#define AUX2M				0x01
#define BlueM				0x02
#define RedM				0x04
#define GreenM				0x08
#define AUX1M				0x10
#define YellowM				0x20
#define AUX3M				0x40
#define BeeperM				0x80

#define ALL_LEDS_ON		SwitchLEDsOn(BlueM|RedM|GreenM|YellowM)
#define AUX_LEDS_ON		SwitchLEDsOn(AUX1M|AUX2M|AUX3M)

#define ALL_LEDS_OFF	SwitchLEDsOff(BlueM|RedM|GreenM|YellowM)
#define AUX_LEDS_OFF	SwitchLEDsOff(AUX1M|AUX2M|AUX3M)

#define ARE_ALL_LEDS_OFF if( (LEDShadow&(BlueM|RedM|GreenM|YellowM))== 0 )

#define LEDRed_ON		SwitchLEDsOn(RedM)
#define LEDBlue_ON		SwitchLEDsOn(BlueM)
#define LEDGreen_ON		SwitchLEDsOn(GreenM)
#define LEDYellow_ON	SwitchLEDsOn(YellowM)
#define LEDAUX1_ON		SwitchLEDsOn(AUX1M)
#define LEDAUX2_ON		SwitchLEDsOn(AUX2M)
#define LEDAUX3_ON		SwitchLEDsOn(AUX3M)
#define LEDRed_OFF		SwitchLEDsOff(RedM)
#define LEDBlue_OFF		SwitchLEDsOff(BlueM)
#define LEDGreen_OFF	SwitchLEDsOff(GreenM)
#define LEDYellow_OFF	SwitchLEDsOff(YellowM)
#define LEDRed_TOG		if( (LEDShadow&RedM) == 0 ) SwitchLEDsOn(RedM); else SwitchLEDsOff(RedM)
#define LEDBlue_TOG		if( (LEDShadow&BlueM) == 0 ) SwitchLEDsOn(BlueM); else SwitchLEDsOff(BlueM)
#define LEDGreen_TOG	if( (LEDShadow&GreenM) == 0 ) SwitchLEDsOn(GreenM); else SwitchLEDsOff(GreenM)
#define Beeper_OFF		SwitchLEDsOff(BeeperM)
#define Beeper_ON		SwitchLEDsOn(BeeperM)
#define Beeper_TOG		if( (LEDShadow&BeeperM) == 0 ) SwitchLEDsOn(BeeperM); else SwitchLEDsOff(BeeperM)

// Bit definitions
#define Armed				(PORTAbits.RA4)

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
#define RC_MINIMUM			0
#define RC_MAXIMUM			238
#define RC_NEUTRAL			((RC_MAXIMUM-RC_MINIMUM+1)/2)

#define RC_THRES_STOP		((15L*RC_MAXIMUM)/100)		
#define RC_THRES_START		((25L*RC_MAXIMUM)/100)		

#define RC_FRAME_TIMEOUT_MS 	25
#define RC_SIGNAL_TIMEOUT_MS 	(5L*RC_FRAME_TIMEOUT_MS)
#define RC_THR_MAX 			RC_MAXIMUM

#ifdef RX6CH 
#define RC_CONTROLS 5			
#else
#define RC_CONTROLS CONTROLS
#endif //RX6CH

// ESC
#define OUT_MINIMUM			1
#define OUT_MAXIMUM			240
#define OUT_NEUTRAL			((150 * _ClkOut/(2*_PreScale1))&0xFF)    //   0%
#define _HolgerMaximum		225 

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
#define BARO_CTL			0xf4
#define BARO_ADC_MSB		0xf6
#define BARO_ADC_LSB		0xf7
#define BARO_TYPE			0xd0
//#define BARO_ID_SMD500	??
#define BARO_ID_BMP085		((uint8)(0x55))

#define BARO_TEMP_TIME_MS		10
#define BMP085_PRESS_TIME_MS 	8
#define SMD500_PRESS_TIME_MS 	35	

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
extern void AltitudeHold(int16);
extern void Descend(void);
extern void AcquireHoldPosition(void);
extern void DoNavigation(void);
extern void CheckThrottleMoved(void);
extern void DoFailsafe(void);
extern void InitNavigation(void);

// compass_altimeter.c
extern void InitCompass(void);
extern void InitHeading(void);
extern void GetHeading(void);
extern void StartBaroADC(void);
extern void ReadBaro(void);
extern void GetBaroPressure(void);
extern void InitBarometer(void);
extern void CheckForHover(void);
extern void BaroAltitudeHold(int16);

// control.c
extern void GyroCompensation(void);
extern void LimitRollSum(void);
extern void LimitPitchSum(void);
extern void LimitYawSum(void);
extern void GetGyroValues(void);
extern void ErectGyros(void);
extern void CalcGyroRates(void);
extern void DoControl(void);

extern void WaitThrottleClosedAndRTHOff(void);
extern void WaitForRxSignalAndArmed(void);
extern void UpdateControls(void);
extern void CaptureTrims(void);
extern void StopMotors(void);
extern void InitControl(void);

// irq.c
extern void ReceivingGPSOnly(uint8);
void DoRxPolarity(void);
extern void MapRC(void);
extern void InitTimersAndInterrupts(void);
extern void ReceivingGPSOnly(uint8);

// gps.c
extern void UpdateField(void);
extern int16 ConvertInt(uint8, uint8);
extern int32 ConvertLatLonM(uint8, uint8);
extern int32 ConvertUTime(uint8, uint8);
extern void ParseGPRMCSentence(void);
extern void ParseGPGGASentence(void);
extern void SetGPSOrigin(void);
extern void ParseGPSSentence(void);
extern void ResetGPSOrigin(void);
extern void PollGPS(uint8);
extern void InitGPS(void);
extern void UpdateGPS(void);

// i2c.c
void I2CDelay(void);
extern void I2CStart(void);
extern void I2CStop(void);
extern uint8 SendI2CByte(uint8);
extern uint8 RecvI2CByte(uint8);

extern void EscI2CDelay(void);
extern void EscWaitClkHi(void);
extern void EscI2CStart(void);
extern void EscI2CStop(void);
extern void SendEscI2CByte(uint8);

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

// utils.c
extern void Delay1mS(int16);
extern void Delay100mSWithOutput(int16);
extern int16 SRS16(int16, uint8);
extern int32 SRS32(int32, uint8);
extern void InitPorts(void);
extern void InitMisc(void);

extern int16 ConvertGPSToM(int16);
extern int16 ConvertMToGPS(int16);
extern int16 SlewLimit(int16, int16, int16);

extern int8 ReadEE(uint8);
extern void ReadParametersEE(void);
extern void WriteEE(uint8, int8);
extern void WriteParametersEE(uint8);
extern void UseDefaultParameters(void);
extern void UpdateParamSetChoice(void);
extern void InitParameters(void);

extern int16 Make2Pi(int16);
extern int16 MakePi(int16);
extern int16 Table16(int16, const int16 *);
extern int16 int16sin(int16);
extern int16 int16cos(int16);
extern int16 int16atan2(int16, int16);
extern int16 int16sqrt(int16);

extern void SendLEDs(void);
extern void SwitchLEDsOn(uint8);
extern void SwitchLEDsOff(uint8);
extern void LEDGame(void);
extern void CheckAlarms(void);

extern void InitStats(void);
extern void CollectStats(void);
extern void FlightStats(void);

extern void DumpTrace(void);

// bootl18f.asm
extern void BootStart(void);

// tests.c
extern void LinearTest(void);
extern uint8 ScanI2CBus(void);
extern void ReceiverTest(void);
extern void DoCompassTest(void);
extern void CalibrateCompass(void);
extern void BaroTest(void);
extern void PowerOutput(int8);
extern void CompassRun(void);
extern void GPSTest(void);
extern void ConfigureESCs(void);

extern void AnalogTest(void);extern void Program_SLA(uint8);

extern void DoLEDs(void);

// Menu strings
extern const rom uint8  SerHello[];
extern const rom uint8  SerSetup[];
extern const rom uint8 SerPrompt[];

// External Variables

enum { Clock, UpdateTimeout, RCSignalTimeout, AlarmUpdate, ThrottleIdleTimeout, FailsafeTimeout, 
      AbortTimeout, RTHTimeout, AltHoldUpdate, GPSTimeout, NavActiveTime, ThrottleUpdate, VerticalDampingUpdate, BaroUpdate, CompassUpdate};
	
enum RCControls {ThrottleC, RollC, PitchC, YawC, RTHC, CamTiltC, NavGainC}; 
#define CONTROLS (NavGainC+1)
#define MAX_CONTROLS 12 // maximum Rx channels
enum WaitGPSStates { WaitGPSSentinel, WaitNMEATag, WaitGPSBody, WaitGPSCheckSum};
enum FlightStates { Starting, Landing, Landed, InFlight};

enum ESCTypes { ESCPPM, ESCHolger, ESCX3D, ESCYGEI2C };
enum GyroTypes { ADXRS300, ADXRS150, IDG300};
enum TxRxTypes { FutabaCh3, FutabaCh2, FutabaDM8, JRPPM, JRDM9, JRDXS12, DX7AR7000, DX7AR6200, CustomTxRx };

enum TraceTags {THE, TCurrentBaroPressure,
				TRollRate,TPitchRate,TYE,
				TRollSum,TPitchSum,TYawSum,
				TAx,TAz,TAy,
				TLRIntKorr, TFBIntKorr,
				TDesiredThrottle,
				TDesiredRoll, TDesiredPitch, TDesiredYaw,
				TMFront, TMBack, TMLeft, TMRight,
				TMCamRoll, TMCamPitch
				};
#define TopTrace TFBIntKorr

enum MotorTags {Front, Left, Right, Back};
#define NoOfMotors 		4

extern uint24 mS[];

extern uint8	State;
extern uint8 	SHADOWB, MF, MB, ML, MR, MT, ME; // motor and servo outputs
extern i16u 	PPM[];
extern boolean	RCFrameOK, GPSSentenceReceived;
extern int8 	PPM_Index;
extern int24 	PrevEdge, CurrEdge;
extern i16u 	Width;
extern int16	PauseTime; // for tests
extern uint8 	GPSRxState;
extern uint8 	ll, tt, gps_ch;
extern uint8 	RxCheckSum, GPSCheckSumChar, GPSTxCheckSum;

extern int16	RC[];
extern uint16	RCGlitches;

#define MAXTAGINDEX 	4
#define GPSRXBUFFLENGTH 80
extern struct {
		uint8 	s[GPSRXBUFFLENGTH];
		uint8 	length;
	} NMEA;

extern const rom uint8 NMEATag[];

extern uint8 CurrentParamSet;

extern int16	RE, PE, YE, HE;
extern int16	REp,PEp,YEp, HEp;
extern int16	PitchSum, RollSum, YawSum;
extern int16	RollRate, PitchRate, YawRate;
extern int16	RollTrim, PitchTrim, YawTrim;
extern int16	HoldRoll, HoldPitch, HoldYaw;
extern int16	RollIntLimit256, PitchIntLimit256, YawIntLimit256, NavIntLimit256;
extern int16	GyroMidRoll, GyroMidPitch, GyroMidYaw;
extern int16	HoverThrottle, DesiredThrottle, IdleThrottle;
extern int16	DesiredRoll, DesiredPitch, DesiredYaw, DesiredHeading, Heading;
extern i16u		Ax, Ay, Az;
extern int8		LRIntKorr, FBIntKorr;
extern int8		NeutralLR, NeutralFB, NeutralUD;
extern int16 	UDAcc, UDSum, VUDComp;

// GPS
extern int16 	GPSLongitudeCorrection;
extern uint8 	GPSNoOfSats;
extern uint8 	GPSFix;
extern int16 	GPSHDilute;
extern int16 	GPSNorth, GPSEast, GPSNorthHold, GPSEastHold;
extern int16 	GPSRelAltitude;

extern int16 	SqrNavClosingRadius, NavClosingRadius, CompassOffset;

enum NavStates { PIC, HoldingStation, ReturningHome, AtHome, Descending, Navigating, Terminating };
extern uint8 	NavState;
extern uint8 	NavSensitivity;
extern int16 	AltSum, AE;

// Waypoints

typedef  struct {
	int16 N, E, A;
} WayPoint;

extern WayPoint WP[];

// Failsafes
extern int16	ThrLow, ThrHigh, ThrNeutral;
			
// Variables for barometric sensor PD-controller
extern int24	OriginBaroPressure;
extern int16	DesiredBaroPressure, CurrentBaroPressure;
extern int16	BE, BEp;
extern i16u		BaroVal;
extern int8		BaroSample;
extern int16	VBaroComp;
extern uint8	BaroType;

extern uint8	MCamRoll,MCamPitch;
extern int16	Motor[NoOfMotors];
extern int16	Rl,Pl,Yl;	// PID output values

extern boolean	Flags[32];
extern uint8	LEDCycles;		// for hover light display
extern int16	NavHoldResetCount;	
extern int8		BatteryVolts; 
extern uint8	LEDShadow;		// shadow register

extern int16	Trace[];

// Principal quadrocopter parameters

#define MAX_PARAMETERS	64		// parameters in EEPROM start at zero
enum Params {
	RollKp, 			// 01
	RollKi,				// 02
	RollKd,				// 03
	BaroTempCoeff,		// 04c not currently used
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
	MiddleUD,			// 22c
	PercentIdleThr,		// 23c
	MiddleLR,			// 24c
	MiddleFB,			// 25c
	CamPitchKp,			// 26
	CompassKp,			// 27
	BaroCompKd,			// 28c
	NavRadius,			// 29
	NavIntLimit,		// 30
	
	NavAltKp,			// 31
	NavAltKi,			// 32
	NavRTHAlt,			// 33
	NavMagVar,			// 34c
	GyroType,			// 35c
	ESCType,			// 36c
	TxRxType			// 37c
	};

#define	LastParam TxRxType

#define FlyXMode 		0
#define FlyXModeMask 	0x01

#define TxMode2 		2
#define TxMode2Mask 	0x04
#define RxSerialPPM 	3
#define RxSerialPPMMask	0x08 

#define UseGPSAlt 		5
#define	UseGPSAltMask	0x20

extern int8 P[];
extern const rom int8 ComParms[];
extern const rom int8 DefaultParams[];
extern const rom uint8 Map[CustomTxRx+1][CONTROLS];
extern const rom boolean PPMPosPolarity[];


