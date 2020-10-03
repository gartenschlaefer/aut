/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    Memory-app-SourceFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	Functions to access Memory
* ------------------------------------------------------------------
*	Date:			    13.07.2011
* lastChanges:
\**********************************************************************/

#include<avr/io.h>

#include "defines.h"
#include "lcd_driver.h"

#include "eval_app.h"
#include "memory_app.h"
#include "mcp7941_driver.h"
#include "memory_func.h"


/* ==================================================================*
 * 						FUNCTIONS-Variables
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Write Var Default - Normal
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteVarDefault(void)
{
	MEM_EEPROM_WriteVar(ON_circ, 		  5);
	MEM_EEPROM_WriteVar(OFF_circ, 		25);
	MEM_EEPROM_WriteVar(ON_air, 			30);
	MEM_EEPROM_WriteVar(OFF_air, 			30);
	MEM_EEPROM_WriteVar(TIME_setDown, 60);
	MEM_EEPROM_WriteVar(ON_pumpOff, 	20);
	MEM_EEPROM_WriteVar(PUMP_pumpOff, 0);
	MEM_EEPROM_WriteVar(ON_MIN_mud, 	0);
	MEM_EEPROM_WriteVar(ON_SEC_mud, 	30);
	MEM_EEPROM_WriteVar(ON_phosphor, 	0);
	MEM_EEPROM_WriteVar(OFF_phosphor, 		55);
	MEM_EEPROM_WriteVar(ON_inflowPump, 		0);
	MEM_EEPROM_WriteVar(OFF_inflowPump,		30);
	MEM_EEPROM_WriteVar(PUMP_inflowPump,	0);
	MEM_EEPROM_WriteVar(SENSOR_inTank, 		0);
	MEM_EEPROM_WriteVar(SENSOR_outTank, 	0);
	MEM_EEPROM_WriteVar(TIME_H_circ,	0);
	MEM_EEPROM_WriteVar(TIME_L_circ,	240);
	MEM_EEPROM_WriteVar(TIME_H_air, 		0);
	MEM_EEPROM_WriteVar(TIME_L_air, 		240);
	MEM_EEPROM_WriteVar(MIN_H_druck,		0);
	MEM_EEPROM_WriteVar(MIN_L_druck,		35);
	MEM_EEPROM_WriteVar(MAX_H_druck, 		0);
	MEM_EEPROM_WriteVar(MAX_L_druck, 		250);
	MEM_EEPROM_WriteVar(ALARM_temp,			60);
	MEM_EEPROM_WriteVar(ALARM_comp, 		1);
	MEM_EEPROM_WriteVar(ALARM_sensor, 	1);
	MEM_EEPROM_WriteVar(CAL_H_druck, 		0);
	MEM_EEPROM_WriteVar(CAL_L_druck, 		0);
	MEM_EEPROM_WriteVar(SONIC_H_LV, 		0);
	MEM_EEPROM_WriteVar(SONIC_L_LV, 		0);
	MEM_EEPROM_WriteVar(SONIC_on, 			0);

	MEM_EEPROM_WriteVar(CAL_Redo_on, 		0);
	MEM_EEPROM_WriteVar(T_IP_off_h, 		0);

	MEM_EEPROM_WriteVar(TANK_H_MinP, 		0);
	MEM_EEPROM_WriteVar(TANK_L_MinP, 		120);
	MEM_EEPROM_WriteVar(TANK_H_Circ, 		0);
	MEM_EEPROM_WriteVar(TANK_L_Circ, 		10);
	MEM_EEPROM_WriteVar(TANK_H_O2, 			0);
	MEM_EEPROM_WriteVar(TANK_L_O2, 			15);
}



/* ------------------------------------------------------------------*
 * 						Write Var Default - Short
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteVarDefault_Short(void)
{
	MEM_EEPROM_WriteVar(ON_circ, 		1);
	MEM_EEPROM_WriteVar(OFF_circ, 		1);
	MEM_EEPROM_WriteVar(ON_air, 			1);
	MEM_EEPROM_WriteVar(OFF_air, 			1);
	MEM_EEPROM_WriteVar(TIME_setDown, 		1);
	MEM_EEPROM_WriteVar(ON_pumpOff, 		1);
	MEM_EEPROM_WriteVar(PUMP_pumpOff, 		0);
	MEM_EEPROM_WriteVar(ON_MIN_mud, 		0);
	MEM_EEPROM_WriteVar(ON_SEC_mud, 		20);
	MEM_EEPROM_WriteVar(ON_phosphor, 		1);
	MEM_EEPROM_WriteVar(OFF_phosphor, 		1);
	MEM_EEPROM_WriteVar(ON_inflowPump, 		2);
	MEM_EEPROM_WriteVar(OFF_inflowPump,		2);
	MEM_EEPROM_WriteVar(PUMP_inflowPump,	0);
	MEM_EEPROM_WriteVar(SENSOR_inTank, 		0);
	MEM_EEPROM_WriteVar(SENSOR_outTank, 	0);
	MEM_EEPROM_WriteVar(TIME_H_circ,	0);
	MEM_EEPROM_WriteVar(TIME_L_circ,	5);
	MEM_EEPROM_WriteVar(TIME_H_air, 		0);
	MEM_EEPROM_WriteVar(TIME_L_air, 		3);
	MEM_EEPROM_WriteVar(MIN_H_druck,		0);
	MEM_EEPROM_WriteVar(MIN_L_druck,		20);
	MEM_EEPROM_WriteVar(MAX_H_druck, 		0);
	MEM_EEPROM_WriteVar(MAX_L_druck, 		250);
	MEM_EEPROM_WriteVar(ALARM_temp,			60);
	MEM_EEPROM_WriteVar(ALARM_comp, 		1);
	MEM_EEPROM_WriteVar(ALARM_sensor, 		1);
	MEM_EEPROM_WriteVar(CAL_H_druck, 		0);
	MEM_EEPROM_WriteVar(CAL_L_druck, 		0);
	MEM_EEPROM_WriteVar(SONIC_H_LV, 		0);
	MEM_EEPROM_WriteVar(SONIC_L_LV, 		0);
	MEM_EEPROM_WriteVar(SONIC_on, 			0);

	MEM_EEPROM_WriteVar(CAL_Redo_on, 		0);
	MEM_EEPROM_WriteVar(T_IP_off_h, 		0);

	MEM_EEPROM_WriteVar(TANK_H_MinP, 		0);
	MEM_EEPROM_WriteVar(TANK_L_MinP, 		120);
	MEM_EEPROM_WriteVar(TANK_H_Circ, 		0);
	MEM_EEPROM_WriteVar(TANK_L_Circ, 		10);
	MEM_EEPROM_WriteVar(TANK_H_O2, 			0);
	MEM_EEPROM_WriteVar(TANK_L_O2, 			15);
}



/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_ReadVar
 * --------------------------------------------------------------
 *	Reads Variable from EEPROM	PageSize: 32Bytes
 * ------------------------------------------------------------------*/

