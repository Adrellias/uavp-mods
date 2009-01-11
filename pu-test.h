#ifndef BATCHMODE
// ==============================================
// =    U.A.V.P Brushless UFO Test-Software     =
// =           Professional Version             =
// = Copyright (c) 2007 Ing. Wolfgang Mahringer =
// ==============================================
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ==============================================
// =  please visit http://www.uavp.de           =
// =               http://www.mahringer.co.at   =
// ==============================================

// C-Ufo Header File

// ==============================================
// == Global compiler switches
// ==============================================

// use this to allow debugging with MPLAB's Simulator
// #define SIMU
//
// select your board version here
// DO NOT FORGET TO SET THE SAME DEFINE IN BOTLOADER.ASM!!!
//#define BOARD_3_0
//#define BOARD_3_1
//
// Only one of the following 2 defines must be activated:
// When using 3 ADXRS300 or -150 gyros
#define OPT_ADXRS
// When using 1 ADXRS300 and 1 IDG300 gyro
//#define OPT_IDG

//
// Select what speeed controllers to use
// to use standard PPM pulse
#define ESC_PPM
// to use X3D BL controllers (courtesy to Jast :-) not yet tested!)
// #define ESC_X3D
// to use Holgers ESCs (tested by ufo-hans)
//#define ESC_HOLGER
// to use YGE I2C controllers (for standard YGEs use ESC_PPM)
//#define ESC_YGEI2C

#endif // !BATCHMODE


// Board version
#ifdef BOARD_3_0
#define Version	"3.04"
#endif
#ifdef BOARD_3_1
#define Version	"3.14"
#endif


// ==============================================
// == External variables
// ==============================================


extern	shrBank	uns8	Flags;
extern	shrBank	uns8	RecFlags;	// Interrupt save registers for FSR


#ifdef BOARD_3_1
extern	shrBank	uns8	LedShadow;	// shadow register
#endif

extern	shrBank	uns8	ConfigReg;
bit NegativePPM		@ConfigReg.0;

// the CurrK1..CurrK7 variables MUST be in this order!
extern	bank0	uns16	CurrK1,CurrK2,CurrK3,CurrK4;
extern	bank0	uns16	CurrK5,CurrK6,CurrK7;
extern	bank1	uns16	PauseTime; 
extern	bank1	uns8	TimeSlot;
extern	bank1	uns8	MVorne, MHinten, MLinks, MRechts;
extern	bank1	uns8	MCamRoll, MCamNick;
extern	bank1	uns8	EscI2CFlags;

// special locations
extern	bank1	uns16	nilgval;


#define _ClkOut		(160/4)	/* 16.0 MHz quartz */
#define _PreScale0	16	/* 1:16 TMR0 prescaler */
#define _PreScale1	8	/* 1:8 TMR1 prescaler */
#define _PreScale2	16
#define _PostScale2	16

// wegen dem dummen Compiler muss man h�ndisch rechnen :-(
#define TMR2_5MS	78	/* 1x 4ms +  */
#define TMR2_14MS	219	/* 1x 15ms = 20ms pause time */

//                    RX impuls times in 10-microseconds units
//                    vvv   ACHTUNG: Auf numerischen �berlauf achten!
#ifdef ESC_PPM
#define	_Minimum	((105* _ClkOut/(2*_PreScale1))&0xFF)	/*-100% */
#endif
#ifdef ESC_X3D
#define _Minimum	0
#endif
#ifdef ESC_HOLGER
#define _Minimum	0
#endif
#ifdef ESC_YGEI2C
#define _Minimum	0
#endif

#define _Neutral	((150* _ClkOut/(2*_PreScale1))&0xFF)    /*   0% */
#define _Maximum	((195* _ClkOut/(2*_PreScale1))&0xFF)	/*+100% */
#define _ThresStop	((113* _ClkOut/(2*_PreScale1))&0xFF)	/*-90% ab hier Stopp! */
#define _ThresStart	((116* _ClkOut/(2*_PreScale1))&0xFF)	/*-85% ab hier Start! */
#define _ProgMode	((160* _ClkOut/(2*_PreScale1))&0xFF)	/*+75% */
#define _ProgUp		((150* _ClkOut/(2*_PreScale1))&0xFF)	/*+60% */
#define _ProgDown	((130* _ClkOut/(2*_PreScale1))&0xFF)	/*-60% */

// ==============================================
// == Sanity checks of the #defines
// ==============================================

