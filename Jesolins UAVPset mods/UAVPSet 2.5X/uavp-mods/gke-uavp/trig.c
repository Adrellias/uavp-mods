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

// coarse sine functions with 0.01 resultion
// arctan function

#include "c-ufo.h"
#include "bits.h"

// Math Library
// only needed for nilarg1 and nilarg2 defines
#include "mymath16.h"

// sine table 0-90 deg, 1 deg resolution
// gives an accuracy of +/-0,01
// 127 = 1.00
//						
#define F 127/1000	
const uns8 sintab[] = {
// 0      1      2      3      4      5      6      7      8      9
  0*F,  18*F,  39*F,  52*F,  70*F,  87*F, 105*F, 122*F, 139*F, 156*F,
174*F, 191*F, 208*F, 225*F, 242*F, 259*F, 276*F, 292*F, 309*F, 326*F,
342*F, 358*F, 375*F, 391*F, 407*F, 423*F, 438*F, 454*F, 469*F, 485*F,
500*F, 515*F, 530*F, 545*F, 559*F, 574*F, 588*F, 602*F, 616*F, 629*F,
643*F, 656*F, 669*F, 682*F, 695*F, 707*F, 719*F, 731*F, 743*F, 755*F,
766*F, 777*F, 788*F, 799*F, 809*F, 819*F, 829*F, 839*F, 848*F, 857*F,
866*F, 875*F, 883*F, 891*F, 899*F, 906*F, 914*F, 921*F, 927*F, 934*F,
940*F, 946*F, 951*F, 956*F, 961*F, 966*F, 970*F, 974*F, 978*F, 981*F,
985*F, 988*F, 990*F, 993*F, 995*F, 996*F, 998*F, 998*F, 999*F, 1000*F,
1000*F };
#undef F


int16 nilparam@nilarg1;
int16 niltempt@nilarg2;


// modifiy an angle until it is in range 0..360
// then cuts the angle down to 0..90 range and sets
// the quadrant bit
void Align_0_360(void)
{
	while( nilparam < 0 )
		nilparam += 360;
	while( (uns16)nilparam > 360 )
		nilparam -= 360;
	_TrigSign = 0;
	if( nilparam < 90 )
		;
	else
	if( nilparam <= 180 )
		nilparam = 180 - nilparam;
	else
	{
		_TrigSign = 1;	// result is negative
		if( nilparam <= 270 )
			nilparam -= 180;
		else
			nilparam = 270 - nilparam;
	}
}

// ultra-fast (26us exec time) sine function
// input param is nilparam, in degrees
// output param sin(90)=127
uns8 Sin(void)
{
	Align_0_360();

	W = sintab[nilparam.low8];
	
	if( _TrigSign )
		W = 0 - W;
	return(W);
}

// cosine function
// input param is nilparam, in degrees
// output param is nilparam, cos(0)=127
uns8 Cos(void)
{
	nilparam += 90;
	return(Sin());
}

// entry W is the side property * 16
// entry must be 0..16
// exit is angle in 1° units
uns8 Arctan(uns8 niprop)
{
	
	if( niprop >= 16 )
		return((uns8)45);
	skip(niprop);     /* 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 */
	#pragma return[] =   0,  2,  6,  9, 13, 16, 19, 23, 26, 28, 31, 34, 36, 38, 41, 43
}