unsigned char MEM_EEPROM_ReadVar(t_var var)
{
	switch(var)
	{
		case ON_circ:		    return (MEM_EEPROM_Read(1,0));	break;
		case OFF_circ:		  return (MEM_EEPROM_Read(1,1));	break;
		case ON_air:			  return (MEM_EEPROM_Read(1,2));	break;
		case OFF_air:			  return (MEM_EEPROM_Read(1,3));	break;
		case TIME_setDown: 	return (MEM_EEPROM_Read(1,4));	break;
		case ON_pumpOff:		return (MEM_EEPROM_Read(1,5));	break;
		case PUMP_pumpOff:	return (MEM_EEPROM_Read(1,6));	break;
		case ON_MIN_mud:		return (MEM_EEPROM_Read(1,7));	break;
		case ON_SEC_mud:		return (MEM_EEPROM_Read(1,8));	break;
		case ON_phosphor:		return (MEM_EEPROM_Read(1,9));	break;
		case OFF_phosphor:		return (MEM_EEPROM_Read(1,10));	break;
		case ON_inflowPump:		return (MEM_EEPROM_Read(1,11));	break;
		case OFF_inflowPump:	return (MEM_EEPROM_Read(1,12));	break;
		case PUMP_inflowPump:	return (MEM_EEPROM_Read(1,13));	break;
		case SENSOR_inTank:		return (MEM_EEPROM_Read(1,14));	break;
		case SENSOR_outTank:	return (MEM_EEPROM_Read(1,15));	break;
		case TIME_H_circ:   return (MEM_EEPROM_Read(1,16));	break;
		case TIME_L_circ:	  return (MEM_EEPROM_Read(1,17));	break;
		case TIME_H_air:		return (MEM_EEPROM_Read(1,18));	break;
		case TIME_L_air:		return (MEM_EEPROM_Read(1,19));	break;
		case MIN_H_druck:		return (MEM_EEPROM_Read(1,20));	break;
		case MIN_L_druck:		return (MEM_EEPROM_Read(1,21));	break;
		case MAX_H_druck:		return (MEM_EEPROM_Read(1,22));	break;
		case MAX_L_druck:		return (MEM_EEPROM_Read(1,23));	break;
		case ALARM_temp:		return (MEM_EEPROM_Read(1,24));	break;
		case ALARM_comp:		return (MEM_EEPROM_Read(1,25));	break;
		case ALARM_sensor:	return (MEM_EEPROM_Read(1,26));	break;
		case CAL_H_druck:		return (MEM_EEPROM_Read(1,27));	break;
		case CAL_L_druck:		return (MEM_EEPROM_Read(1,28));	break;
		case SONIC_H_LV:		return (MEM_EEPROM_Read(1,29));	break;
		case SONIC_L_LV:		return (MEM_EEPROM_Read(1,30));	break;
		case SONIC_on:			return (MEM_EEPROM_Read(1,31));	break;

		case TOUCH_X_max:		return (MEM_EEPROM_Read(0,0));	break;
		case TOUCH_Y_max:		return (MEM_EEPROM_Read(0,1));	break;
		case TOUCH_X_min:		return (MEM_EEPROM_Read(0,2));	break;
		case TOUCH_Y_min:		return (MEM_EEPROM_Read(0,3));	break;
		case TANK_H_MinP:		return (MEM_EEPROM_Read(0,4));	break;
		case TANK_L_MinP:		return (MEM_EEPROM_Read(0,5));	break;
		case TANK_H_Circ:		return (MEM_EEPROM_Read(0,6));	break;
		case TANK_L_Circ:		return (MEM_EEPROM_Read(0,7));	break;
		case TANK_H_O2:			return (MEM_EEPROM_Read(0,8));	break;
		case TANK_L_O2:			return (MEM_EEPROM_Read(0,9));	break;
    case CAL_Redo_on:		return (MEM_EEPROM_Read(0,10));	break;
		case T_IP_off_h:		return (MEM_EEPROM_Read(0,11));	break;

		default: 	break;
	}
	return 0;
}



