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

#ifdef ICD2_DEBUG
#pragma	config OSC=HS, WDT=OFF, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC 
#else
#pragma	config OSC=HS, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC 
#endif

#include "c-ufo.h"
#include "bits.h"

// The globals

#pragma udata mainvars
uint8	IGas;	
uint8	IK5, IK6, IK7;
int8 	IRoll,IPitch,IYaw;

// PID Regler Variablen
int16	RE, PE, YE;					// gyro rate error	
int16	REp, PEp, YEp;				// previous error for derivative
int16	RollSum, PitchSum, YawSum;	// integral 	
int16	RollSamples, PitchSamples;
int16	MidRoll, MidPitch, MidYaw;
int16	Ax, Ay, Az;
int8	LRIntKorr, FBIntKorr;
int16 	UDSum;

// Variables for barometric sensor PD-controller
int24	BaroBasePressure, BaroBaseTemp;
int16	BaroRelPressure, BaroRelTempCorr;
int16	VBaroComp;
uint16	BaroVal;
uint8	BaroType, BaroTemp, BaroRestarts;

uint8	LedShadow;		// shadow register
int16	AbsDirection;	// wanted heading (240 = 360 deg)
int16	CurDeviation;	// deviation from correct heading

uint8	MFront,MLeft,MRight,MBack;	// output channels
uint8	MCamRoll,MCamPitch;
int16	Ml, Mr, Mf, Mb;
int16	Rl,Pl,Yl;		// PID output values
int16	Rp,Pp,Yp;
int16	Vud;

int16	Ax, Ay, Az;

uint8	Flags[8], Flags2[8];
uint8	EscI2CFlags;

uint16	PauseTime;

uint8	IntegralCount;
int16	ThrDownCount;
uint8	DropoutCount;
uint24	RCGlitchCount;
uint8	LedCount;
uint8	BlinkCount, BlinkCycle, BaroCount;
int16	TestTimeSlot;
int8	BatteryVolts;
#pragma udata

#pragma idata params
// Principal quadrocopter parameters - MUST remain in this order
// for block read/write to EEPROM
int8	RollPropFactor		=18;
int8	RollIntFactor		=4;
int8	RollDiffFactor		=-40;
int8	BaroTempCoeff		=13;
int8	RollIntLimit		=16;
int8	PitchPropFactor		=18;
int8	PitchIntFactor		=4;	
int8	PitchDiffFactor		=-40;	 
int8	BaroThrottleProp	=2;
int8	PitchIntLimit		=16;
int8	YawPropFactor		=20;
int8	YawIntFactor		=40;
int8	YawDiffFactor		=6;
int8	YawLimit			=50;
int8	YawIntLimit			=6;
int8	ConfigParam			=0b00000000;
int8	TimeSlot			=4;	// control update interval + LEGACY_OFFSET
int8	LowVoltThres		=43;
int8	CamRollFactor		=0;	// unused
int8	LinFBIntFactor		=0;	// unused
int8	LinUDIntFactor		=8;
int8	MiddleUD			=0;
int8	MotorLowRun			= 40;
int8	MiddleLR			=0;
int8	MiddleFB			=0;
int8	CamPitchFactor		=0x44;
int8	CompassFactor		=5;
int8	BaroThrottleDiff	=4;
#pragma idata
// End Parameters

void LinearTest(void)
{
	TxText(SerLinTst);
	if( _UseLISL )
	{
		TxChar('S');
		TxChar(':');
		TxChar(HT);
		TxChar('0');
		TxChar('x');
		TxValH(ReadLISL(LISL_STATUS + LISL_READ));
		TxNextLine();
	
		ReadAccelerations();
	
		TxChar('L');
		TxChar('R');
		TxChar(':');
		TxChar(HT);	
		TxVal32(((int32)Ax*1000+512)/1024, 3, 'G');	
		TxNextLine();

		TxChar('D');
		TxChar('U');
		TxChar(':');
		TxChar(HT);	
		TxVal32(((int32)Ay*1000+512)/1024, 3, 'G');	
		TxNextLine();
	
		TxChar('F');
		TxChar('B');
		TxChar(':');
		TxChar(HT);	
		TxVal32(((int32)Az*1000+512)/1024, 3, 'G');	
		TxNextLine();
	}
	else
		TxText(SerLinErr);
} // LinearTest

