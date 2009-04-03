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
// and compute correction adders (Rp, Np, Vud)
void CheckLISL(void)
{
	long nila1@nilarg1;

	ReadLISL(LISL_STATUS + LISL_READ);
	// the LISL registers are in order here!!
	Rp.low8  = (int)ReadLISL(LISL_OUTX_L + LISL_INCR_ADDR + LISL_READ);
	Rp.high8 = (int)ReadLISLNext();
	Tp.low8  = (int)ReadLISLNext();
	Tp.high8 = (int)ReadLISLNext();
	Np.low8  = (int)ReadLISLNext();
	Np.high8 = (int)ReadLISLNext();
	LISL_CS = 1;	// end transmission
	
#ifdef DEBUG_SENSORS
	if( IntegralCount == 0 )
	{
		SendComValH(Rp.high8);
		SendComValH(Rp.low8);
		SendComChar(';');
		SendComValH(Np.high8);
		SendComValH(Np.low8);
		SendComChar(';');
		SendComValH(Tp.high8);
		SendComValH(Tp.low8);
		SendComChar(';');
	}
#endif
// 1 unit is 1/4096 of 2g = 1/2048g
	Rp -= MiddleLR;
	Np -= MiddleFB;
	Tp -= MiddleUD;

#if 0
// calc the angles for roll matrices
// rw = arctan( x*16/z/4 )
	niltemp = Tp * 16;
	niltemp1 = niltemp / Rp;
	niltemp1 >>= 2;
	nila1 = niltemp1;
	Rw = Arctan();
#ifdef BOARD_3_1
SendComValS(Rw);
SendComChar(';');
#endif
	
	niltemp1 = niltemp / Np;
	niltemp1 >>= 2;
	nila1 = niltemp1;
	Nw = Arctan();
#ifdef BOARD_3_1
SendComValS(Nw);
SendComChar(0x13);
SendComChar(0x10);
#endif
#endif
	
	Tp -= 1024;	// subtract 1g

#ifdef NADA
// UDSum rises if ufo climbs
	UDSum += Tp;

	Tp = UDSum;
	Tp += 8;
	Tp >>= 4;
	Tp *= LinUDIntFactor;
	Tp += 128;

	if( (BlinkCount & 0x03) == 0 )	
	{
		if( (int)Tp.high8 > Vud )
			Vud++;
		if( (int)Tp.high8 < Vud )
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
	Tl = RollSum * 11;	// Rp um RollSum*11/32 korrigieren
#endif

#ifdef OPT_IDG
	Tl = RollSum * -15; // Rp um RollSum* -15/32 korrigieren
#endif
	Tl += 16;
	Tl >>= 5;
	Rp -= Tl;

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
	Tl = LRSum >> 4;
	Tl >>= 1;
	LRSum -= Tl;	// LRSum * 0.96875
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
// Nick-Achse
// =====================================
// die statische Korrektur der Erdanziehung

#ifdef OPT_ADXRS
	Tl = NickSum * 11;	// Np um RollSum* 11/32 korrigieren
#endif

#ifdef OPT_IDG
	Tl = NickSum * -15;	// Np um RollSum* -14/32 korrigieren
#endif
	Tl += 16;
	Tl >>= 5;

	Np -= Tl;
// no dynamic correction of moved mass necessary

// correct DC level of the integral
	FBIntKorr = 0;
#ifdef OPT_ADXRS
	if( Np > 10 ) FBIntKorr =  1;
	if( Np < 10 ) FBIntKorr = -1;
#endif
#ifdef OPT_IDG
	if( Np > 10 ) FBIntKorr = -1;
	if( Np < 10 ) FBIntKorr =  1;
#endif

#ifdef NADA
// Integral addieren
// Integral addieren, Abkling-Funktion
	Tl = FBSum >> 4;
	Tl >>= 1;
	FBSum -= Tl;	// LRSum * 0.96875
	FBSum += Np;
	Np = FBSum + 128;
	FBSumPosi += (int)Np.high8;
	if( FBSumPosi >  2 ) FBSumPosi -= 2;
	if( FBSumPosi < -2 ) FBSumPosi += 2;

// Korrekturanteil fuer den PID Regler
	Np = FBSumPosi * LinFBIntFactor;
	Np += 128;
	Np = (int)Np.high8;
// limit output
	if( Np >  2 ) Np = 2;
	if( Np < -2 ) Np = -2;
#endif
	Np = 0;
}