/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_WriteVar
 * --------------------------------------------------------------
 *	Writes Variable to EEPROM, configure Memory Space
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteVar(t_var var, unsigned char eeData)
{
	switch(var)
	{
		case ON_circ:		    MEM_EEPROM_WriteByte(1,0,eeData);	break;
		case OFF_circ:		  MEM_EEPROM_WriteByte(1,1,eeData);	break;
		case ON_air:			  MEM_EEPROM_WriteByte(1,2,eeData);	break;
		case OFF_air:			  MEM_EEPROM_WriteByte(1,3,eeData);	break;
		case TIME_setDown:	MEM_EEPROM_WriteByte(1,4,eeData);	break;
		case ON_pumpOff:		MEM_EEPROM_WriteByte(1,5,eeData);	break;
		case PUMP_pumpOff:	MEM_EEPROM_WriteByte(1,6,eeData);	break;
		case ON_MIN_mud:		MEM_EEPROM_WriteByte(1,7,eeData);	break;
		case ON_SEC_mud:		MEM_EEPROM_WriteByte(1,8,eeData);	break;
		case ON_phosphor:		MEM_EEPROM_WriteByte(1,9,eeData);	break;
		case OFF_phosphor:		MEM_EEPROM_WriteByte(1,10,eeData);	break;
		case ON_inflowPump:		MEM_EEPROM_WriteByte(1,11,eeData);	break;
		case OFF_inflowPump:	MEM_EEPROM_WriteByte(1,12,eeData);	break;
		case PUMP_inflowPump:	MEM_EEPROM_WriteByte(1,13,eeData);	break;
		case SENSOR_inTank:		MEM_EEPROM_WriteByte(1,14,eeData);	break;
		case SENSOR_outTank:	MEM_EEPROM_WriteByte(1,15,eeData);	break;
		case TIME_H_circ:	  MEM_EEPROM_WriteByte(1,16,eeData);	break;
		case TIME_L_circ:	  MEM_EEPROM_WriteByte(1,17,eeData);	break;
		case TIME_H_air:		MEM_EEPROM_WriteByte(1,18,eeData);	break;
		case TIME_L_air:		MEM_EEPROM_WriteByte(1,19,eeData);	break;
		case MIN_H_druck:		MEM_EEPROM_WriteByte(1,20,eeData);	break;
		case MIN_L_druck:		MEM_EEPROM_WriteByte(1,21,eeData);	break;
		case MAX_H_druck:		MEM_EEPROM_WriteByte(1,22,eeData);	break;
		case MAX_L_druck:		MEM_EEPROM_WriteByte(1,23,eeData);	break;
		case ALARM_temp:		MEM_EEPROM_WriteByte(1,24,eeData);	break;
		case ALARM_comp:		MEM_EEPROM_WriteByte(1,25,eeData);	break;
		case ALARM_sensor:	MEM_EEPROM_WriteByte(1,26,eeData);	break;
		case CAL_H_druck:		MEM_EEPROM_WriteByte(1,27,eeData);	break;
		case CAL_L_druck:		MEM_EEPROM_WriteByte(1,28,eeData);	break;
		case SONIC_H_LV:		MEM_EEPROM_WriteByte(1,29,eeData);	break;
		case SONIC_L_LV:		MEM_EEPROM_WriteByte(1,30,eeData);	break;
		case SONIC_on:			MEM_EEPROM_WriteByte(1,31,eeData);	break;

		case TOUCH_X_max:		MEM_EEPROM_WriteByte(0,0,eeData);	  break;
		case TOUCH_Y_max:		MEM_EEPROM_WriteByte(0,1,eeData);	  break;
		case TOUCH_X_min:		MEM_EEPROM_WriteByte(0,2,eeData);	  break;
		case TOUCH_Y_min:		MEM_EEPROM_WriteByte(0,3,eeData);   break;
		case TANK_H_MinP:		MEM_EEPROM_WriteByte(0,4,eeData);	  break;
		case TANK_L_MinP:		MEM_EEPROM_WriteByte(0,5,eeData);	  break;
		case TANK_H_Circ:		MEM_EEPROM_WriteByte(0,6,eeData);	  break;
		case TANK_L_Circ:		MEM_EEPROM_WriteByte(0,7,eeData);	  break;
		case TANK_H_O2:			MEM_EEPROM_WriteByte(0,8,eeData);	  break;
		case TANK_L_O2:			MEM_EEPROM_WriteByte(0,9,eeData);	  break;
		case CAL_Redo_on:		MEM_EEPROM_WriteByte(0,10,eeData);	break;
		case T_IP_off_h:		MEM_EEPROM_WriteByte(0,11,eeData);	break;
		default:													                      break;
	}
}




