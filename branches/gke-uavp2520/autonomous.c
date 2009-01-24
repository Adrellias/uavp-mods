// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
// =             Copyright (c) 2007 Ing. Wolfgang Mahringer              =
// =      Rewritten and ported to 18F2520 2008 by Prof. Greg Egan        =
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

// Autonomous flight routines

#include "c-ufo.h"
#include "bits.h"

uint8 RCLinkRestored(int32 d)
{
	// checks for good RC frames for a period d mS
	if ( !_Signal )
		TimerMilliSec = ClockMilliSec + d;
	return(ClockMilliSec > TimerMilliSec );
} // RCLinkRestored

void CheckThrottleMoved(void)
{
    _ThrChanging = (IThrottle > (PrevIThrottle - 5)) && (IThrottle < (PrevIThrottle + 5) );
	PrevIThrottle = IThrottle;
} // CheckThrottleMoved

uint8 Descend(uint8 T)
{
	// need to use accelerometer or baro based descent control
//	if (((ClockMilliSec & 0x000000ff) == 0 ) && ( T > 0 ))
	if ( T > 0 )
		T -= 1;
	return(T);
} // Descend

void Navigation(void)
{




} // Navigation

void DoAutonomous(void)
{

	DesiredThrottle = Descend(DesiredThrottle);
	IRoll = IPitch = IYaw = 0;

} // DoAutonomous