// scan all slave bus addresses (read mode)
// and list found devices
uint8 ScanI2CBus(void)
{
	uint8 s;
	uint8 d = 0;

	for(s=0x10; s<=0xf6; s+=2)
	{	// use all uneven addresses for read mode
		I2CStart();
		if( SendI2CByte(s) == I2C_ACK )
		{
			d++;
			TxChar(HT);
			TxChar('0');
			TxChar('x');
			TxValH(s);
			TxNextLine();
		}
		I2CStop();

		Delay1mS(2);
	}
	return(d);
} // ScanI2CBus

// output all the signal values and the validity of signal
void ReceiverTest(void)
{
	uint8 s;
	int16 *p;
	uint16 v;

	TxNextLine();
	if( NegativePPM )
		TxText(SerPPMN);
	else
		TxText(SerPPMP);
	
	TxText(SerRxTest);
	
	// Be wary as new RC frames are being received as this
	// is being displayed so data may be from overlapping frames

	if( _NewValues )
	{
		_NewValues = false;

		p = &NewK1;
		for( s=1; s <= 7; s++ )
		{
			TxChar(s+'0');
			TxChar(':');
			TxChar(HT);
			v = *p++;
			TxChar('0');
			TxChar('x');
			TxValH((v >> 8) & 0x00ff);
			TxValH(v & 0x00ff);
			TxChar(HT);	
			TxVal32(((int32)(v & 0x00ff)*100)/_Maximum, 0, '%');
			if( ( v & 0xff00) != 0x0100 ) 
				TxText(SerFail);
			TxNextLine();
		}

		// show pause time
		TxChar('P');
		TxChar(':');
		v = 2*PauseTime;
	 	v += (uint16)TMR2_5MS * 64;	// 78 * 16*16/4 us
		TxChar(HT);
		TxVal32( v, 3, 0);		
		TxText(SerMS);
		TxText(SerRxGlitch);
		TxVal32(RCGlitchCount,0,0);
		TxNextLine();
		//if( _NoSignal )
		//	TxText(SerFail);
		//else
		//	TxText(SerOK);
	}
	else
		TxText(SerRxNN);	
} // ReceiverTest

void TogglePPMPolarity(void)
{
	uint8 i;

    Invert(ConfigParam, 4);	// toggle bit

	TxNextLine();
	if( NegativePPM )
		TxText(SerPPMN);
	else
		TxText(SerPPMP);

	NewK1=NewK2=NewK3=NewK4=NewK5=NewK6=NewK7=0xFFFF;

	PauseTime = 0;
	_NewValues = false;
	_NoSignal = true;
} // TogglePPMPolarity

void DoCompassTest(void)
{
	uint8 i;
	uint16 v;

	// 20Hz standby mode - random read?
	#define COMP_OPMODE 0b01100000
	#define COMP_MULT	16

	TxText(SerMagTst);

	// set Compass device to Compass mode 
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('G')  != I2C_ACK ) goto CTerror;
	if( SendI2CByte(0x74) != I2C_ACK ) goto CTerror;

	// select operation mode, standby mode
	if( SendI2CByte(COMP_OPMODE) != I2C_ACK ) goto CTerror;
	I2CStop();

	// set EEPROM shadow register
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('w')  != I2C_ACK ) goto CTerror;
	if( SendI2CByte(0x08) != I2C_ACK ) goto CTerror;

	// select operation mode, standby mode
	if( SendI2CByte(COMP_OPMODE) != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(2);

	// set output mode, cannot be shadowed in EEPROM :-(
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('G')  != I2C_ACK ) goto CTerror;
	if( SendI2CByte(0x4e) != I2C_ACK ) goto CTerror;

	// select heading mode (1/10th degrees)
	if( SendI2CByte(0x00) != I2C_ACK ) goto CTerror;
	I2CStop();

	// set multiple read option, can only be written to EEPROM
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('w')  != I2C_ACK ) goto CTerror;
	if( SendI2CByte(0x06) != I2C_ACK ) goto CTerror;
	if( SendI2CByte(COMP_MULT)   != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(2);

	// read a direction
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CTerror;
	if( SendI2CByte('A')  != I2C_ACK ) goto CTerror;
	I2CStop();

	Delay1mS(100);//COMPASS_TIME);

	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID+1) != I2C_ACK ) goto CTerror;
	v = (RecvI2CByte(I2C_ACK)<<8) | RecvI2CByte(I2C_NACK);
	I2CStop();

	TxChar(HT);
	TxVal32(v, 1, 0);		
	TxText(SerGrad);

	return;
