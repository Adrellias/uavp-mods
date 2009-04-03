// =======================================================================
// =                                 UAVX                                =
// =                         Quadrocopter Control                        =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =     Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer       =
// =                          http://www.uavp.org                        =
// =======================================================================
//
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

// Control routines

#include "UAVX.h"
#include <adc.h>

// Prototypes
void InitGyros(void);
void InitAccelerometers(void);
void HeadingHold(int16);
void AltitudeHold(int16);
void DoIntTestLEDs(void);
int16 GetRollRate(void);
int16 GetPitchRate(void);
int16 GetYawRate(void);
void UpdateControls(void);
void InitAttitude(void);
void DetermineAttitude(void);
void CompensateGyros(void);
void AltitudeHold(int16);
void PID(void);
void DoControl(void);
void InitControl(void);

// Altitude

// Rate of climb in Decimetres/Sec 
// do not change this 0.5Sec update rate to alt

#define THR_ROC_THRESHOLD PERCENT(5,RC_MAXIMUM)  
#define ROC_MAXIMUM		(192 * ROC_INTERVAL/1000)
#define	ROC_MINIMUM		(-15 * ROC_INTERVAL/1000)

#define THR_HOVER		PERCENT(60, RC_MAXIMUM)		
#define THR_WINDOW		PERCENT(5, RC_MAXIMUM)

// Variables

#pragma idata params
// Principal quadrocopter parameters - MUST remain in this order
int8	RollKp				=18;
int8	RollKi				=4;	// 4
int8	RollKd				=-40;
int8	BaroTempCoeff		=13;
int8	RollIntLimit		=16;
int8	PitchKp				=18;
int8	PitchKi				=4;	// 4
int8	PitchKd				=-40;	 
int8	AltitudeKp			=2;
int8	PitchIntLimit		=16;
int8	YawKp				=20;
int8	YawKi				=40;
int8	YawKd				=6;
int8	YawLimit			=50;
int8	YawIntLimit			=6;
int8	ConfigParam			=0b00000000;
int8	ImpulsePeriod		=4;	// control update interval + LEGACY_OFFSET
int8	LowVoltThres		=43;
int8	LinLRKi				=0;	// unused
int8	LinFBKi				=0;	// unused
int8	LinUDKi				=8;
int8	MiddleUD			=0;
int8	MotorLowRun			= PERCENT(16, OUT_MAXIMUM);
int8	MiddleLR			=0;
int8	MiddleFB			=0;
int8	CamKi				=0x44;
int8	CompassKp			=5;
int8	AltitudeKd			=4;
#pragma idata

// RC Mappings
#pragma idata rcmaps
#ifdef RX_DSM
// DX7 and AR7000
// Futaba 9C with Spektrum DM8 / JR 9XII with DM9 module
const uint8 Map[2, CONTROLS] = {{5, 0, 3, 6, 2, 4, 1 }, {4, 2, 1, 0, 5, 3, 6 }); 
#else
const uint8 Map[2][CONTROLS] = {{0, 1, 2, 3, 4, 5, 6 }, {2, 0, 1, 3, 4, 5, 6 }};								
#endif 
#pragma idata

#pragma idata controlvars
// Gyros
int24	RollAngle, PitchAngle, YawAngle;		// PID integral (angle)
int16	RollGyroRate, PitchGyroRate, YawGyroRate;// PID rate (raw gyro values)
int16	RollRate, PitchRate, YawRate;			// PID rate (scaled gyro values)
int16	MidRoll, MidPitch, MidYaw;				// PID gyro neutrals

// Acceleration Corrections
int16	UDVelocity;
int16	Vud;
int16	Ax, Ay, Az;								// LISL sensor accelerations							
int16	NeutralLR, NeutralFB, NeutralUD;		// LISL scaled neutral values					

// PID 
int24	Rl,Pl,Yl;								// PID output values
int16	AE, RE, PE, YE, HE;								// PID error
int16	SumAE, SumRE, SumPE, SumYE;
int16	AEp, REp, PEp, YEp;							// PID previous error
int16	DesiredThrottle, HoverThrottle, DesiredAltitude;
int16	DesiredRoll, DesiredPitch;
int16 	DesiredRollRate, DesiredPitchRate;
int16	DesiredAltitude, DesiredROC;
int16	DesiredYaw, DesiredYawRate;
int16	Valt;

// Altitude
int16	CurrAltitude;

#pragma idata

