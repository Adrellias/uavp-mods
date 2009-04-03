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
int16	AverageYawRate, YawRate;
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

int8	RCRollNeutral, RCPitchNeutral, RCYawNeutral;

uint8	MCamRoll,MCamPitch;
int16	Motor[NoOfMotors];

int16	Rl,Pl,Yl;		// PID output values
int16	Rp,Pp,Yp;
int16	Vud;

uint8	Flags[16];

#ifdef DEBUG_SENSORS
int16	Trace[LastTrace];
#endif // DEBUG_SENSORS

uint16	PauseTime;

int16	IntegralCount, ThrDownCount, DropoutCount, LedCount, BlinkCount, BlinkCycle, BaroCount;
uint24	RCGlitchCount;
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

#if defined ESC_X3D || defined ESC_HOLGER || defined ESC_YGEI2C

void EscI2CDelay(void)
{
	Delay1TCY();
	Delay1TCY();
}

// send a start condition
void EscI2CStart(void)
{
	ESC_DIO=1;	// set SDA to input, output a high
	EscI2CDelay();
	ESC_CIO=1;	// set SCL to input, output a high
	while( ESC_SCL == 0 ) ;	// wait for line to come hi
	EscI2CDelay();
	ESC_SDA = 0;	// start condition
	ESC_DIO = 0;	// output a low
	EscI2CDelay();
	ESC_SCL = 0;
	ESC_CIO = 0;	// set SCL to output, output a low
} // EscI2CStart


void EscI2CStop(void)
{
	ESC_DIO=0;				// set SDA to output
	ESC_SDA = 0;			// output a low
	EscI2CDelay();
	ESC_CIO=1;				// set SCL to input, output a high
	while( ESC_SCL == 0 ) ;	// wait for line to come hi
	EscI2CDelay();

	ESC_DIO=1;				// set SDA to input, output a high, STOP condition
	EscI2CDelay();			// leave clock high
} // EscI2CStop

uint8 SendEscI2CByte(uint8 d)
{
	int8 i;

	for( i=0; i<8; i++)
	{
		if( (d & 0x80) != 0 )
			ESC_DIO = 1;	// switch to input, line is high
		else
		{
			ESC_SDA = 0;			
			ESC_DIO = 0;	// switch to output, line is low
		}
	
		ESC_CIO=1;			// set SCL to input, output a high
		while( ESC_SCL == 0 ) ;	// wait for line to come hi
		EscI2CDelay();
		ESC_SCL = 0;
		ESC_CIO = 0;		// set SCL to output, output a low
		d <<= 1;
	}
	ESC_DIO = 1;			// set SDA to input
	EscI2CDelay();
	ESC_CIO=1;				// set SCL to input, output a high
	while( ESC_SCL == 0 ) ;	// wait for line to come hi

	EscI2CDelay();			// here is the ACK
	i = ESC_SDA;	

	ESC_SCL = 0;
	ESC_CIO = 0;			// set SCL to output, output a low
	EscI2CDelay();
//	ESC_IO = 0;				// set SDA to output
//	ESC_SDA = 0;			// leave output low
	return(i);
} // SendEscI2CByte

#endif	// ESC_X3D || ESC_HOLGER || ESC_YGEI2C

void LinearTest(void)
{
	TxString("\r\nAccelerometer test:\r\n");
	if( _UseLISL )
	{
		ReadAccelerations();
	
		TxString("LR: \t");
		TxVal32(((int32)Ax*1000+512)/1024, 3, 'G');	
		TxNextLine();

		TxString("FB: \t");	
		TxVal32(((int32)Az*1000+512)/1024, 3, 'G');	
		TxNextLine();

		TxString("DU:   \t");	
		TxVal32(((int32)Ay*1000+512)/1024, 3, 'G');	
		TxNextLine();
	}
	else
		TxString("\r\n(Acc. not present)\r\n");
} // LinearTest

uint8 ScanI2CBus(void)
{
	int16 s;
	uint8 d = 0;

	for(s=0x10; s<=0xf6; s+=2)
	{	// use all uneven addresses for read mode
		I2CStart();
		if( SendI2CByte(s) == I2C_ACK )
		{
			d++;
			TxString("\t0x");
			TxValH(s);
			TxNextLine();
		}
		I2CStop();

		Delay1mS(2);
	}
	return(d);
} // ScanI2CBus

