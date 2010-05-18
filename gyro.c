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

// Gyros

#include "uavx.h"

void CompensateRollPitchGyros(void);
void GetRollPitchGyroValues(void);
void GetYawGyroValue(void);
void CheckGyroFault(uint8, uint8, uint8);
void ErectGyros(void);
void CalcGyroRates(void);
void GyroTest(void);
void InitGyros(void);

int16	GyroMidRoll, GyroMidRollBy2, GyroMidPitch, GyroMidPitchBy2, GyroMidYaw;
int16	RollRate, PitchRate, YawRate;
int16	RollRateADC, PitchRateADC, YawRateADC;

#ifdef GYRO_ITG3200

// ITG3200 Register Defines
#define ITG_WHO		0x00
#define	ITG_SMPL	0x15
#define ITG_DLPF	0x16
#define ITG_INT_C	0x17
#define ITG_INT_S	0x1A
#define	ITG_TMP_H	0x1B
#define	ITG_TMP_L	0x1C
#define	ITG_GX_H	0x1D
#define	ITG_GX_L	0x1E
#define	ITG_GY_H	0x1F
#define	ITG_GY_L	0x20
#define ITG_GZ_H	0x21
#define ITG_GZ_L	0x22
#define ITG_PWR_M	0x3E

#define ITG_I2C_ID 	0xD2
#define ITG_R 		(ITG_I2C_ID+1)	
#define ITG_W 		ITG_I2C_ID

// depending on orientation of chip
#define ITG_ROLL_H	ITG_GX_H
#define ITG_ROLL_L	ITG_GX_L

#define ITG_PITCH_H	ITG_GY_H
#define ITG_PITCH_L	ITG_GY_L

#define ITG_YAW_H	ITG_GZ_H
#define ITG_YAW_L	ITG_GZ_L

void ITG3200ViewRegisters(void);
void BlockReadITG3200(void);
uint8 ReadByteITG3200(uint8);
void WriteByteITG3200(uint8, uint8);
void InitITG3200(void);

void BlockReadITG3200(void)
{
	static uint8 G[6], b, Temp, GX, GY, GZ;

	I2CStart();
	if( SendI2CByte(ITG_W) != I2C_ACK ) goto SGerror;

	if( SendI2CByte(ITG_GX_H) != I2C_ACK ) goto SGerror;

	I2CStart();	
	if( SendI2CByte(ITG_R) != I2C_ACK ) goto SGerror;
	RecvI2CString(G, 6);
	I2CStop();

	// zzz need to get orientation
	GX = (G[0] << 8) | G[1];
	GY = (G[2] << 8) | G[3];
	GZ = (G[4] << 8) | G[5];

	RollRateADC = GX;
	PitchRateADC = -GY;
	YawRateADC = -GZ;

	return;	

SGerror:
	I2CStop();
	// GYRO FAILURE - FATAL
	Stats[GyroS]++;
	F.GyroFailure = true;
	return;
} // BlockReadITG3200

uint8 ReadByteITG3200(uint8 address)
{
	uint8 data;
	boolean b;
		
	I2CStart();
	if( SendI2CByte(ITG_W) != I2C_ACK ) goto SGerror;
	if( SendI2CByte(address) != I2C_ACK ) goto SGerror;

	I2CStart();
	if( SendI2CByte(ITG_R) != I2C_ACK ) goto SGerror;	
	data = RecvI2CByte(I2C_NACK);
	I2CStop();
	
	return ( data );

SGerror:
	I2CStop();
	// GYRO FAILURE - FATAL
	Stats[GyroS]++;
	F.GyroFailure = true;
	return ( 0 );
} // ReadByteITG3200