void DoIntTestLEDs(void)
{
	if( IntegralTest )
	{
		ALL_LEDS_OFF;
		if( RollAngle > 255 )
			LedRed_ON;
		else
			if( RollAngle < -256 )
				LedGreen_ON;
		if( PitchAngle >  255 )
			LedYellow_ON;
        else
			if( PitchAngle < -256 )
				LedBlue_ON;
	}
} // DoIntTestLEDs


int16 GetRollRate(void)
{	// + roll right
	return(GYROSIGN_ROLL*ADC(ADCRollChan, ADCEXTVREF_PITCHROLL));
} // GetRollRate

int16 GetPitchRate(void)
{	// + pitch up
	return(GYROSIGN_PITCH*ADC(ADCPitchChan, ADCEXTVREF_PITCHROLL));
}  // GetPitchRate

int16 GetYawRate(void)
{	// + yaw CW
	return(GYROSIGN_YAW * ADC(ADCYawChan, ADCEXTVREF_YAW));		
} // GetYawRate

void InitGyros(void)
{ // quadrocopter MUST be stationary and level
	uint16 s;

    RollAngle = PitchAngle = YawAngle = 0;
	for ( s = 256; s ; s-- )
		{		
		RollAngle += GetRollRate();
		PitchAngle += GetPitchRate();
		YawAngle += GetYawRate();
		}

	if ( !_UseLISL )
	{
		RollAngle += MiddleLR;
		PitchAngle += MiddleFB;
	}

	MidRoll = SRS24(RollAngle + 128, 8);								
	MidPitch = SRS24(PitchAngle + 128, 8);
	MidYaw = SRS24(YawAngle + 128, 8);		
	
	#ifdef OPT_ADXRS150
	MidRoll = SRS24(MidRoll + 1, 1);	
	MidPitch = SRS24(MidPitch + 1, 1);
	#endif

	RollGyroRate = MidRoll;
	PitchGyroRate = MidPitch;
	YawGyroRate = MidYaw;

	RollAngle = PitchAngle = YawAngle = 0;
	REp = PEp = YEp = 0;
	SumRE = SumPE = 0;
} // InitGyros

void InitAccelerometers(void)
{	// quadrocopter MUST be stationary and level
	uint16 s;
	int24 Tx, Ty, Tz;

	IsLISLactive();

	Tx = Ty = Tz = 0;
	if ( _UseLISL )
	{
		LedYellow_ON;
		for( s=256; s ; s--)
		{
			ReadAccelerations();
			Tx += ACCSIGN_X * Ax;
			Ty += ACCSIGN_Y * Ay;
			Tz += ACCSIGN_Z * Az;
		}
		LedYellow_OFF;
	}

	NeutralLR = SRS24(Tx + 128, 8);
	NeutralUD = SRS24(Ty + 128, 8);
	NeutralFB = SRS24(Tz + 128, 8);
	UDVelocity = 0;
} // InitAcelerometers

void CompensateGyros(void)
{	// + Ax right, + Ay up, + Az back
	int16 Rc, Pc, Uc, Temp;

	ReadAccelerations();		
	Rc = ACCSIGN_X * Ax - NeutralLR;
	Uc = ACCSIGN_Y * Ay - NeutralUD;	
	Pc = ACCSIGN_Z * Az - NeutralFB;

	#ifdef DRIFT
	TxVal((int32)Pc, 0, ';');
	Temp = Rc;
	#endif
								
	// Roll
	Rc = -SRS24(Rc * 3 + 1, 1);					// empirical 1.5					
	Rc -= RollAngle;	
	Rc += RollRate;								// turn coordination ???

	if( Rc > 10 ) 
		RollAngle += 4;
	else
		if( Rc < -10 ) 
			RollAngle -= 4;
	
	RollAngle = Decay(RollAngle);

	// Pitch									
	Pc = SRS16(Pc * 3 + 1,1); 
	Pc -= PitchAngle;
								
	if( Pc > 10 ) 
		PitchAngle += 4;
	else
		if( Pc < -10 ) 
			PitchAngle -= 4;

	PitchAngle = Decay(PitchAngle);

	#ifdef DRIFT
	TxVal((int32)PitchAngle, 0, ';');
	TxVal((int32)PitchRate, 0, ';');

	TxVal((int32)Temp, 0, ';');
	TxVal((int32)RollAngle, 0, ';');
	TxVal((int32)RollRate, 0, ';');

	TxNextLine();
	#endif

} // CompensateGyros

