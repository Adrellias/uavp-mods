// ===============================================================================================
// =                                UAVX Quadrocopter Controller                                 =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =		  	  DCM Formulation of Gyro Drift Compensation due to W. Premerlani                =
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

// Gyros

#include "uavx.h"

void ShowGyroType(uint8);

void ApplyMagic(void);
void AttitudeFailsafeEstimate(void);

void Normalise(void); 
void DriftCorrection(void); 
void MUpdate(void); 
void EulerAngles(void);

void CompensateRollPitchGyros(void);
void GetGyroValues(void);
void CalculateGyroRates(void);
void CheckGyroFault(uint8, uint8, uint8);
void ErectGyros(void);
void GyroTest(void);
void InitGyros(void);

int16 GyroADC[3], GyroMid[3], Gyro[3];
i32u YawGyroF;

#ifdef GYRO_ITG3200
	#include "gyro_itg3200.h"
#else // GYRO_ITG3200
	#ifdef GYRO_WII
		#include "gyro_wii.h"
	#else
		#include "gyro_analog.h"
	#endif // GYRO_WII
#endif // GYRO_ITG3200

void ShowGyroType(uint8 T)
{
	switch ( P[T] ) {
		case Gyro300D5V:TxString("ADXRS610/300 or MLX90609\r\n"); break;
		case Gyro150D5V:TxString("ADXRS613/150\r\n"); break;
		case IDG300:TxString("IDG300\r\n"); break;
		case Gyro300D3V:TxString("ST-AY530\r\n"); break;
		case CustomGyro:TxString("Custom\r\n"); break;
	}
} // ShowGyroType

#ifdef USE_DCM

#define Kp_RollPitch 0.02
#define Ki_RollPitch 0.00002
#define Kp_Yaw 1.2
#define Ki_Yaw 0.00002

const float GyroToRadian[5] = { 0.007149, 0.011796, 0.023841, 0.017872, 0.044680 };

float   dT, dTmS;

void ApplyMagic(void)
{ 	// rescale to original units
	// Gyros 11.976;

	Rate[Roll] /= 12;
	Rate[Pitch] /= 12;
	Rate[Yaw] /= 24;
	
	Angle[Roll] *=2;
	Angle[Pitch] *=2;

} // ApplyMagic

void AttitudeFailsafeEstimate(void)
{
	static uint8 i;
	static float Temp;

	for ( i = 0; i < (uint8)3; i++ )
	{
		Temp = (float)Gyro[i] * GyroToRadian[P[GyroRollPitchType]] * 1000.0;
		Angle[i] += (int16)( Temp * dTmS);
		Rate[i] = (int16)Temp;
	}	

 	Temp = (float)Gyro[Yaw] * GyroToRadian[P[GyroYawType]] * 1000.0;
	Angle[Yaw] += (int16)((float)Gyro[Yaw] * GyroToRadian[P[GyroYawType]] * dTmS);
	Rate[Yaw] = (int16)Temp;

} // AttitudeFailsafeEstimate

#ifdef TESTING

void CompensateRollPitchGyros(void)
{
	static uint8 i;
	static int24 Now;

	Now = mSClock();
	dTmS = (float)(Now - mS[AttitudeUpdate]);
	mS[AttitudeUpdate] = Now;

	DoAccelerations();
		
	AttitudeFailsafeEstimate(); // not enough space for DCM

	ApplyMagic();

} // CompensateRollPitchGyros

#else

//_____________________________________________________________________
   
float   RollPitchError[3] = {0,0,0}; 
float   YawError[3] = {0,0,0};
float   AccV[3] = {0,0,0};         
float   GyroV[3] = {0,0,0};
float   OmegaV[3] = {0,0,0}; // corrected gyro data
float   OmegaP[3] = {0,0,0}; // proportional correction
float   OmegaI[3] = {0,0,0}; // integral correction
float   Omega[3] = {0,0,0};
float   DCM[3][3] = {{1,0,0 },{0,1,0} ,{0,0,1}}; 
float   DCMUpdate[3][3] = {{0,1,2},{ 3,4,5} ,{6,7,8}}; //Gyros here
float   TempM[3][3] = {{0,0,0},{0,0,0},{ 0,0,0}};

