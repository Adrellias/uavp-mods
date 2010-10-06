// ===============================================================================================
// =                                UAVX Quadrocopter Controller                                 =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                     http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// ===============================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

// Analog Gyros

void GetGyroValues(void)
{ 
	if ( P[GyroRollPitchType] == IDG300 ) // 500 Deg/Sec
	{
		RollRateADC = ADC(IDGADCRollChan);
		PitchRateADC = ADC(IDGADCPitchChan);
	}
	else
	{
		RollRateADC = ADC(NonIDGADCRollChan);
		PitchRateADC = ADC(NonIDGADCPitchChan);
	}

	YawRateADC = ADC(ADCYawChan);

} // GetGyroValues

void CalculateGyroRates(void)
{
	static i24u Temp;

	RollRate = RollRateADC - GyroMidRoll;
	PitchRate = PitchRateADC - GyroMidPitch;

	switch ( P[GyroRollPitchType] ) {
	case CustomGyro: break;
	case IDG300:// 500 Deg/Sec 
		RollRate = -RollRate; // adjust for reversed roll gyro sense
		break;
 	case Gyro300D3V:// LY530ALH 300deg/S 3.3V
		break;
	case Gyro300D5V:// ADXRS610/300 Melexis90609, ITG3200 or generically 300deg/S 5V
		RollRate = SRS16(RollRate, 1);	
		PitchRate = SRS16(PitchRate, 1);
		break;
	case Gyro150D5V:	 // ADXRS613/150 or generically 150deg/S 5V
		RollRate = SRS16(RollRate, 2); 
		PitchRate = SRS16(PitchRate, 2);
		break;
	} // GyroRollPitchType

	YawRate = (int16)YawRateADC - GyroMidYaw;

	switch ( P[GyroYawType] ) {
	case CustomGyro: break;
	case IDG300:
		// dual not used for Yaw 
		break;
 	case Gyro300D3V:
		break;
	case Gyro300D5V:
		YawRate = SRS16(YawRate, 1);
		break;
	case Gyro150D5V:
		YawRate = SRS16(YawRate, 2);
		break;
	} // GyroYawType

	#ifndef USE_IRQ_ADC_FILTERS
		LPFilter16(&YawRate, &YawRateF, YawFilterA);
	#endif // !USE_IRQ_ADC_FILTERS

} // GetGyroValues

void ErectGyros(void)
{
	static int8 i;
	static uint16 RollAv, PitchAv, YawAv;
	
	RollAv = PitchAv = YawAv = 0;	
    for ( i = 32; i ; i-- )
	{
		LEDRed_TOG;
		Delay100mSWithOutput(1);

		GetGyroValues();

		RollAv += RollRateADC;
		PitchAv += PitchRateADC;	
		YawAv += YawRateADC;
	}
	
	if( !F.AccelerationsValid )
	{
		RollAv += (int16)P[MiddleLR] * 2;
		PitchAv += (int16)P[MiddleFB] * 2;
	}
	
	GyroMidRoll = (int16)((RollAv + 16) >> 5);	
	GyroMidPitch = (int16)((PitchAv + 16) >> 5);
	GyroMidYaw = (int16)((YawAv + 16) >> 5);
	
	RollRate = PitchRate = YawRate = RollSum = PitchSum = YawSum = REp = PEp = YEp = 0;
	LEDRed_OFF;

} // ErectGyros

void InitGyros(void)
{
	// nothing to be done for analog gyros - could check nominal midpoints?
	YawRateF.i32 = 0;
	F.GyroFailure = false;
} // InitGyros

#ifdef TESTING

void CheckGyroFault(uint8 v, uint8 lv, uint8 hv)
{
	TxVal32(v, 1, 0);
	TxString(" (");
	TxVal32(lv,1,0);
	TxString(" >< ");
	TxVal32(hv,1,'V');
	TxString(")");
	if ( ( v < lv ) || ( v > hv ) )
		TxString(" Gyro NC or faulty?");
	TxNextLine();
} // CheckGyroFault

void GyroTest(void)
{
	int8 c, A[5], lv, hv, v;

	for ( c = 1; c <= 5; c++ )
		A[c] = ((int24)ADC(c) * 50L + 512L)/1024L;

	TxString("\r\nGyro Test\r\n");
	if ( (P[GyroRollPitchType] == IDG300) || (P[GyroRollPitchType] == Gyro300D3V) ) // 3V gyros
		{ lv = 10; hv = 20;}
	else
		{ lv = 20; hv = 30;}

	// Roll
	if ( P[GyroRollPitchType] == IDG300 )
		v = A[IDGADCRollChan];
	else
		v = A[NonIDGADCRollChan];

	TxString("Roll: \t"); 
	CheckGyroFault(v, lv, hv);

	// Pitch
	if ( P[GyroRollPitchType] == IDG300 )
		v = A[IDGADCPitchChan]; 
	else 
		v = A[NonIDGADCPitchChan]; 

	TxString("Pitch:\t");		
	CheckGyroFault(v, lv, hv);	

	// Yaw
	if ( P[GyroYawType] == Gyro300D3V )
		{ lv = 10; hv = 20;}
	else
		{ lv = 20; hv = 30;}
	
	v = A[ADCYawChan];
	TxString("Yaw:  \t");
	CheckGyroFault(v, lv, hv);	
	
} // GyroTest

#endif // TESTING








