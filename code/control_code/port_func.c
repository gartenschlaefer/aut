/*********************************************************************\
*	Author:			  Christian Walter
* ------------------------------------------------------------------
* Project:		  Control Interception ICT
*	Name:			    PORT+Interrupt
* ------------------------------------------------------------------
*	uC:        	  ATxmega128A1
*	Compiler:		  avr-gcc (WINAVR 2010)
*	Description:
* ------------------------------------------------------------------
*	PORT Functions for ICT
* ------------------------------------------------------------------
*	Date:			    27.05.2011
* lastChanges:	15.10.2014
\**********************************************************************/

#include<avr/io.h>
#include<avr/interrupt.h>


#include "defines.h"
#include "lcd_driver.h"
#include "lcd_app.h"
#include "memory_app.h"
#include "output_app.h"
#include "mcp9800_driver.h"
#include "adc_func.h"
#include "tc_func.h"
#include "port_func.h"
#include "basic_func.h"
#include "modem_driver.h"
#include "error_func.h"


/* ==================================================================*
 * 						FUNCTIONS Init
 * ==================================================================*/

void PORT_Init(void)
{
	// PORT Direction
	P_OPTO.DIRCLR = 	PIN3_bm | OC1 | OC2 | OC3 | OC4;
	P_VENTIL.DIR = 	0xFF;
	P_RELAIS.DIR = 	0xFF;

  // Pins PULL UP
	PORTCFG.MPCMASK = 0xFF;
  P_OPTO.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
  // FirmwareUpdate PullUp
	PORTD.PIN5CTRL= PORT_OPC_WIREDANDPULL_gc;

	// Interrupts, MediumLevel, LowLevel Interrupts
	PMIC.CTRL =	PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
}

void PORT_SoftwareRst(void)
{
	//Protection
	CCP = 0xD8;
	RST.CTRL= RST_SWRST_bm;
}

void PORT_Bootloader(void)
{
	if(!(PORTD.IN & PIN5_bm))
	{
		LCD_Clean();
		LCD_WriteStringFont(1, 1, "Bootloader-Modus");
		asm volatile("jmp 0x20000");
	}
}



/* ==================================================================*
 * 						FUNCTIONS Buzzer
 * ==================================================================*/

void PORT_Buzzer(t_FuncCmd cmd)
{
	static int count = 0;
	static t_FuncCmd state = _off;

	switch(cmd)
	{
		case _error:
      //***DisableBuzzer
		  if(DEBUG) return;
		  state = _error;				break;	//Error

		case _off:
		  state = _off;
      PORTD.DIRCLR =	PIN6_bm;
      PORTD.OUTCLR =	PIN6_bm;		break;

		case _exe:
		  if(state == _error)
      {
        count++;
        if(count > 400)
        {
          PORTD.DIRCLR =	PIN6_bm;
          PORTD.OUTCLR =	PIN6_bm;
        }
        if(count > 2000)
        {
          count = 0;
          PORTD.DIRSET =	PIN6_bm;
          PORTD.OUTSET =	PIN6_bm;
        }
      }

		default:									break;
	}

}




/* ==================================================================*
 * 						FUNCTIONS Ventilator
 * ==================================================================*/

void PORT_Ventilator(void)
{
	unsigned char temp = 0;
	unsigned char hystOn = 0;
	unsigned char hystOff = 0;

	temp = MCP9800_PlusTemp();                        //ReadPlusTemp
	hystOn =	(MEM_EEPROM_ReadVar(ALARM_temp) - 15);  //HysteresisON
	hystOff =	(MEM_EEPROM_ReadVar(ALARM_temp) - 20);  //HysteresisOFF

	if(!(temp & 0x80))
	{
		if(temp > hystOn)		PORT_RelaisSet(R_VENTILATOR);
		if(temp < hystOff)	PORT_RelaisClr(R_VENTILATOR);
	}
	else PORT_RelaisClr(R_VENTILATOR);
}




/* ==================================================================*
 * 						FUNCTIONS Ventil
 * ==================================================================*/

void PORT_Ventil(t_ventil ventil)
{
	Watchdog_Restart();

	switch(ventil)
	{
		case OPEN_Reserve:
		  P_VENTIL.OUTSET= O_RES;
      TCC0_wait_sec(3);
      P_VENTIL.OUTCLR= O_RES;			break;

		case CLOSE_Reserve:
		  P_VENTIL.OUTSET= C_RES;
      TCC0_wait_3s5();
      P_VENTIL.OUTCLR= C_RES;			break;

		case OPEN_MudPump:
		  P_VENTIL.OUTSET= O_MUD;
      TCC0_wait_sec(3);
      P_VENTIL.OUTCLR= O_MUD;			break;

		case CLOSE_MudPump:
		  P_VENTIL.OUTSET= C_MUD;
      TCC0_wait_3s5();
      P_VENTIL.OUTCLR= C_MUD;			break;

		case OPEN_Air:
      P_VENTIL.OUTSET= O_AIR;
      TCC0_wait_sec(3);
      P_VENTIL.OUTCLR= O_AIR;			break;

		case CLOSE_Air:
		  P_VENTIL.OUTSET= C_AIR;
      TCC0_wait_3s5();
      P_VENTIL.OUTCLR= C_AIR;			break;

		case OPEN_ClearWater:
		  P_VENTIL.OUTSET= O_CLRW;
      TCC0_wait_sec(3);
      P_VENTIL.OUTCLR= O_CLRW;		break;

		case CLOSE_ClearWater:
		  P_VENTIL.OUTSET= C_CLRW;
      TCC0_wait_3s5();
      P_VENTIL.OUTCLR= C_CLRW;		break;

		case CLOSE_IPAir:
		  P_VENTIL.OUTSET= C_AIR | C_RES;
      TCC0_wait_3s5();
      P_VENTIL.OUTCLR= C_AIR | C_RES;	break;

		case OFF_Ventil:
		  P_VENTIL.OUTSET= 0x00;
      P_VENTIL.OUTCLR= 0x00;			break;
		default:											break;
	}
	Watchdog_Restart();
}



