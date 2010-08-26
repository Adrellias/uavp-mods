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

void DoRxPolarity(void);
void InitRC(void);
void MapRC(void);
void ReadParametersEE(void);
void WriteParametersEE(uint8);
void UseDefaultParameters(void);
void UpdateWhichParamSet(void);
boolean ParameterSanityCheck(void);
void InitParameters(void);

const rom uint8 ESCLimits [] = { OUT_MAXIMUM, OUT_HOLGER_MAXIMUM, OUT_X3D_MAXIMUM, OUT_YGEI2C_MAXIMUM };

const rom int8	ComParms[]={ // mask giving common variables across parameter sets
	0,0,0,1,0,0,0,0,0,0,
	0,0,0,0,0,1,1,1,0,1,
	1,1,1,1,1,0,0,0,0,0,
	0,0,0,1,1,1,1,0,0,1,
	0,1,1,1,1,0,1,0,0,1,
	1,1,1,1,1,0,1,0,0,0,
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
	{ 2,0,1,3,4,5,6 }, 	// Futaba Thr 3 Throttle
	{ 1,0,3,2,4,5,6 },	// Futaba Thr 2 Throttle
	{ 4,2,1,0,5,3,6 },	// Futaba 9C Spektrum DM8/AR7000
	{ 0,1,2,3,4,5,6 },	// JR XP8103/PPM
	{ 6,0,3,5,2,4,1 },	// JR 9XII Spektrum DM9 ?

	{ 5,0,3,6,2,1,4 },	// JR DXS12 
	{ 5,0,3,6,2,1,4 },	// Spektrum DX7/AR7000
	{ 4,0,3,5,2,1,6 },	// Spektrum DX7/AR6200

	{ 2,0,1,3,4,6,5 }, 	// Futaba Thr 3 Sw 6/7
	{ 0,1,2,3,4,5,6 },	// Spektrum DX7/AR6000
	{ 0,1,2,3,4,5,6 },	// Graupner MX16S
	{ 4,0,2,3,1,5,6 },	// Spektrum DX6i/AR6200
	{ 2,0,1,3,4,5,6 },	// Futaba Th 3/R617FS

	{ 2,0,1,3,4,5,6 },	// Custom
//{ 4,0,2,1,3,5,6 }	// Custom
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
		true,	// Futaba Thr 3/R617FS
		true	// custom Tx/Rx combination
	};

// Reference Internal Quadrocopter Channel Order
// 0 Throttle
// 1 Aileron
// 2 Elevator
// 3 Rudder
// 4 Gear
// 5 Aux1
// 6 Aux2

uint8	ParamSet;
boolean ParametersChanged, SaveAllowTurnToWP;

int8 RMap[CONTROLS];
#pragma udata params
int8 P[MAX_PARAMETERS];
#pragma udata

#pragma udata orient
int16 OSin[48], OCos[48];
#pragma udata
uint8 Orientation;

uint8 UAVXAirframe;

#pragma udata ppmq
int16x8x4Q PPMQ;
int16 PPMQSum[CONTROLS];
#pragma udata

void DoRxPolarity(void)
{
	if ( F.UsingSerialPPM  ) // serial PPM frame from within an Rx
		CCP1CONbits.CCP1M0 = PPMPosPolarity[TxRxType];
	else
		CCP1CONbits.CCP1M0 = 1;	
}  // DoRxPolarity

void InitRC(void)
{
	int8 c, q;

	DoRxPolarity();

	SignalCount = -RC_GOOD_BUCKET_MAX;
	F.Signal = F.RCNewValues = false;
	
	for (c = 0; c < RC_CONTROLS; c++)
	{
		PPM[c].i16 = 0;
		RC[c] = RC_NEUTRAL;

		#ifdef CLOCK_16MHZ
		for (q = 0; q <= PPMQMASK; q++)
			PPMQ.B[q][c] = RC_NEUTRAL;
		PPMQSum[c] = RC_NEUTRAL * 4;
		#else
		for (q = 0; q <= PPMQMASK; q++)
			PPMQ.B[q][c] = 625;
		PPMQSum[c] = 2500;
		#endif // CLOCK_16MHZ
	}
	PPMQ.Head = 0;

	DesiredRoll = DesiredPitch = DesiredYaw = DesiredThrottle = 0;
	RollRate = PitchRate = YawRate = 0;
	ControlRollP = ControlPitchP = 0;
	RollTrim = PitchTrim = YawTrim = 0;

	PPM_Index = PrevEdge = RCGlitches = 0;
} // InitRC

