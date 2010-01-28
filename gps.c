// =======================================================================
// =                     UAVX Quadrocopter Controller                    =
// =               Copyright (c) 2008 by Prof. Greg Egan                 =
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

void UpdateField(void);
int32 ConvertGPSToM(int32);
int32 ConvertMToGPS(int32);
int24 ConvertInt(uint8, uint8);
int32 ConvertLatLonM(uint8, uint8);
int32 ConvertUTime(uint8, uint8);
void ParseGPRMCSentence(void);
void ParseGPGGASentence(void);
void SetGPSOrigin(void);
void ParseGPSSentence(void);
void InitGPS(void);
void UpdateGPS(void);

// Defines
// Moving average of coordinates - Kalman Estimator probably needed
#define GPSFilter NoFilter
//#define GPSFilter SoftFilter				
//#define GPSFilter MediumFilter

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
int24 GPSNorth, GPSEast, GPSNorthP, GPSEastP, GPSNorthHold, GPSEastHold;
int16 GPSVel;
int16 GPSLongitudeCorrection;
int24 GPSAltitude, GPSRelAltitude, GPSOriginAltitude;
#pragma udata

// Global Variables

#pragma udata gpsvars3
uint8 nll, cc, lo, hi;
boolean EmptyField;
int16 ValidGPSSentences;
#pragma udata

int32 ConvertGPSToM(int32 c)
{	// approximately 1.8553257183 cm per LSB at the Equator
	// conversion max is 21Km
	return ( ((int32)c * (int32)1855)/((int32)100000) );
} // ConvertGPSToM

int32 ConvertMToGPS(int32 c)
{
	// conversion max is 21Km 
	return ( ((int32)c * (int32)100000)/((int32)1855) );
} // ConvertMToGPS

int24 ConvertInt(uint8 lo, uint8 hi)
{
	static uint8 i;
	static int24 r;

	r = 0;
	if ( !EmptyField )
		for (i = lo; i <= hi ; i++ )
			r = r * 10 + NMEA.s[i] - '0';

	return (r);
} // ConvertInt

int32 ConvertLatLonM(uint8 lo, uint8 hi)
{ 	// NMEA coordinates normally assumed as DDDMM.MMMMM ie 5 decimal minute digits
	// but code can deal with 4 and 5 decimal minutes 
	// Positions are stored at 5 decimal minute NMEA resolution which is
	// approximately 1.855 cm per LSB at the Equator.
	static int32 dd, mm, dm, r;
	static int8 dp;	
	
	r = 0;
	if ( !EmptyField )
	{
		dp = lo + 4; // could do this in initialisation for Lat and Lon?
		while ( NMEA.s[dp] != '.') dp++;

	    dd = ConvertInt(lo, dp - 3);
	    mm = ConvertInt(dp - 2 , dp - 1);
		if ( ( hi - dp ) > 4 )
			dm = ConvertInt(dp + 1, dp + 5);
		else
			dm = ConvertInt(dp + 1, dp + 4)* 10L;
			
	    r = dd * 6000000 + mm * 100000 + dm;
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
    
    UpdateField();   //UTime
	//GPSMissionTime=ConvertUTime(lo,hi);

	UpdateField();   	//Lat
    GPSLatitude = ConvertLatLonM(lo, hi);
    UpdateField();   //LatH
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
	GPSAltitude = ( ConvertInt(lo, hi-2) * 10L + ConvertInt(hi, hi) ) * 10L; // Centimetres

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

	if ( ( ValidGPSSentences == GPS_INITIAL_SENTENCES ) && F.GPSValid )
	{
		GPSStartTime = GPSMissionTime;
		GPSOriginLatitude = GPSLatitude;
		GPSOriginLongitude = GPSLongitude;
		GPSNorthHold = GPSEastHold = GPSNorthP = GPSEastP = GPSVel = 0;
		
		mS[LastGPS] = mS[Clock];
			
		Temp = GPSLatitude/600000L; // to degrees * 10
		Temp = Abs(Temp);
		Temp = ConvertDDegToMPi(Temp);
		GPSLongitudeCorrection = int16cos(Temp);
	
		GPSOriginAltitude = GPSAltitude;

		if ( !F.NavValid )
		{
			DoBeep100mSWithOutput(2,0);
			Stats[NavValidS].i16 = true;
			F.NavValid = true;
		}
		F.AcquireNewPosition = true;		
	}
} // SetGPSOrigin

void ParseGPSSentence(void)
{
	static int32 Temp;
	static int24 GPSEastDiff, GPSNorthDiff;
	static int16 GPSVelP;
	static int24 GPSInterval;

	cc = 0;
	nll = NMEA.length;

	ParseGPGGASentence(); 

	if ( State == InFlight ) Stats[GPSSentencesS].i16++;

	if ( F.GPSValid )
	{
	    if ( ValidGPSSentences <  GPS_INITIAL_SENTENCES )
		{   // repetition to ensure GPGGA altitude is captured

			if ( F.GPSTestActive )
			{
				TxVal32( GPS_INITIAL_SENTENCES - ValidGPSSentences, 0, 0);
				TxNextLine();
			}

			F.GPSValid = false;

			if ( GPSHDilute <= GPS_MIN_HDILUTE )
				ValidGPSSentences++;
			else
				ValidGPSSentences = 0;	
		}
		else
		{
			GPSInterval = mS[Clock] - mS[LastGPS];
			mS[LastGPS] = mS[Clock];

			// all coordinates in 0.00001 Minutes or ~1.8553cm relative to Origin
			// There is a lot of jitter in position - could use Kalman Estimator?
			GPSNorth = GPSFilter(GPSNorth, Temp);

			Temp = SRS32((int32)Temp * GPSLongitudeCorrection, 8);
			GPSEast = GPSFilter(GPSEast, Temp);

			#ifdef GPS_INC_GROUNDSPEED
			// nice to have but not essential 
			GPSEastDiff = GPSEast - GPSEastP;
			GPSNorthDiff = GPSNorth - GPSNorthP;
			GPSVelP = GPSVel;
			GPSVel = int32sqrt(GPSEastDiff*GPSEastDiff + GPSNorthDiff*GPSNorthDiff);
			GPSVel = ((int32)GPSVel * 1000L)/GPSInterval;
			GPSVel = GPSVelocityFilter(GPSVelP, GPSVel);

			GPSNorthP = GPSNorth;
			GPSEastP = GPSEast;
			#endif // GPS_INC_GROUNDSPEED			

			GPSRelAltitude = GPSAltitude - GPSOriginAltitude;

			if ( State == InFlight )
			{
				if ( GPSRelAltitude > MaxGPSAltitudeS )
					MaxGPSAltitudeS = GPSRelAltitude;

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

void InitGPS(void)
{ 
	uint8 n;

	cc = 0;

	GPSLongitudeCorrection = 256; // 1.0
	GPSMissionTime = GPSRelAltitude = GPSFix = GPSNoOfSats = GPSHDilute = 0;
	GPSAltitude = 0;
	GPSEast = GPSNorth = GPSVel = 0;

	ValidGPSSentences = 0;

	F.NavValid = F.GPSValid = F.GPSSentenceReceived = false;
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

