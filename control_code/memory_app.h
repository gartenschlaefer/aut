/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			Memory-App-HeaderFile
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
	Header of xmA_Memory_app.c
* ------------------------------------------------------------------
*	Date:			    13.07.2011
* lastChanges:	27.12.2014
\**********************************************************************/



/* ==================================================================*
 * 						Enumeration
 * ==================================================================*/

typedef enum
{	ON_circ, OFF_circ,
  TIME_H_circ,	TIME_L_circ,	          //circulate
	ON_air,			  OFF_air,
	TIME_H_air,		TIME_L_air,			                //air
	TIME_setDown,																  //setDown
	ON_pumpOff,		PUMP_pumpOff,										//pumpOff
	ON_MIN_mud,		ON_SEC_mud,										  //mud
	MIN_H_druck,	MIN_L_druck,
	MAX_H_druck,	MAX_L_druck,		                //compressor
	ON_phosphor,	OFF_phosphor,									  //phoshor
	ON_inflowPump,	OFF_inflowPump,
	PUMP_inflowPump,	T_IP_off_h,			            //inflowPump
	SENSOR_inTank,	SENSOR_outTank,						  	//Sensors
	ALARM_temp,		ALARM_comp,			ALARM_sensor,		//Alarm
	CAL_H_druck,	CAL_L_druck,		CAL_Redo_on,		//Druck
	SONIC_H_LV,	  SONIC_L_LV,		SONIC_on,				//UltraSonic
	TANK_H_Circ,	TANK_L_Circ,
	TANK_H_O2,		TANK_L_O2,		                	//Tank
	TANK_H_MinP,	TANK_L_MinP,										//Tank
	TOUCH_X_max,	TOUCH_Y_max,
	TOUCH_X_min,		TOUCH_Y_min			              //Touch
}t_var;


typedef enum
{	DATA_day,		DATA_month,			DATA_year,			DATA_hour,
	DATA_minute,	DATA_H_O2,			DATA_L_O2,			DATA_ERROR
}t_data;


typedef enum
{	Write_o2,
	Write_Error,
	Write_Entry
}t_AutoEntry;



/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

/* ------------------------------------------------------------------*
 * 						Variables
 * ------------------------------------------------------------------*/

void 			MEM_EEPROM_WriteVarDefault	(void);
unsigned char 	MEM_EEPROM_ReadVar			(t_var var);
void 			MEM_EEPROM_WriteVar			(t_var var, unsigned char eeData);


/* ------------------------------------------------------------------*
 * 						Data
 * ------------------------------------------------------------------*/

unsigned char 	MEM_EEPROM_ReadData		(unsigned char page, unsigned char entry, t_data var);
void 			MEM_EEPROM_LoadData		(unsigned char entry, t_data byte,  unsigned char eeData);

void 			MEM_EEPROM_WriteAutoEntry	(int o2, unsigned char error, t_AutoEntry write);
void 			MEM_EEPROM_WriteManualEntry	(unsigned char h, unsigned char min, t_FuncCmd cmd);
void			MEM_EEPROM_WriteSetupEntry	(void);

void 			MEM_EEPROM_SetZero	(void);
void 			MEM_EEPROM_WriteVarDefault_Short(void);






/***********************************************************************
 *	END of xmA_Memory_app.h
 ***********************************************************************/