CTerror:
	TxNextLine();
	I2CStop();
	TxText(SerFail);
} // DoCompassTest

// calibrate the compass by rotating the ufo through 720 deg smoothly
void CalibrateCompass(void)
{	
	TxText(SerCCalib1);

	while( !RxChar() );
	
	// set Compass device to Calibration mode 
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CCerror;
	if( SendI2CByte('C')  != I2C_ACK ) goto CCerror;
	I2CStop();

	TxText(SerCCalib2);

	while( !RxChar() );

	// set Compass device to End-Calibration mode 
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CCerror;
	if( SendI2CByte('E')  != I2C_ACK ) goto CCerror;
	I2CStop();

	TxText(SerOK);
	return;
CCerror:
	I2CStop();
	TxText(SerFail);
} // CalibrateCompass

void BaroTest(void)
{
	uint8 r;

	if ( !_UseBaro ) goto BAerror;

	if ( BaroType == BARO_ID_BMP085 )
		TxText(SerBaroBMP085);
	else
		TxText(SerBaroSMD500);
	
	if( !StartBaroADC(BARO_PRESS) ) goto BAerror;
	Delay1mS(BARO_PRESS_TIME);
	r = ReadValueFromBaro();
	TxChar(HT);
	TxText(SerBaroOK);	
	TxVal32(BaroVal, 0, 0);	
		
	if( !StartBaroADC(BaroTemp) ) goto BAerror;
	Delay1mS(BARO_TEMP_TIME);
	r = ReadValueFromBaro();
	TxText(SerBaroT);
	TxVal32(BaroVal, 0, 0);	
	TxNextLine();

	TxNextLine();

	return;
BAerror:
	TxNextLine();
	I2CStop();

	TxText(SerFail);
} // BaroTest

// flash output for a second, then return to its previous state
void PowerOutput(uint8 d)
{
	uint8 s;

	for( s=0; s < 10; s++ )	// 10 flashes (count MUST be even!)
	{
		if (d == 0 ) LedShadow ^= 0x01; else 	// toggle AUX
		if (d == 1 ) LedShadow ^= 0x02; else 	// toggle BLUE
		if (d == 2 ) LedShadow ^= 0x04; else 	// toggle RED
		if (d == 3 ) LedShadow ^= 0x08; else 	// toggle GREEN
		if (d == 4 ) LedShadow ^= 0x10; else 	// toggle AUX
		if (d == 5 ) LedShadow ^= 0x20; else 	// toggle YELLOW
		if (d == 7 ) LedShadow ^= 0x40; else 	// toggle AUX
		if (d == 7 ) LedShadow ^= 0x80; 		// toggle BEEPER

		SendLeds();	// send LEDs to bus
		Delay1mS(200);
	}		
} // PowerOutput

