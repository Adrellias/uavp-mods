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

// The PID controller algorithm

#pragma codepage=1
#include "c-ufo.h"
#include "bits.h"

// Math Library
#include "mymath16.h"


// compute the correction adders for the motors
// using the gyro values (PID controller)
// for the axes Roll and Nick
void PID(void)
{

	if( IntegralTest
#ifdef BOARD_3_1
        || CompassTest
#endif
                       )
		ALL_LEDS_OFF;

// Roll/Nick Linearsensoren
	Rp = 0;
	Np = 0;
//	Vud = 0;

	if( _UseLISL )
	{
		CheckLISL();	// get the linear sensors data, if available
	}
#ifdef DEBUG_SENSORS
	else
	{
		SendComChar(';');
		SendComChar(';');
		SendComChar(';');
	}
#endif

// PID controller
// E0 = current gyro error
// E1 = previous gyro error
// Sum(Ex) = integrated gyro error, sinst start of ufo!
// A0 = current correction value
// fx = programmable controller factors
//
// for Roll and Nick:
//       E0*fP + E1*fD     Sum(Ex)*fI
// A0 = --------------- + ------------
//            16               256


// ####################################
// Roll

// Differential and Proportional for Roll axis

	Rl  = -RE;
	Rl += REp;
	Rl *= (long)RollDiffFactor;
	Rl += (long)RE   * (long)RollPropFactor;

	Rl += 8;
	Rl >>= 4;

	Rl += Rp;	// add proportional part

#ifdef BOARD_3_1
	if( CompassTest )
	{
		if( CurDeviation > 0 )
			LedGreen_ON;
		if( CurDeviation < 0 )
			LedRed_ON;
		if( AbsDirection > COMPASS_MAX )
			LedYellow_ON;
	}
#endif

	if( IntegralTest )
	{
		if( (int)RollSum.high8 > 0 )
		{
			LedRed_ON;
		}
		if( (int)RollSum.high8 < -1 )
		{
			LedGreen_ON;
		}
	}

// Integral part for Roll

	if( IntegralCount == 0 )
	{
		Rp = RollSum * (long)RollIntFactor;
		Rp += 128;
		Rl += (int)Rp.high8;
	}

// muss so gemacht werden, weil CC5X kein if(Nl < -RollNickLimit) kann!
//	NegFact = -RollLimit;
//	if( Rl < NegFact ) Rl = NegFact;
//	if( Rl > RollLimit ) Rl = RollLimit;

// subtract stick signal
	Rl -= IRoll;


// ####################################
// Nick

// Differential and Proportional for Nick

	Nl  = -NE;
	Nl += NEp;
	Nl *= (long)NickDiffFactor;
	Nl += (long)NE   * (long)NickPropFactor;
	
	Nl += 8;
	Nl >>= 4;	// divide rounded by 16

	Nl += Np;	// add proportional part

	if( IntegralTest )
	{
		if( (int)NickSum.high8 >  0 )
		{
			LedYellow_ON;
		}
		if( (int)NickSum.high8 < -1 )
		{
			LedBlue_ON;
		}
	}

// Integral part for Nick
	if( IntegralCount == 0 )
	{
		Np = NickSum * (long)NickIntFactor;
		Np += 128;
		Nl += (int)Np.high8;
	}

// muss so gemacht werden, weil CC5X kein if(Nl < -RollNickLimit) kann!
//	NegFact = -NickLimit;
//	if( Nl < NegFact ) Nl = NegFact;
//	if( Nl > NickLimit ) Nl = NickLimit;

// subtract stick signal
	Nl -= INick;

// PID controller for Yaw (Heading Lock)
//       E0*fp + E1*fD     Sum(Ex)*fI
// A0 = --------------- + ------------
//             16              256

// ####################################
// Yaw

// the yaw stick signal is already added in LimitYawSum() !
//	TE += ITurn;

	Tl = YawSum * (long)TurnIntFactor;
	Tl += 128;		// divide rounded by 256
	Tl = (int)Tl.high8;

// Differential for Yaw (for quick and stable reaction)

	Tp  = -TE;
	Tp += TEp;
	Tp *= (long)TurnDiffFactor;
	Tp += (long)TE  * (long)TurnPropFactor;
	Tp += 8;
	Tp >>= 4;	// divide rounded by 16
	Tl += Tp;

	NegFact = -YawLimit;
	if( Tl < NegFact ) Tl = NegFact;
	if( Tl > YawLimit ) Tl = YawLimit;

//
// calculate camera servos
//
// use only integral part (direct angle)
//
	if( (IntegralCount == 0) && 
		(CamRollFactor != 0) && (CamNickFactor != 0) )
	{
		Rp = RollSum / (long)CamRollFactor;
		Np = NickSum / (long)CamNickFactor;
	}
	else
	{
		Rp = 0;
		Np = 0;
	}
}