void WriteByteITG3200(uint8 address, uint8 data)
{
	I2CStart();	// restart
	if( SendI2CByte(ITG_W) != I2C_ACK ) goto SGerror;
	if( SendI2CByte(address) != I2C_ACK ) goto SGerror;
	if(SendI2CByte(data) != I2C_ACK ) goto SGerror;
	I2CStop();
	return;

SGerror:
	I2CStop();
	// GYRO FAILURE - FATAL
	Stats[GyroS]++;
	F.GyroFailure = true;
	return;
} // WriteByteITG3200

void InitITG3200(void)
{
	F.GyroFailure = false; // reset optimistically!

	WriteByteITG3200(ITG_PWR_M, 0x80);			// Reset to defaults
	WriteByteITG3200(ITG_SMPL, 0x00);			// continuous update
	WriteByteITG3200(ITG_DLPF, 0b00011001);		// 188Hz, 2000deg/S
	WriteByteITG3200(ITG_INT_C, 0b00000000);	// no interrupts
	WriteByteITG3200(ITG_PWR_M, 0b00000001);	// X Gyro as Clock Ref.
} // InitITG3200

void GetRollPitchGyroValues(void)
{ // invoked TWICE per control cycle so roll/pitch "rates" are double actual

	BlockReadITG3200();

	RollRate = RollRate + RollRateADC;
	PitchRate = PitchRate + PitchRateADC;

} // GetRollPitchGyroValues

void GetYawGyroValue(void)
{
	// YawRateADC already acquired in block read for roll/pitch
	YawRate = YawRateADC - GyroMidYaw; 
	YawRate = SRS16(YawRate, 1);

} // GetYawGyroValue

#ifdef TESTING

void CheckGyroFault(uint8 v, uint8 lv, uint8 hv)
{
	// not used for ITG-3000
} // CheckGyroFault

void GyroTest(void)
{
	TxString("\r\nITG3200 3 axis I2C Gyro Test\r\n");
	TxString("WHO_AM_I  \t"); TxValH(ReadByteITG3200(ITG_WHO)); TxNextLine();
//	Delay1mS(1000);
	TxString("SMPLRT_DIV\t"); TxValH(ReadByteITG3200(ITG_SMPL)); TxNextLine();
	TxString("DLPF_FS   \t"); TxValH(ReadByteITG3200(ITG_DLPF)); TxNextLine();
	TxString("INT_CFG   \t"); TxValH(ReadByteITG3200(ITG_INT_C)); TxNextLine();
	TxString("INT_STATUS\t"); TxValH(ReadByteITG3200(ITG_INT_S)); TxNextLine();
	TxString("TEMP      \t"); TxVal32( (ReadByteITG3200(ITG_TMP_H)<<8) | ReadByteITG3200(ITG_TMP_L),0,0); TxNextLine();
	TxString("GYRO_X    \t"); TxVal32( (ReadByteITG3200(ITG_GX_H)<<8) | ReadByteITG3200(ITG_GX_L),0,0); TxNextLine();
	TxString("GYRO_Y    \t"); TxVal32( (ReadByteITG3200(ITG_GY_H)<<8) | ReadByteITG3200(ITG_GY_L),0,0); TxNextLine();
	TxString("GYRO_Z    \t"); TxVal32( (ReadByteITG3200(ITG_GZ_H)<<8) | ReadByteITG3200(ITG_GZ_L),0,0); TxNextLine();
	TxString("PWR_MGM   \t"); TxValH(ReadByteITG3200(ITG_PWR_M)); TxNextLine();
	TxNextLine();
	if ( F.GyroFailure )
		TxString("Test FAILED\r\n");
	else
		TxString("Test OK\r\n");

} // GyroTest

#endif // TESTING

void InitGyros(void)
{
	InitITG3200();
} // InitGyros

#else

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

void InitGyros(void)
{
	// nothing to be done for analog gyros - could check nominal midpoints?
	F.GyroFailure = false;
} // InitGyros

#endif // GYRO_ITG3200

