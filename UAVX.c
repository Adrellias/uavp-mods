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
#pragma	config OSC=HSPLL, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC, XINST = OFF
#else
#pragma	config OSC=HS, WDT=OFF, PWRT=ON, MCLRE=OFF, LVP=OFF, PBADEN=OFF, CCP2MX = PORTC, XINST = OFF  
#endif

#include "uavx.h"

Flags 	F;
uint8 p;
#pragma udata access statevars
int8 near State, NavState, FailState;
#pragma udata

void main(void)
{
	DisableInterrupts;

	InitPortsAndUSART();
	InitTimersAndInterrupts();

	InitMisc();
	InitADC();
	InitI2C();
	InitParameters();
	ReadStatsEE();		
	InitRC();
	InitMotors();

    EnableInterrupts;

	LEDYellow_ON;
	Delay100mSWithOutput(5);	// let all the sensors startup

	InitAccelerometers();
	InitGyros();
	InitCompass();
	InitHeading();
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
		F.FirstArmed = false;

		while ( Armed )
		{ // no command processing while the Quadrocopter is armed
	
			ReceivingGPSOnly(true); 

			UpdateGPS();
			if ( F.RCNewValues )
				UpdateControls();

			if ( ( F.Signal ) && ( FailState == MonitoringRx ) )
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
					InitGPS();
					InitNavigation();

					DesiredThrottle = 0;
					ErectGyros();				// DO NOT MOVE AIRCRAFT!
					ZeroStats();
					InitBarometer(); // try to get launch alt as close as possible.
					DoStartingBeepsWithOutput(3);

					SendParameters(0);
					SendParameters(1);

					mS[ArmedTimeout] = mSClock() + ARMED_TIMEOUT_MS;
					mS[RxFailsafeTimeout] = mSClock() + RC_NO_CHANGE_TIMEOUT_MS;
					F.ForceFailsafe = F.LostModel = false;

					State = Landed;
					break;
				case Landed:
					DesiredThrottle = 0;
					if ( mSClock() > mS[ArmedTimeout] )
						DoShutdown();
					else	
						if ( StickThrottle < IdleThrottle )
						{
							SetGPSOrigin();
							GetHeading();
							DecayNavCorr(6);
	    					if ( F.NewCommands )
								F.LostModel = F.ForceFailsafe;
						}
						else
						{
							#ifdef SIMULATE
							FakeBaroRelAltitude = 0;
							#endif // SIMULATE						
							LEDPattern = 0;
							mS[NavActiveTime] = mSClock() + NAV_ACTIVE_DELAY_MS;
							Stats[RCGlitchesS] = RCGlitches; // start of flight
							SaveLEDs();

							mS[RxFailsafeTimeout] = mSClock() + RC_NO_CHANGE_TIMEOUT_MS;
							F.ForceFailsafe = F.LostModel = false;

							if ( ParameterSanityCheck() )
								State = InFlight;
							else
								ALL_LEDS_ON;	
						}						
					break;
				case Landing:
					if ( StickThrottle > IdleThrottle )
					{
						DesiredThrottle = 0;
						State = InFlight;
					}
					else
						if ( mSClock() < mS[ThrottleIdleTimeout] )
							DesiredThrottle = IdleThrottle;
						else
						{
							DecayNavCorr(6);
							DesiredThrottle = AltComp = 0; // to catch cycles between Rx updates
							F.MotorsArmed = false;
							Stats[RCGlitchesS] = RCGlitches - Stats[RCGlitchesS];	
							WriteStatsEE();
							mS[ArmedTimeout] = mSClock() + ARMED_TIMEOUT_MS;
							State = Landed;
						}
					break;
				case Shutdown:
					// wait until arming switch is cycled
					NavCorr[Roll] = NavCorr[Pitch] = 0;
					F.LostModel = true;
					DesiredRoll = DesiredPitch = DesiredYaw = AltComp = 0;
					StopMotors();
					break;
				case InFlight:
					F.MotorsArmed = true;		
					LEDChaser();

					DesiredThrottle = SlewLimit(DesiredThrottle, StickThrottle, 1);

					DoNavigation();
					AltitudeHold();

					if ( StickThrottle < IdleThrottle )
					{
						AltComp = 0;
						mS[ThrottleIdleTimeout] = mSClock() + THROTTLE_LOW_DELAY_MS;
						RestoreLEDs();
						State = Landing;
					}

					break;
				} // Switch State
				mS[FailsafeTimeout] = mSClock() + FAILSAFE_TIMEOUT_MS;
				FailState = MonitoringRx;
			}
			else
				if ( F.FailsafesEnabled )
					DoFailsafe();

			GetHeading();

			while ( WaitingForSync ) {};

			PIDUpdate = mSClock() + PID_CYCLE_MS;

			GetGyroValues();
			
			DoControl();

			MixAndLimitMotors();
			MixAndLimitCam();
			OutSignals();

			GetTemperature(); 
			CheckAlarms();

			CheckTelemetry();
			SensorTrace();
		
		} // flight while armed
	}

} // main

