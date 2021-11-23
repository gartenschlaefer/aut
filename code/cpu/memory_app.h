// --
//  internal memory applications

// Include guard
#ifndef MEMORY_APP_H   
#define MEMORY_APP_H


/* ==================================================================*
 *            Defines
 * ==================================================================*/

// atxmega128a1
// num of pages: 64

// start and end page of data section
#define DATA_START_PAGE 2
#define DATA_END_PAGE 50

// 3 Manual pages
#define MANUAL_START_PAGE 2
#define MANUAL_END_PAGE 6

// 3 Setup pages
#define SETUP_START_PAGE 7
#define SETUP_END_PAGE 11

// 16 Auto pages
#define AUTO_START_PAGE 12
#define AUTO_END_PAGE 42

// amount of data pages
#define DATA_PAGE_NUM_MANUAL (3 - 1)
#define DATA_PAGE_NUM_SETUP (3 - 1)
#define DATA_PAGE_NUM_AUTO (16 - 1)


/* ==================================================================*
 *            Enumeration
 * ==================================================================*/

typedef enum
{ ON_circ, OFF_circ,
  TIME_H_circ,  TIME_L_circ,            //circulate
  ON_air,       OFF_air,
  TIME_H_air,   TIME_L_air,                     //air
  TIME_setDown,                                 //setDown
  ON_pumpOff,   PUMP_pumpOff,                   //pumpOff
  ON_MIN_mud,   ON_SEC_mud,                     //mud
  MIN_H_druck,  MIN_L_druck,
  MAX_H_druck,  MAX_L_druck,                    //compressor
  ON_phosphor,  OFF_phosphor,                   //phoshor
  ON_inflowPump,  OFF_inflowPump,
  PUMP_inflowPump,  T_IP_off_h,                 //inflowPump
  SENSOR_inTank,  SENSOR_outTank,               //Sensors
  ALARM_temp,   ALARM_comp,     ALARM_sensor,   //Alarm
  CAL_H_druck,  CAL_L_druck,    CAL_Redo_on,    //Druck
  SONIC_H_LV,   SONIC_L_LV,   SONIC_on,       //UltraSonic
  TANK_H_Circ,  TANK_L_Circ,
  TANK_H_O2,    TANK_L_O2,                      //Tank
  TANK_H_MinP,  TANK_L_MinP,                    //Tank
  TOUCH_X_max,  TOUCH_Y_max,
  TOUCH_X_min,    TOUCH_Y_min                   //Touch
}t_var;


typedef enum
{ DATA_day,   DATA_month,     DATA_year,      DATA_hour,
  DATA_minute,  DATA_H_O2,      DATA_L_O2,      DATA_ERROR
}t_data;


typedef enum
{ Write_o2,
  Write_Error,
  Write_Entry
}t_AutoEntry;


/* ==================================================================*
 *            FUNCTIONS - API
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Variables
 * ------------------------------------------------------------------*/

void MEM_EEPROM_WriteVarDefault(void);
unsigned char MEM_EEPROM_ReadVar(t_var var);
void MEM_EEPROM_WriteVar(t_var var, unsigned char eeData);


/* ------------------------------------------------------------------*
 *            Data
 * ------------------------------------------------------------------*/

unsigned char MEM_EEPROM_ReadData(unsigned char page, unsigned char entry, t_data var);
void MEM_EEPROM_LoadData(unsigned char entry, t_data byte,  unsigned char eeData);

void MEM_EEPROM_WriteAutoEntry(int o2, unsigned char error, t_AutoEntry write);
void MEM_EEPROM_WriteManualEntry(unsigned char h, unsigned char min, t_FuncCmd cmd);
void MEM_EEPROM_WriteSetupEntry(void);

void MEM_EEPROM_SetZero(void);
void MEM_EEPROM_WriteVarDefault_Short(void);

#endif

