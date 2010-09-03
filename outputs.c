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

#include "uavx.h"

void DoMulticopterMix(int16);
void CheckDemand(int16);
void MixAndLimitMotors(void);
void MixAndLimitCam(void);
void OutSignals(void);
void InitI2CESCs(void);
void StopMotors(void);

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

void DoMulticopterMix(int16 CurrThrottle)
{
	static int16 Temp, YlNeg, YlNegOn2, YlPos, YlPosOn2, RlOn2, PlOn2;

	PWM[FrontC] = PWM[LeftC] = PWM[RightC] = PWM[BackC] = CurrThrottle;
	#ifdef TRICOPTER
		//	Temp = (Pl * 50)/115; // compensate for 30deg angle of rear arms
		Temp = SRS16(Pl * 56, 7);
		PWM[FrontC] -= Pl ;				// front motor
		PWM[LeftC] += (Temp - Rl);		// right rear
		PWM[RightC] += (Temp + Rl); 	// left rear
		PWM[BackC] = PWMSense[RudderC] * Yl + OUT_NEUTRAL;	// yaw servo	
	#else
	    #ifdef HEXACOPTER

			NOT YET!

			#define NEG_SHIFT 2 
			#define POS_SHIFT 1

			RlOn2 = SRS16(Rl, 1);
			PlOn2 = SRS16(Pl, 1);
  
			PWM[FrontC]  +=  ( RlOn2 + PlOn2 ); 
			PWM[LeftC]  -= Rl; 
			PWM[RightC] += Rl; PWM[BackC] -= ( RlOn2 + PlOn2 );
  
			if ( Yl < 0) 
			{
				YlNeg = SRS16(Yl, NEG_SHIFT);
				YlNegOn2 = SRS16(YlNeg, 1);
				PWM[FrontC] += YlNeg; 
				PWM[LeftC]  -= YlNegOn2; 
				PWM[RightC] -= YlNegOn2; 
				PWM[BackC]  += YlNeg;
			 } 
			else 
			{ 
				YlPos = SRS16(Yl, POS_SHIFT);
				YlPosOn2 = SRS16(YlNeg, 1);
				PWM[FrontC] += YlPos;
			 	PWM[LeftC]  -= YlPosOn2;
			 	PWM[RightC] -= YlPosOn2; 
				PWM[BackC]  += YlPos; 
			}
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
		Scale = Min(ScaleHigh, ScaleLow);
		if ( Scale < 256 )
		{
			MotorDemandRescale = true;
			Rl = (Rl * Scale)/256;  // could get rid of the divides
			Pl = (Pl * Scale)/256; 
			Yl = (Yl * Scale)/256; 
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
			 CurrThrottle += (DUComp + AltComp); // vertical compensation not optional
			
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
			PWM[FrontC] = PWM[BackC] = PWM[LeftC] = PWM[RightC] = CurrThrottle;
	#else
		CurrThrottle += AltComp; // simple - faster to climb with no elevator yet
		
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
	static int16 Cr, Cp;

	// use only roll/pitch angle estimates
	Cr = SRS32((int24)RollSum * P[CamRollKp], 8) + (int16)P[CamRollTrim];
	Cr = PWMSense[CamRollC] * Cr + OUT_NEUTRAL;

	Cp = SRS32((int24)PitchSum * P[CamPitchKp], 8) + DesiredCamPitchTrim;
	Cp = PWMSense[CamPitchC] * Cp + OUT_NEUTRAL; 			

	PWM[CamRollC] = Limit(Cr, 1, OUT_MAXIMUM);
	PWM[CamPitchC] = Limit(Cp, 1, OUT_MAXIMUM);

} // MixAndLimitCam

#if ( defined TRICOPTER | defined MULTICOPTER )
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
		#ifdef TRICOPTER
			PWM[BackC] = OUT_NEUTRAL;
		#else
			PWM[BackC] = ESCMin;
		#endif	
	#else
		PWM[ThrottleC] = ESCMin;
		PWM[1] = PWM[2] = PWM[3] = OUT_NEUTRAL;
	#endif // MULTICOPTER

	PWM[CamRollC] = PWM[CamPitchC] = OUT_NEUTRAL;

	F.MotorsArmed = false;
} // StopMotors





