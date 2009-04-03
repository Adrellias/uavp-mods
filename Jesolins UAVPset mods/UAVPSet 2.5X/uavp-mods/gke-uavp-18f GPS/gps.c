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
int32 GPSOriginLatitude, GPSOriginLongitude;
int16 GPSNorth, GPSEast, GPSNorthHold, GPSEastHold;
int16 GPSHeading, GPSAltitude, GPSOriginAltitude, GPSGroundSpeed;
#pragma udata

#ifdef USE_GPS

// Global Variables
#pragma udata gpsvars2
boolean FirstGPSSentence;
//int32 GPSStartTime, GPSMissionTime;
uint8 GPSNoOfSats;
#pragma udata

// Include decoding of time etc.
//#define NMEA_ALL

// Prototypes
void UpdateField(void);
int16 ConvertInt(uint8, uint8);
real32 ConvertReal(uint8, uint8);
real32 ConvertLatLon(uint8, uint8);

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

  if (EmptyField)
    ival=0;
  else
    {
    ival=0;
    for (i=lo;i<=hi;i++)
      {
      ival*=10;
      ival+=(GPSRxBuffer[i]-'0');
      }
    }
  return (ival);
} // ConvertInt

real32 ConvertReal(uint8 lo, uint8 hi)
{
  int16 i, n, dp;
  boolean Positive;
  int16 whole;
  real32 rval;

  if (EmptyField)
    rval=0.0;
  else
    {
    if (GPSRxBuffer[lo]=='-') 
      {
      Positive=false;
      lo++;
      } 
    else
      Positive=true;

    dp=lo;
    while ((GPSRxBuffer[dp] != '.')&&(dp<=hi))
      dp++;

    whole=ConvertInt(lo, dp-1);
    rval=ConvertInt(dp + 1, hi);

    n=hi-dp;
    for (i=1;i<=n;i++)
      rval/=10.0;
	  
    if (Positive)
      rval=(whole+rval);
    else
      rval=-(whole+rval);
  }

  return(rval);
}// ConvertReal

real32 ConvertLatLon(uint8 lo, uint8 hi)
{
  real32 dd, mm, rval;

  if (EmptyField)
    rval=0.0;
  else
    {
    dd=(real32)(ConvertInt(lo, hi-7));
    mm=ConvertReal(hi-6, hi);
    rval=dd + mm * 0.0166666667;
    }

  return(rval);
} // ConvertLatLon

#ifdef NMEA_ALL
int32 ConvertUTime(uint8 lo, uint8 hi)
{
 	int32 ival, hh;
 	 int16 mm, ss;

	if (EmptyField)
		ival=0;
	else
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
	// ~7.5mS 18f2520 @ 16MHz
	int32 GPSLatitude, GPSLongitude;
	uint8 GPSFix;

    UpdateField();
    
    UpdateField();   //UTime
	#ifdef NMEA_ALL
	GPSMissionTime=ConvertUTime(lo,hi);
	#else
//	GPSMissionTime = 0;
	#endif // NMEA_ALL      
     
    UpdateField();   //Lat
    GPSLatitude=(int32)(ConvertLatLon(lo,hi)*DEGTOM);
    UpdateField();   //LatH
    if (GPSRxBuffer[lo]=='S')
      GPSLatitude= -GPSLatitude;
    
    UpdateField();   //Lon
    // no latitude compensation on longditude    
    GPSLongitude=(int32)(ConvertLatLon(lo,hi)*DEGTOM);
    UpdateField();   //LonH
	if (GPSRxBuffer[lo]=='W')
      GPSLongitude=-GPSLongitude;
           
    UpdateField();   //Fix 
    GPSFix=(uint8)(ConvertInt(lo,hi));

    UpdateField();   //Sats
    GPSNoOfSats=(uint8)(ConvertInt(lo,hi));

    UpdateField();   // HDilute

    UpdateField();   // Alt
    GPSAltitude=(int16) (ConvertReal(lo, hi)+0.5);

    //UpdateField();   // AltUnit

    //UpdateField();   // GHeight 
    //UpdateField();   // GHeightUnit 
 
    _GPSValid=(GPSFix>0);
// zzz	mS[GPSTimeout] = mS[Clock] + GPS_TIMEOUT;
    
    if (FirstGPSSentence&&_GPSValid)
	{
//		GPSStartTime=GPSMissionTime;
      	GPSOriginLatitude=GPSLatitude;
      	GPSOriginLongitude=GPSLongitude;

		// No Longitude correction

      	GPSOriginAltitude=GPSAltitude;
      	FirstGPSSentence=false;
	}
	// all cordinates in Metres relative to Origin
	GPSNorth = GPSLatitude - GPSOriginLatitude;
	GPSEast = GPSLongitude - GPSOriginLongitude; 
} // ParseGPSSentence

void PollGPS(void)
{
  if (RxHead!=RxTail)
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
	_NMEADetected = false;
	GPSSentenceReceived=false;
	FirstGPSSentence = true;
	_GPSValid = false; 
  	GPSRxState=WaitGPSSentinel; 
  	ll=0;
  	cc=ll;
  	ch=' ';
} // InitGPS

void UpdateGPS(void)
{
	if ( GPSSentenceReceived )
	{
		GPSSentenceReceived=false; // 7.5mS 18f2520 @ 16MHz 
		ParseGPSSentence();
	}
	else
//zzz		if ( mS[Clock] >= mS[GPSTimeout] )
			_GPSValid = false;

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

void UpdateGPS(void)
{
	// empty
} // UpdateGPS

void GPSAltitudeHold(int16 DesiredAltitude)
{

	// empty

} // GPSAltitudeHold

#endif // USE_GPS

