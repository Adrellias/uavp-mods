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
//  MERCHANTABILITY or FITPESS FOR A PARTICULAR PURPOSE.  See the
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

// roll/nick matrix compensation

#pragma codepage = 2

#include "c-ufo.h"
#include "bits.h"

// Math Library
#include "mymath16.h"


// compute new RE and PE
void MatrixCompensate(void)
{
// Rnew = cos(Pitch)*RE + 
//        sin(Pitch)*sin(Roll)*PE
//
// Nnew = cos(Roll)*PE
//        
#if 0
	int16 nila1@nilarg1;
	int8 ni1,ni2;

	nila1=Pw;
	niltemp1 = (int8)Cos();
	niltemp1 *= (int16)RE;

	nila1=Pw;
	ni1 = Sin();
	nila1=Rw;
	ni2 = Sin();
	niltemp = (int16)ni1 * (int16)ni2;
	niltemp += 64;
	niltemp /= 128;
	niltemp *= (int16)PE;

	niltemp += niltemp1;
	niltemp += 64;
	niltemp /= 128;
	RE = niltemp.low8;


	nila1=Rw;
	niltemp = Cos();
	niltemp *= PE;
	niltemp += 64;
	niltemp /= 128;
	PE = niltemp.low8;
SendComValS(RE);
SendComValS(PE);
#endif
}
