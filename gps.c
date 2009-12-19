// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008, 2009 by Prof. Greg Egan           =
// =           http://code.google.com/p/uavp-mods/ http://uavp.ch        =
// =======================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

// 	GPS routines

#include "uavx.h"

// Prototypes

int16 ConvertGPSToM(int16);
int16 ConvertMToGPS(int16);
void UpdateField(void);
int16 ConvertInt(uint8, uint8);
int32 ConvertLatLonM(uint8, uint8);
int32 ConvertUTime(uint8, uint8);
void ParseGPRMCSentence(void);
void ParseGPGGASentence(void);
void SetGPSOrigin(void);
void ParseGPSSentence(void);
void ResetGPSOrigin(void);
void InitGPS(void);
void UpdateGPS(void);

// Defines
#define GPSFilter SoftFilter				// probably need Kalman Estimator?
#define GPSVelocityFilter SoftFilterU		// done after position filter

// Variables

#pragma udata gpsvars
uint8 GPSNoOfSats;
uint8 GPSFix;
int16 GPSHDilute;
#pragma udata

#pragma udata gpsvars2
int32 GPSMissionTime, GPSStartTime;
int32 GPSLatitude, GPSLongitude;
int32 GPSOriginLatitude, GPSOriginLongitude;
int16 GPSNorth, GPSEast, GPSNorthHold, GPSEastHold, GPSNorthP, GPSEastP, GPSVel;
int16 GPSLongitudeCorrection;
int16 GPSAltitude, GPSRelAltitude, GPSOriginAltitude;
#pragma udata

// Global Variables

#pragma udata gpsvars3
uint8 nll, cc, lo, hi;
boolean EmptyField;
int16 ValidGPSSentences;
#pragma udata

int16 ConvertGPSToM(int16 c)
{	// approximately 0.18553257183 Metres per LSB at the Equator
	// only for converting difference in coordinates to 32K
	return ( ((int32)c * (int32)18553)/((int32)100000) );
} // ConvertGPSToM

int16 ConvertMToGPS(int16 c)
{
	return ( ((int32)c * (int32)100000)/((int32)18553) );
} // ConvertMToGPS

int16 ConvertInt(uint8 lo, uint8 hi)
{
	static uint8 i;
	static int16 r;

	r = 0;
	if ( !EmptyField )
		for (i = lo; i <= hi ; i++ )
			r = r * 10 + NMEA.s[i] - '0';

	return (r);
} // ConvertInt

int32 ConvertLatLonM(uint8 lo, uint8 hi)
{ 	// NMEA coordinates normally assumed as DDDMM.MMMM ie 4 decimal minute digits
	// but code can deal with 4 and 5 decimal minutes 
	// Positions are stored at 4 decimal minute NMEA resolution which is
	// approximately 0.1855 Metres per LSB at the Equator.
	static int32 dd, mm, dm, r;
	static int8 dp;	
	
	r = 0;
	if ( !EmptyField )
	{
		dp = lo + 4; // could do this in initialisation for Lat and Lon?
		while ( NMEA.s[dp] != '.') dp++;

	    dd = ConvertInt(lo, dp - 3);
	    mm = ConvertInt(dp - 2 , dp - 1);
		dm = ConvertInt(dp + 1, dp + 4);
	    r = dd * 600000 + mm * 10000 + dm;
	}
	
	return(r);
} // ConvertLatLonM

/*	
int32 ConvertUTime(uint8 lo, uint8 hi)
{
	int32 ival, hh;
	int16 mm, ss;
	
	ival=0;
	if ( !EmptyField )
		ival=(int32)(ConvertInt(lo, lo+1))*3600+
				(int32)(ConvertInt(lo+2, lo+3)*60)+
				(int32)(ConvertInt(lo+4, hi));
	      
	return(ival);
} // ConvertUTime
*/

void UpdateField(void)
{
	static uint8 ch;

	lo = cc;

	ch = NMEA.s[cc];
	while (( ch != ',' ) && ( ch != '*' ) && ( cc < nll )) 
		ch = NMEA.s[++cc];

	hi = cc - 1;
	cc++;
	EmptyField = hi < lo;
} // UpdateField

void ParseGPGGASentence(void)
{ 	// full position $GPGGA fix 

    UpdateField();
    
    UpdateField();   	//UTime
	//GPSMissionTime=ConvertUTime(lo,hi);

	UpdateField();   	//Lat
    GPSLatitude = ConvertLatLonM(lo, hi);
    UpdateField();   	//LatH
    if (NMEA.s[lo] == 'S')
      	GPSLatitude = -GPSLatitude;

    UpdateField();   	//Lon
    // no latitude compensation on longitude - yet!    
    GPSLongitude = ConvertLatLonM(lo, hi);
    UpdateField();   	//LonH
	if (NMEA.s[lo] == 'W')
      	GPSLongitude = -GPSLongitude;
        
    UpdateField();   	//Fix 
    GPSFix = (uint8)(ConvertInt(lo,hi));

    UpdateField();   	//Sats
    GPSNoOfSats = (uint8)(ConvertInt(lo,hi));

    UpdateField();   	// HDilute
	GPSHDilute = ConvertInt(lo, hi-3) * 100 + ConvertInt(hi-1, hi); 

    UpdateField();   	// Alt
	GPSAltitude = ConvertInt(lo, hi-2) * 10 + ConvertInt(hi, hi); // Decimetres

    //UpdateField();   // AltUnit - assume Metres!

    //UpdateField();   // GHeight 
    //UpdateField();   // GHeightUnit 
 
    F.GPSValid = (GPSFix >= GPS_MIN_FIX) && ( GPSNoOfSats >= GPS_MIN_SATELLITES );  

	if ( F.GPSTestActive )
	{
		TxString("$GPGGA ");
		if ( !F.GPSValid )
		{
			TxString(" fx=");
			TxVal32(GPSFix, 0, ' ');
	
			TxString("s=");
			TxVal32(GPSNoOfSats, 0, ' ');
	
			TxString("hd=");
			TxVal32(GPSHDilute, 2, ' ');

			TxString("invalid\r\n");
		}
	}
} // ParseGPGGASentence