void AnalogTest(void)
{
	int32 v;

	TxText(SerAnTest);

	// Battery
	v = ((int24)ADC(ADCBattVoltsChan, ADCVREF5V) * 46 + 9)/17; // resolution is 0,01 Volt
	TxChar('V');
	TxChar('b');
	TxChar(':');
	TxChar(HT);	
	TxVal32(v, 2, 'V');	
	TxNextLine();

	// Roll
	v = ((int24)ADC(ADCRollChan, ADCVREF5V) * 49 + 5)/10; // resolution is 0,001 Volt
	TxChar('V'); 
	TxChar('r'); 
	TxChar(':');
	TxChar(HT);	
	TxVal32(v, 3, 'V'); 
	TxNextLine();

	// Pitch
	v = ((int24)ADC(ADCPitchChan, ADCVREF5V) * 49 + 5)/10; // resolution is 0,001 Volt
	TxChar('V');
	TxChar('p');
	TxChar(':');
	TxChar(HT);		
	TxVal32(v, 3, 'V');	
	TxNextLine();

	// Yaw
	v = ((int24)ADC(ADCYawChan, ADCVREF5V) * 49 + 5)/10; // resolution is 0,001 Volt
	TxChar('V');
	TxChar('y');
	TxChar(':');
	TxChar(HT);	
	TxVal32(v, 3, 'V');	
	TxNextLine();

	// VRef
	v = ((int24)ADC(ADCVRefChan, ADCVREF5V) * 49 + 5)/10; // resolution is 0,001 Volt	
	TxChar('V');
	TxChar('f');
	TxChar(':');
	TxChar(HT);		
	TxVal32(v, 3, 'V');	
	TxNextLine();

} // AnalogTest

void main(void)
{
	uint8	i;

	DisableInterrupts;

	InitPorts();
	InitADC();

	OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
			USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_HIGH, _B38400);
	
	OpenTimer0(TIMER_INT_OFF&T0_8BIT&T0_SOURCE_INT&T0_PS_1_16);
	OpenTimer1(T1_8BIT_RW&TIMER_INT_OFF&T1_PS_1_8&T1_SYNC_EXT_ON&T1_SOURCE_CCP&T1_SOURCE_INT);

	OpenCapture1(CAPTURE_INT_ON & C1_EVERY_FALL_EDGE); 	// capture mode every falling edge
	CCP1CONbits.CCP1M0 = NegativePPM;

	OpenTimer2(TIMER_INT_ON&T2_PS_1_16&T2_POST_1_16);		
	PR2 = TMR2_5MS;		// set compare reg to 9ms

	INTCONbits.TMR0IE = false;

	// setup flags register
	for ( i = 0; i<8; i++ )
		Flags2[i] = Flags[i] = false;

	_NoSignal = true;
	InitArrays();
	ReadParametersEE();
	ConfigParam = 0;

    ALL_LEDS_OFF;
	Beeper_OFF;

	LedBlue_ON;

//	INTCONbits.TMR0IE = true;
	INTCONbits.PEIE = true;		// enable peripheral interrupts
	EnableInterrupts;

	LedRed_ON;		// red LED on
	Delay100mSWithOutput(1);	// wait 1/10 sec until LISL is ready to talk
	IsLISLactive();
#ifdef ICD2_DEBUG
	_UseLISL = 1;	// because debugger uses RB7 (=LISL-CS) :-(
#endif

	NewK1 = NewK2 = NewK3 = NewK4 =NewK5 = NewK6 = NewK7 = 0xFFFF;

	PauseTime = 0;

	InitBarometer();

	// send hello text to serial COM
	Delay100mSWithOutput(1);	// just to see the output after powerup

	ShowSetup(true);

Delay1mS(50);

	while(1)
	{
		// turn red LED on of signal missing or invalid, green if OK
		// Yellow led to indicate linear sensor functioning.
		if( _NoSignal || !Switch )
		{
			LedRed_ON;
			LedGreen_OFF;
			if ( _UseLISL  )
				LedYellow_ON;
		}
		else
		{
			LedGreen_ON;
			LedRed_OFF;
			LedYellow_OFF;
		}

		TestTimeSlot = 10; // ignore parameter settings
		while( TestTimeSlot-- > 0 )
		{
			Delay1mS(1);	
			ProcessComCommand();
		}
		OutSignals();
	}
} // main

