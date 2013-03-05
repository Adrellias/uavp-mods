// ===============================================================================================
// =                                UAVX Quadrocopter Controller                                 =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                     http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// ===============================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

#include "uavx.h"

void DoAltitudeHold(void);
void AltitudeHold(void);
void DoOrientationTransform(void);
void GainSchedule(void);
void DoControl(void);

void CheckThrottleMoved(void);
void InitControl(void);

#pragma udata axisvars			
AxisStruct A[3];
#pragma udata

#pragma udata hist
uint32 CycleHist[16];
#pragma udata

uint8 PIDCyclemS, PIDCycleShift;
		
int16 CameraAngle[3];				

int16 HeadingE;

int16 CurrMaxRollPitch;

int16 AttitudeHoldResetCount;
int24 DesiredAltitude, Altitude, Altitudep; 
int16 AltFiltComp, AltComp, BaroROC, BaroROCp, RangefinderROC, ROC, MinROCCmpS;
int24 RTHAltitude;

int8 BeepTick = 0;

void DoAltitudeHold(void) { 	// Syncronised to baro intervals independant of active altitude source	
	static int24 AltE;
	static int16 ROCE, DesiredROC;
	static int16 NewComp;

	#ifdef ALT_SCRATCHY_BEEPER
	if ( (--BeepTick <= 0) && !F.BeeperInUse ) 
		Beeper_TOG;
	#endif

	AltE = DesiredAltitude - Altitude;
	AltE = Threshold(AltE, 10);
	DesiredROC = Limit(AltE, MinROCCmpS, ALT_MAX_ROC_CMPS);

	ROCE = DesiredROC - ROC;

	NewComp = SRS16(ROCE * P[AltKp], 6);

	#ifdef NAV_WING

		// Elevator/Airspeed control here later?

	#endif // NAV_WING

	AltComp = Limit1(NewComp, ALT_MAX_THR_COMP);
				
	#ifdef ALT_SCRATCHY_BEEPER
	if ( (BeepTick <= 0) && !F.BeeperInUse) {
		Beeper_TOG;
		BeepTick = 5;
	}
	#endif

} // DoAltitudeHold	

void AltitudeHold() {  // relies upon good cross calibration of baro and rangefinder!!!!!!
	static int16 ActualThrottle;

	if (F.NewBaroValue) {
		F.NewBaroValue = false;
		GetRangefinderAltitude();
		CheckThrottleMoved();
		if (F.AltHoldEnabled) {
			if (F.UsingRangefinderAlt) {
				Altitude = RangefinderAltitude;
				ROC = RangefinderROC;
			} else {
				Altitude = BaroAltitude;
				ROC = BaroROC;
			}
			if (F.ForceFailsafe || ((NavState != HoldingStation)
					&& F.AllowNavAltitudeHold)) { // Navigating - using CruiseThrottle
				F.HoldingAlt = true;
				DoAltitudeHold();
			} else if (F.ThrottleMoving) {
				F.HoldingAlt = false;
				SetDesiredAltitude(Altitude);
				AltComp = Decay1(AltComp);
			} else {
				F.HoldingAlt = true;
				DoAltitudeHold(); // not using cruise throttle
				#ifndef SIMULATE
					ActualThrottle = DesiredThrottle + AltComp;
					if ((Abs(ROC) < ALT_HOLD_MAX_ROC_CMPS) && (ActualThrottle
							> THROTTLE_MIN_CRUISE)) {
						NewCruiseThrottle
								= HardFilter(NewCruiseThrottle, ActualThrottle);
						NewCruiseThrottle
								= Limit(NewCruiseThrottle, THROTTLE_MIN_CRUISE, THROTTLE_MAX_CRUISE );
						CruiseThrottle = NewCruiseThrottle;
					}
				#endif // SIMULATE
			}
		} else {
			Altitude = BaroAltitude;
			ROC = 0;
			AltComp = Decay1(AltComp);
			F.HoldingAlt = false;
		}
	}
} // AltitudeHold


void DoInertialDamping(void) {
	#ifdef INC_DAMPING
	static int16 Temp;
	static uint8 a;

	if (F.NearLevel && !F.NavigationActive)
		for (a = LR; a<=FB; a++) {
			Temp = -SRS32(Threshold(A[a].Acc, (GRAVITY/10)) * P[HorizDampKp], 3);
			A[a].Damping = Limit1(Temp, FromPercent(10,RC_MAXIMUM));
		} 
	else
		A[Roll].Damping = A[Pitch].Damping = 0;
	#endif // INC_DAMPING
} // DoInertialDamping


