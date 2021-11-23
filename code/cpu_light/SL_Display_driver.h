/*********************************************************************\
*	Author:			Red_Calcifer
*	Projekt:		Display-Driver-Functions-HeaderFile									
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
*	Header für Display-Driver-Funktionen, changed and improved 
*	version of halloWorld-PRG from Elektronic Assembly
* ------------------------------------------------------------------														
*	Date:			19.05.2011  	
* 	lastChanges:	18.06.2011										
\**********************************************************************/



/* ===================================================================*
 * 						Defines
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Basic Commands for EA DOGXL160x-7  LCD
 * -------------------------------------------------------------------*/

#define Set_Com_End_H                 	0xF1
#define Set_Com_End_L                 	0x67
#define Set_LCD_Mapping_Control       	0xC0
#define Set_Scroll_Line_LSB           	0x40
#define Set_Scroll_Line_MSB           	0x50
#define Set_Panel_Loading             	0x2B
#define Set_LCD_Bias_Ratio            	0xEB
#define Set_Vbias_Potentiometer_H     	0x81
#define Set_Vbias_Potentiometer_L     	0x5F
#define Set_RAM_Address_Control       	0x89
#define Set_Display_Enable            	0xAF
#define Set_All_Pixels_On             	0xA5
#define System_Reset                  	0xE2
#define Page_Address                  	0x60
#define Column_LSB0                   	0x00
#define Column_MSB0                   	0x10
#define CD                           	0x10

#define Set_All_Pixels_On_Off           0xA4

#define INVERSE							0xA7
#define NOT_INVERSE						0xA6

#define WPEN							0xF9	//Window Programming Enable
#define WPDIS							0xF8	//Window Programming Disable
#define WPC0							0xF4	//Window Programming CA0
#define WPP0							0xF5	//Window Programming PA0
#define WPC1							0xF6	//Window Programming CA1
#define WPP1							0xF7	//Window Programming PA1



/* -------------------------------------------------------------------*
 * 						TWI-Addresses
 * -------------------------------------------------------------------*/

#define W_CMD							(0x78>>1)
#define W_DATA							(0x7A>>1)
#define R_DATA							(0x7B>>1)
#define R_STATUS						(0x79>>1)	



/* ===================================================================*
 * 						Types 
 * ===================================================================*/
 
typedef signed   char   int8;
typedef unsigned char   uint8;
typedef signed   short  int16;
typedef unsigned short  uint16;
typedef signed   long   int32;
typedef unsigned long   uint32;
typedef unsigned char   bool;
typedef uint16          halDataAlign_t;



/* ===================================================================*
 * 						Enumeration
 * ===================================================================*/

typedef enum 
{ 	Backlight_ON,		Backlight_OFF,		Backlight_Count,	
	Backlight_ErrorOn,	Backlight_ErrorOff, Backlight_Reset
}t_Backlight;



/* ===================================================================*
 * 						FUNCTIONS - API 
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Config and Transfer
 * -------------------------------------------------------------------*/

void 			LCD_Init				(void);
unsigned char 	LCD_SendCmd				(uint8* SCmd, uint8 i);
void 			LCD_SendData			(uint8* SData, uint8 i);
void 			LCD_SetPageAddress		(uint8 PA);
void 			LCD_SetColumnAdress		(uint8 CA);

/* -------------------------------------------------------------------*
 * 						Commands
 * -------------------------------------------------------------------*/

void LCD_AllPixelsOn		(void);
void LCD_AllPixelsOnOff		(void);
void LCD_Rst				(void);
void LCD_HardwareRst		(void);
void LCD_Clean				(void);
void LCD_Fill				(void);

void LCD_Inverse			(void);
void LCD_NotInverse			(void);

void LCD_Backlight			(t_Backlight back);


/* -------------------------------------------------------------------*
 * 						Window Programming
 * -------------------------------------------------------------------*/

void LCD_WP_Enable			(void);
void LCD_WP_Disable			(void);
void LCD_WP_Column			(unsigned char startCA, unsigned char endCA);
void LCD_WP_Page			(unsigned char startPA, unsigned char endPA);


/* -------------------------------------------------------------------*
 * 						Write Font
 * -------------------------------------------------------------------*/

void LCD_WriteFont				(uint8 row, uint8 col, uint16 word);
void LCD_WriteFontNeg			(uint8 row, uint8 col, uint16 word);
void LCD_WriteFontNum			(uint8 row, uint8 col, unsigned char word);	//8x16
void LCD_WriteMyFont			(uint8 row, uint8 col, unsigned char word);	//4x6
void LCD_WriteMyFontNeg			(uint8 row, uint8 col, unsigned char word);

void LCD_WriteStringFont		(uint8 y, uint8 x, char word[]);
void LCD_WriteStringFontNeg		(uint8 y, uint8 x, char word[]);
void LCD_WriteStringMyFont		(uint8 y, uint8 x, char word[]);
void LCD_WriteStringMyFontNeg	(uint8 y, uint8 x, char word[]);


/* -------------------------------------------------------------------*
 * 						Write Value
 * -------------------------------------------------------------------*/

void LCD_WriteValue		(uint8 y, uint8 x, int value);
void LCD_WriteValueNeg	(uint8 y, uint8 x, int value);

void LCD_WriteValue2	(uint8 y, uint8 x, int value);
void LCD_WriteValueNeg2	(uint8 y, uint8 x, int value);

void LCD_WriteValue_MyFont(uint8 y, uint8 x, int value);
void LCD_WriteValue_MyFont2(uint8 y, uint8 x, int value);
void LCD_WriteValue_MyFont5(uint8 y, uint8 x, int value);



/* -------------------------------------------------------------------*
 * 						Write Symbols
 * -------------------------------------------------------------------*/

void LCD_Write_Symbol_1	(uint8 row, uint8 col, t_Symbols_35x23 sym);
void LCD_Write_Symbol_2	(uint8 row, uint8 col, t_Symbols_29x17 sym);
void LCD_Write_Symbol_3	(uint8 row, uint8 col, t_Symbols_19x24 sym);


/* -------------------------------------------------------------------*
 * 						Write Pin + Text
 * -------------------------------------------------------------------*/

void LCD_Write_Pin			(uint8 row, uint8 col, t_pinSymbols sym, uint8 num);
void LCD_Write_TextButton	(uint8 row, uint8 col, t_textSymbols text, uint8 pos);
void LCD_Write_Purator		(uint8 row, uint8 col);
void LCD_Write_HECS			(uint8 row, uint8 col);



/* -------------------------------------------------------------------*
 * 						Built in Functions
 * -------------------------------------------------------------------*/

unsigned char 	LCD_ConvertWP	(unsigned char con);
void 			LCD_WP_SetFrame	(uint8 row, uint8 col, uint8 height, uint8 len);

void 			LCD_FillSpace	(uint8 row, uint8 col, uint8 height, uint8 len);
void 			LCD_ClrSpace	(uint8 row, uint8 col, uint8 height, uint8 len);







/***********************************************************************
 *	END of Display_driver.h
 ***********************************************************************/


