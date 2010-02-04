// =================================================================================================
// =                                  UAVX Quadrocopter Controller                                 =
// =                             Copyright (c) 2008 by Prof. Greg Egan                             =
// =                   Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                       http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// =================================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even 
//    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//    General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

#include "uavx.h"

int8 ReadEE(uint8);
void WriteEE(uint8, int8);

int8 ReadEE(uint8 addr)
{
	static int8 b;

	EEADR = addr;
	EECON1bits.EEPGD = false;
	EECON1bits.RD = true;
	b=EEDATA;
	EECON1 = 0;
	return(b);	
} // ReadEE

void WriteEE(uint8 addr, int8 d)
{
	int8 rd;
	uint8 IntsWereEnabled;
	
	rd = ReadEE(addr);
	if ( rd != d )						// avoid redundant writes
	{
		EEDATA = d;				
		EEADR = addr;
		EECON1bits.EEPGD = false;
		EECON1bits.WREN = true;
		
		IntsWereEnabled = InterruptsEnabled;
		DisableInterrupts;
		EECON2 = 0x55;
		EECON2 = 0xaa;
		EECON1bits.WR = true;
		while(EECON1bits.WR);
		if ( IntsWereEnabled )
			EnableInterrupts;

		EECON1bits.WREN = false;
	}
} // WriteEE


