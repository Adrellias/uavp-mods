// ==============================================
// =    U.A.V.P Brushless UFO Test-Software     =
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
// =  please visit http://www.uavp.de           =
// =               http://www.mahringer.co.at   =
// ==============================================

// Power output test

#pragma codepage = 1

#include "pu-test.h"
#include "bits.h"

// flash output for a second, then return to its previous state
void PowerOutput(uns8 niout)
{
	bank1 uns8 nii, nij;

	for( nij=0; nij < 10; nij++ )	// 10 flashes (count MUST be even!)
	{
	
		if (niout == 0 ) LedShadow ^= 0x01; else 	// toggle AUX
		if (niout == 1 ) LedShadow ^= 0x02; else 	// toggle BLUE
		if (niout == 2 ) LedShadow ^= 0x04; else 	// toggle RED
		if (niout == 3 ) LedShadow ^= 0x08; else 	// toggle GREEN
		if (niout == 4 ) LedShadow ^= 0x10; else 	// toggle AUX
		if (niout == 5 ) LedShadow ^= 0x20; else 	// toggle YELLOW
		if (niout == 7 ) LedShadow ^= 0x40; else 	// toggle AUX
		if (niout == 7 ) LedShadow ^= 0x80; 		// toggle BEEPER

		SendLeds();	// send LEDs to bus
		for( nii=0; nii < 200; nii++)
		{
			T0IF = 0;
			while( T0IF == 0 )	;
		}
	}		
}