/* ==================================================================*
 * 						Data Variables
 * ==================================================================*/

/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_ReadVar
 * --------------------------------------------------------------
 *	Reads Variable from EEPROM	PageSize: 32Bytes
 * ------------------------------------------------------------------*/

unsigned char MEM_EEPROM_ReadData(unsigned char page,
unsigned char entry, t_data var)
{
	if(entry>4) entry= 4;
	switch(var)
	{
			case DATA_day:
			  return	(MEM_EEPROM_Read(page, (0+(entry*8))));	break;
			case DATA_month:
			  return	(MEM_EEPROM_Read(page, (1+(entry*8))));	break;
			case DATA_year:
			  return	(MEM_EEPROM_Read(page, (2+(entry*8))));	break;
			case DATA_hour:
			  return	(MEM_EEPROM_Read(page, (3+(entry*8))));	break;
			case DATA_minute:
			  return	(MEM_EEPROM_Read(page, (4+(entry*8))));	break;
			case DATA_H_O2:
			  return	(MEM_EEPROM_Read(page, (5+(entry*8))));	break;
			case DATA_L_O2:
			  return	(MEM_EEPROM_Read(page, (6+(entry*8))));	break;
			case DATA_ERROR:
			  return	(MEM_EEPROM_Read(page, (7+(entry*8))));	break;
			default:															            break;
	}
	return 0;
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_WriteAutoEntry
 * --------------------------------------------------------------
 *	Safes Variables or write Auto Entry to EEPROM
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteAutoEntry(int o2, unsigned char error, t_AutoEntry write)
{
	unsigned char i = 0;
	unsigned char data[8] = {0x00};

	unsigned char page = 0;
	unsigned char entry = 0;
	unsigned char null = 0;

	static int s_o2=0;
	static unsigned char s_err=0;

	unsigned char *p_null = Eval_Memory_NoEntry(Auto);
	unsigned char *p_old = Eval_Memory_OldestEntry(Auto);

	switch(write)
	{
		case Write_Error:	s_err |= error;		break;
		case Write_o2: s_o2 = o2;			      break;

		case Write_Entry:
				data[0]= MCP7941_ReadTime(TIC_DATE);	//Day
				data[1]= MCP7941_ReadTime(TIC_MONTH); //Month
				data[2]= MCP7941_ReadTime(TIC_YEAR);	//Year
				data[3]= MCP7941_ReadTime(TIC_HOUR);	//Hour
				data[4]= MCP7941_ReadTime(TIC_MIN);	  //Min
				data[6]= (s_o2 & 0x00FF);							//O2 Low
				data[5]= ((s_o2>>8) & 0x00FF);				//O2 High
				data[7]= s_err;										//Error Code
				s_err= 0;
				//--------------------------------------------UpdatePointer
				page = *p_null;
				p_null++;
				entry= *p_null;
				p_null++;
				null= *p_null;

				if(!null)
				{
					page = *p_old;
					p_old++;
					entry= *p_old;
				}

				//--------------------------------------------Write-Entry
				if(page < AUTO_START_PAGE)	page = AUTO_START_PAGE;		//Write Protection
				for(i=0; i<8; i++)
				{
					MEM_EEPROM_LoadData(entry, i, data[i]);
				}
				MEM_EEPROM_PageEraseWrite(page);		break;

			default:									break;
		}
}


/* ------------------------------------------------------------------*
 * 						ManualEntry
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteManualEntry(unsigned char h, unsigned char min, t_FuncCmd cmd)
{
	unsigned char i=0;
	unsigned char data[7]={0x00};

	unsigned char page =0;
	unsigned char entry=0;
	unsigned char null=0;

	static unsigned char entryH=0;
	static unsigned char entryMin=0;

	unsigned char *p_null= 	Eval_Memory_NoEntry(Manual);
	unsigned char *p_old=	Eval_Memory_OldestEntry(Manual);

	if(cmd ==_saveValue)
	{
		entryH=		(((h>>4) & 0x0F)*10 + (h & 0x0F));
		entryMin= (((min>>4) & 0x0F)*10 + (min & 0x0F));
	}
	else
	{
    data[0]= MCP7941_ReadTime(TIC_DATE);	//Day
    data[1]= MCP7941_ReadTime(TIC_MONTH); //Month
    data[2]= MCP7941_ReadTime(TIC_YEAR);	//Year
    data[3]= entryH;						          //Hour1
		data[4]= entryMin;						        //Minute1
    data[5]= MCP7941_ReadTime(TIC_HOUR);	//Hour2
    data[6]= MCP7941_ReadTime(TIC_MIN);	  //Min2

		//-------------------------------------------Update-Var-Read-Pointer
		page = *p_null;
		p_null++;
		entry= *p_null;
		p_null++;
		null= *p_null;

		if(!null)
		{
			page = *p_old;
			p_old++;
			entry= *p_old;
		}

		//--------------------------------------------Write-Entry
		if(page < MANUAL_START_PAGE)	page = MANUAL_START_PAGE;		//Write Protection
		for(i=0; i<7; i++)
		{
			MEM_EEPROM_LoadData(entry, i, data[i]);
		}
		MEM_EEPROM_PageEraseWrite(page);
	}
}


/* ------------------------------------------------------------------*
 * 						SetupEntry
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteSetupEntry(void)
{
	unsigned char i=0;
	unsigned char data[7]={0x00};

	unsigned char page =0;
	unsigned char entry=0;
	unsigned char null=0;

	unsigned char *p_null= 	Eval_Memory_NoEntry(Setup);
	unsigned char *p_old=	Eval_Memory_OldestEntry(Setup);

  data[0]= MCP7941_ReadTime(TIC_DATE);	//Day
  data[1]= MCP7941_ReadTime(TIC_MONTH); //Month
  data[2]= MCP7941_ReadTime(TIC_YEAR);	//Year
  data[3]= MCP7941_ReadTime(TIC_HOUR);	//Hour
  data[4]= MCP7941_ReadTime(TIC_MIN);	  //Min

	//-------------------------------------------Update-Variables
	page = *p_null;
	p_null++;
	entry= *p_null;
	p_null++;
	null= *p_null;

	if(!null)
	{
		page = *p_old;
		p_old++;
		entry= *p_old;
	}

	//--------------------------------------------Write-Entry
	if(page < SETUP_START_PAGE)	page = SETUP_START_PAGE;		//Write Protection
	for(i = 0; i < 6; i++)
	{
		MEM_EEPROM_LoadData(entry, i, data[i]);
	}
	MEM_EEPROM_PageEraseWrite(page);
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_SetZero
 * --------------------------------------------------------------
 *	Writes all EEPROM Data 0x00
 * ------------------------------------------------------------------*/

void MEM_EEPROM_SetZero(void)
{
	unsigned char i = 0;
	unsigned char e = 0;
	unsigned char eep = 0;
	unsigned char data[8] = {0x00};

	// pages
	for(eep = DATA_START_PAGE; eep <= DATA_END_PAGE; eep++)
	{
		// entries
		for(e = 0; e < 4; e++)
		{
			// bytes
			for(i = 0; i < 8; i++)
			{
				MEM_EEPROM_LoadData(e, i, data[i]);
			}
		}
		MEM_EEPROM_PageEraseWrite(eep);
	}
}


/*-------------------------------------------------------------------*
 * 	MEM_EEPROM_LoadData
 * --------------------------------------------------------------
 *	Load eeData to EEPROM Buffer, entry select one of 4 DataSections
 *	8Byte Data Byte == 1 Display Output		Page
 * ------------------------------------------------------------------*/

void MEM_EEPROM_LoadData(unsigned char entry, t_data byte,  unsigned char eeData)
{
	switch(entry)
	{
		case 0:
      switch(byte)
      {
        case DATA_day:		MEM_EEPROM_LoadPageBuffer(0, eeData);	break;
        case DATA_month:	MEM_EEPROM_LoadPageBuffer(1, eeData);	break;
        case DATA_year:		MEM_EEPROM_LoadPageBuffer(2, eeData);	break;
        case DATA_hour:		MEM_EEPROM_LoadPageBuffer(3, eeData);	break;
        case DATA_minute:	MEM_EEPROM_LoadPageBuffer(4, eeData);	break;
        case DATA_H_O2:		MEM_EEPROM_LoadPageBuffer(5, eeData);	break;
        case DATA_L_O2:		MEM_EEPROM_LoadPageBuffer(6, eeData);	break;
        case DATA_ERROR:	MEM_EEPROM_LoadPageBuffer(7, eeData);	break;
        default:											break;
      }																break;

		case 1:
      switch(byte)
      {
        case DATA_day:		MEM_EEPROM_LoadPageBuffer(8, eeData);	break;
        case DATA_month:	MEM_EEPROM_LoadPageBuffer(9, eeData);	break;
        case DATA_year:		MEM_EEPROM_LoadPageBuffer(10, eeData);	break;
        case DATA_hour:		MEM_EEPROM_LoadPageBuffer(11, eeData);	break;
        case DATA_minute:	MEM_EEPROM_LoadPageBuffer(12, eeData);	break;
        case DATA_H_O2:		MEM_EEPROM_LoadPageBuffer(13, eeData);	break;
        case DATA_L_O2:		MEM_EEPROM_LoadPageBuffer(14, eeData);	break;
        case DATA_ERROR:	MEM_EEPROM_LoadPageBuffer(15, eeData);	break;
        default:											break;
      }																break;

		case 2:
      switch(byte)
      {
        case DATA_day:		MEM_EEPROM_LoadPageBuffer(16, eeData);	break;
        case DATA_month:	MEM_EEPROM_LoadPageBuffer(17, eeData);	break;
        case DATA_year:		MEM_EEPROM_LoadPageBuffer(18, eeData);	break;
        case DATA_hour:		MEM_EEPROM_LoadPageBuffer(19, eeData);	break;
        case DATA_minute:	MEM_EEPROM_LoadPageBuffer(20, eeData);	break;
        case DATA_H_O2:		MEM_EEPROM_LoadPageBuffer(21, eeData);	break;
        case DATA_L_O2:		MEM_EEPROM_LoadPageBuffer(22, eeData);	break;
        case DATA_ERROR:	MEM_EEPROM_LoadPageBuffer(23, eeData);	break;
        default:											break;
      }																break;

		case 3:
		  switch(byte)
      {
        case DATA_day:		MEM_EEPROM_LoadPageBuffer(24, eeData);	break;
        case DATA_month:	MEM_EEPROM_LoadPageBuffer(25, eeData);	break;
        case DATA_year:		MEM_EEPROM_LoadPageBuffer(26, eeData);	break;
        case DATA_hour:		MEM_EEPROM_LoadPageBuffer(27, eeData);	break;
        case DATA_minute:	MEM_EEPROM_LoadPageBuffer(28, eeData);	break;
        case DATA_H_O2:		MEM_EEPROM_LoadPageBuffer(29, eeData);	break;
        case DATA_L_O2:		MEM_EEPROM_LoadPageBuffer(30, eeData);	break;
        case DATA_ERROR:	MEM_EEPROM_LoadPageBuffer(31, eeData);	break;
        default:											break;
      }																break;
	}
}





/***********************************************************************
 *	End of file
 ***********************************************************************/