void ReceiverTest(void)
{
	int8 s;
	int16 *p;
	uint16 v;

	if( NegativePPM )
		TxString("\r\nNeg. Rx PPM\r\n");
	else
		TxString("\r\nPos. Rx PPM\r\n");
	
	TxString("Rx vals:\r\n");
	
	// Be wary as new RC frames are being received as this
	// is being displayed so data may be from overlapping frames

	if( _NewValues )
	{
		_NewValues = false;

		p = &NewK1;
		for( s=1; s <= 7; s++ )
		{
			TxChar(s+'0');
			TxString(":\t 0x");
			v = *p++;
			TxValH16(v);
			TxChar(HT);	
			TxVal32(((int32)(v & 0x00ff)*100)/_Maximum, 0, '%');
			if( ( v & 0xff00) != (uint16)0x0100 ) 
				TxString(" FAIL");
			TxNextLine();
		}

		// show pause time
		TxString("Gap:\t");
		v = 2*PauseTime;
	 	v += (uint16)TMR2_5MS * 64;	// 78 * 16*16/4 us
		TxVal32( v, 3, 0);		
		TxString("mS\r\nGlitches:\t");
		TxVal32(RCGlitchCount,0,0);
		TxNextLine();
	}
	else
		TxString("(no new vals)\r\n");	
} // ReceiverTest

void TogglePPMPolarity(void)
{
    Invert(ConfigParam, 4);	// toggle bit

	if( NegativePPM )
		TxString("\r\nNeg. Rx PPM\r\n");
	else
		TxString("\r\nPos. Rx PPM\r\n");

	NewK1=NewK2=NewK3=NewK4=NewK5=NewK6=NewK7=0xFFFF;
	
	RCGlitchCount = 0;
	PauseTime = 0;
	_NewValues = false;
	_NoSignal = true;
} // TogglePPMPolarity

void DoCompassTest()
{
	uint16 v;

	TxString("\r\nCompass test\r\n");

	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID+1) != I2C_ACK ) goto CTerror;
	v = ((uint16)RecvI2CByte(I2C_ACK)*256) | RecvI2CByte(I2C_NACK);
	I2CStop();

	TxVal32((int32)v, 1, 0);
	TxString("deg\r\n");		
	return;
CTerror:
	I2CStop();
	TxString("FAIL\r\n");
} // DoCompassTest

void CalibrateCompass(void)
{	// calibrate the compass by rotating the ufo through 720 deg smoothly
	TxString("\r\nCalib. compass. Press any key to cont.\r\n");

	while( !RxChar() );
	
	// set Compass device to Calibration mode 
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CCerror;
	if( SendI2CByte('C')  != I2C_ACK ) goto CCerror;
	I2CStop();

	TxString("\r\n720 deg in ~30 sec.!\r\nPress any key to cont.\r\n");
	while( !RxChar() );

	// set Compass device to End-Calibration mode 
	I2CStart();
	if( SendI2CByte(COMPASS_I2C_ID) != I2C_ACK ) goto CCerror;
	if( SendI2CByte('E')  != I2C_ACK ) goto CCerror;
	I2CStop();

	Delay1mS(50);

	TxString("OK\r\n");
	return;
CCerror:
	I2CStop();
	TxString("FAIL\r\n");
} // CalibrateCompass

void BaroTest(void)
{
	uint8 r;

	TxString("\r\nBarometer test\r\n");
	if ( !_UseBaro ) goto BAerror;

	if ( BaroType == BARO_ID_BMP085 )
		TxString("Type:\tBMP085\r\n");
	else
		TxString("Type:\tSMD500\r\n");
	
	if( !StartBaroADC(BARO_PRESS) ) goto BAerror;
	Delay1mS(BARO_PRESS_TIME);
	r = ReadValueFromBaro();
	TxString("Press: \t");	
	TxVal32((int32)BaroVal, 0, 0);
		
	if( !StartBaroADC(BaroTemp) ) goto BAerror;
	Delay1mS(BARO_TEMP_TIME);
	r = ReadValueFromBaro();
	TxString("\tTemp: ");
	TxVal32((int32)BaroVal, 0, 0);	
	TxNextLine();

	TxNextLine();

	return;
BAerror:
	I2CStop();
	TxString("FAIL\r\n");
} // BaroTest

// flash output for a second, then return to its previous state
void PowerOutput(int8 d)
{
	int8 s;
	uint8 m;

	m = 1 << d;

	for( s=0; s < 10; s++ )	// 10 flashes (count MUST be even!)
	{
		LedShadow ^= m;
		SendLeds();
		Delay1mS(200);
	}		
} // PowerOutput