void AltitudeHold(int16 DesiredAltitude)
{
	#define AltitudeKi 2

	uint16 Temp;

	AE = DesiredAltitude - CurrAltitude;
	AE = Limit(AE, -3, 8); 						// "soften" rate

	Valt = AE*AltitudeKp;						// P

	SumAE = Limit(SumAE+AE, -16, 16);			// I
	Valt += Limit(SumAE*AltitudeKi, -4, 4);
		
	Valt += Limit((AE-AEp)*AltitudeKd, -8, 8);	// D

	// if QC is off the ground with a small amount of compensation slowly "learn" Hover
	if ( Abs(Valt < 3) && (CurrAltitude > 10 ) && 
							!(( State == Landed) || (State == Initialising )))
		HoverThrottle = HardFilter(HoverThrottle, DesiredThrottle);
				
	Valt = Limit(Valt, -5, 15);  				// faster up than down

	AEp = AE;

} // AltitudeHold

void HeadingHold(int16 DesiredHeading)
{
	int16 Temp, CurrYaw;

	if ( _UseCompass )
	{



	}
	else
	{


	}
/*
		if ((CurrYaw < (DesiredYaw - COMPASS_MIDDLE)) || (CurrYaw > (DesiredYaw + COMPASS_MIDDLE)))
			CompassHeading = COMPASS_INVAL;		// new hold zone
		else
			YE -= Limit(CurrDeviation, -COMPASS_MAXDEV, COMPASS_MAXDEV);
*/

} // HeadingHold

void DetermineAttitude(void)
{	// Roll is + right, Pitch is + up, Yaw + CW
	int24 Temp;

	RollGyroRate = GyroFilter(RollGyroRate, GetRollRate());	
	PitchGyroRate = GyroFilter(PitchGyroRate, GetPitchRate());
	YawGyroRate = GyroFilter(YawGyroRate, GetYawRate());

	#ifdef OPT_ADXRS150
	// throw away 1 bit of precision ???
	RollGyroRate = SRS16(RollGyroRate + 1, 1);
	PitchGyroRate =  SRS16(PitchGyroRate + 1, 1);
	#endif

	RollRate = RollGyroRate - MidRoll;
	PitchRate = PitchGyroRate - MidPitch;

	if( FlyCrossMode )								// does this work with compensation NO ??? 
	{
		// Real Roll = 0.707 * (P + R), Pitch = 0.707 * (P - R)
		Temp = ((RollRate + PitchRate) * 7 + 5)/10 ;	
		PitchRate = ((PitchRate - RollRate) * 7 + 5)/10;
		RollRate = Temp;	
	}
	
	RollAngle += (int24)RollRate;					// use 10 bit res. for I control							
	RollAngle = Limit(RollAngle, -(RollIntLimit*256), RollIntLimit*256);

	PitchAngle += (int24)PitchRate;
	PitchAngle = Limit(PitchAngle, -(PitchIntLimit*256), PitchIntLimit*256);
  	
//	Temp = YawGyroRate - MidYaw;
//	if ( Abs(Temp) <= 2 )							// needs further thought ???
//		MidYaw = Limit(YawGyroRate, (YawGyroRate - 5), YawGyroRate + 5);

	YawRate = YawGyroRate - MidYaw;
	YE = SRS16(YawRate + 2, 2);						// scaling ??

	YawAngle = Limit(YawAngle + YE, -(YawIntLimit*256), YawIntLimit*256);
	YawAngle = Decay(YawAngle);

	CompensateGyros();

} // DetermineAttitude

