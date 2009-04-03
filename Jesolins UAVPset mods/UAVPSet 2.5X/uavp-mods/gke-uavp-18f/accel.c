// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
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

// Accelerator sensor routine

#include "c-ufo.h"
#include "bits.h"

void GetNeutralAccelerations(void)
{
	// this routine is called ONLY ONCE while booting
	// read 16 time all 3 axis of linear sensor.
	// Puts values in Neutralxxx registers.
	uint8 i;

	Delay100mSWithOutput(2);	// wait 1/10 sec until LISL is ready to talk
	// already done in caller program
	Rp = 0;
	Pp = 0;
	Yp = 0;
	for( i=0; i < 16; i++)
	{
		while( (ReadLISL(LISL_STATUS + LISL_READ) & 0x08) == 0 );
		ReadAccelerations();
		
		Rp += Ax;
		Pp += Az;
		Yp += Ay;
	}
	Rp = SRS16(Rp + 8, 4);
	Pp = SRS16(Pp + 8, 4);
	Yp = SRS16(Yp + 8, 4);

	NeutralLR = Limit(Rp, -99, 99);
	NeutralFB = Limit(Pp, -99, 99);
	NeutralUD = Limit(Yp-1024, -99, 99); // -1g
} // GetNeutralAccelerations

void AccelerationCompensation(void)
{
	int16 AbsRollSum, AbsPitchSum, Temp;
	if( _UseLISL )
	{
		ReadAccelerations();

		Rp = Ax;
		Yp = Ay;
		Pp = Az;;
		
		// NeutralLR ,NeutralFB, NeutralUD pass through UAVPSet 
		// and come back as MiddleLR etc.
	
		// 1 unit is 1/4096 of 2g = 1/2048g
		Rp -= MiddleLR;
		Pp -= MiddleFB;
		Yp -= MiddleUD;
	
		Yp -= 1024;	// subtract 1g
	
		#ifdef ENABLE_VERTICAL_VELOCITY_DAMPING
		// UDSum rises if ufo climbs
		// Empirical - vertical acceleration decreases at ~approx Angle/8

		AbsRollSum = Abs(RollSum);
		AbsPitchSum = Abs(PitchSum);

		if ( (AbsRollSum < 200) && ( AbsPitchSum < 200) ) // ~ 10deg
			UDSum += Yp + SRS16( AbsRollSum + AbsPitchSum, 3);

		UDSum = Limit(UDSum , -16384, 16384); 
		UDSum = DecayBand(UDSum, -10, 10, 10);
	
		Temp = SRS16(SRS16(UDSum + 8, 4) * (int16) LinUDIntFactor + 128, 8);
		if( (BlinkCount & 0x03) == 0 )	
			if( Temp > Vud ) 
				Vud++;
			else
				if( Temp < Vud )
					Vud--;
	
		Vud = Limit(Vud, -20, 20);
	
		#endif // ENABLE_VERTICAL_VELOCITY_DAMPING

		#ifdef DEBUG_SENSORS
		if( IntegralCount == 0 )
		{
			Trace[TAx]= Rp;
			Trace[TAz] = Pp;
			Trace[TAy] = Yp;

			Trace[TUDSum] = UDSum;
			Trace[TVud] = Vud;
		}
		#endif
	
		// Roll

		// Static compensation due to Gravity
		#ifdef OPT_ADXRS
		Rp -= SRS16(RollSum * 11 + 16, 5);
		#else // OPT_IDG
		Rp -= SRS16(RollSum * (-15) + 16, 5); 
		#endif
	
		#ifndef ENABLE_DYNAMIC_MASS_COMP_ROLL
		// dynamic correction of moved mass
		#ifdef OPT_ADXRS
		Rp += (int16)RollRate << 1;
		#else // OPT_IDG
		Rp -= (int16)RollRate;
		#endif	
		#else
		// no dynamic correction of moved mass
		#endif

		// correct DC level of the integral
		LRIntKorr = 0;
		#ifdef OPT_ADXRS
		if( Rp > 10 ) LRIntKorr =  1;
		else
			if( Rp < 10 ) LRIntKorr = -1;
		#else // OPT_IDG
		if( Rp > 10 ) LRIntKorr = -1;
		else
			if( Rp < 10 ) LRIntKorr =  1;
		#endif
	
		// Pitch

		Pp = -Pp;				// long standing BUG!

		// Static compensation due to Gravity
		#ifdef OPT_ADXRS
		Pp -= SRS16(PitchSum * 11 + 16, 5);	
		#else // OPT_IDG
		Pp -= SRS16(PitchSum * (-15) + 16, 5);
		#endif
		
		#ifndef ENABLE_DYNAMIC_MASS_COMP_PITCH
		// dynamic correction of moved mass
		#ifdef OPT_ADXRS
		Pp += (int16)PitchRate << 1;
		#else // OPT_IDG
		Pp -= (int16)PitchRate;
		#endif	
		#else
		// no dynamic correction of moved mass
		#endif
	
		// correct DC level of the integral
		FBIntKorr = 0;
		#ifdef OPT_ADXRS
		if( Pp > 10 ) FBIntKorr =  1; 
		else 
			if( Pp < 10 ) FBIntKorr = -1;
		#endif
		#ifdef OPT_IDG
		if( Pp > 10 ) FBIntKorr = -1;
		else
			if( Pp < 10 ) FBIntKorr =  1;
		#endif		
	}	
	else
	{
		Vud = 0;
		#ifdef DEBUG_SENSORS
		Trace[TAx] = 0;
		Trace[TAz] = 0;
		Trace[TAy] = 0;

		Trace[TUDSum] = 0;
		Trace[TVud] = 0;
		#endif
	}
} // AccelerationCompensation

