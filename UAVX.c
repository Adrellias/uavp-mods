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
	InitI2C(MASTER, SLEW_ON);

	InitParameters();		
	InitRC();
	InitTimersAndInterrupts();

	StopMotors();
	INTCONbits.PEIE = true;	
	INTCONbits.TMR0IE = true; 
	EnableInterrupts;

	InitAccelerometers();
	InitGyros();
	InitCompass();
	InitRangefinder();
	InitGPS();
	InitNavigation();
	InitTemperature();
	InitBarometer();

	ShowSetup(true);

	FirstPass = true;
	
	while( true )
	{
		StopMotors();
		ReceivingGPSOnly(false);
		EnableInterrupts;

		LightsAndSirens();	// Check for Rx signal, disarmed on power up, throttle closed, gyros ONLINE

		State = Starting;

		while ( Armed )
		{ // no command processing while the Quadrocopter is armed
	
			ReceivingGPSOnly(true); 

			UpdateGPS();
			UpdateControls();

			if ( ( F.Signal ) && ( FailState == MonitoringRx ) )
			{
				switch ( State  ) {
				case Starting:	// this state executed once only after arming

					LEDYellow_OFF;

					InitControl();
					CaptureTrims();
					InitHeading();
					InitGPS();
					InitNavigation();

					DesiredThrottle = 0;
					ErectGyros();				// DO NOT MOVE AIRCRAFT!
					ZeroStats();
					DoStartingBeepsWithOutput(3);
 					InitBarometer();

					State = Landed;
					break;
				case Landed:
					if ( DesiredThrottle < IdleThrottle )
						SetGPSOrigin();
					else
					{
						#ifdef SIMULATE
						FakeBaroRelAltitude = 0;
						#endif // SIMULATE
						InitHeading();						
						LEDPattern = 0;
						mS[NavActiveTime] = mSClock() + NAV_ACTIVE_DELAY_MS;
						Stats[RCGlitchesS] = RCGlitches; // start of flight
						SaveLEDs = LEDShadow;
						State = InFlight;	
					}
						
					break;
				case Landing:
					if ( DesiredThrottle > IdleThrottle )
						State = InFlight;
					else
						if ( mSClock() < mS[ThrottleIdleTimeout] )
							DesiredThrottle = IdleThrottle;
						else
						{
							DesiredThrottle = 0; // to catch cycles between Rx updates
							F.MotorsArmed = false;
							Stats[RCGlitchesS] = RCGlitches - Stats[RCGlitchesS];	
							WriteStatsEE();
							State = Landed;
						}
					break;
				case Shutdown:
					// wait until arming switch is cycled
					DesiredRoll = DesiredPitch = DesiredYaw = DesiredThrottle = 0;
					StopMotors();
					break;
				case InFlight:
					F.MotorsArmed = true;
					if ( F.GPSValid && F.CompassValid  && F.NewCommands && F.AltHoldEnabled 
						&& ( mSClock() > mS[NavActiveTime]) )
						DoNavigation();
					
					LEDChaser();

					if ( DesiredThrottle < IdleThrottle )
					{
						mS[ThrottleIdleTimeout] = mSClock() + THROTTLE_LOW_DELAY_MS;
						LEDShadow = SaveLEDs;
						State = Landing;
					}
					break;
				} // Switch State
				F.LostModel = false;
				mS[FailsafeTimeout] = mSClock() + FAILSAFE_TIMEOUT_MS;
				FailState = MonitoringRx;
			}
			else
			#ifdef USE_PPM_FAILSAFE
				DoPPMFailsafe();
			#else
			{
				Stats[RCFailsafesS]++;
				DesiredRoll = DesiredPitch = DesiredYaw = 0;
			}
			#endif // USE_PPM_FAILSAFE

			GetHeading();
			AltitudeHold();

			while ( WaitingForSync ) {};

			mS[UpdateTimeout] = mSClock() + (uint24)P[TimeSlots];

			GetGyroValues();
			
			DoControl();

			MixAndLimitMotors();
			MixAndLimitCam();
			OutSignals();							// some jitter because sync precedes this

			GetTemperature(); 
			CheckAlarms();			CheckTelemetry();

			SensorTrace();
		
		} // flight while armed
	}
} // main

