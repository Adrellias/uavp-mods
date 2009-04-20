// =======================================================================
// =                                 UAVX                                =
// =                         Quadrocopter Control                        =
// =               Copyright (c) 2008-9 by Prof. Greg Egan               =
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

// 	GPS routines

#include "c-ufo.h"
#include "bits.h"

// Prototypes
void ParseGPSSentence(void);
void PollGPS(void);
void InitGPS(void);
void UpdateGPS(void);

// Defines


// Variables

#pragma udata gpsvars
boolean GPSSentenceReceived;
uint8 GPSFix;
int8 GPSHDilute;
int32 GPSOriginLatitude, GPSOriginLongitude;
int16 GPSNorth, GPSEast, GPSNorthHold, GPSEastHold;
int16 GPSHeading, GPSAltitude, GPSOriginAltitude, GPSGroundSpeed;
#pragma udata

#ifdef USE_GPS

// Global Variables
#pragma udata gpsvars2
int8 ValidGPSSentences;
//int32 GPSStartTime, GPSMissionTime;
uint8 GPSNoOfSats;
#pragma udata

// Include decoding of time etc.
//#define NMEA_ALL

// Prototypes
void UpdateField(void);
int16 ConvertInt(uint8, uint8);
int32 ConvertLatLon(uint8, uint8);

#pragma udata gpsvars
enum WaitGPSStates {WaitGPSSentinel, WaitGPSTag, WaitGPSBody, WaitGPSCheckSum};
uint8 GPSRxState;

#define GPSRXBUFFLENGTH 80
uint8 ch, GPSRxBuffer[GPSRXBUFFLENGTH];
uint8 cc, ll, tt, lo, hi;

#define MAXTAGINDEX 4
const uint8 GPGGATag[6]= {"GPGGA"}; 				// full positioning fix

boolean EmptyField;
uint8 GPSTxCheckSum, GPSRxCheckSum, GPSCheckSumChar;
#pragma udata

int16 ConvertInt(uint8 lo, uint8 hi)
{
	uint8 i;
	int16 ival;

	ival=0;
	if (!EmptyField)
		for (i=lo;i<=hi;i++)
		{
			ival*=10;
			ival+=(GPSRxBuffer[i]-'0');
		}

	return (ival);
} // ConvertInt

int32 ConvertLatLonM(uint8 lo, uint8 hi)
{ 	// metres
	// positions are stored at maximum transmitted GPS resolution which
	// is approximately 0.18553257183 Metres per LSB

	uint32 dd, mm, ss;	
	int32 ival;
	
	ival=0;
	if ( !EmptyField )
	{
	    dd=ConvertInt(lo, hi-7);
	    mm=ConvertInt(hi-6, hi-5);
		ss=ConvertInt(hi-3, hi);
	    ival = dd *600000 + mm*100000 + ss;
	}
	
	return(ival);
} // ConvertLatLonM
	
#ifdef NMEA_ALL
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
#endif // NMEA_ALL

void UpdateField()
{
	lo=cc;
	hi=lo-1;
	if ((GPSRxBuffer[cc] != ',')&&(GPSRxBuffer[cc]!='*'))
    {
		while ((cc<ll)&&(GPSRxBuffer[cc]!=',')&&(GPSRxBuffer[cc]!='*'))
		cc++;
		hi=cc-1;
	}
	cc++;
	EmptyField=(hi<lo);
} // UpdateField

void ParseGPSSentence()
{ 	// full position $GPGGA fix 
	int32 GPSLatitude, GPSLongitude;

    UpdateField();
    
    UpdateField();   //UTime
	#ifdef NMEA_ALL
	GPSMissionTime=ConvertUTime(lo,hi);
	#else
//	GPSMissionTime = 0;
	#endif // NMEA_ALL      

	UpdateField();   //Lat
    GPSLatitude = ConvertLatLonM(lo,hi);
    UpdateField();   //LatH
    if (GPSRxBuffer[lo]=='S')
      	GPSLatitude = -GPSLatitude;

    UpdateField();   //Lon
    // no latitude compensation on longitude    
    GPSLongitude = ConvertLatLonM(lo,hi);
    UpdateField();   //LonH
	if (GPSRxBuffer[lo]=='W')
      	GPSLongitude = -GPSLongitude;
         
    UpdateField();   //Fix 
    GPSFix=(uint8)(ConvertInt(lo,hi));

    UpdateField();   //Sats
    GPSNoOfSats=(uint8)(ConvertInt(lo,hi));

    UpdateField();   // HDilute
	GPSHDilute = ConvertInt(lo, hi-2) * 10 + ConvertInt(hi, hi); 

    UpdateField();   // Alt
	GPSAltitude = ConvertInt(lo, hi-2) * 10 + ConvertInt(hi, hi); // Decimetres

    //UpdateField();   // AltUnit - assume Metres!

    //UpdateField();   // GHeight 
    //UpdateField();   // GHeightUnit 
 
    _GPSValid = (GPSFix >= MIN_FIX) && ( GPSNoOfSats >= MIN_SATELLITES );
    
	if ( _GPSValid )
	{
	    if ( ValidGPSSentences < INITIAL_GPS_SENTENCES )
		{
			_GPSValid = false;
			ValidGPSSentences++;
	
	//		GPSStartTime=GPSMissionTime;
	      	GPSOriginLatitude = GPSLatitude;
	      	GPSOriginLongitude = GPSLongitude;
	
			// No Longitude correction i.e. Cos(Latitude)
	
	      	GPSOriginAltitude=GPSAltitude;
		}

		// all cordinates in 0.0001 Minutes relative to Origin
		GPSNorth = GPSLatitude - GPSOriginLatitude;
		GPSEast = GPSLongitude - GPSOriginLongitude; 
		GPSAltitude -= GPSOriginAltitude;
	}
} // ParseGPSSentence