/* ------------------------------------------------------------------*
 * 						Ventil all open
 * ------------------------------------------------------------------*/

void PORT_Ventil_AllOpen(void)
{
	Watchdog_Restart();
	P_VENTIL.OUTSET= O_RES;
	TCC0_wait_ms(500);
	P_VENTIL.OUTSET= O_MUD;
	TCC0_wait_ms(500);
	P_VENTIL.OUTSET= O_AIR;
	TCC0_wait_ms(500);
	P_VENTIL.OUTSET= O_CLRW;

	TCC0_wait_sec(1);
	TCC0_wait_ms(500);

	P_VENTIL.OUTCLR= O_RES;
	TCC0_wait_ms(500);
	P_VENTIL.OUTCLR= O_MUD;
	TCC0_wait_ms(500);
	P_VENTIL.OUTCLR= O_AIR;
	TCC0_wait_ms(500);
	P_VENTIL.OUTCLR= O_CLRW;
	TCC0_wait_ms(500);
	Watchdog_Restart();
}


/* ------------------------------------------------------------------*
 * 						Ventil all close
 * ------------------------------------------------------------------*/

void PORT_Ventil_AllClose(void)
{
	Watchdog_Restart();
	P_VENTIL.OUTSET= C_RES;
	TCC0_wait_ms(500);
	P_VENTIL.OUTSET= C_MUD;
	TCC0_wait_ms(500);
	P_VENTIL.OUTSET= C_AIR;
	TCC0_wait_ms(500);
	P_VENTIL.OUTSET= C_CLRW;
	TCC0_wait_ms(500);

	TCC0_wait_sec(2);

	P_VENTIL.OUTCLR= C_RES;
	TCC0_wait_ms(500);
	P_VENTIL.OUTCLR= C_MUD;
	TCC0_wait_ms(500);
	P_VENTIL.OUTCLR= C_AIR;
	TCC0_wait_ms(500);
	P_VENTIL.OUTCLR= C_CLRW;
	TCC0_wait_ms(500);
	Watchdog_Restart();
}

void PORT_Ventil_AllOff(void)
{
	P_VENTIL.OUT= 0x00;
}




/* ------------------------------------------------------------------*
 * 						Ventil Auto Close
 * ------------------------------------------------------------------*/

void PORT_Ventil_AutoClose(t_page page)
{
	switch(page)
	{
		case AutoZone:		OUT_Clr_Air();	    break;
		case AutoSetDown:								      break;
		case AutoPumpOff:	OUT_Clr_PumpOff();	break;
		case AutoMud:			OUT_Clr_Mud();			break;
		case AutoCirc:		OUT_Clr_IPAir();		break;
		case AutoAir:			OUT_Clr_IPAir();		break;

		case ManualCirc:	      OUT_Clr_Air();	      break;
		case ManualAir:			    OUT_Clr_Air();			  break;
		case ManualSetDown:								            break;
		case ManualPumpOff:		  OUT_Clr_PumpOff();		break;
		case ManualMud:			    OUT_Clr_Mud();			  break;
		case ManualCompressor:	OUT_Clr_Compressor();	break;
		case ManualPhosphor:	  OUT_Clr_Phosphor();		break;
		case ManualInflowPump:	OUT_Clr_InflowPump();	break;

		default:										break;
	}
}





/* ==================================================================*
 * 						FUNCTIONS Relais
 * ==================================================================*/

void PORT_RelaisSet(unsigned char relais)
{
	P_RELAIS.OUTSET= relais;
}

void PORT_RelaisClr(unsigned char relais)
{
	P_RELAIS.OUTCLR= relais;
}

void PORT_Relais_AllOff(void)
{
	P_RELAIS.OUT= 0x00;
}



/* ==================================================================*
 * 						FUNCTIONS RunTime
 * ==================================================================*/

void PORT_RunTime(struct InputHandler *in)
{
	static int runTime = 0;

	runTime++;
	if(runTime > 2500)
	{
		runTime = 0;
		PORT_Ventilator();

		// Floating switch alarm
		if(IN_FLOAT_S3 && !in->float_sw_alarm)
    {
      if(MEM_EEPROM_ReadVar(ALARM_sensor))
      {
        Modem_CallAllNumbers();
        Error_ON();
      }
      in->float_sw_alarm = 1;
    }
    else if(!IN_FLOAT_S3 && in->float_sw_alarm)
    {
      if(MEM_EEPROM_ReadVar(ALARM_sensor))
      {
        Error_OFF();
      }
      in->float_sw_alarm = 0;
    }

		//***USVCheckVoltageSupply
		if(!DEBUG)
    {
      ADC_USV_Check();
    }
	}
}


/* ------------------------------------------------------------------*
 * 						Input Handler Init
 * ------------------------------------------------------------------*/

void InputHandler_init(struct InputHandler *in)
{
   in->float_sw_alarm = 0;
}




/*********************************************************************\
 * End of port_func.c
\**********************************************************************/
