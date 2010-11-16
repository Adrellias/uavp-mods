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

#include "uavx.h"

uint8 TC(int16);
void DoMulticopterMix(int16);
void CheckDemand(int16);
void MixAndLimitMotors(void);
void MixAndLimitCam(void);
void OutSignals(void);
void InitI2CESCs(void);
void StopMotors(void);
void InitMotors(void);

boolean OutToggle;
int16 PWM[6];
int16 PWMSense[6];
int16 ESCI2CFail[4];
int16 CurrThrottle;
int8 ServoInterval;

#pragma udata access outputvars
near uint8 SHADOWB, PWM0, PWM1, PWM2, PWM3, PWM4, PWM5;
near int8 ServoToggle;
#pragma udata

int16 ESCMin, ESCMax;

#ifdef MULTICOPTER

#pragma idata throttlecurve
const int16 ThrottleCurve[17] = { 
	0,57,81,99,114,128,140, 151,
	162,171,181,190,198,206,214,221,
	229
   };
#pragma idata

uint8 TC(int16 T)
{
	static int16 r;

	r = Limit(T, ESCMin, ESCMax);

	#ifdef CASTLE_ESCS 
	if ( P[ESCType] == ESCPPM )
		r =  Table16( r, ThrottleCurve );
	#endif // CASTLE_ESCS

	return ( r );

} // TC

void DoMulticopterMix(int16 CurrThrottle)
{
	static int16 Temp, YlNeg, YlNegOn2, YlPos, YlPosOn2, RlOn2, PlOn2;

	PWM[FrontC] = PWM[LeftC] = PWM[RightC] = PWM[BackC] = CurrThrottle;
	#ifdef TRICOPTER // usually flown K1 motor to the rear which is set using orientation of 24
		#ifdef TRI_REBALANCE 
			PWM[FrontC] -= Pl ;		// front motor
			PWM[LeftC] -= Rl;		// right rear
			PWM[RightC] += Rl; 		// left rear
	
			PWM[BackC] = PWMSense[RudderC] * Yl + OUT_NEUTRAL;	// yaw servo
		#else
			Temp = SRS16(Pl * 56, 7); 	// compensate for 30deg angle of rear arms
			PWM[FrontC] -= Pl ;			// front motor
			PWM[LeftC] += (Temp - Rl);	// right rear
			PWM[RightC] += (Temp + Rl); // left rear
	
			PWM[BackC] = PWMSense[RudderC] * Yl + OUT_NEUTRAL;	// yaw servo
		#endif	// TRI_REBALANCE
	#else
	    #ifdef VTCOPTER 	// usually flown VTail (K1+K4) to the rear which is set using orientation of 24
			#ifdef VT_TREBALANCE
				PWM[LeftC] -= Rl;		// right rear
				PWM[RightC] += Rl; 		// left rear
	
				PWM[FrontLeftC] -= Pl + PWMSense[RudderC] * Yl; 
				PWM[FrontRightC] -= Pl - PWMSense[RudderC] * Yl; 
			#else			
				Temp = SRS16(Pl * 56, 7); 	// compensate for 30deg angle of rear arms
				PWM[LeftC] += (Temp - Rl);	// right rear
				PWM[RightC] += (Temp + Rl); // left rear
	
				PWM[FrontLeftC] -= Pl + PWMSense[RudderC] * Yl; 
				PWM[FrontRightC] -= Pl - PWMSense[RudderC] * Yl;
			#endif // VT_REBALANCE
		#else // QUADROCOPTER
			PWM[LeftC]  += -Rl - Yl;	
			PWM[RightC] +=  Rl - Yl;
			PWM[FrontC] += -Pl + Yl;
			PWM[BackC]  +=  Pl + Yl;
		#endif
	#endif

} // DoMulticopterMix

boolean 	MotorDemandRescale;

void CheckDemand(int16 CurrThrottle)
{
	static int24 Scale, ScaleHigh, ScaleLow, MaxMotor, DemandSwing;

	MaxMotor = Max(PWM[FrontC], PWM[LeftC]);
	MaxMotor = Max(MaxMotor, PWM[RightC]);
	#ifndef TRICOPTER
		MaxMotor = Max(MaxMotor, PWM[BackC]);
	#endif // TRICOPTER

	DemandSwing = MaxMotor - CurrThrottle;

	if ( DemandSwing > 0 )
	{		
		ScaleHigh = (( OUT_MAXIMUM - (int24)CurrThrottle) * 256 )/ DemandSwing;	 
		ScaleLow = (( (int24)CurrThrottle - IdleThrottle) * 256 )/ DemandSwing;
		Scale = Min(ScaleHigh, ScaleLow); // just in case!
		if ( Scale < 0 ) Scale = 1;
		if ( Scale < 256 )
		{
			MotorDemandRescale = true;
			Rl = (Rl * Scale)/256;  // could get rid of the divides
			Pl = (Pl * Scale)/256;
			#ifndef TRICOPTER 
				Yl = (Yl * Scale)/256;
			#endif // TRICOPTER 
		}
		else
			MotorDemandRescale = false;	
	}
	else
		MotorDemandRescale = false;	

} // CheckDemand

#endif // MULTICOPTER

