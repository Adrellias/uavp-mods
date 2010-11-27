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
	if ( P[GyroType] == IDG300Gyro ) // 500 Deg/Sec
	{
		GyroADC[Roll] = ADC(IDGADCRollChan);
		GyroADC[Pitch] = ADC(IDGADCPitchChan);
	}
	else
	{
		GyroADC[Roll] = ADC(NonIDGADCRollChan);
		GyroADC[Pitch] = ADC(NonIDGADCPitchChan);
	}

	GyroADC[Yaw] = ADC(ADCYawChan);

} // GetGyroValues

void CalculateGyroRates(void)
{
	static i24u Temp;

	Rate[Roll] = GyroADC[Roll] - GyroNeutral[Roll];
	Rate[Pitch] = GyroADC[Pitch] - GyroNeutral[Pitch];
	Rate[Yaw] = (int16)GyroADC[Yaw] - GyroNeutral[Yaw];

	switch ( P[GyroType] ) {
	case IDG300Gyro:// 500 Deg/Sec 
		Rate[Roll] = -Rate[Roll] * 2; // adjust for reversed roll gyro sense
		Rate[Pitch] *= 2;
		Rate[Yaw] = SRS16(Rate[Yaw], 1); // ADXRS150 assumed
		break;
 	case LY530Gyro:// generically 300deg/S 3.3V
		Rate[Yaw] = SRS16(Rate[Yaw], 1);
		break;
	case MLX90609Gyro:// generically 300deg/S 5V
		Rate[Roll] = SRS16(Rate[Roll], 1);	
		Rate[Pitch] = SRS16(Rate[Pitch], 1);
		Rate[Yaw] = SRS16(Rate[Yaw], 2);
		break;
	case ADXRS300Gyro:// ADXRS610/300 300deg/S 5V
		Rate[Roll] = SRS16(Rate[Roll], 1);	// scaling is not correct
		Rate[Pitch] = SRS16(Rate[Pitch], 1);
		Rate[Yaw] = SRS16(Rate[Yaw], 2);
		break;
	case ITG3200Gyro:// ITG3200
		Rate[Roll] = SRS16(Rate[Roll], 1);	
		Rate[Pitch] = SRS16(Rate[Pitch], 1);
		Rate[Yaw] = SRS16(Rate[Yaw], 2);
		break;
	case ADXRS150Gyro:	 // ADXRS613/150 or generically 150deg/S 5V
		Rate[Roll] = SRS16(Rate[Roll], 2); 
		Rate[Pitch] = SRS16(Rate[Pitch], 2);
		Rate[Yaw] = SRS16(Rate[Yaw], 3);
		break;
	default:;
	} // GyroType

	LPFilter16(&Rate[Yaw], &YawRateF, YawFilterA);

} // GetGyroValues

void ErectGyros(void)
{
	static int8 i, g;
	static uint16 Av[3];

	for ( g = 0; g < (int8)3; g++ )	
		Av[g] = 0;

    for ( i = 32; i ; i-- )
	{
		LEDRed_TOG;
		Delay100mSWithOutput(1);

		GetGyroValues();

		Av[Roll] += GyroADC[Roll];
		Av[Pitch] += GyroADC[Pitch];	
		Av[Yaw] += GyroADC[Yaw];
	}
	
	if( !F.AccelerationsValid )
	{
		Av[Roll] += (int16)P[MiddleLR] * 2;
		Av[Pitch] += (int16)P[MiddleFB] * 2;
	}
	
	for ( g = 0; g < (int8)3; g++ )
	{
		GyroNeutral[g] = (int16)((Av[g] + 16) >> 5);	
		Rate[g] =  Ratep[g] = Angle[g] = 0;
	}
 
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
	if ( (P[GyroType] == IDG300Gyro ) || (P[GyroType] == LY530Gyro ) ) // 3V gyros
		{ lv = 10; hv = 20;}
	else
		{ lv = 20; hv = 30;}

	// Roll
	if ( P[GyroType] == IDG300Gyro )
		v = A[IDGADCRollChan];
	else
		v = A[NonIDGADCRollChan];

	TxString("Roll: \t"); 
	CheckGyroFault(v, lv, hv);

	// Pitch
	if ( P[GyroType] == IDG300Gyro )
		v = A[IDGADCPitchChan]; 
	else 
		v = A[NonIDGADCPitchChan]; 

	TxString("Pitch:\t");		
	CheckGyroFault(v, lv, hv);	

	// Yaw
	if ( (P[GyroType] == IDG300Gyro ) || (P[GyroType] == LY530Gyro ) ) // 3V gyros
		{ lv = 10; hv = 20;}
	else
		{ lv = 20; hv = 30;}
	
	v = A[ADCYawChan];
	TxString("Yaw:  \t");
	CheckGyroFault(v, lv, hv);	
	
} // GyroTest

#endif // TESTING








