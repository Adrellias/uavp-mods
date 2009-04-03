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

#ifdef ICD2_DEBUG
#pragma	config OSC=HS, WDT=OFF, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC
#else
#pragma	config OSC=HS, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC 
#endif

#include "UAVX.h"

// Prototypes

void main(void);
void CheckThrottleClosed(void);
void InitMisc(void);

// Defines

#define LEGACY_OFFSET 5
#define LOCKSTEP_RCFRAMES

// Variables

uint8	State;
uint8	Flags[32];

void InitMisc(void)
{
	uint8 i;

	LedShadow = 0;
    ALL_LEDS_OFF;

	for (i=32; i ; i--)
		Flags[i] =  false;

	InitControl();

	InitOutputs();
					
} // InitMisc

int16 xxx; // zzz

void main(void)
{
	uint8 i;

	DisableInterrupts;
	InitMisc();
	InitPorts();
	OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
			USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_HIGH, _B38400);	
	InitADC();
						
#ifdef INC_PARAMS
	for (i=_EESet2*2; i ; i--)					// clear EEPROM parameter space
		WriteEE(i, -1);
	WriteParametersEE(1);						// copy RAM initial values to EE
	WriteParametersEE(2);
#endif
	ReadParametersEE();
	InitTimersAndInterrupts();
	InitAttitude();				
	AcquireSatellites();

	ShowSetup(1);

	_MotorsEnabled = false;
	DesiredThrottle = 0;

	ALL_LEDS_OFF;
	LedRed_ON;

	#ifdef LOCKSTEP_RCFRAMES
	mS[UpdateTimeout] = mS[Clock] + RC_SIGNAL_TIMEOUT; // large timout initially
	#else
	mS[UpdateTimeout]	= mS[Clock] + ImpulsePeriod + LEGACY_OFFSET;
	#endif // LOCKSTEP_RCFRAMES

	State = Initialising;

	while ( 1 )
	{
		ProcessCommand();

		#ifdef LOCKSTEP_RCFRAMES
		if ( _NewValues || ( mS[Clock] >= mS[UpdateTimeout] ) )
		{
			mS[UpdateTimeout] = mS[Clock] + RC_FRAME_TIMEOUT;	
			if ( _NewValues )
				UpdateControls();
			DoControl();
		} 
		#else
		if ( Clock] >= Update] )
		{
			mS[UpdateTimeout] = mS[Clock] + Limit(ImpulsePeriod + LEGACY_OFFSET, 15, 20);	
			if ( _NewValues )
				UpdateControls();
			DoControl();
		}
		#endif // LOCKSTEP_RCFRAMES

#ifdef DUMMY_GPS
		if ( 1 )
#else
		if ( Switch )
#endif
		{		
			switch ( State) {
			case Flying:
			{
				ALL_LEDS_OFF;
				LedGreen_ON;
				if ( _Signal )
				{
					ResetTimeOuts();
					if ( _Hold )
						HoldStation();
					else
						_HoldingStation = false;
				}	
				else
					if ( mS[Clock] > mS[FailsafeTimeout] )
					{
						mS[FailsafeTimeout] = mS[Clock] + FAILSAFE_CANCEL;
						State = Failsafe;
					}
				break;
			}

			case Failsafe:
			{
	 			// hold all current settings including throttle
				ALL_LEDS_OFF;
				LedYellow_ON;
				if ( mS[Clock] > mS[AutonomousTimeout] )
				{
					mS[Autonomous] = mS[Clock] + AUTONOMOUS_CANCEL;
					State = Autonomous;
				}
				else
					if ( RCLinkRestored(FAILSAFE_CANCEL) )
						State = Flying;
			}

			case Autonomous:
			{
				ALL_LEDS_OFF;
				LedRed_ON;
				DoAutonomous();
				if ( RCLinkRestored(AUTONOMOUS_CANCEL) )
					State = Flying;
				break;
			}

			case Landed:
			{
#ifdef DUMMY_GPS
DesiredThrottle = 100;
_Signal = true;
#endif	
				if ( _Signal )
				{
					ALL_LEDS_OFF;
					LedGreen_ON;
					ResetTimeOuts();
	
					if ( DesiredThrottle > RC_THRES_START )
					{
						_MotorsEnabled = true;
						State = Flying;
					}
				}
				else
				{
					// do nothing - Red Led should be flashing
					DesiredThrottle = DesiredRoll = DesiredPitch = DesiredYawRate = 0;
				}
				break;
			}

			case Initialising:
			{
				_Armed |= RC[ThrottleC] < RC_THRES_START;
				_UseCh7Trigger =  RC[CamRollC] < RC_NEUTRAL;
				ResetTimeOuts();


#ifdef DUMMY_GPS
_Armed = true;
#endif				
				if ( _Armed )
				{
					_MotorsEnabled = true;
					State = Landed;
				}
				break;
			} // States

			}
		
			CheckAlarms();
			DoDebugTraces();

		} // while arming switch is on
		else
		{
			ALL_LEDS_OFF;
			LedRed_ON;
			_MotorsEnabled = _Armed = false;
			State = Initialising;
		}
	}
	// CPU should never arrive here	
} // Main
