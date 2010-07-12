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

void MapRC(void);
void ReadParametersEE(void);
void WriteParametersEE(uint8);
void UseDefaultParameters(void);
void UpdateWhichParamSet(void);
void InitParameters(void);

const rom uint8 ESCLimits [] = { OUT_MAXIMUM, OUT_HOLGER_MAXIMUM, OUT_X3D_MAXIMUM, OUT_YGEI2C_MAXIMUM };

const rom int8	ComParms[]={ // mask giving common variables across parameter sets
	0,0,0,1,0,0,0,0,0,0,
	0,0,0,0,0,1,1,1,0,1,
	1,1,1,1,1,0,0,0,0,0,
	0,0,0,1,1,1,1,0,0,1,
	0,1,1,1,1,0,1,0,0,1,
	1,1,1,1,1,0,0,0,0,0,
	0,0,0,0
	};

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

const rom uint8 Map[CustomTxRx+1][CONTROLS] = {
	{ 3,1,2,4,5,6,7 }, 	// Futaba Thr 3 Throttle
	{ 2,1,4,3,5,6,7 },	// Futaba Thr 2 Throttle
	{ 5,3,2,1,6,4,7 },	// Futaba 9C Spektrum DM8/AR7000
	{ 1,2,3,4,5,6,7 },	// JR XP8103/PPM
	{ 7,1,4,6,3,5,2 },	// JR 9XII Spektrum DM9 ?

	{ 6,1,4,7,3,2,5 },	// JR DXS12 
	{ 6,1,4,7,3,2,5 },	// Spektrum DX7/AR7000
	{ 5,1,4,6,3,2,7 },	// Spektrum DX7/AR6200

	{ 3,1,2,4,5,7,6 }, 	// Futaba Thr 3 Sw 6/7
	{ 1,2,3,4,5,6,7 },	// Spektrum DX7/AR6000
	{ 1,2,3,4,5,6,7 },	// Graupner MX16S
	{ 5,1,3,4,2,6,7 },	// Spektrum DX6i/AR6200

	{ 5,1,3,2,4,6,7 }	// Custom
	};

// Rx signalling polarity used only for serial PPM frames usually
// by tapping internal Rx circuitry.
const rom boolean PPMPosPolarity[CustomTxRx+1] =
	{
		false, 	// Futaba Ch3 Throttle
		false,	// Futaba Ch2 Throttle
		true,	// Futaba 9C Spektrum DM8/AR7000
		true,	// JR XP8103/PPM
		true,	// JR 9XII Spektrum DM9/AR7000

		true,	// JR DXS12
		true,	// Spektrum DX7/AR7000
		true,	// Spektrum DX7/AR6200
		false,	// Futaba Thr 3 Sw 6/7
		true,	// Spektrum DX7/AR6000
		true,	// Graupner MX16S
		true,	// Graupner DX6i/AR6200
		true	// custom Tx/Rx combination
	};

// Reference Internal Quadrocopter Channel Order
// 1 Throttle
// 2 Aileron
// 3 Elevator
// 4 Rudder
// 5 Gear
// 6 Aux1
// 7 Aux2

int8	ParamSet;
boolean ParametersChanged;
int8 RMap[CONTROLS];
#pragma udata params
int8 P[MAX_PARAMETERS];
#pragma udata

