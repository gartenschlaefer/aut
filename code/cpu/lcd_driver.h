// --
// EADOGXL160-7 Display driver

// include guard
#ifndef LCD_DRIVER_H   
#define LCD_DRIVER_H

#include "enums.h"
#include "structs.h"


/* ------------------------------------------------------------------*
 *            PORT
 * ------------------------------------------------------------------*/

#define LCD_RST_DIR   (PORTC.DIRSET = PIN2_bm)
#define LCD_RST_OFF   (PORTC.OUTSET = PIN2_bm)
#define LCD_RST_ON    (PORTC.OUTCLR = PIN2_bm)


/* ------------------------------------------------------------------*
 *            Basic Commands for EA DOGXL160x-7  LCD
 * ------------------------------------------------------------------*/

#define Set_Com_End_H                 0xF1
#define Set_Com_End_L                 0x67
#define Set_LCD_Mapping_Control       0xC0
#define Set_Scroll_Line_LSB           0x40
#define Set_Scroll_Line_MSB           0x50
#define Set_Panel_Loading             0x2B
#define Set_LCD_Bias_Ratio            0xEB
#define Set_Vbias_Potentiometer_H     0x81
#define Set_Vbias_Potentiometer_L     0x5F
#define Set_RAM_Address_Control       0x89
#define Set_Display_Enable            0xAF
#define Set_All_Pixels_On             0xA5
#define System_Reset                  0xE2
#define Page_Address                  0x60
#define Column_LSB0                   0x00
#define Column_MSB0                   0x10
#define CD                            0x10
#define Set_All_Pixels_On_Off         0xA4

#define INVERSE       0xA7
#define NOT_INVERSE   0xA6

#define WPEN          0xF9  //Window Programming Enable
#define WPDIS         0xF8  //Window Programming Disable
#define WPC0          0xF4  //Window Programming CA0
#define WPP0          0xF5  //Window Programming PA0
#define WPC1          0xF6  //Window Programming CA1
#define WPP1          0xF7  //Window Programming PA1


/* ------------------------------------------------------------------*
 *            TWI-Addresses
 * ------------------------------------------------------------------*/

#define W_CMD         (0x78 >> 1)
#define W_DATA        (0x7A >> 1)
#define R_DATA        (0x7B >> 1)
#define R_STATUS      (0x79 >> 1)


/* ------------------------------------------------------------------*
 *            Config and Transfer
 * ------------------------------------------------------------------*/

void LCD_Init(void);
unsigned char LCD_SendCmd(unsigned char* SCmd, unsigned char i);
void LCD_SendData(unsigned char* SData, unsigned char i);
void LCD_SetPageAddress(unsigned char PA);
void LCD_SetColumnAdress(unsigned char CA);


/* ------------------------------------------------------------------*
 *            Commands
 * ------------------------------------------------------------------*/

void LCD_Rst(void);
void LCD_HardwareRst(void);
void LCD_Clean(void);


/* ------------------------------------------------------------------*
 *            Window Programming
 * ------------------------------------------------------------------*/

void LCD_WP_Enable(void);
void LCD_WP_Disable(void);
void LCD_WP_Column(unsigned char startCA, unsigned char endCA);
void LCD_WP_Page(unsigned char startPA, unsigned char endPA);


/* ------------------------------------------------------------------*
 *            font, strings and values
 * ------------------------------------------------------------------*/

unsigned char LCD_WriteAnyFont(t_font_type font_type, unsigned char row, unsigned char col, unsigned short word);
void LCD_WriteAnyStringFont(t_font_type font_type, unsigned char y, unsigned char x, char word[]);
void LCD_WriteAnyValue(t_font_type font_type, unsigned char num, unsigned char y, unsigned char x, int value);


/* ------------------------------------------------------------------*
 *            symbols and other
 * ------------------------------------------------------------------*/

void LCD_WriteAnySymbol(t_symbol_type symbol_type, unsigned char row, unsigned char col, t_any_symbol any_symbol);
void LCD_Write_TextButton(unsigned char row, unsigned char col, t_text_buttons text, unsigned char pos);
void LCD_DeathMan(struct PlantState *ps, unsigned char row, unsigned char col);


/* ------------------------------------------------------------------*
 *            built in functions
 * ------------------------------------------------------------------*/

unsigned char LCD_ConvertWP(unsigned char con);
void LCD_WP_SetFrame(unsigned char row, unsigned char col, unsigned char height, unsigned char len);
void LCD_FillSpace(unsigned char row, unsigned char col, unsigned char height, unsigned char len);
void LCD_ClrSpace(unsigned char row, unsigned char col, unsigned char height, unsigned char len);

#endif