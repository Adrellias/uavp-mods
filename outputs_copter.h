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


void OutSignals(void)
{	// The PWM pulses are in two parts these being a 1mS preamble followed by a 0-1mS part. 
	// Interrupts are enabled during the first part which uses TMR0.  TMR0 is monitored until 
	// there is just sufficient time for one remaining interrupt latency before disabling 
	// interrupts.  We do this because there appears to be no atomic method of detecting the 
	// remaining time AND conditionally disabling the interupt. 
	static int8 m;
	static uint8 r;
	static i16u SaveTimer0;
	static uint24 SaveClockmS;

	#ifdef UAVX_HW
		ServoToggle = 1;
	#endif // UAVX_HW

	#if !( defined SIMULATE | defined TESTING )

	if ( !F.MotorsArmed )
		StopMotors();

	// Save TMR0 and reset
	DisableInterrupts;
	SaveClockmS = mS[Clock];
	GetTimer0;
	SaveTimer0.u16 = Timer0.u16;
	FastWriteTimer0(TMR0_1MS);
	INTCONbits.TMR0IF = false;
	EnableInterrupts;

	PWM[FrontC] = Limit(PWM[FrontC], ESCMin, ESCMax);
	PWM[LeftC] = Limit(PWM[LeftC], ESCMin, ESCMax);
	PWM[RightC] = Limit(PWM[RightC], ESCMin, ESCMax);
	#ifdef TRICOPTER
		PWM[BackC] = Limit(PWM[BackC], 1, OUT_MAXIMUM);
	#else
		PWM[BackC] = Limit(PWM[BackC], ESCMin, ESCMax);
	#endif
	PWM[CamRollC] = Limit(PWM[CamRollC], 1, OUT_MAXIMUM);
	PWM[CamPitchC] = Limit(PWM[CamPitchC], 1, OUT_MAXIMUM);

	PWM0 = PWM[FrontC];
	PWM1 = PWM[LeftC];
	PWM2 = PWM[RightC];
	PWM3 = PWM[BackC];
	PWM4 = PWM[CamRollC];
	PWM5 = PWM[CamPitchC];

	if ( P[ESCType] == ESCPPM )
	{
		#ifdef TRICOPTER
			PORTB |= 0x07;
		#else
			PORTB |= 0x0f;
		#endif // TRICOPTER

		_asm
		MOVLB	0							// select Bank0
		#ifdef TRICOPTER
			MOVLW	0x07					// turn on 3 motors
		#else
			MOVLW	0x0f					// turn on all motors
		#endif // TRICOPTER
		MOVWF	SHADOWB,1
		_endasm
	
		SyncToTimer0AndDisableInterrupts();

		if( ServoToggle == 0 )	// driver cam servos only every 2nd pulse
		{
			PORTB |= 0x3f;
			_asm
			MOVLB	0						// select Bank0
			MOVLW	0x3f					// turn on all motors
			MOVWF	SHADOWB,1
			_endasm	
		}
		else
		{
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY();
		}

		_asm
		MOVLB	0							// select Bank0
OS005:
		MOVF	SHADOWB,0,1	
		MOVWF	PORTB,0
		#ifdef TRICOPTER
			ANDLW	0x07		
		#else
			ANDLW	0x0f
		#endif //TRICOPTER
		BZ		OS006
			
		DECFSZ	PWM0,1,1				
		GOTO	OS007
					
		BCF		SHADOWB,0,1
OS007:
		DECFSZ	PWM1,1,1		
		GOTO	OS008
					
		BCF		SHADOWB,1,1
OS008:
		DECFSZ	PWM2,1,1
		GOTO	OS009
					
		BCF		SHADOWB,2,1

OS009:
		#ifndef TRICOPTER
		DECFSZ	PWM3,1,1	
		GOTO	OS010
						
		BCF		SHADOWB,3,1	
		#endif // !TRICOPTER		

OS010:
		_endasm

		#ifdef TRICOPTER
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY();  
		#endif // TRICOPTER 

		#ifdef CLOCK_40MHZ
			Delay10TCYx(2); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
		#endif // CLOCK_40MHZ
		_asm				
		GOTO	OS005
OS006:
		_endasm
		
		EnableInterrupts;
		SyncToTimer0AndDisableInterrupts();	
	} 
	else
	{ // I2C ESCs
			if( ServoToggle == 0 )	// driver cam servos only every 2nd pulse
		{
			#ifdef TRICOPTER
				PORTB |= 0x38;
			#else
				PORTB |= 0x30;
			#endif // TRICOPTER

			_asm
			MOVLB	0					// select Bank0
			#ifdef TRICOPTER
				MOVLW	0x38				// turn on 3 servoes
			#else
				MOVLW	0x30				// turn on 2 servoes
			#endif // TRICOPTER
			MOVWF	SHADOWB,1
			_endasm	
		}
		else
		{
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY();
		}

		#ifdef MULTICOPTER
		// in X3D and Holger-Mode, K2 (left motor) is SDA, K3 (right) is SCL.
		// ACK (r) not checked as no recovery is possible. 
		// Octocopters may have ESCs paired with common address so ACK is meaningless.
		// All motors driven with fourth motor ignored for Tricopter.
	
		if ( P[ESCType] ==  ESCHolger )
			for ( m = 0 ; m < NoOfI2CESCOutputs ; m++ )
			{
				ESCI2CStart();
				r = WriteESCI2CByte(0x52 + ( m*2 ));		// one command, one data byte per motor
				r += WriteESCI2CByte( PWM[m] );
				ESCI2CFail[m] += r;
				ESCI2CStop();
			}
		else
			if ( P[ESCType] == ESCYGEI2C )
				for ( m = 0 ; m < NoOfI2CESCOutputs ; m++ )
				{
					ESCI2CStart();
					r = WriteESCI2CByte(0x62 + ( m*2) );	// one cmd, one data byte per motor
					r += WriteESCI2CByte( PWM[m]>>1 );
					ESCI2CFail[m] += r;
					ESCI2CStop();
				}
			else
				if ( P[ESCType] == ESCX3D )
				{
					ESCI2CStart();
					r = WriteESCI2CByte(0x10);				// one command, 4 data bytes
					r += WriteESCI2CByte( PWM[FrontC] ); 
					r += WriteESCI2CByte( PWM[BackC] );
					r += WriteESCI2CByte( PWM[LeftC] );
					r += WriteESCI2CByte( PWM[RightC] );
					ESCI2CFail[0] += r;
//  other ESCs if a Hexacopter
					ESCI2CStop();
				}
		#endif //  MULTICOPTER
	}

	if ( ServoToggle == 0 )
	{	
		_asm
		MOVLB	0
OS001:
		MOVF	SHADOWB,0,1	
		MOVWF	PORTB,0
		#ifdef TRICOPTER
			ANDLW	0x38
		#else
			ANDLW	0x30
		#endif  // TRICOPTER		
		BZ		OS002	
	
		DECFSZ	PWM4,1,1
		GOTO	OS003

		BCF		SHADOWB,4,1
	
OS003:
		DECFSZ	PWM5,1,1
		GOTO	OS004
	
		BCF		SHADOWB,5,1

OS004:
		#ifdef TRICOPTER
			DECFSZ	PWM3,1,1
			GOTO	OS0011
	
			BCF		SHADOWB,3,1
		#endif // TRICOPTER
OS0011:
		_endasm
	
		#ifndef TRICOPTER
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
		#endif // !TRICOPTER
 
		Delay1TCY();
		Delay1TCY(); 
		Delay1TCY();
	
		#ifdef CLOCK_40MHZ
			Delay10TCYx(2); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
			Delay1TCY(); 
		#endif // CLOCK_40MHZ
	
		_asm
	
		GOTO	OS001
OS002:
		_endasm

		EnableInterrupts;
		SyncToTimer0AndDisableInterrupts();
	}

	FastWriteTimer0(SaveTimer0.u16);
	// the 1mS clock seems to get in for 40MHz but not 16MHz so like this for now?
	if ( P[ESCType] == ESCPPM )
		if ( ServoToggle == 0 )
			Clock[mS] = SaveClockmS + 3;
		else
			Clock[mS] = SaveClockmS + 2;
	else
		if ( ServoToggle == 0 )
			Clock[mS] = SaveClockmS + 2;
		else
			Clock[mS] = SaveClockmS;

	if ( ++ServoToggle == ServoInterval )
		ServoToggle = 0;

	EnableInterrupts;
	
	#endif // !(SIMULATE | TESTING)

} // OutSignals




