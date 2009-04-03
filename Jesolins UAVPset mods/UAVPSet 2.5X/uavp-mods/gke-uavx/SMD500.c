// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
// =                    Written 2008 by Prof. Greg Egan                  =
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

#include "c-ufo.h"
#include "bits.h"


//uint16 F1, F2, F3, F4;
//uint32 AC1, AC2, AC3, AC4, AC5, AC6;

int16 AltitudeCompensation(uint16 P, uint16 T)
{	// altitudes are relative to Origin altitude

	// SMD500 Bosch Pressure Sensor
	// too expensive computationally
	// ~0.3M/LSB 26C 300M
	// or ~1 foot per LSB 


	uint32	X1, X2, X3, X4, Y1, Y2, Y3, Y4;
	uint32 B1, B2, B3, B4, B5;
	int32 P;

	// strictly only needs to be done once
	AC1 = F1 & 0x3fff;
	AC2 = (F2>>8)|(F1>>14);
	AC3 = F2 & 0x01ff;
	AC4 = F3 & 0x1fff;
	AC5 = (F4>>12)|(F3>>13);
	AC6 = F4 & 0x0fff; 

	//AC1=8498; AC2=256;AC3=259;AC4=4461;AC5=60; AC6=2871;
	// UT = 4826;
	// UP = 38575;

	B1 = SRS32((AC3-2063)*(-28006)), 11) - 20108;
	B2 = ((AC2-475)*SRS32(B1,4))/(AC3-2063);

	X1 = (int32)T-1413-AC6;
	X2 = SRS32(X1*X1, 10);
	Y2 = SRS32(-4402*X2, 10);
	X3 = SRS32(X2*X1, 13);
	Y3 = SRS32(8638*X3, 9);
	X4 = SRS32(X2*X2, 16);
	Y4 = SRS32(-7743*X4, 8);
	B5 = SRS32((493+AC5)*(16*X1+Y2+Y3+Y4), 10);
	T = SRS32(B5+8, 4);

	X1 = SRS32(B2*SRS32(B5*B5, 12), 11);
	X2 = (AC2-475)*SRS32(B5, 9);
	X3 = SRS32(X1+X2+2, 2);
	B3 = (AC1-1998)*2+X3;

	Y1 = SRS32((AC3-2063)*B5, 10);
	Y2 = SRS32(B1*(B5*SRS32(B5, 12)), 16);
	Y3 = SRS32(Y1+Y2+2, 2);
	B4 = SRS32((AC4+9584)*(Y3+32768), 14);

	P = (((int32)P-B3)*100000)/B4;
	X1 = SRS32(P, 8) * SRS32(P, 8);
	X1 = SRS32(X1*3038, 16);
	X2 = SRS32(-7537*P, 16);
	P = P +SRS32(X1+X2+3791, 4);
	// then convert to altitude!!!! ... probably lookup table


	return (P); 
} // AltitudeCompensation