void AnalogTest(void)
{
	int32 v;

	TxString("\r\nAnalog ch. test\r\n");

	// Battery
	v = ((int24)ADC(ADCBattVoltsChan, ADCVREF5V) * 46 + 9)/17; // resolution is 0,01 Volt
	TxVal32(ADCBattVoltsChan, 0, ' ');
	TxString("Batt:\t");
	TxVal32(v, 2, 'V');
	if ( v < 900 )
		TxString(" ** LIPO ALARM < 9V ** ");
	else	
	if ( v < 950 )
		TxString(" ** LOW < 9.5V ** ");
	TxNextLine();

	// Pitch
	v = ((int24)ADC(ADCPitchChan, ADCVREF5V) * 49 + 5)/10; // resolution is 0,001 Volt
	TxVal32(ADCPitchChan, 0, ' ');
	TxString("Pitch:\t");		
	TxVal32(v, 3, 'V');	
	TxNextLine();

	// Roll
	v = ((int24)ADC(ADCRollChan, ADCVREF5V) * 49 + 5)/10; // resolution is 0,001 Volt
	TxVal32(ADCRollChan, 0, ' ');
	TxString("Roll: \t"); 
	TxVal32(v, 3, 'V'); 
	TxNextLine();

	// VRef
	v = ((int24)ADC(ADCVRefChan, ADCVREF5V) * 49 + 5)/10; // resolution is 0,001 Volt
	TxVal32(ADCVRefChan, 0, ' ');	
	TxString("Ref:  \t");	
	TxVal32(v, 3, 'V');	
	TxNextLine();

	// Yaw
	v = ((int24)ADC(ADCYawChan, ADCVREF5V) * 49 + 5)/10; // resolution is 0,001 Volt
	TxVal32(ADCYawChan, 0, ' ');
	TxString("Yaw:  \t");
	TxVal32(v, 3, 'V');	
	TxNextLine();

} // AnalogTest

#ifdef ESC_YGEI2C

void Program_SLA(uint8 niaddr)
{
	uint8 nii;

	for(nii = 0x10; nii<0xF0; nii+=2)
	{
		EscI2CStart();
		if( SendEscI2CByte(nii) == 0 )
		{
			if( nii == niaddr )
			{	// controller is already programmed OK
				EscI2CStop();
				TxString("controller at SLA 0x");
				TxValH(nii);
				TxString(" is already programmed OK\r\n");
				return;
			}
			else
			{
				if( SendEscI2CByte(0x87) == 0 ) // select register 0x07
				{
					if( SendEscI2CByte(niaddr) == 0 ) // new slave address
					{
						EscI2CStop();
						TxString("controller at SLA 0x");
						TxValH(nii);
						TxString(" reprogrammed to SLA 0x");
						TxValH(niaddr);
						TxNextLine();
						return;
					}
				}
			}
		}
		EscI2CStop();
	}
	TxString("no controller found or reprogram failed\r\n");
} // Program_SLA

void ConfigureESCs(void)
{
	uint8 nic;

	for( nic=0; nic<4; nic++)
	{
		TxString("\r\nConnect ONLY ");
		switch(nic)
		{
			case 0 : TxString("front"); break;
			case 1 : TxString("back");  break;
			case 2 : TxString("right"); break;
			case 3 : TxString("left");  break;
		}
		TxString(" controller, then press any key\r\n");
		while( RxChar() == '\0' );
		TxString("\r\nprogramming the controller...\r\n");

		Program_SLA(0x62+nic+nic);
	}
} // ConfigureESCs

#endif // ESC_YGEI2C

void OutSignals(void)
{

	// no motor output

} // OutSignals

void main(void)
{
	int8	i;

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
	for ( i = 0; i<16; i++ )
		Flags[i] = false;

	_NoSignal = true;
	InitArrays();
	ReadParametersEE();
	ConfigParam = 0;

    ALL_LEDS_OFF;
	Beeper_OFF;

	LedBlue_ON;

	INTCONbits.PEIE = true;		// enable peripheral interrupts
	EnableInterrupts;

	LedRed_ON;		// red LED on

	Delay1mS(100);
	IsLISLactive();
#ifdef ICD2_DEBUG
	_UseLISL = 1;	// because debugger uses RB7 (=LISL-CS) :-(
#endif

	NewK1 = NewK2 = NewK3 = NewK4 =NewK5 = NewK6 = NewK7 = 0xFFFF;

	PauseTime = 0;

	InitBarometer();

	InitDirection();
	Delay1mS(COMPASS_TIME);

	// send hello text to serial COM
	Delay1mS(100);
	ShowSetup(true);
	Delay1mS(BARO_PRESS_TIME);
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

		ProcessComCommand();

	}
} // main

