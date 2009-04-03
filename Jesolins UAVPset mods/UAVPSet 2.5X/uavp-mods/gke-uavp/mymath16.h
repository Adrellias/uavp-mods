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
// before including this .h file!

// to define the bank for the math routine itself,
// set MATHBANK_PROG to 0, 1, 2, or 3
// before including this .h file!


// Sanity checks
#ifndef MATHBANK_VARS
#define MATHBANK_VARS 0
#endif
#ifndef MATHBANK_PROG
#define MATHBANK_PROG 0
#endif

// don't touch these defines!
#if MATHBANK_VARS == 0
#define MATHBANK_RAM bank0
#endif
#if MATHBANK_VARS == 1
#define MATHBANK_RAM bank1
#endif
#if MATHBANK_VARS == 2
#define MATHBANK_RAM bank2
#endif
#if MATHBANK_VARS == 3
#define MATHBANK_RAM bank3
#endif

#if MATHBANK_PROG == 0
#define MATHBANK_PAGE page0
#endif
#if MATHBANK_PROG == 1
#define MATHBANK_PAGE page1
#endif
#if MATHBANK_PROG == 2
#define MATHBANK_PAGE page2
#endif
#if MATHBANK_PROG == 3
#define MATHBANK_PAGE page3
#endif

//extern MATHBANK_VARS uns16 nilarg1;
//extern MATHBANK_VARS uns16 nilarg2;
extern MATHBANK_RAM uns16 nilrval;
extern MATHBANK_RAM uns8  rm;
extern MATHBANK_RAM uns16 rm16;
extern MATHBANK_RAM int8  tmpArg2;
extern MATHBANK_RAM char  sign;

// outside of math calls, these 2 longs can be used as temp stores
#if MATHBANK_VARS == 0
#define nilarg1	0x6C
#define nilarg2 0x6E
#endif
#if MATHBANK_VARS == 1
#define nilarg1	0xDC
#define nilarg2 0xDE
#endif
#if MATHBANK_VARS == 2
#define nilarg1	0x16C
#define nilarg2 0x16E
#endif
#if MATHBANK_VARS == 3
#define nilarg1	0x1DC
#define nilarg2 0x1DE
#endif

extern MATHBANK_PAGE void MathMultU8x8(void);
extern MATHBANK_PAGE void MathMultS8x8(void);
extern MATHBANK_PAGE void MathMultU16x8(void);
extern MATHBANK_PAGE void MathMultU16x16(void);
extern MATHBANK_PAGE void MathDivU16_8(void);
extern MATHBANK_PAGE void MathDivU16_16(void);
extern MATHBANK_PAGE void MathDivS16_8(void);
extern MATHBANK_PAGE void MathDivS16_16(void);
extern MATHBANK_PAGE void MathModU8_8(void);

// All the math routines ensure that the bankbits
// are set to access the math vars!
#pragma updateBank 0	// saves a lot of prog memory!


uns16 operator* (uns8 arg1@nilarg1, uns8 arg2@nilarg2) @
int8 operator* (int8 arg1@nilarg1, int8 arg2@nilarg2)
{
	MathMultU8x8();	// passt scho so!
	return nilrval;
}

int16 operator* (int8 arg1@nilarg1, int8 arg2@nilarg2)
{
	MathMultS8x8();
	return nilrval;
}

uns16 operator* (uns8 arg1@nilarg2, uns16 arg2@nilarg1) @
uns16 operator* (uns16 arg1@nilarg1, uns8 arg2@nilarg2)
{
	MathMultU16x8();
	return nilrval;
}

uns16 operator* (uns16 arg1@nilarg1, uns16 arg2@nilarg2) @
int16 operator* (int16 arg1@nilarg1, int16 arg2@nilarg2)
{
	MathMultU16x16();
	return nilrval;
}

uns16 operator/ (uns16 arg1@nilarg1, uns8 arg2@nilarg2)
{
	MathDivU16_8();
	return arg1;
} 

uns16 operator/ (uns16 arg1@nilarg1, uns16 arg2@nilarg2)
{
	MathDivU16_16();
	return arg1;
}

int8  operator/ (int8 arg1@nilarg1, int8 arg2@nilarg2) @
int16 operator/ (int16 arg1@nilarg1, int8 arg2@nilarg2)
{
	MathDivS16_8();
	return arg1.low8;
}

int16 operator/ (int16 arg1@nilarg1, int16 arg2@nilarg2)
{
	MathDivS16_16();
	return arg1;
}

uns8 operator% (uns8 arg1@nilarg1, uns8 arg2@nilarg2)
{
	MathModU8_8();
	return arg1;
}

#pragma updateBank 1
