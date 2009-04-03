// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
// =     Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer       =
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

#include "c-ufo.h"
#include "bits.h"

void LimitRollSum(void)
{
	RollSum += RollRate;

	if( IntegralCount == 0 )
	{
		RollSum = Limit(RollSum, -RollIntLimit*256, RollIntLimit*256);
		RollSum += LRIntKorr;
		RollSum = Decay(RollSum);		// damps to zero even if still rolled
	}

} // LimitRollSum

void LimitPitchSum(void)
{
	PitchSum += PitchRate;

	if( IntegralCount == 0 )
	{
		PitchSum = Limit(PitchSum, -PitchIntLimit*256, PitchIntLimit*256);
		PitchSum += FBIntKorr;		
		PitchSum = Decay(PitchSum);	// damps to zero even if still pitched
	}
} // LimitPitchSum

void LimitYawSum(void)
{
	int16 Temp;

	YE += IYaw;						// add the yaw stick value

	if ( _UseCompass )
	{
		// CurDeviation is negative if quadrocopter has yawed to the right (go back left)
		if ( Abs(IYaw) > COMPASS_MIDDLE )
			AbsDirection = COMPASS_INVAL; // acquire new heading
		else		
			if( CurDeviation > COMPASS_MAXDEV )
				YE -= COMPASS_MAXDEV;
			else
				if( CurDeviation < -COMPASS_MAXDEV )
					YE += COMPASS_MAXDEV;
				else
					YE -= CurDeviation;
	}

	YawSum += (int16)YE;
	YawSum = Limit(YawSum, -YawIntLimit*256, YawIntLimit*256);

	YawSum = Decay(YawSum); // GKE added to kill gyro drift
	YawSum = Decay(YawSum); 

} // LimitYawSum

void GetGyroValues(void)
{
	#ifdef OPT_IDG
	RollRate += ADC(ADCRollChan, ADCVREF);
	PitchRate += ADC(ADCPitchChan, ADCVREF);
	#else
	RollRate += ADC(ADCRollChan, ADCVREF5V);
	PitchRate += ADC(ADCPitchChan, ADCVREF5V);
	#endif // OPT_IDG
} // GetGyroValues

// Calc the gyro values from added RollRate and PitchRate
void CalcGyroValues(void)
{
	int16 Temp;

	// RollRate & Pitchsamples hold the sum of 2 consecutive conversions
	// Approximately 4 bits of precision are discarded in this and related 
	// calculations presumably because of the range of the 16 bit arithmetic.

	#ifdef OPT_ADXRS150
	RollRate = (RollRate + 2)>>2; // recreate the 10 bit resolution
	PitchRate = (PitchRate + 2)>>2;
	#else // IDG300 and ADXRS300
	RollRate = (RollRate + 1)>>1;	
	PitchRate = (PitchRate + 1)>>1;
	#endif
	
	if( IntegralCount > 0 )
	{
		// pre-flight auto-zero mode
		RollSum += RollRate;
		PitchSum += PitchRate;

		if( IntegralCount == 1 )
		{
			RollSum += 8;
			PitchSum += 8;
			if( !_UseLISL )
			{
				RollSum = RollSum + MiddleLR;
				PitchSum = PitchSum + MiddleFB;
			}
			GyroMidRoll = RollSum >> 4;	
			GyroMidPitch = PitchSum >> 4;
			GyroMidYaw = 0;
			RollSum = PitchSum = LRIntKorr = FBIntKorr = 0;
		}
	}
	else
	{
		// standard flight mode
		RollRate -= GyroMidRoll;
		PitchRate -= GyroMidPitch;

		// calc Cross flying mode
		if( FlyCrossMode )
		{
			// Real Roll = 0.707 * (P + R)
			//      Pitch = 0.707 * (P - R)
			// the constant factor 0.667 is used instead
			Temp = RollRate + PitchRate;	
			PitchRate -= RollRate;	
			RollRate = (Temp * 2)/3;
			PitchRate = (PitchRate * 2)/3; // 7/10 with int24
		}

		#ifdef DEBUG_SENSORS
		Trace[TRollRate] = RollRate;
		Trace[TPitchRate] = PitchRate;
		#endif
	
		// Roll
		#ifdef OPT_ADXRS
		RE = SRS16(RollRate + 2, 2);
		#else // OPT_IDG
		RE = SRS16(RollRate + 1, 1); // use 8 bit res. for PD controller
		#endif	

		#ifdef OPT_ADXRS
		RollRate = SRS16(RollRate + 1, 1); // use 9 bit res. for I controller	
		#endif

		LimitRollSum();		// for roll integration

		// Pitch
		#ifdef OPT_ADXRS
		PE = SRS16(PitchRate + 2, 2);
		#else // OPT_IDG
		PE = SRS16(PitchRate + 1, 1);
		#endif

		#ifdef OPT_ADXRS
		PitchRate = SRS16(PitchRate + 1, 1); // use 9 bit res. for I controller	
		#endif

		LimitPitchSum();		// for pitch integration

		// Yaw is sampled only once every frame, 8 bit A/D resolution
		YE = ADC(ADCYawChan, ADCVREF5V) >> 2;	
		if( GyroMidYaw == 0 )
			GyroMidYaw = YE;
		YE -= GyroMidYaw;
		YawRate = YE;

		LimitYawSum();

		#ifdef DEBUG_SENSORS
		Trace[TYE] = YE;
		Trace[TRollSum] = RollSum;
		Trace[TPitchSum] = PitchSum;
		Trace[TYawSum] = YawSum;
		#endif
	}
} // CalcGyroValues

void PID(void)
{

	AccelerationCompensation();	
	
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

	// Roll

	// Differential and Proportional for Roll axis
	Rl  = SRS16(RE *(int16)RollPropFactor + (REp-RE) * RollDiffFactor + 8, 4);

	// Integral part for Roll
	if( IntegralCount == 0 )
		Rl += SRS16(RollSum * (int16)RollIntFactor + 128, 8); // thanks Jim

	Rl -= IRoll;								// subtract stick signal

	// Pitch

	// Differential and Proportional for Pitch
	Pl  = SRS16(PE *(int16)PitchPropFactor + (PEp-PE) * PitchDiffFactor + 8, 4);

	// Integral part for Pitch
	if( IntegralCount == 0 )
		Pl += SRS16(PitchSum * (int16)PitchIntFactor +128, 8);

	Pl -= IPitch;								// subtract stick signal

	// Yaw

	// the yaw stick signal is already added in LimitYawSum() !
	//	YE += IYaw;

	// Differential and Proportional for Yaw
	Yl  = SRS16(YE *(int16)YawPropFactor + (YEp-YE) * YawDiffFactor + 8, 4);
	Yl += SRS16(YawSum * (int16)YawIntFactor + 128, 8);
	Yl = Limit(Yl, -YawLimit, YawLimit);		// effective slew limit

	DoPIDDisplays();

} // PID
