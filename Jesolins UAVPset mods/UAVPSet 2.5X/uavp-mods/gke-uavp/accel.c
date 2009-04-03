// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
// =             Copyright (c) 2007 Ing. Wolfgang Mahringer              =
// =           Extensively modified 2008-9 by Prof. Greg Egan            =
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

#pragma codepage=2
#include "c-ufo.h"
#include "bits.h"

// Math Library
#include "mymath16.h"

// read all acceleration values from LISL sensor
// and compute correction adders (Rp, Pp, Vud)
void CheckLISL(void)
{
	int16 nila1@nilarg1;

	if( _UseLISL )
	{
		ReadLISL(LISL_STATUS + LISL_READ);
		// the LISL registers are in order here!!
		Rp.low8  = (int8)ReadLISL(LISL_OUTX_L + LISL_INCR_ADDR + LISL_READ);
		Rp.high8 = (int8)ReadLISLNext();
		Yp.low8  = (int8)ReadLISLNext();
		Yp.high8 = (int8)ReadLISLNext();
		Pp.low8  = (int8)ReadLISLNext();
		Pp.high8 = (int8)ReadLISLNext();
		LISL_CS = 1;	// end transmission
			
		// NeutralLR ,NeutralFB, NeutralUD pass through UAVPSet 
		// and come back as MiddleLR etc.
	
		// 1 unit is 1/4096 of 2g = 1/2048g
		Rp -= MiddleLR;
		Pp -= MiddleFB;
		Yp -= MiddleUD;
	
		#if 0
		// calc the angles for roll matrices
		// rw = arctan( x*16/z/4 )
		niltemp = Yp * 16;
		niltemp1 = niltemp / Rp;
		niltemp1 >>= 2;
		nila1 = niltemp1;
		Rw = Arctan();
		
		SendComValS(Rw);
		SendComChar(';');
		
		niltemp1 = niltemp / Pp;
		niltemp1 >>= 2;
		nila1 = niltemp1;
		Pw = Arctan();
		
		SendComValS(Pw);
		SendComChar(0x13);
		SendComChar(0x10);
	
		#endif
		
		Yp -= 1024;	// subtract 1g

		#ifdef DEBUG_SENSORS
		if( IntegralCount == 0 )
		{
			SendComValH(Rp.high8);
			SendComValH(Rp.low8);
			SendComChar(';');
			SendComValH(Pp.high8);
			SendComValH(Pp.low8);
			SendComChar(';');
			SendComValH(Yp.high8);
			SendComValH(Yp.low8);
			SendComChar(';');
		}
		#endif
	
		// DO NOT USE - Requires roll & pitch angle compensation
		#ifdef NADA
		// UDSum rises if ufo climbs
		UDSum += Yp - RollPitchComp;
	
		Yp = UDSum;
		Yp += 8;
		Yp >>= 4;
		Yp *= LinUDIntFactor;
		Yp += 128;
	
		if( (BlinkCount & 0x03) == 0 )	
		{
			if( (int8)Yp.high8 > Vud ) 
				Vud++;
			else
				if( (int8)Yp.high8 < Vud )
					Vud--;
	
			if( Vud >  10 ) // was 20
				Vud =  10; 
			else
				if( Vud < -10 ) 
					Vud = -10;
		}
		if( UDSum >  10 ) 
			UDSum -= 10;
		else
			if( UDSum < -10 ) 
				UDSum += 10;
		#endif // ACCEL_VUD
	
		// =====================================
		// Roll-Axis
		// =====================================
		// Static compensation due to Gravity
	
		#ifdef OPT_ADXRS
		Yl = RollSum * 11;	// Rp um RollSum*11/32 korrigieren
		#endif
	
		#ifdef OPT_IDG
		Yl = RollSum * -15; // Rp um RollSum* -15/32 korrigieren
		#endif
		Yl += 16;
		Yl >>= 5;
		Rp -= Yl;
	
		// dynamic correction of moved mass
		#ifdef OPT_ADXRS
		Rp += (int16)RollSamples;
		Rp += (int16)RollSamples;
		#endif
	
		#ifdef OPT_IDG
		Rp -= (int16)RollSamples;
		#endif
	
		// correct DC level of the integral
		LRIntKorr = 0;
		#ifdef OPT_ADXRS
		if( Rp > 10 ) LRIntKorr =  1;
		else
			if( Rp < 10 ) LRIntKorr = -1;
		#endif
		#ifdef OPT_IDG
		if( Rp > 10 ) LRIntKorr = -1;
		else
			if( Rp < 10 ) LRIntKorr =  1;
		#endif
	
		#ifdef NADA
		// Integral addieren, Abkling-Funktion
		Yl = LRSum >> 4;
		Yl >>= 1;
		LRSum -= Yl;	// LRSum * 0.96875
		LRSum += Rp;
		Rp = LRSum + 128;
		LRSumPosi += (int8)Rp.high8;
		if( LRSumPosi >  2 ) LRSumPosi -= 2;
		else
			if( LRSumPosi < -2 ) LRSumPosi += 2;
	
		// Korrekturanteil fuer den PID Regler
		Rp = LRSumPosi * LinLRIntFactor;
		Rp += 128;
		Rp = (int8)Rp.high8;
		// limit output
		if( Rp >  2 ) Rp = 2;
		else
			if( Rp < -2 ) Rp = -2;
		#endif // NADA
	
		// =====================================
		// Pitch-Axis
		// =====================================
		// Static compensation due to Gravity

		Pp =-Pp; // long standing error???
	
		#ifdef OPT_ADXRS
		Yl = PitchSum * 11;	// Pp um RollSum* 11/32 korrigieren
		#endif
		#ifdef OPT_IDG
		Yl = PitchSum * -15;	// Pp um RollSum* -14/32 korrigieren
		#endif
		Yl += 16;
		Yl >>= 5;
	
		Pp -= Yl;
		// no dynamic correction of moved mass necessary
	
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
	
		#ifdef NADA
		// Integral addieren
		// Integral addieren, Abkling-Funktion
		Yl = FBSum >> 4;
		Yl >>= 1;
		FBSum -= Yl;	// LRSum * 0.96875
		FBSum += Pp;
		Pp = FBSum + 128;
		FBSumPosi += (int8)Pp.high8;
		if( FBSumPosi >  2 ) FBSumPosi -= 2;
		else
			if( FBSumPosi < -2 ) FBSumPosi += 2;
	
		// Korrekturanteil fuer den PID Regler
		Pp = FBSumPosi * LinFBIntFactor;
		Pp += 128;
		Pp = (int8)Pp.high8;
		// limit output
		if( Pp >  2 ) 
			Pp = 2;
		else
			if( Pp < -2 ) 
				Pp = -2;
		#endif // NADA
	}
	else
	{
		Vud = 0;

		#ifdef DEBUG_SENSORS
		SendComChar(';');
		SendComChar(';');
		SendComChar(';');
		#endif
	}

} // CheckLISL
