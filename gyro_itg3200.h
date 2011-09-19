// ===============================================================================================
// =                                UAVX Quadrocopter Controller                                 =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                     http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// ===============================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

// ITG3200 3-axis I2C Gyro

// ITG3200 Register Defines
#define ITG_WHO		0x00
#define	ITG_SMPL	0x15
#define ITG_DLPF	0x16
#define ITG_INT_C	0x17
#define ITG_INT_S	0x1A
#define	ITG_TMP_H	0x1B
#define	ITG_TMP_L	0x1C
#define	ITG_GX_H	0x1D
#define	ITG_GX_L	0x1E
#define	ITG_GY_H	0x1F
#define	ITG_GY_L	0x20
#define ITG_GZ_H	0x21
#define ITG_GZ_L	0x22
#define ITG_PWR_M	0x3E

uint8 ITG_ID;

void ITG3200ViewRegisters(void);
void BlockReadITG3200(void);
void InitITG3200(void);
boolean ITG3200GyroActive(void);

void BlockReadITG3200(void)
{	// Roll Right +, Pitch Up +, Yaw ACW +

	static uint8 G[6];
	static i16u GX, GY, GZ;

	F.GyroFailure = !ReadI2CString(ITG_ID, ITG_GX_H, G, 6);
	if ( F.GyroFailure ) 
		Stats[GyroFailS]++;
	else
	{	
		if ( GyroType == ITG3200DOF9 )
		{
			GX.b0 = G[1]; GX.b1 = G[0]; GX.i16 = -GX.i16; // Roll
			GY.b0 = G[3]; GY.b1 = G[2]; // Pitch
			GZ.b0 = G[5]; GZ.b1 = G[4]; GZ.i16 = - GZ.i16; // Yaw
		}
		else
		{
			GX.b0 = G[1]; GX.b1 = G[0]; GX.i16 = - GX.i16; // Roll
			GY.b0 = G[3]; GY.b1 = G[2]; // Pitch
			GZ.b0 = G[5]; GZ.b1 = G[4]; GZ.i16 = - GZ.i16; // Yaw
		}
	
		GyroADC[Roll] = GX.i16;
		GyroADC[Pitch] = GY.i16;
		GyroADC[Yaw] = GZ.i16;
	}

} // BlockReadITG3200

void InitITG3200(void)
{
	WriteI2CByteAtAddr(ITG_ID,ITG_PWR_M, 0x80);			// Reset to defaults
	WriteI2CByteAtAddr(ITG_ID,ITG_SMPL, 0x00);			// continuous update
	WriteI2CByteAtAddr(ITG_ID,ITG_DLPF, 0b00011001);	// 188Hz, 2000deg/S
	WriteI2CByteAtAddr(ITG_ID,ITG_INT_C, 0b00000000);	// no interrupts
	WriteI2CByteAtAddr(ITG_ID,ITG_PWR_M, 0b00000001);	// X Gyro as Clock Ref.

	Delay1mS(50);

} // InitITG3200

boolean ITG3200GyroActive(void) 
{
	ITG_ID = ITG_ID_3DOF;

	F.GyroFailure = !I2CResponse(ITG_ID);
	if ( F.GyroFailure )
	{
		ITG_ID = ITG_ID_6DOF;
	  	F.GyroFailure = !I2CResponse(ITG_ID);
	}

  return ( !F.GyroFailure );
} // ITG3200GyroActive

#ifdef TESTING

void GyroITG3200Test(void)
{
	TxString("\r\nITG3200 3 axis I2C Gyro Test\r\n");

	if ( I2CResponse(ITG_ID))
	{
		TxString("WHO_AM_I  \t"); TxValH(ReadI2CByteAtAddr(ITG_ID,ITG_WHO)); TxNextLine();
	//	Delay1mS(1000);
		TxString("SMPLRT_DIV\t"); TxValH(ReadI2CByteAtAddr(ITG_ID,ITG_SMPL)); TxNextLine();
		TxString("DLPF_FS   \t"); TxValH(ReadI2CByteAtAddr(ITG_ID,ITG_DLPF)); TxNextLine();
		TxString("INT_CFG   \t"); TxValH(ReadI2CByteAtAddr(ITG_ID,ITG_INT_C)); TxNextLine();
		TxString("INT_STATUS\t"); TxValH(ReadI2CByteAtAddr(ITG_ID,ITG_INT_S)); TxNextLine();
		TxString("TEMP      \t"); TxVal32((ReadI2CByteAtAddr(ITG_ID,ITG_TMP_H)<<8) | ReadI2CByteAtAddr(ITG_ID,ITG_TMP_L),0,0); TxNextLine();
		TxString("GYRO_X    \t"); TxVal32((ReadI2CByteAtAddr(ITG_ID,ITG_GX_H)<<8) | ReadI2CByteAtAddr(ITG_ID,ITG_GX_L),0,0); TxNextLine();
		TxString("GYRO_Y    \t"); TxVal32((ReadI2CByteAtAddr(ITG_ID,ITG_GY_H)<<8) | ReadI2CByteAtAddr(ITG_ID,ITG_GY_L),0,0); TxNextLine();
		TxString("GYRO_Z    \t"); TxVal32((ReadI2CByteAtAddr(ITG_ID,ITG_GZ_H)<<8) | ReadI2CByteAtAddr(ITG_ID,ITG_GZ_L),0,0); TxNextLine();
		TxString("PWR_MGM   \t"); TxValH(ReadI2CByteAtAddr(ITG_ID,ITG_PWR_M)); TxNextLine();
		TxString("\r\nTest OK\r\n");
	}
	else
		TxString("\r\nTest FAILED\r\n");

} // GyroITG3200Test

#endif // TESTING







