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

#ifdef CLOCK_40MHZ
#pragma	config OSC=HSPLL, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC
#else
#pragma	config OSC=HS, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC  
#endif

#include "uavx.h"

Flags 	F;
  
void main(void)
{
	static int16	Temp;
	static uint8	b;

	DisableInterrupts;

	InitMisc();
	ReadStatsEE();

	InitPorts();

	OpenUSART(USART_TX_INT_OFF&USART_RX_INT_OFF&USART_ASYNCH_MODE&
				USART_EIGHT_BIT&USART_CONT_RX&USART_BRGH_HIGH, _B38400);

	InitADC();		
	InitTimersAndInterrupts();
	InitParameters();

	StopMotors();
	INTCONbits.PEIE = true;	
	INTCONbits.TMR0IE = true; 
	EnableInterrupts;

	InitLISL();
	InitCompass();
	InitRangefinder();
	InitGPS();
	InitNavigation();
	InitBarometer();

	ShowSetup(1);

	FirstPass = true;
	
	while( true )
	{
		StopMotors();
		ReceivingGPSOnly(false);
		EnableInterrupts;

		LightsAndSirens();	// Check for Rx Signal, Disarmed on power up, Throttle closed
	
		State = Starting;
		F.MotorsArmed = true;

		#ifdef FAKE_FLIGHT 

		FakeFlight();

		#else

		while ( Armed )
		{ // no command processing while the Quadrocopter is armed
	
			ReceivingGPSOnly(true); 

			UpdateGPS();
			UpdateControls();

			if ( F.Signal && ( FailState != Terminated ) && ( FailState != Returning ) )
			{
				switch ( State  ) {
				case Starting:	// this state executed once only after arming

					LEDYellow_OFF;
					ZeroStats();
					InitControl();
					CaptureTrims();

					InitHeading();
					InitGPS();
					InitNavigation();

					DesiredThrottle = 0;
					ErectGyros();				// DO NOT MOVE AIRCRAFT!
					InitBarometer(); 			// as late as possible to allow some warmup
					DoStartingBeepsWithOutput(3);

					State = Landed;
					break;
				case Landed:
					if ( DesiredThrottle < IdleThrottle  )
						SetGPSOrigin();
					else
					{
						InitHeading();						
						LEDCycles = 1;
						mS[NavActiveTime] = mS[Clock] + NAV_ACTIVE_DELAY_MS;
						Stats[RCGlitchesS].i16 = RCGlitches; // start of flight
						State = InFlight;	
					}
						
					break;
				case Landing:
					if ( DesiredThrottle > IdleThrottle )
						State = InFlight;
					else
						if ( mS[Clock] < mS[ThrottleIdleTimeout] )
							DesiredThrottle = IdleThrottle;
						else
						{		
							Stats[RCGlitchesS].i16 = RCGlitches - Stats[RCGlitchesS].i16;	
							WriteStatsEE();
							State = Landed;
						}
					break;
				case InFlight:
					if ( F.NavValid && F.GPSValid && F.CompassValid  && F.NewCommands 
						&& ( mS[Clock] > mS[NavActiveTime]) )
						DoNavigation();

					LEDGame();
					if ( DesiredThrottle < IdleThrottle )
					{
						mS[ThrottleIdleTimeout] = mS[Clock] + THROTTLE_LOW_DELAY_MS;
						State = Landing;
					}
					break;

				} // Switch State
				F.LostModel = false;
				mS[FailsafeTimeout] = mS[Clock] + FAILSAFE_TIMEOUT_MS;
				FailState = Waiting;
			}
			else
				DoPPMFailsafe();

			GetRollPitchGyroValues();				// First gyro sample
			GetHeading();
			CheckThrottleMoved();
			GetBaroAltitude();
			AltitudeHold();
	
			while ( mS[Clock] < mS[UpdateTimeout] ) {}; // cycle sync. point
			mS[UpdateTimeout] = mS[Clock] + (uint24)P[TimeSlots];

			GetRollPitchGyroValues();				// Second gyro sample
			DoControl();

			MixAndLimitMotors();
			MixAndLimitCam();
			OutSignals();							// some jitter because sync precedes this

			CheckAlarms();
			DumpTrace();
		
		} // flight while armed
		#endif // FAKE_FLIGHT
	}
} // main

