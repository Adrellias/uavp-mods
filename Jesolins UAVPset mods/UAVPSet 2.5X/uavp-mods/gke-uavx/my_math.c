// =======================================================================
// =                   U.A.V.P Brushless UFO Controller                  =
// =                         Professional Version                        =
// =             Copyright (c) 2007  Prof. Greg Egan                     =
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

// Misc math routines

#include "c-ufo.h"
#include "bits.h"

/*
int16 int16sqrt(int16 n)
// 16 bit numbers 
{
  int16 r, b;

  r=0;
  b=256;
  while (b>0) 
    {
    if (r*r>n)
      r-=b;
    b=(b>>1);
    r+=b;
    }
  return(r);
} // int16sqrt

int16 int32sqrt(int32 n)
// 32 bit numbers 
{
  int32 b;
  int16 r;

  b=65536;
  r=0;
  while (b>0) 
    {
    if (((int32)(r)*(int32)(r))>n)
      r-=b;
    b=(b>>1);
    r+=b;
    }
  return(r);
} // int32sqrt

*/

#pragma idata arctantable
const int16 ArctanTable[17]={
    0,  62, 124, 185, 245, 303, 359, 412,
  464, 512, 559, 602, 644, 682, 719, 753,785
   };
#pragma idata

int16 iatan2q1(int16 Val)
{
 uint8 Index,Offset;
 int16 Temp, Low, High, Result;

 Index=(uint8) (Val>>4);
 Offset=(uint8) (Val & 0x0F);
 Low=ArctanTable[Index];
 High=ArctanTable[++Index];
 Temp=(High-Low)*Offset;
 Result = Low+(Temp>>4);

 return(Result);
} // iatan2q1

int16 int16atan2(int32 y, int32 x)
{
 int32 Absx, Absy;
 int16 A;

 Absy=Abs(y);
 Absx=Abs(x);
 
 if ((Absy>32000)||(Absx>32000))//  stops force negative by shift <<8
   {
   Absy=Absy>>8;
   Absx=Absx>>8;
   }
   
 if (x==0)
   if (y<0)
     A=-HALFMILLIPI;
   else
     A=HALFMILLIPI;
 else
   if (y==0)
     if (x<0)
       A=MILLIPI;
     else
       A=0;
   else
     {
     if (Absy<=Absx)
       A=iatan2q1((Absy<<8)/Absx);
     else
       A=HALFMILLIPI-iatan2q1((Absx<<8)/Absy); 

     if (x<0)
       if (y>0) // 2nd Quadrant 
         A=MILLIPI-A;
       else // 3rd Quadrant 
         A=MILLIPI+A;
     else
       if (y<0) // 4th Quadrant 
         A=TWOMILLIPI-A;
     }
  return(A);
} // int16atan2


/*
int16 int16atan(int16 y, int16 x)
{
 int32 Absx, Absy;
 int16 A;

 Absy=Abs(y);
 Absx=Abs(x);

 if (x==0)
   if (y<0)
     A=-HALFMILLIPI;
   else
     A=HALFMILLIPI;
 else
   if (y==0)
     if (x<0)
       A=MILLIPI;
     else
       A=0;
   else
     {
     if (Absy<=Absx)
       A=iatan2q1((Absy<<8)/Absx);
     else
       A=HALFMILLIPI-iatan2q1((Absx<<8)/Absy); 

     if (x<0)
       if (y>0) // 2nd Quadrant 
         A=MILLIPI-A;
       else // 3rd Quadrant 
         A=MILLIPI+A;
     else
       if (y<0) // 4th Quadrant 
         A=TWOMILLIPI-A;
     }

  return(A);
}
*/