void MapRC(void) // re-arrange arithmetic reduces from 736uS to 207uS @ 40MHz
{  // re-maps captured PPM to Rx channel sequence
	static int8 c;
	static int16 LastThrottle, Temp, i;
	static uint16 Sum;
	static i32u Temp2; 

	LastThrottle = RC[ThrottleC];

	for (c = 0 ; c < RC_CONTROLS ; c++) 
	{
		Sum = PPM[c].u16;
		PPMQSum[c] -= PPMQ.B[PPMQ.Head][c];
		PPMQ.B[PPMQ.Head][c] = Sum;
		PPMQSum[c] += Sum;
		PPMQ.Head = (PPMQ.Head + 1) & PPMQMASK;
	}

	for (c = 0 ; c < RC_CONTROLS ; c++) 
	{
		i = Map[P[TxRxType]][c];
		#ifdef CLOCK_16MHZ
			RC[c] = SRS16(PPMQSum[i], 2) & 0xff; // clip to bottom byte 0..255
		#else // CLOCK_40MHZ	
			//RC[c] = ( (int32)PPMQSum[i] * RC_MAXIMUM + 2500L )/5000L; // scale to 4uS res. for now	
			Temp2.i32 = (int32)PPMQSum[i] * (RC_MAXIMUM * 13L) + 32768L; 
			RC[c] = (uint8)Temp2.w1;
		#endif // CLOCK_16MHZ		
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
			F.UsingPositionHoldLock = false;
		#else
			F.UsingPositionHoldLock = ( (P[ConfigBits] & UsePositionHoldLockMask ) != 0);
		#endif // RX6CH

		for ( i = 0; i < CONTROLS; i++) // make reverse map
			RMap[Map[P[TxRxType]][i]] = i;
	
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

		#ifdef MULTICOPTER
			#ifdef USE_ORIENT
				Orientation = P[Orient];
				if (Orientation == 0xff ) // uninitialised
					Orientation = 0;			
			#else // USE_ORIENT
				Orientation = 0;
				#if ( defined QUADROCOPTER | defined TRICOPTER )
					#ifdef TRICOPTER
						if ( !F.UsingAltOrientation ) // K1 forward
							Orientation = 24;
					#else
						if ( F.UsingAltOrientation )
							Orientation = 6;
					#endif // TRICOPTER
				#endif // QUADROCOPTER | TRICOPTER
			#endif // USE_ORIENT
			CompassOffset -= (MILLIPI * (int16)Orientation) / 24L;
		#else
			Orientation = 0;
		#endif // MULTICOPTER
	
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

		#ifndef USE_ADC_FILTERS
			YawFilterA	= ( (int24) P[TimeSlots] * 256L) / ( 1000L / ( 6L * (int24) ADC_YAW_FREQ ) + (int24) P[TimeSlots] );
		#endif // USE_ADC_FILTERS

		BatteryVoltsLimitADC = BatteryVoltsADC = ((int24)P[LowVoltThres] * 1024 + 70L) / 139L; // UAVPSet 0.2V units
		BatteryCurrentADC = 0;
		
		F.ParametersValid = ParameterSanityCheck();

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

	if ( ReadEE((uint16)TxRxType) == -1 )
		UseDefaultParameters();

	#ifndef USE_ADC_FILTERS
		YawFilterA	= ( (int24) P[TimeSlots] * 256L) / ( 1000L / ( 6L * (int24) ADC_YAW_FREQ ) + (int24) P[TimeSlots] );
	#endif // USE_ADC_FILTERS

	ParamSet = 1;
	ReadParametersEE();
	ParametersChanged = true;

	ALL_LEDS_OFF;
} // InitParameters


