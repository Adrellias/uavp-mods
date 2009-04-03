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

// to define the bank for the variables, set MATHBANK_VARS
// to shrBank, bank0, bank1, bank2, or bank3
// before including mymath16.h file!

// to define the bank for the math routine itself,
// set MATHBANK_PROG to 0, 1, 2, or 3
// before including mymath16.h file!

#include "mymath16.h"

#ifdef MATHBANK_PROG

#pragma codepage = MATHBANK_PROG

#endif

#if __CoreSet__ < 1600
 #define genAdd(r,a) W=a; btsc(Carry); W=incsz(a); r+=W
 #define genSub(r,a) W=a; btss(Carry); W=incsz(a); r-=W
 #define genAddW(r,a) W=a; btsc(Carry); W=incsz(a); W=r+W
 #define genSubW(r,a) W=a; btss(Carry); W=incsz(a); W=r-W
#else
 #define genAdd(r,a) W=a; r=addWFC(r)
 #define genSub(r,a) W=a; r=subWFB(r)
 #define genAddW(r,a) W=a; W=addWFC(r)
 #define genSubW(r,a) W=a; W=subWFB(r)
#endif

uns16 nilgarg1@nilarg1;
uns16 nilgarg2@nilarg2;
char  counter;
uns8  rm @rm16;
uns16 nilrval;
uns16 rm16;
int8  tmpArg2;
char  sign @tmpArg2;

// OK
void MathMultU8x8(void)	
{
    counter = 8;
#pragma updateBank 0
    nilrval = 0;
    W = nilgarg1.low8;
    do  {
        nilgarg2.low8 = rr( nilgarg2.low8);
        if (Carry)
            nilrval.high8 += W;
        nilrval = rr( nilrval);
        counter = decsz(counter);
    } while (1);
	return;
}
#pragma updateBank 1


void MathMultS8x8(void)
{
    counter = 8;
#pragma updateBank 0
    tmpArg2 = nilgarg2.low8;
    nilrval = 0;
    W = nilgarg1.low8;
    do  {
        tmpArg2 = rr( tmpArg2);
        if (Carry)
            nilrval.high8 += W;
        nilrval = rr( nilrval);
        counter = decsz(counter);
    } while (1);
    W = nilgarg2.low8;
    if ((int8)nilgarg1.low8 < 0)
        nilrval.high8 -= W;
    W = nilgarg1.low8;
    if ((int8)nilgarg2.low8 < 0)
        nilrval.high8 -= W;
	return;
}
#pragma updateBank 1


void MathMultU16x8(void)
{
    counter = 16;
#pragma updateBank 0
    do  {
        Carry = 0;
        nilrval = rl( nilrval);
        nilgarg1 = rl( nilgarg1);
        if (Carry)
            nilrval += nilgarg2.low8;
        counter = decsz(counter);
    } while (1);
	return;
}
#pragma updateBank 1


void MathMultU16x16(void)
{
    counter = 16;
#pragma updateBank 0
    do  {
        Carry = 0;
        nilrval = rl( nilrval);
        nilgarg1 = rl( nilgarg1);
        if (Carry)
            nilrval += nilgarg2;
        counter = decsz(counter);
    } while (1);
	return;
}
#pragma updateBank 1


void MathDivU16_8(void)
{
    rm = 0;
#pragma updateBank 0
    counter = 16+1;
    goto ENTRY;
    do  {
        rm = rl( rm);
        tmpArg2 = rl( tmpArg2);	// shift in carry from previous operation
        W = rm - nilgarg2.low8;
        if (tmpArg2&1)
            Carry = 1;
        if (Carry)
            rm = W;
       ENTRY:
        nilgarg1 = rl( nilgarg1);
        counter = decsz(counter);
    } while (1);
//	nilrval = nilgarg1;	
	return;
}
#pragma updateBank 1


void MathDivU16_16(void)
{
    rm16 = 0;
#pragma updateBank 0
    counter = 16+1;
    goto ENTRY;
    do  {
        rm16 = rl( rm16);
        W = rm16.low8 - nilgarg2.low8;
        genSubW( rm16.high8, nilgarg2.high8);
        if (!Carry)
            goto ENTRY;
        rm16.high8 = W;
        rm16.low8 -= nilgarg2.low8;
        Carry = 1;
       ENTRY:
        nilgarg1 = rl( nilgarg1);
        counter = decsz(counter);
    } while (1);
    return;
}
#pragma updateBank 1


void MathDivS16_8(void)
{
    rm = 0;
#pragma updateBank 0
    counter = 16+1;
    sign = nilgarg1.high8 ^ nilgarg2.low8;
    if ((int16)nilgarg1 < 0)  {
       INVERT:
        nilgarg1 = -nilgarg1;
        if (!counter)
		{
			nilrval = nilgarg1;
            return;
		}
    }
    if ((int8)nilgarg2.low8 < 0)
        (int8)nilgarg2.low8 = -(int8)nilgarg2.low8;
    goto ENTRY;
    do  {
        rm = rl( rm);
        W = rm - (int8)nilgarg2.low8;
        if (Carry)
            rm = W;
       ENTRY:
        nilgarg1 = rl( nilgarg1);
        counter = decsz(counter);
    } while (1);
    if (sign & 0x80)
        goto INVERT;
	return;
}
#pragma updateBank 1


void MathDivS16_16(void)
{
    rm16 = 0;
#pragma updateBank 0
    counter = 16+1;
    sign = nilgarg1.high8 ^ nilgarg2.high8;
    if ((int16)nilgarg1 < 0)  {
       INVERT:
        nilgarg1 = -nilgarg1;
        if (!counter)
		{
			nilrval = nilgarg1;
			return;
		}
    }
    if ((int16)nilgarg2 < 0)
        nilgarg2 = -nilgarg2;
    goto ENTRY;
    do  {
        rm16 = rl( rm16);
        W = rm16.low8 - nilgarg2.low8;
        genSubW( rm16.high8, nilgarg2.high8);
        if (!Carry)
            goto ENTRY;
        rm16.high8 = W;
        rm16.low8 -= nilgarg2.low8;
        Carry = 1;
       ENTRY:
        nilgarg1 = rl( nilgarg1);
        counter = decsz(counter);
    } while (1);
    if (sign & 0x80)
        goto INVERT;
	nilrval = nilgarg1;
	return;
}
#pragma updateBank 1

void MathModU8_8(void)
{
    uns8 rm = 0;
#pragma updateBank 0
    counter = 8;
    do  {
        nilgarg1.low8 = rl( nilgarg1.low8);
        rm = rl( rm);
        tmpArg2 = rl( tmpArg2);	// shift in carry from previous operation
        W = rm - nilgarg2.low8;
        if (tmpArg2&1)
            Carry = 1;
        if (Carry)
            rm = W;
        counter = decsz(counter);
    } while (1);
    nilgarg1.low8 = rm;
	return;
}	
#pragma updateBank 1
