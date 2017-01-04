/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    UltraSonic-Application-HeaderFile
* ------------------------------------------------------------------
*	µC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	UltraSonic Applications
* ------------------------------------------------------------------
*	Date:			    21.05.2014
* lastChanges:  17.05.2015
\**********************************************************************/


/* ==================================================================*
 * 						Defines
 * ==================================================================*/

#define		DISA	(1<<0)		//Data Distance available
#define		TEMPA	(1<<1)		//Temp available
#define		DERR	(1<<2)		//DistanceError
#define		TERR	(1<<3)		//TempError

#define D_LIM 50



/* ==================================================================*
 * 						Enumerations
 * ==================================================================*/

typedef enum
{
	US_wait, 	US_exe,		US_reset,
	T_ini,		D1_ini,		D5_ini,			//Init
	T_wo,		  D1_wo,		D5_wo,			//Working
	T_err,		D_err,						    //Error
	R_sreg,		R_dreg,		R_treg			//ReadReg
}t_US;



/* ==================================================================*
 * 						FUNCTIONS API
 * ==================================================================*/

int Sonic_App(t_US cmd);
void Sonic_Data_Shot(void);
void Sonic_Data_Auto(void);
void Sonic_Data_Boot(t_FuncCmd cmd);
void Sonic_Data_BootRead(void);
void Sonic_Data_BootWrite(void);

t_page Sonic_ReadTank(t_page page, t_FuncCmd cmd);
t_page Sonic_ChangePage(t_page page, int sonic);
int Sonic_LevelCal(t_FuncCmd cmd);
unsigned char Sonic_sVersion(void);



/**********************************************************************\
 * End of file
\**********************************************************************/