void Normalise(void) // W. Premerlani
{
	static float Error = 0;
	static float Renorm = 0.0;
	static boolean Problem;
	static uint8 r;

	Error= -VDot(&DCM[0][0], &DCM[1][0]) * 0.5;			//eq.19

	VScale(&TempM[0][0], &DCM[1][0], Error);   			//eq.19
	VScale(&TempM[1][0], &DCM[0][0], Error);      		//eq.19

	VAdd(&TempM[0][0], &TempM[0][0], &DCM[0][0]);   	//eq.19
	VAdd(&TempM[1][0], &TempM[1][0], &DCM[1][0]);  		//eq.19

	VCross(&TempM[2][0],&TempM[0][0], &TempM[1][0]);  	// c= a * b eq.20

	Problem = false;
	for ( r = 0; r < (uint8)3; r++ )
	{
		Renorm = VDot(&TempM[r][0],&TempM[r][0]);
		if ( (Renorm <  1.5625) && (Renorm > 0.64) ) 
			Renorm = 0.5 * (3.0 - Renorm);       		//eq.21
		else 
			if ( (Renorm < 100.0) && (Renorm > 0.01) ) 
				Renorm = 1.0 / sqrt( Renorm );   
			else 
				Problem = true;

		VScale(&DCM[r][0], &TempM[r][0], Renorm);
	}

	if ( Problem ) // Divergent - force to initial conditions and hope!
	{
		DCM[0][0] = 1.0;
		DCM[0][1] = 0.0;
	    DCM[0][2] = 0.0;
	    DCM[1][0] = 0.0;
	    DCM[1][1] = 1.0;
	    DCM[1][2] = 0.0;
	    DCM[2][0] = 0.0;
	    DCM[2][1] = 0.0;
	    DCM[2][2] = 1.0; 
	}

} // Normalise

void DriftCorrection(void) // W. Premerlani
{ 
	static float ScaledOmegaP[3], ScaledOmegaI[3];
	static float AccMagnitude, AccWeight;
	static float YawError[3];
 	static float ErrorCourse, HeadingSin, HeadingCos;
	static float Constrain;
	
	AccMagnitude = sqrt(AccV[0]*AccV[0] + AccV[1]*AccV[1] + AccV[2]*AccV[2]);
	AccMagnitude = AccMagnitude / (float)GRAVITY;
	
	// dynamic weighting of Accelerometer info (reliability filter)
	// weight for Accelerometer info ( < 0.5G = 0.0, 1G = 1.0 , > 1.5G = 0.0)
	// AccWeight = constrain(1 - 2 * abs(1 - AccMagnitude), 0, 1); 

	Constrain = 1.0 - 2.0 * fabs(1.0 - AccMagnitude);
	if ( Constrain < 0.0 )
		AccWeight = 0.0;
	else
		if ( Constrain > 1.0 )
			AccWeight = 1.0;
	    else
	        AccWeight = Constrain; 
	
	VCross(&RollPitchError[0], &AccV[0], &DCM[2][0]); //adjust the reference ground
	VScale(&OmegaP[0], &RollPitchError[0], Kp_RollPitch * AccWeight);
	
	VScale(&ScaledOmegaI[0], &RollPitchError[0], Ki_RollPitch * AccWeight);
	VAdd(&OmegaI[0], &OmegaI[0], &ScaledOmegaI[0]); 

	// Yaw - drift correction based on compass heading
  	HeadingCos = (float)int16cos(Heading)/256.0;
 	HeadingSin = (float)int16sin( Heading)/256.0;
	ErrorCourse = ( DCMUpdate[0][0] * HeadingSin ) - ( DCMUpdate[1][0] * HeadingCos );  
  	VScale(YawError, &DCMUpdate[2][0], ErrorCourse ); 

  	VScale(&ScaledOmegaP[0], &YawError[0], Kp_Yaw ); 
  	VAdd(&OmegaP, &OmegaP, &ScaledOmegaP);

  	VScale(&ScaledOmegaI[0], &YawError[0], Ki_Yaw ); 
  	VAdd(&OmegaI, &OmegaI, &ScaledOmegaI);
    	
} // DriftCorrection