void ErectGyros(void)
{
	static uint8 i;
	static uint16 RollAv, PitchAv, YawAv;
	
	RollAv = PitchAv = YawAv = 0;	
    for ( i = 32; i ; i-- )
	{
		LEDRed_TOG;
		Delay100mSWithOutput(GYRO_ERECT_DELAY);

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

#define AccFilter NoFilter

void CompensateRollPitchGyros(void)
{
	static int16 Temp;
	static int24 Temp24;
	static int16 LRGrav, LRDyn, FBGrav, FBDyn;
	static int16 NewLRAcc, NewDUAcc, NewFBAcc;

	if( F.AccelerationsValid )
	{
		ReadAccelerations();

		if ( F.UsingFlatAcc ) // chip up and twisted over
		{
			NewLRAcc = -Ax.i16;	
			NewFBAcc = Ay.i16;
			NewDUAcc = Az.i16;
		}
		else
		{	
			NewLRAcc = Ax.i16;
			NewDUAcc = Ay.i16;
			NewFBAcc = Az.i16;
		}

		// NeutralLR, NeutralFB, NeutralDU pass through UAVPSet 
		// and come back as MiddleLR etc.

		NewLRAcc -= (int16)P[MiddleLR];
		NewFBAcc -= (int16)P[MiddleFB];
		NewDUAcc -= (int16)P[MiddleDU];

		NewDUAcc -= 1024L;	// subtract 1g - not corrrect for other than level
						// ??? could check for large negative Acc => upside down?

		LRAcc = AccFilter((int32)LRAcc, (int32)NewLRAcc);
		DUAcc = AccFilter((int32)DUAcc, (int32)NewDUAcc);
		FBAcc = AccFilter((int32)FBAcc, (int32)NewFBAcc);

		#if ( defined QUADROCOPTER | defined TRICOPTER )
			#ifdef TRICOPTER
			if ( F.UsingAltOrientation ) // K1 forward
			{
				FBAcc = -FBAcc;	
				LRAcc = -LRAcc;
			}
			#else
			if ( F.UsingAltOrientation )
			{
				// "Real" LR =  0.707 * (LR + FB), FB = 0.707 * ( FB - LR )
				Temp24 = FBAcc + LRAcc;	
				FBAcc -= LRAcc;		
				FBAcc = (FBAcc * 7L)/10L;
				LRAcc = (Temp * 7L)/10L;
			}
			#endif // TRICOPTER
		#endif // QUADROCOPTER | TRICOPTER 
			
		// Roll

		// static compensation due to Gravity
		LRGrav = -SRS16(RollSum * (int16)P[GravComp], 5); 
	
		// dynamic correction of moved mass
		#ifdef DISABLE_DYNAMIC_MASS_COMP_ROLL
		LRDyn = 0;
		#else
		LRDyn = RollRate;	
		#endif

		// correct DC level of the integral
		LRIntCorr = SRS16(LRAcc + LRGrav + LRDyn, 3); // / 10;
		LRIntCorr = Limit(LRIntCorr, -(int16)P[CompSteps], (int16)P[CompSteps]); 
	
		// Pitch

		// static compensation due to Gravity
		FBGrav = -SRS16(PitchSum * (int16)P[GravComp], 5); 
	
		// dynamic correction of moved mass		
		#ifdef DISABLE_DYNAMIC_MASS_COMP_PITCH
		FBDyn = 0;
		#else
		FBDyn = PitchRate;
		#endif

		// correct DC level of the integral	
		FBIntCorr = SRS16(FBAcc + FBGrav + FBDyn, 3); // / 10;
		FBIntCorr = Limit(FBIntCorr, -(int16)P[CompSteps], (int16)P[CompSteps]); 
	}	
	else
		LRIntCorr = FBIntCorr = DUAcc = 0;

} // CompensateRollPitchGyros

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
		{ 	// 1.0
			// Average of two readings
			RollRate = ( RollRate >> 1 ) - GyroMidRoll;	
			PitchRate = ( PitchRate >> 1 ) - GyroMidPitch;
	
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
