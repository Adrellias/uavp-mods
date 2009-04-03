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

// roll/nick matrix compensation

#pragma codepage = 2

#include "c-ufo.h"
#include "bits.h"

// Math Library
#include "mymath16.h"


// compute new RE and NE
void MatrixCompensate(void)
{
// Rnew = cos(Nick)*RE + 
//        sin(Nick)*sin(Roll)*NE
//
// Nnew = cos(Roll)*NE
//        
#if 0
	long nila1@nilarg1;
	int ni1,ni2;

	nila1=Nw;
	niltemp1 = (int)Cos();
	niltemp1 *= (long)RE;

	nila1=Nw;
	ni1 = Sin();
	nila1=Rw;
	ni2 = Sin();
	niltemp = (long)ni1 * (long)ni2;
	niltemp += 64;
	niltemp /= 128;
	niltemp *= (long)NE;

	niltemp += niltemp1;
	niltemp += 64;
	niltemp /= 128;
	RE = niltemp.low8;


	nila1=Rw;
	niltemp = Cos();
	niltemp *= NE;
	niltemp += 64;
	niltemp /= 128;
	NE = niltemp.low8;
SendComValS(RE);
SendComValS(NE);
#endif
}
