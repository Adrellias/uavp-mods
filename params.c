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

void ReadParametersEE(void);
void WriteParametersEE(uint8);
void UseDefaultParameters(void);
void UpdateWhichParamSet(void);
boolean ParameterSanityCheck(void);
void InitParameters(void);

const rom uint8 ESCLimits [] = { OUT_MAXIMUM, OUT_HOLGER_MAXIMUM, OUT_X3D_MAXIMUM, OUT_YGEI2C_MAXIMUM };

#ifdef MULTICOPTER
	#include "uavx_multicopter.h"
#else
	#ifdef HELICOPTER
		#include "uavx_helicopter.h"	
	#else
		#ifdef ELEVONS
			#include "uavx_elevon.h"
		#else
			#include "uavx_aileron.h"
		#endif
	#endif
#endif 


uint8	ParamSet;
boolean ParametersChanged, SaveAllowTurnToWP;

#pragma udata params
int8 P[MAX_PARAMETERS];
#pragma udata

#pragma udata orient
int16 OSin[48], OCos[48];
#pragma udata
int8 Orientation , PolarOrientation;
uint8 UAVXAirframe;

void ReadParametersEE(void)
{
	int8 i,b;
	static uint16 a;

	if ( ParametersChanged )
	{   // overkill if only a single parameter has changed but is not in flight loop
		a = (ParamSet - 1)* MAX_PARAMETERS;	
		for ( i = 0; i < MAX_PARAMETERS; i++)
			P[i] = ReadEE(a + i);

		ESCMax = ESCLimits[P[ESCType]];
		if ( P[ESCType] == ESCPPM )
			ESCMin = 1;
		else
		{
			ESCMin = 0;
			for ( i = 0; i < NoOfPWMOutputs; i++ )
				ESCI2CFail[i] = 0;
			InitI2CESCs();
		}

		InitGyros();
		InitAccelerometers();

		b = P[ServoSense];
		for ( i = 0; i < 6; i++ )
		{
			if ( b & 1 )
				PWMSense[i] = -1;
			else
				PWMSense[i] = 1;
			b >>=1;
		}

		F.UsingPositionHoldLock = ( (P[ConfigBits] & UsePositionHoldLockMask ) != 0);
		F.UsingPolarCoordinates = ( (P[ConfigBits] & UsePolarMask ) != 0);

		for ( i = 0; i < CONTROLS; i++) // make reverse map
			RMap[Map[P[TxRxType]][i]] = i;
	
		IdleThrottle = Limit((int16)P[PercentIdleThr], 10, 30); // 10-30%
		IdleThrottle = (IdleThrottle * OUT_MAXIMUM )/100L;
		CruiseThrottle = ((int16)P[PercentCruiseThr] * OUT_MAXIMUM )/100L;

		RollIntLimit2048 = (int16)P[RollIntLimit] * 2048L;
		PitchIntLimit2048 = (int16)P[PitchIntLimit] * 2048L;
		YawIntLimit256 = (int16)P[YawIntLimit] * 256L;
	
		NavNeutralRadius = Limit((int16)P[NeutralRadius], 0, NAV_MAX_NEUTRAL_RADIUS);
		NavNeutralRadius = ConvertMToGPS(NavNeutralRadius); 
		NavPolarRadius = ConvertMToGPS(NAV_POLAR_RADIUS);

		NavYCorrLimit = Limit((int16)P[NavYawLimit], 5, 50);

		CompassOffset = ((((int16)P[CompassOffsetQtr] * 90L + (int16)P[NavMagVar])*MILLIPI)/180L);

		#ifdef MULTICOPTER
			Orientation = P[Orient];
			if (Orientation == -1 ) // uninitialised
				Orientation = 0;
			else
				if (Orientation < 0 )
					Orientation += 48;
		#else
			Orientation = 0;
		#endif // MULTICOPTER
	
		F.UsingSerialPPM = ( P[TxRxType] == FrSkyDJT_D8R ) || ( P[TxRxType] == ExternalDecoder ) || ( (P[ConfigBits] & RxSerialPPMMask ) != 0);
		PIE1bits.CCP1IE = false;
		DoRxPolarity();
		PPM_Index = PrevEdge = 0;
		PIE1bits.CCP1IE = true;

		F.RFInInches = ((P[ConfigBits] & RFInchesMask) != 0);

		F.FailsafesEnabled = ((P[ConfigBits] & UseFailsafeMask) == 0);

		F.UsingTxMode2 = ((P[ConfigBits] & TxMode2Mask) != 0);
		F.UsingGPSAlt = false;
		F.UsingRTHAutoDescend = ((P[ConfigBits] & UseRTHDescendMask) != 0);
		NavRTHTimeoutmS = (uint24)P[DescentDelayS]*1000L;

		YawFilterA	= ( PID_CYCLE_MS * 256L) / ( 1000L / ( 6L * (int24) YAW_MAX_FREQ ) + PID_CYCLE_MS );

		BatteryVoltsLimitADC = BatteryVoltsADC = ((int24)P[LowVoltThres] * 1024 + 70L) / 139L; // UAVPSet 0.2V units
		BatteryCurrentADC = 0;
		
		F.ParametersValid = ParameterSanityCheck();

		ParametersChanged = false;

		ServoInterval = ( 24 + (PID_CYCLE_MS >> 1) ) / PID_CYCLE_MS;
		ServoToggle = 0;
	}
	
} // ReadParametersEE