void MixAndLimitMotors(void)
{ 	// expensive ~160uSec @ 40MHz
    static int16 Temp, TempElevon, TempElevator;

	if ( DesiredThrottle < IdleThrottle )
		CurrThrottle = 0;
	else
		CurrThrottle = DesiredThrottle;

	#ifdef MULTICOPTER
		if ( State == InFlight )
			 CurrThrottle += (Comp[DU] + Comp[Alt]); // vertical compensation not optional
			
		Temp = (int16)(OUT_MAXIMUM * 90 + 50) / 100; // 10% headroom for control
		CurrThrottle = Limit(CurrThrottle, 0, Temp ); 
			
		if ( CurrThrottle > IdleThrottle )
		{
			DoMulticopterMix(CurrThrottle);
			
			CheckDemand(CurrThrottle);
			
			if ( MotorDemandRescale )
				DoMulticopterMix(CurrThrottle);
		}
		else
		{
			PWM[FrontC] = PWM[LeftC] = PWM[RightC] = CurrThrottle;
			#ifdef TRICOPTER
				PWM[BackC] = PWMSense[RudderC] * Yl + OUT_NEUTRAL;	// yaw servo
			#else
				PWM[BackC] = CurrThrottle;
			#endif // !TRICOPTER
		}
	#else
		CurrThrottle += Comp[Alt]; // simple - faster to climb with no elevator yet
		
		PWM[ThrottleC] = CurrThrottle;
		PWM[RudderC] = PWMSense[RudderC] * Yl + OUT_NEUTRAL;
		
		#if ( defined AILERON | defined HELICOPTER )
			PWM[AileronC] = PWMSense[AileronC] * Rl + OUT_NEUTRAL;
			PWM[ElevatorC] = PWMSense[ElevatorC] * Pl + OUT_NEUTRAL;
		#else // ELEVON
			TempElevator = PWMSense[2] * Pl + OUT_NEUTRAL;
			PWM[RightElevonC] = PWMSense[RightElevonC] * (TempElevator + Rl);
			PWM[LeftElevonC] = PWMSense[LeftElevonC] * (TempElevator -  Rl);		
		#endif
	#endif

} // MixAndLimitMotors

void MixAndLimitCam(void)
{
	static i24u Temp;

	// use only roll/pitch angle estimates
	Temp.i24 = (int24)CameraRollAngle * P[CamRollKp];
	PWM[CamRollC] = Temp.i2_1 + (int16)P[CamRollTrim];
	PWM[CamRollC] = PWMSense[CamRollC] * PWM[CamRollC] + OUT_NEUTRAL;

	Temp.i24 = (int24)CameraPitchAngle * P[CamPitchKp];
	PWM[CamPitchC] = Temp.i2_1 + DesiredCamPitchTrim;
	PWM[CamPitchC] = PWMSense[CamPitchC] * PWM[CamPitchC] + OUT_NEUTRAL; 			

} // MixAndLimitCam

#if ( defined TRICOPTER | defined MULTICOPTER | defined VTCOPTER )
	#include "outputs_copter.h"
#else
	#include "outputs_conventional.h"
#endif // TRICOPTER | MULTICOPTER

void InitI2CESCs(void)
{
	static int8 m;
	static uint8 r;
	
	#ifdef MULTICOPTER

	if ( P[ESCType] ==  ESCHolger )
		for ( m = 0 ; m < NoOfI2CESCOutputs ; m++ )
		{
			ESCI2CStart();
			r = WriteESCI2CByte(0x52 + ( m*2 ));		// one cmd, one data byte per motor
			r += WriteESCI2CByte(0);
			ESCI2CFail[m] += r;  
			ESCI2CStop();
		}
	else
		if ( P[ESCType] == ESCYGEI2C )
			for ( m = 0 ; m < NoOfPWMOutputs ; m++ )
			{
				ESCI2CStart();
				r = WriteESCI2CByte(0x62 + ( m*2 ));	// one cmd, one data byte per motor
				r += WriteESCI2CByte(0);
				ESCI2CFail[m] += r; 
				ESCI2CStop();
			}
		else
			if ( P[ESCType] == ESCX3D )
			{
				ESCI2CStart();
				r = WriteESCI2CByte(0x10);			// one command, 4 data bytes
				r += WriteESCI2CByte(0); 
				r += WriteESCI2CByte(0);
				r += WriteESCI2CByte(0);
				r += WriteESCI2CByte(0);
				ESCI2CFail[0] += r;
				ESCI2CStop();
			}
	#endif // MULTICOPTER
} // InitI2CESCs

void StopMotors(void)
{
	#ifdef MULTICOPTER
		PWM[FrontC] = PWM[LeftC] = PWM[RightC] = ESCMin;
		#ifndef TRICOPTER
			PWM[BackC] = ESCMin;
		#endif // !TRICOPTER	
	#else
		PWM[ThrottleC] = ESCMin;
	#endif // MULTICOPTER

	F.MotorsArmed = false;
} // StopMotors

void InitMotors(void)
{
	StopMotors();
	#ifdef TRICOPTER
		PWM[BackC] = OUT_NEUTRAL;
	#endif // !TRICOPTER

	PWM[CamRollC] = OUT_NEUTRAL;
	PWM[CamPitchC] = OUT_NEUTRAL;

} // InitMotors






