/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Interception ICT
*	Name:			    AT24C-app-SourceFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	SourceFile for EEPROM AT24C512
* ------------------------------------------------------------------
*	Date:			    30.12.2014
* lastChanges:  09.02.2016
\**********************************************************************/


#include <avr/io.h>

#include "at24c_driver.h"
#include "at24c_app.h"



/* ==================================================================*
 * 						Memory Map
 * ==================================================================*/


/* ------------------------------------------------------------------*
 *            WriteVar
 * ------------------------------------------------------------------*/

void AT24C_WriteVar(t_EEvar var, unsigned char sData)
{
  switch(var)
  {
      case TEL1_0:  AT24C_WriteByte(0x0000, sData); break;
      case TEL1_1:  AT24C_WriteByte(0x0001, sData); break;
      case TEL1_2:  AT24C_WriteByte(0x0002, sData); break;
      case TEL1_3:  AT24C_WriteByte(0x0003, sData); break;

      case TEL1_4:  AT24C_WriteByte(0x0004, sData); break;
      case TEL1_5:  AT24C_WriteByte(0x0005, sData); break;
      case TEL1_6:  AT24C_WriteByte(0x0006, sData); break;
      case TEL1_7:  AT24C_WriteByte(0x0007, sData); break;

      case TEL1_8:  AT24C_WriteByte(0x0008, sData); break;
      case TEL1_9:  AT24C_WriteByte(0x0009, sData); break;
      case TEL1_A:  AT24C_WriteByte(0x000A, sData); break;
      case TEL1_B:  AT24C_WriteByte(0x000B, sData); break;

      case TEL1_C:  AT24C_WriteByte(0x000C, sData); break;
      case TEL1_D:  AT24C_WriteByte(0x000D, sData); break;
      case TEL1_E:  AT24C_WriteByte(0x000E, sData); break;
      case TEL1_F:  AT24C_WriteByte(0x000F, sData); break;

      case TEL2_0:  AT24C_WriteByte(0x0010, sData); break;
      case TEL2_1:  AT24C_WriteByte(0x0011, sData); break;
      case TEL2_2:  AT24C_WriteByte(0x0012, sData); break;
      case TEL2_3:  AT24C_WriteByte(0x0013, sData); break;

      case TEL2_4:  AT24C_WriteByte(0x0014, sData); break;
      case TEL2_5:  AT24C_WriteByte(0x0015, sData); break;
      case TEL2_6:  AT24C_WriteByte(0x0016, sData); break;
      case TEL2_7:  AT24C_WriteByte(0x0017, sData); break;

      case TEL2_8:  AT24C_WriteByte(0x0018, sData); break;
      case TEL2_9:  AT24C_WriteByte(0x0019, sData); break;
      case TEL2_A:  AT24C_WriteByte(0x001A, sData); break;
      case TEL2_B:  AT24C_WriteByte(0x001B, sData); break;

      case TEL2_C:  AT24C_WriteByte(0x001C, sData); break;
      case TEL2_D:  AT24C_WriteByte(0x001D, sData); break;
      case TEL2_E:  AT24C_WriteByte(0x001E, sData); break;
      case TEL2_F:  AT24C_WriteByte(0x001F, sData); break;

      default: break;
  }
}


/* ------------------------------------------------------------------*
 *            ReadVar
 * ------------------------------------------------------------------*/

unsigned char AT24C_ReadVar(t_EEvar var)
{
  switch(var)
  {
      case TEL1_0:  return AT24C_ReadByte(0x0000); break;
      case TEL1_1:  return AT24C_ReadByte(0x0001); break;
      case TEL1_2:  return AT24C_ReadByte(0x0002); break;
      case TEL1_3:  return AT24C_ReadByte(0x0003); break;

      case TEL1_4:  return AT24C_ReadByte(0x0004); break;
      case TEL1_5:  return AT24C_ReadByte(0x0005); break;
      case TEL1_6:  return AT24C_ReadByte(0x0006); break;
      case TEL1_7:  return AT24C_ReadByte(0x0007); break;

      case TEL1_8:  return AT24C_ReadByte(0x0008); break;
      case TEL1_9:  return AT24C_ReadByte(0x0009); break;
      case TEL1_A:  return AT24C_ReadByte(0x000A); break;
      case TEL1_B:  return AT24C_ReadByte(0x000B); break;

      case TEL1_C:  return AT24C_ReadByte(0x000C); break;
      case TEL1_D:  return AT24C_ReadByte(0x000D); break;
      case TEL1_E:  return AT24C_ReadByte(0x000E); break;
      case TEL1_F:  return AT24C_ReadByte(0x000F); break;

      case TEL2_0:  return AT24C_ReadByte(0x0010); break;
      case TEL2_1:  return AT24C_ReadByte(0x0011); break;
      case TEL2_2:  return AT24C_ReadByte(0x0012); break;
      case TEL2_3:  return AT24C_ReadByte(0x0013); break;

      case TEL2_4:  return AT24C_ReadByte(0x0014); break;
      case TEL2_5:  return AT24C_ReadByte(0x0015); break;
      case TEL2_6:  return AT24C_ReadByte(0x0016); break;
      case TEL2_7:  return AT24C_ReadByte(0x0017); break;

      case TEL2_8:  return AT24C_ReadByte(0x0018); break;
      case TEL2_9:  return AT24C_ReadByte(0x0019); break;
      case TEL2_A:  return AT24C_ReadByte(0x001A); break;
      case TEL2_B:  return AT24C_ReadByte(0x001B); break;

      case TEL2_C:  return AT24C_ReadByte(0x001C); break;
      case TEL2_D:  return AT24C_ReadByte(0x001D); break;
      case TEL2_E:  return AT24C_ReadByte(0x001E); break;
      case TEL2_F:  return AT24C_ReadByte(0x001F); break;

      default: break;
  }
  return 0;
}








/**********************************************************************\
 * End of at24c_app.c
\**********************************************************************/