void MUpdate(void) // W. Premerlani
{
  	static uint8 i, j, k;
	static float op[3];
    
	GyroV[Roll] = (float)Gyro[Roll] * GyroToRadian[P[GyroRollPitchType]];
	GyroV[Pitch] = (float)Gyro[Pitch] * GyroToRadian[P[GyroRollPitchType]];
	GyroV[Yaw] = (float)Gyro[Yaw] * GyroToRadian[P[GyroYawType]];

	for ( i = 0; i < (uint8)3 ; i++)
	    AccV[i] = (float)Acc[i]; // includes 1G on Z
	
	VAdd(&Omega[0], &GyroV[0], &OmegaI[0]);
	VAdd(&OmegaV[0], &Omega[0], &OmegaP[0]); 

#define CORRECT_DRIFT
#ifdef CORRECT_DRIFT         
	DCMUpdate[0][0] = 0.0;
	DCMUpdate[0][1] = -dT * OmegaV[2];	//-z
	DCMUpdate[0][2] = dT * OmegaV[1];   // y
	DCMUpdate[1][0] = dT * OmegaV[2];   // z
	DCMUpdate[1][1] = 0.0;
	DCMUpdate[1][2] = -dT * OmegaV[0]; 	//-x
	DCMUpdate[2][0] = -dT * OmegaV[1]; 	//-y
	DCMUpdate[2][1] = dT * OmegaV[0];   // x
	DCMUpdate[2][2] = 0.0;
#else // no drift correction
	DCMUpdate[0][0] = 0.0;
	DCMUpdate[0][1] = -dT * GyroV[2];   //-z
	DCMUpdate[0][2] = dT * GyroV[1];  	// y
	DCMUpdate[1][0] = dT * GyroV[2]; 	// z
	DCMUpdate[1][1] = 0.0;
	DCMUpdate[1][2] = -dT * GyroV[0];
	DCMUpdate[2][0] = -dT * GyroV[1];
	DCMUpdate[2][1] = dT * GyroV[0];
	DCMUpdate[2][2] = 0.0;
#endif // CORRECT_DRIFT

	for( i = 0; i < (uint8)3; i++ ) // matrix multiply
		for ( j = 0; j < (uint8)3; j++ )
	    {
	       for ( k = 0; k < (uint8)3; k++ )
	         op[k] = DCM[i][k] * DCMUpdate[k][j];
	
	       TempM[i][j] = op[0] + op[1] + op[2];
		}

	for ( i = 0; i < (uint8)3; i++ )
		for (j = 0; j < (uint8)3; j++ )
			DCM[i][j] += TempM[i][j];

} // MUpdate

float DCMAnglep[3] = { 0.0, 0.0, 0.0 };

void EulerAngles(void)
{
	static uint8 i;
	static float TempM[3];

	TempM[Roll] = atan2(DCM[2][1], DCM[2][2]);
	TempM[Pitch] = -asin(DCM[2][0]);
	TempM[Yaw] = atan2(DCM[1][0], DCM[0][0]);

	for ( i = 0; i < (uint8)3; i++ )
	{
		Rate[i] = (int16)( ( ( TempM[i] - DCMAnglep[i] ) * 83.0 ) / dT ); //1000.0
		DCMAnglep[i] = TempM[i];
		Angle[i] = (int16) (TempM[i] * 2000.0);  // 1000
	} 

} // EulerAngles

void CompensateRollPitchGyros(void)
{
	static int24 Now;

	DoAccelerations();

	Now = mSClock();
	dTmS = (float)(Now - mS[AttitudeUpdate]);
	mS[AttitudeUpdate] = Now;
	dT = dTmS * 0.001;

	if ( F.AccelerationsValid )
	{
		MUpdate();
   		Normalise();
    	DriftCorrection();
		EulerAngles(); 
	}
	else
		AttitudeFailsafeEstimate();

//	ApplyMagic();

} // CompensateRollPitchGyros

#endif // TESTING

#else

void CompensateRollPitchGyros(void)
{
	#define GRAV_COMP 11L
	#define GYRO_COMP_STEP 4 // was 1!

	static int16 Temp;
	static int24 Temp24;
	static int16 Grav[2], Dyn[2];

	DoAccelerations();

	if( F.AccelerationsValid )
	{
		// Pitch

		// static compensation due to Gravity
		Grav[BF] = -SRS16(Angle[Pitch] * GRAV_COMP, 5); 
	
		// dynamic correction of moved mass		
		#ifdef DISABLE_DYNAMIC_MASS_COMP_PITCH
		Dyn[BF] = 0;
		#else
		Dyn[BF] = Rate[Pitch];
		#endif

		// correct DC level of the integral	
		IntCorr[BF] = SRS16(-Acc[BF] + Grav[BF] + Dyn[BF], 3); // / 10;
		IntCorr[BF] = Limit(IntCorr[BF], -GYRO_COMP_STEP, GYRO_COMP_STEP); 

		// Roll

		// static compensation due to Gravity
		Grav[LR] = -SRS16(Angle[Roll] * GRAV_COMP, 5); 
	
		// dynamic correction of moved mass
		#ifdef DISABLE_DYNAMIC_MASS_COMP_ROLL
		Dyn[LR] = 0;
		#else
		Dyn[LR] = Rate[Roll];	
		#endif

		// correct DC level of the integral
		IntCorr[LR] = SRS16(Acc[LR] + Grav[LR] + Dyn[LR], 3); // / 10;
		IntCorr[LR] = Limit(IntCorr[LR], -GYRO_COMP_STEP, GYRO_COMP_STEP); 
	}	
	else
		IntCorr[LR] = IntCorr[BF] = Acc[LR] = Acc[BF] = Acc[UD] = 0;

} // CompensateRollPitchGyros

#endif // USE_DCM