#if _Minimum >= _Maximum
#error _Minimum < _Maximum!
#endif
#if _ThresStart <= _ThresStop
#error _ThresStart <= _ThresStop!
#endif
#if (_Maximum < _Neutral)
#error Maximum < _Neutral !
#endif

#if defined OPT_ADXRS && defined OPT_IDG
#error OPT_ADXRS and OPT_IDG set!
#endif
#if !defined OPT_ADXRS && !defined OPT_IDG
#error OPT_ADXRS and OPT_IDG both not set!
#endif

#if defined BOARD_3_0 && defined BOARD_3_1
#error BOARD_3_0 and BOARD_3_1 set!
#endif
#if !defined BOARD_3_0 && !defined BOARD_3_1
#error BOARD_3_0 and BOARD_3_1 both not set!
#endif

// Parameters for UART port

#define _B9600	(_ClkOut*100000/(4*9600) - 1)
#define _B19200	(_ClkOut*100000/(4*19200) - 1)
#define _B38400	(_ClkOut*100000/(4*38400) - 1)


// Parameters for SendComText()
#define _SerHello	0
#define _SerSetup	1
#define _SerLSavail	2
#define _SerLSnone	3
#define _SerCompass	4
#define _SerAlti	5

#define _SerHelp	6
#define _SerRxTest	7
#define _SerRxNN	8
#define _SerRxRes	9
#define _SerRxFail	10
#define _SerRxOK	11

#define _SerAnTest	12
#define _SerLinTst	13
#define _SerLinErr	14

#define _SerSrvRun	15

#define _SerI2CRun	16
#define _SerI2CCnt	17

#define _SerMagTst	18

#define _SerPowTst	19
#define _SerPowAux1	20
#define _SerPowAux2	21
#define _SerPowAux3	22
#define _SerPowBlue	23
#define _SerPowRed	24
#define _SerPowGreen	25
#define _SerPowYellow	26
#define _SerPowBeep		27

#define _SerSrvOK	28
#define _SerPrompt	29

#define _SerVolt	30
#define _SerFail	31
#define _SerGrad	32
#define _SerI2CFail	33
#define _SerMS		34

#define _SerPPMP	35
#define _SerPPMN	36

#define _SerLinG	37

#define	_SerBaroOK	38
#define	_SerBaroT	39

#define _SerCCalib1 40
#define _SerCCalib2 41
#define _SerCCalib3 42
#define _SerCCalibE 43


// defines for SendComValUL
#define NKSMASK 0b0.0.000.111
#define	NKS0	0b0.0.000.000
#define	NKS1	0b0.0.000.001
#define	NKS2	0b0.0.000.010
#define	NKS3	0b0.0.000.011
#define	NKS4	0b0.0.000.100

#define LENMASK 0b0.0.111.000
#define LEN5	0b0.0.101.000
#define LEN4	0b0.0.100.000
#define LEN3	0b0.0.011.000
#define LEN2	0b0.0.010.000

#define VZ		0b0.1.000.000

// Prototypes

// Bank 0
extern	page0	void ShowSetup(uns8);
extern	page0	char RecvComChar(void);
extern	page0	void ProcessComCommand(void);
extern	page0	void SendComChar(char);
extern	page0	void SendComCRLF(void);
extern	page0	void SendComValH(uns8);
extern	page0	void SendComValUL(uns8);
extern	page1	void AnalogTest(void);
extern	page0	void OutSignals(void);
extern	page0	void TestServos(void);
extern	page0	void ConfigureESCs(void);

// Bank 1
extern	page1	void ReceiverTest(void);
extern	page1	void TogglePPMPolarity(void);

extern	page1	void Delaysec(uns8);
extern	page1	void AcqTime(void);
#ifdef BOARD_3_1
extern	page1	void SendLeds(void);
extern	page1	void SwitchLedsOn(uns8);
extern	page1	void SwitchLedsOff(uns8);
extern	page1	uns8 ScanI2CBus(void);
extern	page1	void CompassTest(void);
extern	page1	void CalibrateCompass(void);
extern	page1	void BaroTest(void);
extern	page1	void I2CStart(void);
extern	page1	void I2CStop(void);
extern	page1	uns8 SendI2CByte(uns8);
extern	page1	uns8 RecvI2CByte(uns8);
#endif
extern	page1	void PowerOutput(uns8);
extern	page2	void SendComText(uns8);

// Bank 2
extern	page2	void IsLISLactive(void);
#ifdef BOARD_3_1
extern	page2	void LinearTest(void);
#endif

// End of pu-test.h

