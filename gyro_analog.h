// =================================================================================================
// =                                  UAVX Quadrocopter Controller                                 =
// =                             Copyright (c) 2008 by Prof. Greg Egan                             =
// =                   Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                       http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// =================================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even 
//    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//    General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

// Analog Gyros

void GetRollPitchGyroValues(void)
{ // invoked TWICE per control cycle so roll/pitch "rates" are double actual
	if ( P[GyroRollPitchType] == IDG300 ) // 500 Deg/Sec
	{
		RollRateADC = (int16)ADC(IDGADCRollChan);
		PitchRateADC = (int16)ADC(IDGADCPitchChan);
	}
	else
	{
		RollRateADC = (int16)ADC(NonIDGADCRollChan);
		PitchRateADC = (int16)ADC(NonIDGADCPitchChan);
	}

	RollRate = RollRate + RollRateADC;
	PitchRate = PitchRate + PitchRateADC;

} // GetRollPitchGyroValues

void GetYawGyroValue(void)
{
	YawRateADC = ADC(ADCYawChan);
	YawRate = YawRateADC - GyroMidYaw; 

	if ( P[GyroYawType] == Gyro150D5V )
		YawRate = SRS16(YawRate, 2);
	else // 
		if ( P[GyroYawType] == Gyro300D5V )
			YawRate = SRS16(YawRate, 1);
	//	else // Gyro300D3V  
	//		YawRate = YawRate;

} // GetYawGyroValue

void CalcGyroRates(void)
{
	static int16 Temp;

	// RollRate & PitchRate hold the sum of 2 consecutive conversions
	// 300 Deg/Sec is the "reference" gyro full scale rate

	if ( P[GyroRollPitchType] == IDG300 ) // 2.0
	{ 	// 500 Deg/Sec, 5/3.3 FS => ~2.5 so do not average 
		RollRate -= GyroMidRollBy2;
		PitchRate -= GyroMidPitchBy2;
		RollRate = -RollRate;			// adjust for reversed roll gyro sense
 	}
	else
		if ( P[GyroRollPitchType] == Gyro300D3V ) // 2.0
		{ 	// 300 Deg/Sec, 5/2.4 FS => ~2.0 so do not average 
			RollRate -= GyroMidRollBy2;
			PitchRate -= GyroMidPitchBy2;
	 	}
		else
		{ 	// 1.0 - includes ITG-3200
			// Average of two readings
			RollRate = SRS16( RollRate, 1) - GyroMidRoll;	
			PitchRate = SRS16( PitchRate, 1) - GyroMidPitch;
	
			if ( P[GyroRollPitchType] == Gyro150D5V )	// 0.5
			{ // 150 Deg/Sec or 0.5
				RollRate = SRS16(RollRate, 1); 
				PitchRate = SRS16(PitchRate, 1);
			}
		}

	#if ( defined QUADROCOPTER | defined TRICOPTER )
		#ifdef TRICOPTER
		if ( F.UsingAltOrientation ) // K1 forward
		{
			RollRate = -RollRate;
			PitchRate = -PitchRate;
		}		
		#else
		if ( F.UsingAltOrientation )
		{
			// "Real" Roll = 0.707 * (P + R), Pitch = 0.707 * (P - R)
			Temp = RollRate + PitchRate;	
			PitchRate -= RollRate;	
			RollRate = (Temp * 7L)/10L;
			PitchRate = (PitchRate * 7L)/10L; 
		}
		#endif // TRICOPTER
	#endif // QUADROCOPTER | TRICOPTER 
	
	// Yaw is sampled only once every frame
	GetYawGyroValue();	

} // CalcGyroRates

void ErectGyros(void)
{
	static uint8 i;
	static uint16 RollAv, PitchAv, YawAv;
	
	RollAv = PitchAv = YawAv = 0;	
    for ( i = 32; i ; i-- )
	{
		LEDRed_TOG;
		Delay100mSWithOutput(1);

		GetRollPitchGyroValues();
		GetYawGyroValue();

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

	// compute here to remove from main control loop
	GyroMidRollBy2 = GyroMidRoll * 2;
	GyroMidPitchBy2 = GyroMidPitch * 2;
	
	RollRate = PitchRate = YawRate = RollSum = PitchSum = YawSum = REp = PEp = YEp = 0;
	LEDRed_OFF;

} // ErectGyros

void InitGyros(void)
{
	// nothing to be done for analog gyros - could check nominal midpoints?
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
	uint8 c, A[5], lv, hv, v;

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