void WriteParametersEE(uint8 s)
{
	int8 p;
	uint16 addr;
	
	addr = (s - 1)* MAX_PARAMETERS;

	for ( p = 0; p < MAX_PARAMETERS; p++)
		WriteEE( addr + p,  P[p]);
} // WriteParametersEE

void UseDefaultParameters(void)
{ // loads a representative set of initial parameters as a base for tuning
	uint16 p;

	for ( p = 0; p < (uint16)MAX_EEPROM; p++ )
		WriteEE( p,  0xff);

	for ( p = 0; p < (uint16)MAX_PARAMETERS; p++ )
		P[p] = DefaultParams[p][0];

	WriteParametersEE(1);
	WriteParametersEE(2);
	ParamSet = 1;

	WriteEE(NAV_NO_WP, 0); // set NoOfWaypoints to zero

	TxString("\r\nDefault Parameters Loaded\r\n");
	TxString("Do a READ CONFIG to refresh the UAVPSet parameter display\r\n");	
} // UseDefaultParameters

void UpdateParamSetChoice(void)
{
	#define STICK_WINDOW 30

	uint8 NewParamSet, NewAllowNavAltitudeHold, NewAllowTurnToWP;
	int8 Selector;

	NewParamSet = ParamSet;
	NewAllowNavAltitudeHold = F.AllowNavAltitudeHold;
	NewAllowTurnToWP = F.AllowTurnToWP;

	if ( F.UsingTxMode2 )
		Selector = DesiredRoll;
	else
		Selector = -DesiredYaw;

	if ( (Abs(DesiredPitch) > STICK_WINDOW) && (Abs(Selector) > STICK_WINDOW) )
	{
		if ( DesiredPitch > STICK_WINDOW ) // bottom
		{
			if ( Selector < -STICK_WINDOW ) // left
			{ // bottom left
				NewParamSet = 1;
				NewAllowNavAltitudeHold = true;
			}
			else
				if ( Selector > STICK_WINDOW ) // right
				{ // bottom right
					NewParamSet = 2;
					NewAllowNavAltitudeHold = true;
				}
		}		
		else
			if ( DesiredPitch < -STICK_WINDOW ) // top
			{		
				if ( Selector < -STICK_WINDOW ) // left
				{
					NewAllowNavAltitudeHold = false;
					NewParamSet = 1;
				}
				else 
					if ( Selector > STICK_WINDOW ) // right
					{
						NewAllowNavAltitudeHold = false;
						NewParamSet = 2;
					}
			}

		if ( ( NewParamSet != ParamSet ) || ( NewAllowNavAltitudeHold != F.AllowNavAltitudeHold ) )
		{	
			ParamSet = NewParamSet;
			F.AllowNavAltitudeHold = NewAllowNavAltitudeHold;
			LEDBlue_ON;
			DoBeep100mSWithOutput(2, 2);
			if ( ParamSet == (uint8)2 )
				DoBeep100mSWithOutput(2, 2);
			if ( F.AllowNavAltitudeHold )
				DoBeep100mSWithOutput(4, 4);
			ParametersChanged |= true;
			Beeper_OFF;
			LEDBlue_OFF;
		}
	}

	if ( F.UsingTxMode2 )
		Selector = -DesiredYaw;
	else
		Selector = DesiredRoll;

	if ( (Abs(RC[ThrottleC]) < STICK_WINDOW) && (Abs(Selector) > STICK_WINDOW ) )
	{
		if ( Selector < -STICK_WINDOW ) // left
			NewAllowTurnToWP = false;
		else
			if ( Selector > STICK_WINDOW ) // left
				NewAllowTurnToWP = true; // right
			
		if ( NewAllowTurnToWP != F.AllowTurnToWP )
		{		
			F.AllowTurnToWP = NewAllowTurnToWP;
			LEDBlue_ON;
		//	if ( F.AllowTurnToWP )
				DoBeep100mSWithOutput(4, 2);

			LEDBlue_OFF;
		}
	}
	
	SaveAllowTurnToWP = F.AllowTurnToWP;

} // UpdateParamSetChoice

boolean ParameterSanityCheck(void)
{
	static boolean Fail;

	Fail = 	(P[RollKp] == 0) || 
			(P[PitchKp] == 0) || 
			(P[YawKp] == 0);

	return ( !Fail );
} // ParameterSanityCheck

void InitParameters(void)
{
	static int8 i;
	static int16 A;

	UAVXAirframe = AF_TYPE;

	for (i = 0; i < 48; i++)
	{
		A = (int16)(((int32)i * MILLIPI)/24L);
		OSin[i] = int16sin(A);
		OCos[i] = int16cos(A);
	}
	Orientation = 0;

	ALL_LEDS_ON;
	ParamSet = 1;

	if ( ( ReadEE((uint16)TxRxType) == -1 ) || (ReadEE(MAX_PARAMETERS + (uint16)TxRxType) == -1 ) )
		UseDefaultParameters();

	ParamSet = 1;
	ReadParametersEE();
	ParametersChanged = true;

	YawFilterA	= ( PID_CYCLE_MS * 256L) / ( 1000L / ( 6L * (int24) YAW_MAX_FREQ ) + PID_CYCLE_MS );

	ALL_LEDS_OFF;  
} // InitParameters


