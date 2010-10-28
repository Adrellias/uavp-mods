// ==============================================
// =      U.A.V.P Brushless UFO Controller      =
// =           Professional Version             =
// = Copyright (c) 2007 Ing. Wolfgang Mahringer =
// ==============================================
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ==============================================
// =  please visit http://www.uavp.org          =
// =               http://www.mahringer.co.at   =
// ==============================================

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
	long nila1@nilarg1;

	ReadLISL(LISL_STATUS + LISL_READ);
	// the LISL registers are in order here!!
	Rp.low8  = (int)ReadLISL(LISL_OUTX_L + LISL_INCR_ADDR + LISL_READ);
	Rp.high8 = (int)ReadLISLNext();
	Yp.low8  = (int)ReadLISLNext();
	Yp.high8 = (int)ReadLISLNext();
	Pp.low8  = (int)ReadLISLNext();
	Pp.high8 = (int)ReadLISLNext();
	LISL_CS = 1;	// end transmission
	
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

#ifdef NADA
// UDSum rises if ufo climbs
	UDSum += Yp;

	Yp = UDSum;
	Yp += 8;
	Yp >>= 4;
	Yp *= LinUDIntFactor;
	Yp += 128;

	if( (BlinkCount & 0x03) == 0 )	
	{
		if( (int)Yp.high8 > Vud )
			Vud++;
		if( (int)Yp.high8 < Vud )
			Vud--;
		if( Vud >  20 ) Vud =  20;
		if( Vud < -20 ) Vud = -20;
	}
	if( UDSum >  10 ) UDSum -= 10;
	if( UDSum < -10 ) UDSum += 10;
#endif // NADA

// =====================================
// Roll-Achse
// =====================================
// die statische Korrektur der Erdanziehung

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
	Rp += (long)RollSamples;
	Rp += (long)RollSamples;
#endif

#ifdef OPT_IDG
	Rp -= (long)RollSamples;
#endif

// correct DC level of the integral
	LRIntKorr = 0;
#ifdef OPT_ADXRS
	if( Rp > 10 ) LRIntKorr =  1;
	if( Rp < 10 ) LRIntKorr = -1;
#endif

#ifdef OPT_IDG
	if( Rp > 10 ) LRIntKorr = -1;
	if( Rp < 10 ) LRIntKorr =  1;
#endif

#ifdef NADA
// Integral addieren, Abkling-Funktion
	Yl = LRSum >> 4;
	Yl >>= 1;
	LRSum -= Yl;	// LRSum * 0.96875
	LRSum += Rp;
	Rp = LRSum + 128;
	LRSumPosi += (int)Rp.high8;
	if( LRSumPosi >  2 ) LRSumPosi -= 2;
	if( LRSumPosi < -2 ) LRSumPosi += 2;


// Korrekturanteil fuer den PID Regler
	Rp = LRSumPosi * LinLRIntFactor;
	Rp += 128;
	Rp = (int)Rp.high8;
// limit output
	if( Rp >  2 ) Rp = 2;
	if( Rp < -2 ) Rp = -2;
#endif
	Rp = 0;

// =====================================
// Pitch-Achse
// =====================================
// die statische Korrektur der Erdanziehung

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
	if( Pp < 10 ) FBIntKorr = -1;
#endif
#ifdef OPT_IDG
	if( Pp > 10 ) FBIntKorr = -1;
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
	FBSumPosi += (int)Pp.high8;
	if( FBSumPosi >  2 ) FBSumPosi -= 2;
	if( FBSumPosi < -2 ) FBSumPosi += 2;

// Korrekturanteil fuer den PID Regler
	Pp = FBSumPosi * LinFBIntFactor;
	Pp += 128;
	Pp = (int)Pp.high8;
// limit output
	if( Pp >  2 ) Pp = 2;
	if( Pp < -2 ) Pp = -2;
#endif
	Pp = 0;
}