void PollGPS(void)
{
	if (RxHead != RxTail)
		switch (GPSRxState) {
	    case WaitGPSBody:
	      {
	      ch=RxChar(); RxCheckSum^=ch;
	      GPSRxBuffer[ll]=ch;
	      ll++;         
	      if ((ch=='*')||(ll==GPSRXBUFFLENGTH))      
	        {
	        GPSCheckSumChar=0;
	        GPSTxCheckSum=0;
	        GPSRxState=WaitGPSCheckSum;
	        }
	      else 
	        if (ch=='$') // abort partial Sentence 
	          {
	          ll=0;
	          cc=0;
	          tt=0;
	          RxCheckSum=0;
	          GPSRxState=WaitGPSTag;
	          }
	        else
	          GPSRxCheckSum=RxCheckSum;
	      break;
	      }
	    case WaitGPSCheckSum:
	      {
	      ch=RxChar();
	      if (GPSCheckSumChar<2)
	        {
	        if (ch>='A')
	          GPSTxCheckSum=GPSTxCheckSum*16+((int16)(ch)+10-(int16)('A'));
	        else
	          GPSTxCheckSum=GPSTxCheckSum*16+((int16)(ch)-(int16)('0'));
	        GPSCheckSumChar++;
	        }
	      else
	        {
	        GPSSentenceReceived=GPSRxCheckSum==GPSTxCheckSum;
	        GPSRxState=WaitGPSSentinel;
	        }
	      break;
	      }
	    case WaitGPSTag:
	      {
	      ch=RxChar(); RxCheckSum^=ch;
	      if (ch==GPGGATag[tt])
	        if (tt==MAXTAGINDEX)
	          GPSRxState=WaitGPSBody;
	        else
	          tt++;
	      else
	        GPSRxState=WaitGPSSentinel;
	      break;
	      }
	    case WaitGPSSentinel:
	      {
	      ch=RxChar();
	      if (ch=='$')
	        {
	        ll=0;
	        cc=0;
	        tt=0;
	        RxCheckSum=0;
	        GPSRxState=WaitGPSTag;
	        }
	      break;
	      }
    }  
} // PollGPS

void InitGPS()
{ 
	uint8 c;

	GPSEast = GPSNorth = 0;
	GPSNoOfSats = 0;
	GPSFix = 0;
	GPSSentenceReceived=false;
	ValidGPSSentences = 0;
//	FirstGPSSentence = true;
	_GPSValid = false; 
	GPSCount = 0;
  	GPSRxState=WaitGPSSentinel; 
  	ll=0;
  	cc=ll;
  	ch=' ';
} // InitGPS

void UpdateGPS(void)
{
	if ( GPSSentenceReceived )
	{
		LedBlue_ON;
		LedRed_OFF;
		GPSSentenceReceived=false;  
		ParseGPSSentence(); // 7.5mS 18f2520 @ 16MHz
		if ( _GPSValid )
			GPSCount = 0;
	}
	else
		if( (BlinkCount & 0x000f) == 0 )
			if( GPSCount > GPSDROPOUT )
				_GPSValid = false;
			else
				GPSCount++;

	LedBlue_OFF;
	if ( _GPSValid )
		LedRed_OFF;
	else
		LedRed_ON;	

} // UpdateGPS

void GPSAltitudeHold(int16 DesiredAltitude)
{


} // GPSAltitudeHold


#else

void InitGPS(void)
{
	GPSSentenceReceived=false;
	_GPSValid=false;  
}  // InitGPS

void PollGPS(void)
{

}  // PollGPS

void UpdateGPS(void)
{
	// empty
} // UpdateGPS

void GPSAltitudeHold(int16 DesiredAltitude)
{

	// empty

} // GPSAltitudeHold

#endif // USE_GPS