void MapRC(void)
{  // re-maps captured PPM to Rx channel sequence
	static int8 c;
	static int16 LastThrottle, Temp, i; 

	LastThrottle = RC[ThrottleC];

	for (c = 0 ; c < RC_CONTROLS ; c++)
	{
		i = Map[P[TxRxType]][c]-1;
		#ifdef CLOCK_16MHZ
		Temp = PPM[i].b0; // clip to bottom byte 0..255
		#else // CLOCK_40MHZ
		Temp = ( (int32)PPM[i].i16 * RC_MAXIMUM + 625L )/1250L; // scale to 4uS res. for now
		#endif // CLOCK_16MHZ
		RC[c] = RxFilter(RC[c], Temp);		
	}

	if ( THROTTLE_SLEW_LIMIT > 0 )
		RC[ThrottleC] = SlewLimit(LastThrottle, RC[ThrottleC], THROTTLE_SLEW_LIMIT);

} // MapRC

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

		b = P[ServoSense];
		for ( i = 0; i < 6; i++ )
		{
			if ( b & 1 )
				PWMSense[i] = -1;
			else
				PWMSense[i] = 1;
			b >>=1;
		}

		#ifdef RX6CH
		NavSensitivity = ((int16)P[PercentNavSens6Ch] * RC_MAXIMUM)/100L;
		#endif // RX6CH

		for ( i = 0; i < CONTROLS; i++) // make reverse map
			RMap[Map[P[TxRxType]][i]-1] = i+1;
	

		IdleThrottle = Limit((int16)P[PercentIdleThr], 10, 30); // 10-30%
		IdleThrottle = (IdleThrottle * OUT_MAXIMUM )/100L;
		CruiseThrottle = ((int16)P[PercentCruiseThr] * OUT_MAXIMUM )/100L;
	
		RollIntLimit256 = (int16)P[RollIntLimit] * 256L;
		PitchIntLimit256 = (int16)P[PitchIntLimit] * 256L;
		YawIntLimit256 = (int16)P[YawIntLimit] * 256L;
	
		NavNeutralRadius = Limit((int16)P[NeutralRadius], 0, NAV_MAX_NEUTRAL_RADIUS);
		NavClosingRadius = Limit((int16)P[NavRadius], NAV_MAX_NEUTRAL_RADIUS+1, NAV_MAX_RADIUS);

		NavNeutralRadius = ConvertMToGPS(NavNeutralRadius); 
		NavClosingRadius = ConvertMToGPS(NavClosingRadius);
		NavCloseToNeutralRadius = NavClosingRadius - NavNeutralRadius;

		CompassOffset = ((((int16)P[CompassOffsetQtr] * 90L - (int16)P[NavMagVar])*MILLIPI)/180L);
		F.UsingAltOrientation = ( (P[ConfigBits] & FlyAltOrientationMask) != 0);
		#if ( defined QUADROCOPTER | defined TRICOPTER )
			#ifdef TRICOPTER
			if ( F.UsingAltOrientation ) // K1 forward
				CompassOffset -= MILLIPI;
			#else
			if ( F.UsingAltOrientation )
				CompassOffset -= QUARTERMILLIPI;
			#endif // TRICOPTER
		#endif // QUADROCOPTER | TRICOPTER
	
		F.UsingSerialPPM = ((P[ConfigBits] & RxSerialPPMMask) != 0);
		PIE1bits.CCP1IE = false;
		DoRxPolarity();
		PPM_Index = PrevEdge = 0;
		PIE1bits.CCP1IE = true;

		F.UsingFlatAcc = ((P[ConfigBits] & UseFlatAccMask) != 0);
		F.RFInInches = ((P[ConfigBits] & RFInchesMask) != 0);

		F.UsingTxMode2 = ((P[ConfigBits] & TxMode2Mask) != 0);
		F.UsingGPSAlt = ((P[ConfigBits] & UseGPSAltMask) != 0);
		F.UsingRTHAutoDescend = ((P[ConfigBits] & UseRTHDescendMask) != 0);
		NavRTHTimeoutmS = (uint24)P[DescentDelayS]*1000L;

		BatteryVoltsLimitADC = BatteryVoltsADC = ((int24)P[LowVoltThres] * 1024 + 70L) / 139L; // UAVPSet 0.2V units
		BatteryCurrentADC = 0;
		
		ParametersChanged = false;
	}
	
} // ReadParametersEE

void WriteParametersEE(uint8 s)
{
	int8 p;
	uint8 b;
	uint16 addr;
	
	addr = (s - 1)* MAX_PARAMETERS;
	for ( p = 0; p < MAX_PARAMETERS; p++)
		WriteEE( addr + p,  P[p]);
} // WriteParametersEE

void UseDefaultParameters(void)
{ // loads a representative set of initial parameters as a base for tuning
	int8 p;

	for ( p = 0; p < MAX_PARAMETERS; p++ )
		P[p] = DefaultParams[p];

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

	int8 NewParamSet, Selector;
	uint8 NewTurnToWP;

	NewParamSet = ParamSet;
	NewTurnToWP = F.TurnToWP;

	if ( F.UsingTxMode2 )
		Selector = DesiredRoll;
	else
		Selector = -DesiredYaw;

	if ( F.UsingTxMode2 )
		Selector = -DesiredYaw;
	else
		Selector = DesiredRoll;

	if ( (Abs(RC[ThrottleC]) < STICK_WINDOW) && (Abs(Selector) > STICK_WINDOW ) )
	{
		if ( Selector < -STICK_WINDOW ) // left
			NewTurnToWP = false;
		else
			if ( Selector > STICK_WINDOW ) // left
				NewTurnToWP = true; // right
			
		if ( NewTurnToWP != F.TurnToWP )
		{		
			F.TurnToWP = NewTurnToWP;
			LEDBlue_ON;
		// do it either way	if ( F.TurnToWP )
				DoBeep100mSWithOutput(4, 2);

			LEDBlue_OFF;
		}
	}
} // UpdateParamSetChoice

void InitParameters(void)
{
	ALL_LEDS_ON;
	ParamSet = 1;

	if ( ReadEE((uint16)TxRxType) == -1 )
		UseDefaultParameters();
	
	ParamSet = 1;
	ParametersChanged = true;
	ReadParametersEE();
	ALL_LEDS_OFF;
} // InitParameters


