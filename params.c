// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =                 Copyright (c) 2008 by Prof. Greg Egan               =
// =       Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer     =
// =           http://code.google.com/p/uavp-mods/ http://uavp.ch        =
// =======================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "uavx.h"

// Prototypes

void ReadParametersEE(void);
void WriteParametersEE(uint8);
void UseDefaultParameters(void);
void UpdateWhichParamSet(void);
void InitParameters(void);

void ReadParametersEE(void)
{
	uint8 i;
	static uint16 addr;

	if ( ParametersChanged )
	{   // overkill if only a single parameter has changed but is not in flight loop
		addr = (CurrentParamSet - 1)* MAX_PARAMETERS;	
		for ( i = 0; i < MAX_PARAMETERS; i++)
			P[i] = ReadEE(addr + i);

		ESCMax = ESCLimits[P[ESCType]];
		if ( P[ESCType] == ESCPPM )
			ESCMin = 1;
		else
		{
			ESCMin = 0;
			for ( i = 0; i < NoOfMotors; i++ )
				ESCI2CFail[i] = false;
			InitI2CESCs();
		}

		#ifdef RX6CH
		NavSensitivity = ((int16)P[PercentNavSens6Ch] * RC_MAXIMUM)/100L;
		#endif // RX6CH

		for ( i = 0; i < CONTROLS; i++) // make reverse map
			RMap[Map[P[TxRxType]][i]-1] = i+1;
	
		IdleThrottle = ((int16)P[PercentIdleThr] * OUT_MAXIMUM )/100L;
		IdleThrottle = Max( IdleThrottle, RC_THRES_STOP );
		HoverThrottle = ((int16)P[PercentHoverThr] * OUT_MAXIMUM )/100L;
	
		RollIntLimit256 = (int16)P[RollIntLimit] * 256L;
		PitchIntLimit256 = (int16)P[PitchIntLimit] * 256L;
		YawIntLimit256 = (int16)P[YawIntLimit] * 256L;
	
		NavNeutralRadius = Limit((int16)P[NeutralRadius], 0, NAV_MAX_NEUTRAL_RADIUS);
		NavClosingRadius = Limit((int16)P[NavRadius], NAV_MAX_NEUTRAL_RADIUS+1, NAV_MAX_RADIUS);
		NavNeutralRadius *= 100; // cm
		NavClosingRadius *= 100;
		NavCloseToNeutralRadius = NavClosingRadius - NavNeutralRadius;

		CompassOffset = (((COMPASS_OFFSET_DEG - (int16)P[NavMagVar])*MILLIPI)/180L);
		F.UsingXMode = ( (P[ConfigBits] & FlyXModeMask) != 0);
		if ( F.UsingXMode )
			CompassOffset -= QUARTERMILLIPI;
	
		F.UsingSerialPPM = ((P[ConfigBits] & RxSerialPPMMask) != 0);
		PIE1bits.CCP1IE = false;
		DoRxPolarity();
		PPM_Index = PrevEdge = 0;
		PIE1bits.CCP1IE = true;

		F.UsingTxMode2 = ((P[ConfigBits] & TxMode2Mask) != 0);
		F.UsingGPSAlt = ((P[ConfigBits] & UseGPSAltMask) != 0);
		F.UsingRTHAutoDescend = ((P[ConfigBits] & UseRTHDescendMask) != 0);
		F.UsingTelemetry = ((P[ConfigBits] & UseTelemetryMask) != 0);

		BatteryVolts = (int16)P[LowVoltThres];

		P[BaroScale] = TEMPORARY_BARO_SCALE; // zzz TEMP until new UAVPSet
		
		ParametersChanged = false;
	}
	
} // ReadParametersEE

void WriteParametersEE(uint8 s)
{
	uint8 p;
	uint8 b;
	uint16 addr;
	
	addr = (s - 1)* MAX_PARAMETERS;
	for ( p = 0; p < MAX_PARAMETERS; p++)
		WriteEE( addr + p,  P[p]);
} // WriteParametersEE

void UseDefaultParameters(void)
{ // loads a representative set of initial parameters as a base for tuning
	uint8 p;

	for ( p = 0; p < MAX_PARAMETERS; p++ )
		P[p] = DefaultParams[p];

	WriteParametersEE(1);
	WriteParametersEE(2);
	CurrentParamSet = 1;
	TxString("\r\nDefault Parameters Loaded\r\n");
	TxString("Do a READ CONFIG to refresh the UAVPSet parameter display\r\n");	
} // UseDefaultParameters

void UpdateParamSetChoice(void)
{
	#define STICK_WINDOW 30

	int8 NewParamSet, NewRTHAltitudeHold, NewTurnToHome, Selector;

	NewParamSet = CurrentParamSet;
	NewRTHAltitudeHold = F.RTHAltitudeHold;
	NewTurnToHome = F.TurnToHome;

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
				NewRTHAltitudeHold = true;
			}
			else
				if ( Selector > STICK_WINDOW ) // right
				{ // bottom right
					NewParamSet = 2;
					NewRTHAltitudeHold = true;
				}
		}		
		else
			if ( DesiredPitch < -STICK_WINDOW ) // top
			{		
				if ( Selector < -STICK_WINDOW ) // left
				{
					NewRTHAltitudeHold = false;
					NewParamSet = 1;
				}
				else 
					if ( Selector > STICK_WINDOW ) // right
					{
						NewRTHAltitudeHold = false;
						NewParamSet = 2;
					}
			}

		if ( ( NewParamSet != CurrentParamSet ) || ( NewRTHAltitudeHold != F.RTHAltitudeHold ) )
		{	
			CurrentParamSet = NewParamSet;
			F.RTHAltitudeHold = NewRTHAltitudeHold;
			LEDBlue_ON;
			DoBeep100mSWithOutput(2, 2);
			if ( CurrentParamSet == 2 )
				DoBeep100mSWithOutput(2, 2);
			if ( F.RTHAltitudeHold )
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
			NewTurnToHome = false;
		else
			if ( Selector > STICK_WINDOW ) // left
				NewTurnToHome = true; // right
			
		if ( NewTurnToHome != F.TurnToHome )
		{		
			F.TurnToHome = NewTurnToHome;
			LEDBlue_ON;
			if ( F.TurnToHome )
				DoBeep100mSWithOutput(4, 2);

			LEDBlue_OFF;
		}
	}
} // UpdateParamSetChoice

void InitParameters(void)
{
	ALL_LEDS_ON;
	CurrentParamSet = 1;

	if ( ReadEE(TxRxType) == -1 )
		UseDefaultParameters();
	
	CurrentParamSet = 1;
	ParametersChanged = true;
	ReadParametersEE();
	ALL_LEDS_OFF;
} // InitParameters