void SetGPSOrigin(void)
{
	static int32 Temp;

	GPSStartTime = GPSMissionTime;
	GPSOriginLatitude = GPSLatitude;
	GPSOriginLongitude = GPSLongitude;
	GPSNorthHold = GPSEastHold = GPSNorthP = GPSEastP = GPSVel = 0;

	mS[LastGPS] = mS[Clock];
	
	Temp = GPSLatitude/60000L;
	Temp = Abs(Temp);
	Temp = ConvertDDegToMPi(Temp);
	GPSLongitudeCorrection = int16cos(Temp);

	GPSOriginAltitude = GPSAltitude;
} // SetGPSOrigin

void ParseGPSSentence(void)
{
	static int16 EastDiff, NorthDiff, GPSVelP, Temp;
	static int24 GPSInterval;

	cc = 0;
	nll = NMEA.length;

	ParseGPGGASentence(); 

	if ( State == InFlight ) Stats[GPSSentencesS].i16++;

	if ( F.GPSValid )
	{
	    if ( ValidGPSSentences <=  GPS_INITIAL_SENTENCES )
		{   // repetition to ensure GPGGA altitude is captured

			if ( F.GPSTestActive )
			{
				TxVal32( GPS_INITIAL_SENTENCES - ValidGPSSentences, 0, 0);
				TxNextLine();
			}

			F.GPSValid = false;

			if ( GPSHDilute <= GPS_MIN_HDILUTE )
				if ( ValidGPSSentences ==  GPS_INITIAL_SENTENCES )
					if ( State == InFlight ) // already flying!
					{
						SetGPSOrigin();
						F. GPSOriginValid = false;
					}
					else
					{
						SetGPSOrigin();
						F. GPSOriginValid = true;
						DoBeep100mSWithOutput(2,0);
					}
				else
					ValidGPSSentences++;
			else
				ValidGPSSentences = 0;	
		}
		else
		{
			GPSInterval = mS[Clock] - mS[LastGPS];
			mS[LastGPS] = mS[Clock];

			// all coordinates in 0.0001 Minutes or ~0.185M units relative to Origin
			// There is a lot of jitter in position - could use Kalman Estimator?
			GPSNorth = GPSFilter(GPSNorth, GPSLatitude - GPSOriginLatitude);
			Temp = GPSLongitude - GPSOriginLongitude;
			GPSEast = GPSFilter(GPSEast, SRS32((int32)GPSEast * GPSLongitudeCorrection, 8)); 

			EastDiff = GPSEast - GPSEastP;
			NorthDiff = GPSNorth - GPSNorthP;
			GPSVelP = GPSVel;
			GPSVel = int16sqrt(EastDiff*EastDiff + NorthDiff*NorthDiff);
			GPSVel = ((int24)GPSVel * 1000L)/GPSInterval;
			GPSVel = GPSVelocityFilter(GPSVelP, GPSVel);

			GPSNorthP = GPSNorth;
			GPSEastP = GPSEast;			

			GPSRelAltitude = GPSAltitude - GPSOriginAltitude;

			if ( State == InFlight )
			{
				if ( GPSRelAltitude > Stats[GPSAltitudeS].i16 ) 
					Stats[GPSAltitudeS].i16 = GPSRelAltitude;
				if ( GPSVel > Stats[GPSVelS].i16 )
					Stats[GPSVelS].i16 = GPSVel;

				if ( GPSHDilute > Stats[MaxHDiluteS].i16 ) 
					Stats[MaxHDiluteS].i16 = GPSHDilute; 
				else 
					if ( GPSHDilute < Stats[MinHDiluteS].i16 ) 
						Stats[MinHDiluteS].i16 = GPSHDilute;
			}
		}
	}
	else
		if ( State == InFlight ) 
		{
			Stats[GPSInvalidS].i16++;
			F.GPSFailure = true;
		}

} // ParseGPSSentence

void ResetGPSOrigin(void)
{
	if ( ValidGPSSentences >  GPS_INITIAL_SENTENCES )
		SetGPSOrigin();	
	// otherwise continue with first acquisition
} // ResetGPSOrigin

void InitGPS(void)
{ 
	uint8 n;

	cc = 0;

	GPSMissionTime = GPSRelAltitude = GPSFix = GPSNoOfSats = GPSHDilute = 0;
	GPSEast = GPSNorth = GPSVel = 0;

	ValidGPSSentences = 0;

	F.GPSOriginValid = F.GPSValid = F.GPSSentenceReceived = false;
  	GPSRxState = WaitGPSSentinel; 
  	
} // InitGPS

void UpdateGPS(void)
{
	if ( F.GPSSentenceReceived )
	{
		LEDBlue_ON;
		LEDRed_OFF;
		F.GPSSentenceReceived = false;  
		ParseGPSSentence(); // 7.5mS 18f2520 @ 16MHz
		if ( F.GPSValid )
		{
			F.NavComputed = false;
			mS[GPSTimeout] = mS[Clock] + GPS_TIMEOUT_MS;
		}

		SendUAVXState();	// Tx overlapped with next GPS packet Rx
	}
	else
		if( mS[Clock] > mS[GPSTimeout] )
			F.GPSValid = false;

	LEDBlue_OFF;
	if ( F.GPSValid )
		LEDRed_OFF;
	else
		LEDRed_ON;	
} // UpdateGPS

