// ===============================================================================================
// =                              UAVXArm Quadrocopter Controller                                =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                           http://code.google.com/p/uavp-mods/                               =
// ===============================================================================================

//    This is part of UAVXArm.

//    UAVXArm is free software: you can redistribute it and/or modify it under the terms of the GNU
//    General Public License as published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.

//    UAVXArm is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.
//    If not, see http://www.gnu.org/licenses/


void OutSignals(void) {   // The PWM pulses are in two parts these being a 1mS preamble followed by a 0-1mS part.
    // Interrupts are enabled during the first part which uses TMR0.  TMR0 is monitored until
    // there is just sufficient time for one remaining interrupt latency before disabling
    // interrupts.  We do this because there appears to be no atomic method of detecting the
    // remaining time AND conditionally disabling the interupt.
    static int8 m;
    static uint8 r, s;
    static i16u SaveTimer0;
    static uint24 SaveClockmS;

    if ( !F.MotorsArmed )
        StopMotors();

    for ( m = 0; m < 6; m++ )
        PWM[m] = Limit(PWM[m], 0, ESCMax);

#if !( defined SIMULATE | defined TESTING )

    Out0.pulsewidth_us(1000 + (int16)( PWM[FrontTC] * PWMScale ) );
    Out1.pulsewidth_us(1000 + (int16)( PWM[RightTC] * PWMScale ) );
    Out2.pulsewidth_us(1000 + (int16)( PWM[LeftTC] * PWMScale ) );
    Out3.pulsewidth_us(1000 + (int16)( PWM[FrontBC] * PWMScale ) );

    xxx   #ifdef USING_PWM4AND5
    Out4.pulsewidth_us(1000 + (int16)( PWM[RightBC] * PWMScale ) );
    Out5.pulsewidth_us(1000 + (int16)( PWM[LeftBC] * PWMScale ) );
#endif // USING_PWM4AND5

#endif // !(SIMULATE | TESTING)

} // OutSignals




