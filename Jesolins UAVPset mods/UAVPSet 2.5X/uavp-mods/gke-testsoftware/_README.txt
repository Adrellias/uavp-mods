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
= Pre-compiled Firmware                      =
==============================================

Firmware HEX files for most configurations are available at
the project sites if you wish to use them rather than going
through the following procedures. If you choose to do this you
should download an appropriate HEX file and skip to:

    * Transferring firmware to the PIC (below). 

==============================================
= How to setup the Compilation Software      =
==============================================

    * Download the cc5x compiler (http://www.bknd.com/cc5xfree.exe).
    * Run the executable to install the compiler. 
      Install it into c:\program files\microchip\cc5x.
    * Download MPLAB (http://ww1.microchip.com/downloads/...eDoc/mp810.zip).
    * Unzip mp810.zip into a temp directory. Run Install_MPLAB_v810.exe.
    * Unzip the uavp source into a newly created directory.
    * Launch a command prompt and cd into that directory.
    * If not using mod2 or later: Run "copy c:\program files\microchip\cc5x\reloc.inc ."

==============================================
= How to Build Versions of UAVP              =
==============================================

    * Download the latest release source from the Downloads section
      (http://code.google.com/p/uavp-mods/). See various Disclaimers.

    * Edit "makeall.bat". There are sets of parameters including 
      those for all possible versions of the TestSoftware. 
      Change the "personal" sets to cover the range of 
      versions you are interest in.
    * Run makeall.bat (this does not delete old hex files 
      so check the creation dates)
    * Done! 

==============================================
= Transferring Firmware to the PIC           =
==============================================

If you already have a PIC with at least software version 3.05,
You can upload your new version using the bootloader.
Use UAVPset.exe to do that.

But be careful: If you mock up things completely, you can lock
yourself out. Then you would need a PIC programmer to get things 
working again.

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