void DoOrientationTransform(void) {

	#ifdef INC_POLAR
	static int16 Polar;

	if (F.PolarCoords && F.NavigationActive && (Nav.Distance > 150)) {
		Polar = Make2Pi(Nav.Bearing - Heading);
		Rotate(&A[Pitch].Control, &A[Roll].Control, A[Pitch].Desired,
				A[Roll].Desired, Polar);
	} 
	else
	#endif // INC_POLAR
		FastRotate(&A[Pitch].Control, &A[Roll].Control, A[Pitch].Desired,
			A[Roll].Desired, Orientation);

	if ( !F.NavigationActive )
		DecayNavCorr();

	A[Pitch].Control += A[Pitch].NavCorr - A[FB].Damping;
	A[Roll].Control += A[Roll].NavCorr + A[LR].Damping;
	A[Yaw].Control = A[Yaw].Desired + A[Yaw].NavCorr;

} // DoOrientationTransform


void ComputeRateDerivative(AxisStruct *C, int16 Rate) {
	static int32 r;

	r = MediumFilter32(C->Ratep, Rate);
	C->RateD =  r - C->Ratep;
	C->Ratep = r;

	C->RateD = MediumFilter32(C->RateDp, C->RateD);
	C->RateDp = C->RateD;
} // ComputeRateDerivative

void DoWolfControl(AxisStruct *C) { // Origins Dr. Wolfgang Mahringer
	static int32 Temp, pd, i;

	ComputeRateDerivative(C, C->Rate);
	pd =  SRS32((int32)C->Rate * C->RateKp + C->RateD * C->RateKd, 5);

    Temp = SRS32((int32)C->Angle * C->RateKi , 9);
	i = Limit1(Temp, C->IntLimit);

	C->Out = (pd + i) - C->Control;
	
} // DoWolfControl


void DoAngleControl(AxisStruct *C) { // PI_PD with Dr. Ming Liu
	static int32 p, i, d, DesRate, AngleE, AngleEp, RateE;

	AngleEp = C->AngleE;
	AngleE = C->Control * RC_STICK_ANGLE_SCALE - C->Angle;

	p = AngleE * C->RateKp;

	if (Sign(AngleE) != Sign(C->AngleEInt))
		C->AngleEInt = 0;
	C->AngleEInt += AngleE;
	C->AngleEInt = Limit1(C->AngleEInt, C->IntLimit);
	i = C->AngleEInt * C->AngleKi;

	DesRate = SRS32(p + i, 10);
 	DesRate = Limit1(DesRate, MAX_BANK_ANGLE_DEG * DEG_TO_ANGLE_UNITS);

	RateE = DesRate - C->Rate; 	

	ComputeRateDerivative(C, C->Rate); // ignore set point change RateE);
	C->Out = -SRS32(RateE * C->RateKp - C->RateD * C->RateKd, 5);

	C->AngleE = AngleE;

} // DoAngleControl


void UpdateDesiredHeading(void) {

	if (F.MagnetometerValid) {
		if (A[Yaw].Hold > COMPASS_MIDDLE)
			DesiredHeading = Heading;
	} else
		DesiredHeading = Heading;
} // UpdateDesiredHeading

void YawControl(void) {
	static int24 r;

	UpdateDesiredHeading();

	HeadingE = MinimumTurn(DesiredHeading - Heading);
	HeadingE = Limit1(HeadingE, DegreesToRadians(30));
	r = SRS32((int24)HeadingE * A[Yaw].AngleKp, 7);

	r -= A[Yaw].Rate;
	r  = SRS32((int24)r * A[Yaw].RateKp, 4);
	r = Limit1(r, A[Yaw].Limiter);

	if ((Abs(A[Yaw].Control) > FromPercent(2, RC_NEUTRAL))|| (NavState
			!= HoldingStation))
		r -= A[Yaw].Control;

	A[Yaw].Out = r;

} // YawControl

void DoControl(void){

	#ifdef SIMULATE
		DoOrientationTransform();
		DoEmulation();
	#else
		static uint8 a;
	
		GetAttitude();
		DoInertialDamping();
		GetBaroAltitude();
		GetHeading();
		DoOrientationTransform();
	
		for ( a = Roll; a<=(uint8)Pitch; a++) 
			if ( F.UsingAltControl )	
				DoAngleControl(&A[a]);
			else
				DoWolfControl(&A[a]);		
				
		YawControl();
			
		Rl = A[Roll].Out; Pl = A[Pitch].Out; Yl = A[Yaw].Out;

	#endif // SIMULATE

	F.NearLevel = Max(Abs(A[Roll].Angle), Abs(A[Pitch].Angle)) < NAV_RTH_LOCKOUT;

	OutSignals(); 

} // DoControl

void InitControl(void) {
	static uint8 a;
	static AxisStruct *C;

	AltComp = A[FB].Damping = A[LR].Damping = 0;

	for ( a = Roll; a<=(uint8)Yaw; a++) {
		C = &A[a];
		C->Outp = C->Ratep = 0;
		C->RateDp = C->RateD = C->Angle = 0;
	}

} // InitControl