void AttitudeHold(void)
{
	// General Form of PID controller
	// xE = current error
	// xEP = previous error
	// SumxE = sum of all errors since restart!
	// xl = current correction value
	// Kz = programmable controller factors

	//                                 SumxE*Ki
	// xl =  xE*Kp + (xEP-xE)*Kd  +  ------------ * Scale
	//                                   16

	// Roll
	#ifdef LEGACY_CTL // make roll rate zero
	RE = RollRate;
	Rl  = SRS24(RE * RollKp + (REp-RE) * RollKd + 8, 4);
	Rl += SRS24(RollAngle * RollKi * 2 + 128, 8);		// *2 to compensate for double sampling
	Rl -= DesiredRollRate;
	#else
	RE = RollAngle - DesiredRoll;
	SumRE = Limit(SumRE + RE, -RollIntLimit, RollIntLimit);
	Rl = RE*(int24) RollKp+(REp-RE)*(int24) RollKd;				
	Rl += SRS24(SumRE * (int24) RollKi + 8, 4);
	Rl = SRS16(Rl + 16, 5);
	#endif // LEGACY_CTL
										
	// Pitch
	#ifdef LEGACY_CTL // make pitch rate zero
	PE = PitchRate;
	Pl = SRS24(PE * PitchKp + (PEp-PE) * PitchKd + 8, 4);
	Pl += SRS24(PitchAngle * PitchKi * 2 + 128, 8);
	Pl -= DesiredRollRate;
	#else
	PE = PitchAngle - DesiredPitch;
	SumPE = Limit(SumPE+PE, -PitchIntLimit, PitchIntLimit);
	Pl = PE*(int24) PitchKp+(PEp-PE)*(int24) PitchKd;
	Pl += SRS24(SumPE * (int24) PitchKi + 8, 4);
	Pl = SRS16(Pl + 16, 5);
	#endif // LEGACY_CTL

	// Yaw
YE = 0;
	Yl  = SRS24(YE*(int24) YawKp+(YEp-YE)*(int24) YawKd + 8, 4);
	Yl += SRS24(YawAngle*(int24) YawKi + 128, 8);
	Yl = Limit(Yl, -YawLimit, YawLimit);

	DoIntTestLEDs();

	REp = RE;
	PEp = PE;
	YEp = YE;
} // AttitudeHold

void InitAttitude(void)
{
	uint24 TimerMilliSec;

	// DON'T MOVE THE UFO!
	// ES KANN LOSGEHEN!
	LedRed_ON;
	TimerMilliSec = mS[Clock] + 2000;
	while ( mS[Clock] <= TimerMilliSec ) {}; // ~2Sec. to get hands away after power up
	Beeper_ON;
	InitDirection();		
	InitBarometer();
	InitAccelerometers();
	InitGyros();
	Beeper_OFF;					
	LedRed_OFF;
} // InitAttitude

void UpdateControls(void)
{ 	// only called when RC link is up
	uint8 c;
	int16 ROCThrottle;

	_NewValues = false;
	MapRC();									// remap PPM to RC

	#ifdef LEGACY_CTL

	DesiredRollRate = RC[RollC]- RC_NEUTRAL;
	DesiredPitchRate = RC[PitchC]- RC_NEUTRAL;
	DesiredYawRate = RC[YawC] - RC_NEUTRAL;
	DesiredThrottle = (OUT_MAXIMUM * (int24) RC[ThrottleC] + RC_NEUTRAL)/RC_MAXIMUM;
	_Hold = false;

	#else

	DesiredRoll = ((RC[RollC]- RC_NEUTRAL) * MAX_ROLL + RC_NEUTRAL)/RC_MAXIMUM;
	DesiredPitch = ((RC[PitchC] - RC_NEUTRAL) * MAX_PITCH + RC_NEUTRAL)/RC_MAXIMUM;
	DesiredYawRate = RC[YawC] - RC_NEUTRAL;	
	_Hold = RC[ParamC] > _Neutral;

    #ifdef THROTTLE_ROC
	if ( _UseBaro && ( RC[ParamC] > RC_NEUTRAL) )
	{
		ROCThrottle = RC[ThrottleC] - RC_NEUTRAL;
		if ( Abs(ROCThrottle) >=  THR_ROC_THRESHOLD ) 
		{
			if ( mS[Clock] >= mS[ROCUpdate] )
			{
				mS[ROCUpdate] += ROC_INTERVAL;
				DesiredROC = Limit(ROCThrottle, ROC_MINIMUM, ROC_MAXIMUM);
				DesiredAltitude = CurrAltitude + DesiredROC;
			}
		}	
	}
	else
	#endif // THROTTLE_ROC
		DesiredThrottle = (OUT_MAXIMUM*(int32) RC[ThrottleC] + RC_NEUTRAL)/RC_MAXIMUM;
	#endif // LEGACY_CTL
} // UpdateControls

void DoControl()
{			
	DetermineAttitude();
	AttitudeHold();
	#ifdef THROTTLE_ROC
	AltitudeHold(DesiredAltitude);
	#endif

	CheckFrameOverrun();

	OutSignals();

	// less critical - do during sampling period wait
	GetDirection();
	GetBaroAltitude();
	UpdateGPS();

} // DoControl

void InitControl(void)
{
	DesiredThrottle = DesiredRoll = DesiredPitch = DesiredYawRate = 0;
	HoverThrottle = THR_HOVER;

	Rl = Pl = Yl = Valt = Vud = UDVelocity = 0;

} // InitControl

