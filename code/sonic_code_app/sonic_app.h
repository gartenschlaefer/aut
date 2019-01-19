/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  UltraSonic
*	Name:			    sonic_app.h
* ------------------------------------------------------------------
*	µ-Controler:	AT90CAN128/32
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	UltraSonic send and receive applications
* ------------------------------------------------------------------
*	Date:			    12.04.2015
* lastChanges:  17.05.2015
\**********************************************************************/



/* ==================================================================*
 * 						Defines
 * ==================================================================*/

//*-*Configuration

#define SONIC_TRIGGER	  230		  //Trigger Value
#define TSAFE           200     //Time2nextSend in ms
#define SVERS           0x11    //SortwareVersion x.x

//*-*Sonic Config - 125kHz
#define SONIC_OFFSET  33      //Offset in mm - 125kHz
#define SONIC_TYPE    0x01    //Indication of type
#define PULSEWIDTH_IC 128
#define PULSEWIDTH_OC 64
#define STVALUE       0x30    //StartValue172mm - 125kHz
//*/


/*-*Sonic Config - 75kHz
#define SONIC_OFFSET  57      //Offset in mm - 75kHz
#define SONIC_TYPE    0x00    //Indication of type
#define PULSEWIDTH_IC 212
#define PULSEWIDTH_OC 106
#define STVALUE       0x52   //StartValue300mm - 75kHz
//*/




/* ==================================================================*
 * 						FUNCTIONS - API
 * ==================================================================*/

t_UScmd Sonic_5Shots(t_FuncCmd cmd);
t_UScmd Sonic_OneShot(void);
t_UScmd Sonic_Temp(void);
int Sonic_Temp_Calc(int adc);

void Sonic_StartMeasurement(void);
int Sonic_Time2mm(unsigned char *p_time);






/**********************************************************************\
 * End of file
\**********************************************************************/
