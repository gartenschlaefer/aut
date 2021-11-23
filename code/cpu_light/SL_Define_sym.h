/*********************************************************************\
*	Author:			Red_Calcifer
*	Projekt:		Display-Symbols-HeaderFile									
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio 4.18 mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	This File contains the Enums of SL_sym.h
* ------------------------------------------------------------------														
*	Date:			18.06.2011  	
* 	lastChanges:	20.06.2011									
\**********************************************************************/


/* ===================================================================*
 * 						Enumerations
 * ===================================================================*/


/* -------------------------------------------------------------------*
 * 						Function Commands
 * -------------------------------------------------------------------*/

typedef enum 
{	_exe, 		_init, 			_reset, 		_add, 		_calc,
	_write, 	_saveValue,		_off,			_on,		_normal,
	_start,		_count,			_old,			_new,		_save,	
	_set,		_outSet,		_state,			_sym,		_error,
	_enabled,	_disabled,		_stop,			_clean,		_clear
}t_FuncCmd;


/* -------------------------------------------------------------------*
 * 						Display Pages
 * -------------------------------------------------------------------*/

typedef enum 
{	AutoPage, 			ManualPage,			SetupPage,		DataPage,			
	PinManual,			PinSetup, 			AutoZone,

	AutoSetDown,		AutoPumpOff,		AutoMud,		AutoCirculate,
	AutoAir,			AutoAirOff,			AutoCirculateOff,
			
	SetupMain,			SetupCirculate,		SetupAir, 			SetupSetDown,		
	SetupPumpOff,		SetupMud,			SetupCompressor,	SetupPhosphor,	
	SetupInflowPump,	SetupCal,			SetupCalPressure,	SetupAlarm,			
	SetupWatch,			SetupZone,			

	ManualMain,			ManualPumpOff_On,
	ManualCirculate,	ManualAir, 			ManualSetDown,		ManualPumpOff,	
	ManualMud,			ManualCompressor,	ManualPhosphor,		ManualInflowPump,

	DataMain,			DataAuto,			DataManual,			DataSetup,	
	
	ErrorMPX
}t_page;



/* -------------------------------------------------------------------*
 * 						EEPROM Pages
 * -------------------------------------------------------------------*/

typedef enum 
{	Page1,				Page2,				Page3,			Page4,	
	Page5,				Page6,				Page7,			Page8,
}t_eeDataPage;




/* -------------------------------------------------------------------*
 * 						Symbols
 * -------------------------------------------------------------------*/
 
typedef enum 
{	n_pumpOff, 		n_mud, 			n_inflowPump,	n_pump2,
	p_pumpOff, 		p_mud, 			p_inflowPump,	p_pump2  
}t_Symbols_35x23;


typedef enum 
{ 	n_setDown,	 	n_alarm, 		n_air,			n_sensor,	n_watch,		
	n_compressor,	n_circulate,	n_cal,			n_zone,		n_level,
	p_setDown,	 	p_alarm, 		p_air,			p_sensor,	p_watch,		
	p_compressor,	p_circulate,	p_cal,			p_zone,		p_level
}t_Symbols_29x17;


typedef enum 
{ 	n_phosphor,	 	n_pump, 		n_esc,			n_plus,
	n_minus,		n_arrowUp,		n_arrowDown,	n_ok,		n_grad,
	p_phosphor,	 	p_pump, 		p_esc,			p_plus,
	p_minus,		p_arrowUp,		p_arrowDown,	p_ok,
	p_line,			p_grad	
}t_Symbols_19x24;


typedef enum 
{ 	frame, 			p_escape, 		p_del, 
	black, 			n_escape,		n_del
}t_pinSymbols;


typedef enum 
{ 	Auto, 			Manual, 		Setup, 			Data
}t_textSymbols;

typedef enum 
{ 	Circulate, 
	Air
}t_TimeAir;



/* -------------------------------------------------------------------*
 * 						Setup Page Symbols
 * -------------------------------------------------------------------*/

typedef enum 
{ 	sn_circulate,	sn_air,			sn_setDown,	 	sn_pumpOff,
	sn_mud,			sn_compressor,	sn_phosphor,	sn_inflowPump,
	sn_cal,			sn_alarm, 		sn_watch,		sn_zone, 
	sp_circulate,	sp_air,			sp_setDown,	 	sp_pumpOff,
	sp_mud,			sp_compressor,	sp_phosphor,	sp_inflowPump,
	sp_cal,			sp_alarm, 		sp_watch,		sp_zone,
}t_SetupSym;



/* -------------------------------------------------------------------*
 * 						Control Buttons
 * -------------------------------------------------------------------*/

typedef enum 
{ 	sn_plus,		sn_minus,		sn_esc,		sn_ok,
	sp_plus,		sp_minus,		sp_esc,		sp_ok
}t_CtrlButtons;



/* -------------------------------------------------------------------*
 * 						Date Time Page
 * -------------------------------------------------------------------*/

typedef enum 
{ 	n_h,		n_min,		n_day,		n_month,		n_year,
	p_h,		p_min,		p_day,		p_month,		p_year
}t_DateTime;






/**********************************************************************\
 * End of Display_Symbols.h
\**********************************************************************/



