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
	InitPortsAndUSART();
	InitADC();
	InitI2C(MASTER, SLEW_ON);
	InitParameters();		
	InitRC();
	InitTimersAndInterrupts();
	InitMotors();

	EnableInterrupts;

	LEDYellow_ON;
	Delay100mSWithOutput(5);	// let all the sensors startup

	InitAttitude();
	InitRangefinder();
	InitTemperature();
	InitBarometer();

	ShowSetup(true);

	FirstPass = true;
	
	while( true )
	{
		StopMotors();

		ReceivingRazorOnly(false);
		EnableInterrupts;

		LightsAndSirens();	// Check for Rx signal, disarmed on power up, throttle closed, gyros ONLINE

		State = Starting;
		F.FirstArmed = false;

		while ( Armed )
		{ // no command processing while the Quadrocopter is armed
			ReceivingRazorOnly(true);

			if ( F.RCNewValues )
				UpdateControls();

			if ( F.Signal )
			{
				switch ( State  ) {
				case Starting:	// this state executed once only after arming

					LEDYellow_OFF;

					if ( !F.FirstArmed )
					{
						mS[StartTime] = mSClock();
						F.FirstArmed = true;
					}

					InitControl();
					CaptureTrims();

					DesiredThrottle = 0;
					ErectGyros();				// DO NOT MOVE AIRCRAFT!
					ZeroStats();
					DoStartingBeepsWithOutput(3);

					State = Landed;
					break;
				case Landed:
					if ( StickThrottle < IdleThrottle )
						DesiredThrottle = 0;
					else
					{
						#ifdef SIMULATE
						FakeBaroRelAltitude = 0;
						#endif // SIMULATE						
						LEDPattern = 0;
						Stats[RCGlitchesS] = RCGlitches; // start of flight
						SaveLEDs = LEDShadow;
						if ( ParameterSanityCheck() )
							State = InFlight;
						else
							ALL_LEDS_ON;	
					}
						
					break;
				case Landing:
					if ( StickThrottle > IdleThrottle )
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
					LEDChaser();

					DesiredThrottle = SlewLimit(DesiredThrottle, StickThrottle, 1);

					if ( StickThrottle < IdleThrottle )
					{
						mS[ThrottleIdleTimeout] = mSClock() + THROTTLE_LOW_DELAY_MS;
						LEDShadow = SaveLEDs;
						State = Landing;
					}
					break;
				} // Switch State
				F.LostModel = false;
			}
			else
			{
				Stats[RCFailsafesS]++;
				DesiredRoll = DesiredPitch = DesiredYaw = 0;
			    DesiredThrottle = CruiseThrottle; 
			}

			AltitudeHold();

			while ( WaitingForSync ) {};

			mS[UpdateTimeout] += (int24)P[TimeSlots];

			GetAttitude(10); // Razor 9DOF
			
			DoControl();

			MixAndLimitMotors();
			MixAndLimitCam();
			OutSignals();							// some jitter because sync precedes this

			Razor('?');		// start after output routine which locks interrupts

			GetTemperature(); 
			CheckAlarms();

			SensorTrace();
		
		} // flight while armed
	}
} // main
