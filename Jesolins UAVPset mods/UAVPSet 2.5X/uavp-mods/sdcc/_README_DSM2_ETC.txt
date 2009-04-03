==============================================
=   U    U      AA     V     V   PPPPP       =
=   U    U     A  A    V     V   P    P      =
=   U    U    A    A   V     V   P    P      =
=   U    U    AAAAAA    V   V    PPPPP       =
=   U    U    A    A     V V     P           =
=    UUUU  U  A    A A    V  V   P    P      =
=                                            =
=  Universal  Airial   Video     Platform    =
=                                            =
=      U.A.V.P Brushless UFO Controller      =
=           Professional Version             =
= Copyright (c) 2007 Ing. Wolfgang Mahringer =
=                                            =
=      Modified by others (see below)        =
=                                            =
==============================================

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

==============================================
=  Reference Sites                           =
==============================================

The original project may be found at:

	http://www.uavp.org 

The variants of UAVP to which this readme applies are hosted
at http:

	http://code.google.com/p/uavp-mods/ 

The related RCGroups thread is at:

	http://www.rcgroups.com/forums/showthread.php?t=811550

Disclaimer: The project at the Google Site is not endorsed by 
nor is it part of the official UAVP project.

==============================================
= The GNU GENERAL PUBLIC LICENSE             =
==============================================

This software is released under GNU GPL license.
The GPL license is contained in the files
gpl-en.txt (in English) and gpl-de.txt (in German).

==============================================
= 2.4GHz and other Diversity Systems                               =
==============================================

2.4GHz systems vary the order in which their Receivers (Rx) emit their servo control 
signals. The order does NOT correspond to the physical ordering of servo sockets on 
the Rx. You should try the default DSM2 configuration first which is known to work 
for the DX7/AR7000 combination. Use UAVPSet to select Throttle on 1 and 
Positive Impulse. In general your Transmitter (Tx) should be set to ACTRO with 
absolutely NO MIXES.

To determine the order for your Tx/Rx combination you need to use UAVPSet and the 
TestSoftware appropriate to your configuration e.g. TestSoftware-V315-ADX-PPM. 
The Test software you use at this stage is not specific to your 2.4GHz system.
This software (when selecting display Rx values), displays channels in time-order 
of arrival from the Rx. These channels from 1-7  correspond to the variables 
NewK1-NewK7. Initially, unless you are very lucky, one or more will be shown as invalid.

The first step is to determine the combination of 4 servo channels that shows all 
channels to be valid. These will correspond to the odd numbered channels as the Rx 
emits them. You need to re-display each time you make a change. This is the 
tedious part.

Next Work through the controls on your Tx re-displaying the channels
each time noting which channel has changed and what the associated control is. 
If necessary edit the section of irq.c marked as EDIT HERE -> to reflect the actual 
order for your Tx/Rx combination. Do this by changing only the NewKx names.   

Finally for your particular mix to work you must have Ch3 selected for Throttle 
under UAVPSet. Make sure you do a write to update it on the UAVP board.
 
For the particular combination of the DSM2 signaling,  DX7 Tx and AR7000 Rx the 
servo channels which must be connected to the UAVP board are known to be:

   * Aileron, Gear, Aux2 and Rudder

Greg Egan Nov 2008

==============================================
= SAFETY FIRST!                              =
==============================================

If you work on your ufo model, PLEASE TAKE CARE!

Nowadays, extremely powerful motors and accumulators
can pose a serious threat to your and others safety.

Safety measures:
- Remove the propellers before you test parameter or firmware modifications
- Work with a good bench power supply instead of an accupack.
  A short circuit will then not be able to destroy your hardware too easily
- Your ufo mainboards electronics can be damaged by 
  electrostatic discharge sparks. Make sure you are doing any
  work at the electronics in an ESD safe workplace.
- When using your transmitter, be sure to use the correct model memory on it.
- Be sure to have no other transmitters on your channel on air.

The original author and those modifying the program based on
the original V3.15 will NOT BE RESPONSIBLE FOR ANY ACCIDENTS 
OR INJURIES IN ANY WAY!

PLEASE do yourself a favour and get an insurance which covers risks by doing model flight!
Imaging what happens if you only damage someone elses car or something....


==============================================
= Questions?                                 =
==============================================

BUT PLEASE, BEFORE YOU ASK, READ THE MANUAL. It is all in there, really! 
The manual may be found at the original project site.

Also, if you have some hardware related questions, please download the 
datasheets for the chips and read them thoroughly!

The variants of UAVP to which this readme applies are hosted
at:
	 http://code.google.com/p/uavp-mods/

There is an active RCGroups thread at:

	http://www.rcgroups.com/forums/showthread.php?t=811550

If you have any questions on the Official UAVP Project, please go to the forum at:
 
	http://forum.uavp.ch 

or the original project site at:

	http://www.uavp.org 

There are a lot of people that will happily help you,
including the original author (his nickname is "Wolferl").

However, a registration is required to gain access to the forum.

Make sure you have the most recent version of the firmware!

==============================================
= End of README file                         =
==============================================
