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

// The PID controller algorithm

#pragma codepage=1
#include "c-ufo.h"
#include "bits.h"

// Math Library
#include "mymath16.h"

// compute the correction adders for the motors
// using the gyro values (PID controller)
// for the axes Roll and Pitch
void PID(void)
{

	if( IntegralTest || CompassTest )
		ALL_LEDS_OFF;

	CheckLISL();	// get the linear sensors data, if available

	// PID controller
	// E0 = current gyro error
	// E1 = previous gyro error
	// Sum(Ex) = integrated gyro error, sinst start of ufo!
	// A0 = current correction value
	// fx = programmable controller factors
	//
	// for Roll and Pitch:
	//       E0*fP + E1*fD     Sum(Ex)*fI
	// A0 = --------------- + ------------
	//            16               256


	// ####################################
	// Roll

	// Differential and Proportional for Roll axis
	Rl  = -RE;
	Rl += REp;
	Rl *= (int16)RollDiffFactor;
	Rl += (int16)RE   * (int16)RollPropFactor;

	Rl += 8;
	Rl >>= 4;

	if( CompassTest )
	{
		if( CurDeviation > 0 )
			LedGreen_ON;
		else
			if( CurDeviation < 0 )
				LedRed_ON;
		if( AbsDirection > COMPASS_MAX )
			LedYellow_ON;
	}

	if( IntegralTest )
		if( (int8)RollSum.high8 > 0 )
			LedRed_ON;
		else
			if( (int8)RollSum.high8 < -1 )
				LedGreen_ON;

	// Integral part for Roll
	if( IntegralCount == 0 )
	{
		Rp = RollSum * (int16)RollIntFactor;
		Rp += 128;
		Rl += (int8)Rp.high8;
	}

	// Old test for roll limits
	// muss so gemacht werden, weil CC5X kein if(Pl < -RollPitchLimit) kann!
	//	NegFact = -RollLimit;
	//	if( Rl < NegFact ) Rl = NegFact;
	//	if( Rl > RollLimit ) Rl = RollLimit;

	// subtract stick signal
	Rl -= IRoll;


	// ####################################
	// Pitch

	// Differential and Proportional for Pitch
	Pl  = -PE;
	Pl += PEp;
	Pl *= (int16)PitchDiffFactor;
	Pl += (int16)PE   * (int16)PitchPropFactor;
	
	Pl += 8;
	Pl >>= 4;	// divide rounded by 16

	if( IntegralTest )
		if( (int8)PitchSum.high8 >  0 )
			LedYellow_ON;
		else
			if( (int8)PitchSum.high8 < -1 )
				LedBlue_ON;

	// Integral part for Pitch
	if( IntegralCount == 0 )
	{
		Pp = PitchSum * (int16)PitchIntFactor;
		Pp += 128;
		Pl += (int8)Pp.high8;
	}

	// Old test for pitch limits
	// muss so gemacht werden, weil CC5X kein if(Pl < -RollPitchLimit) kann!
	//	NegFact = -PitchLimit;
	//	if( Pl < NegFact ) Pl = NegFact;
	//	if( Pl > PitchLimit ) Pl = PitchLimit;

	// subtract stick signal
	Pl -= IPitch;

	// PID controller for Yaw (Heading Lock)
	//       E0*fp + E1*fD     Sum(Ex)*fI
	// A0 = --------------- + ------------
	//             16              256

	// ####################################
	// Yaw

	// the yaw stick signal is already added in LimitYawSum() !
	//	YE += IYaw;

	Yl = YawSum * (int16)YawIntFactor;
	Yl += 128;		// divide rounded by 256
	Yl = (int8)Yl.high8;

	// Differential for Yaw (for quick and stable reaction)
	Yp  = -YE;
	Yp += YEp;
	Yp *= (int16)YawDiffFactor;
	Yp += (int16)YE  * (int16)YawPropFactor;
	Yp += 8;
	Yp >>= 4;	// divide rounded by 16
	Yl += Yp;

	NegFact = -YawLimit;
	if( Yl < NegFact ) Yl = NegFact;
	if( Yl > YawLimit ) Yl = YawLimit;

	// ####################################
	// Camera

	// use only integral part (direct angle)
	if( (IntegralCount == 0) && (CamRollFactor != 0) && (CamPitchFactor != 0) )
	{
		Rp = RollSum / (int16)CamRollFactor;
		Pp = PitchSum / (int16)CamPitchFactor;
	}
	else
	{
		Rp = 0;
		Pp = 0;
	}
} // PID
