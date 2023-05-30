// --
// EADOGXL160-7 Display driver

#include <avr/io.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "lcd_driver.h"
#include "twi_func.h"
#include "symbols.h"
#include "tc_func.h"


/*-------------------------------------------------------------------*
 *  LCD initialize
 * ------------------------------------------------------------------*/

void LCD_Init(void)
{
  unsigned char LcdInitMacro[] = {Set_Com_End_H, Set_Com_End_L, Set_LCD_Mapping_Control, Set_Scroll_Line_LSB, Set_Scroll_Line_MSB, Set_Panel_Loading, Set_LCD_Bias_Ratio, Set_Vbias_Potentiometer_H, Set_Vbias_Potentiometer_L, Set_RAM_Address_Control, Set_Display_Enable};

  // set ports
  LCD_RST_DIR;
  LCD_RST_OFF;
  LCD_Reset_Software();

  // initialize macros
  while(LCD_SendCmd(LcdInitMacro, 11));
}


/*-------------------------------------------------------------------*
 *  LCD software reset
 * ------------------------------------------------------------------*/

void LCD_Reset_Software(void)
{
  unsigned char Cmd[] = {System_Reset};
  while(LCD_SendCmd(Cmd, 1));
}


/*-------------------------------------------------------------------*
 *  LCD hardware reset
 * ------------------------------------------------------------------*/

void LCD_Reset_Hardware(void)
{
  LCD_RST_DIR;
  LCD_RST_ON;
}


/*-------------------------------------------------------------------*
 *  LCD_SendCmd
 * ------------------------------------------------------------------*/

unsigned char LCD_SendCmd(unsigned char* cmd, unsigned char i)
{
  unsigned char twiErr = 0;

  // address, command, count of bytes
  twiErr = TWI_C_Master_WriteString(W_CMD, cmd, i);

  // check if error occurred
  if(twiErr == E_TWI_NO_DATA || twiErr == E_TWI_WAIT || twiErr == E_TWI_ARBLOST || twiErr == E_TWI_BUSERR  || twiErr == E_TWI_NO_SENT){ return 1; }

  return 0;
}


/*-------------------------------------------------------------------*
 *  LCD_SendData
 * ------------------------------------------------------------------*/

void LCD_SendData(unsigned char* data, unsigned char i)
{
  TWI_C_Master_WriteString(W_DATA, data, i);
}


/*-------------------------------------------------------------------*
 *  LCD_SetPageAddress
 * ------------------------------------------------------------------*/

void LCD_SetPageAddress(unsigned char page)
{
  // page address command
  unsigned char Cmd[] = {Page_Address + page};

  // protection
  if((Cmd[0] < 0x60) || (Cmd[0] > 0x78)){ return; }

  // send
  while(LCD_SendCmd(Cmd, 1));
}


/*-------------------------------------------------------------------*
 *  LCD_SetColumnAddress
 * ------------------------------------------------------------------*/

void LCD_SetColumnAdress(unsigned char col)
{
  unsigned char ColumnAddress[] = {Column_LSB0, Column_MSB0};

  // protection
  if(col > LCD_SPEC_MAX_COL){ return; }

  // column addresses
  ColumnAddress[0] = (Column_LSB0 + (col & 0x0F));
  ColumnAddress[1] = (Column_MSB0 + ((col & 0xF0) >> 4));

  // send
  while(LCD_SendCmd(ColumnAddress, 2));
}


/*-------------------------------------------------------------------*
 *  LCD_WP_Enable / LCD_WP_Disable
 * ------------------------------------------------------------------*/

void LCD_WP_Enable(void)
{
  unsigned char Cmd[] = {WPEN};
  while(LCD_SendCmd(Cmd, 1));
}

void LCD_WP_Disable(void)
{
  unsigned char Cmd[] = {WPDIS};
  while(LCD_SendCmd(Cmd, 1));
}


/*-------------------------------------------------------------------*
 *  Window Programming, Set Start Page-Address and End Page-Address
 * ------------------------------------------------------------------*/

void LCD_WP_Page(unsigned char start_page, unsigned char end_page)
{
  unsigned char Cmd[] = {WPP0, start_page, WPP1, end_page};
  while(LCD_SendCmd(Cmd, 4));
}


/*-------------------------------------------------------------------*
 *  Window Programming, Set Start Column-Address and End Column-Address
 * ------------------------------------------------------------------*/

void LCD_WP_Column(unsigned char start_col, unsigned char end_col)
{
  unsigned char Cmd[] = {WPC0, start_col, WPC1, end_col};
  while(LCD_SendCmd(Cmd, 4));
}


/*-------------------------------------------------------------------*
 *  sets the frame for window programming
 * ------------------------------------------------------------------*/

void LCD_WP_SetFrame(unsigned char row, unsigned char col, unsigned char height, unsigned char len)
{ 
  // page and column address
  LCD_SetPageAddress(row);
  LCD_SetColumnAdress(col);

  // set frame
  LCD_WP_Enable();
  LCD_WP_Page(row, row + height);
  LCD_WP_Column(col, col + len - 1);
}


/*-------------------------------------------------------------------*
 *  converts a nibble to the LED standards of the Display:
 *  each pixel in the H is converted to 2 Bits, 11 px On - 00 px off
 * ------------------------------------------------------------------*/

unsigned char LCD_WP_ConvertData(unsigned char con)
{
  unsigned char convert = 0;

  if(con & 0x01){ convert += 0x03; }
  if(con & 0x02){ convert += 0x0C; }
  if(con & 0x04){ convert += 0x30; }
  if(con & 0x08){ convert += 0xC0; }

  return convert;
}


/*-------------------------------------------------------------------*
 *  fill space
 * ------------------------------------------------------------------*/

void LCD_FillSpace(unsigned char row, unsigned char col, unsigned char height, unsigned char len)
{
  // allocate memory
  unsigned char *lcd_data = malloc(len);

  // fill memory
  for(unsigned char p = 0; p < len; p++){ lcd_data[p] = 0xFF; }

  // window programming
  LCD_WP_SetFrame(row, col, height, len);
  
  // for each page
  for(unsigned char p = 0; p < height; p++){ LCD_SendData(lcd_data, len); }
  LCD_WP_Disable();
  free(lcd_data);
}


/*-------------------------------------------------------------------*
 *  clear space
 * ------------------------------------------------------------------*/

void LCD_ClrSpace(unsigned char row, unsigned char col, unsigned char height, unsigned char len)
{
  // allocate memory
  unsigned char *lcd_data = malloc(len);

  // zero memory
  for(unsigned char p = 0; p < len; p++){ lcd_data[p] = 0x00; }

  // window programming
  LCD_WP_SetFrame(row, col, height, len);
  
  // for each page
  for(unsigned char p = 0; p < height; p++){ LCD_SendData(lcd_data, len); }
  LCD_WP_Disable();
  free(lcd_data);
}


/*-------------------------------------------------------------------*
 *  fill or clr
 * ------------------------------------------------------------------*/

void LCD_FillOrClrSpace(bool fill, unsigned char row, unsigned char col, unsigned char height, unsigned char len){ (fill ? LCD_FillSpace(row, col, height, len) : LCD_ClrSpace(row, col, height, len)); }


/*-------------------------------------------------------------------*
 *  clean whole screen
 * ------------------------------------------------------------------*/

void LCD_Clean(void){ LCD_ClrSpace(0, 0, LCD_SPEC_MAX_PAG, LCD_SPEC_MAX_COL); }


/*-------------------------------------------------------------------*
 *  write any font with window programming
 * ------------------------------------------------------------------*/

unsigned char LCD_WriteAnyFont(t_font_type font_type, unsigned char row, unsigned char col, unsigned short word)
{
  unsigned char *symbol_pointer = NULL;
  bool negative = false;
  unsigned char term = 2;

  switch(font_type)
  {
    case f_6x8_p: symbol_pointer = Font_6X8; term = 8; break;
    case f_6x8_n: symbol_pointer = Font_6X8; negative = true; term = 8; break;
    case f_4x6_p: symbol_pointer = FontNumbers_4X6; break;
    case f_4x6_n: symbol_pointer = FontNumbers_4X6_Neg; break;
    case f_8x16_p: symbol_pointer = Font_Numbers_8X16; break;
    case f_8x16_n: symbol_pointer = Font_Numbers_8X16; negative = true; break;
    default: return 0;
  }

  // get length and height
  unsigned char len = symbol_pointer[0];
  unsigned char height = symbol_pointer[1];

  // allocate memory
  unsigned char *lcd_data = malloc(2 * len);

  // set frame
  LCD_WP_SetFrame(row, col, 2 * height, len);

  // pages
  for(unsigned char p = 0; p < height; p++)
  {
    // columns
    for(unsigned char c = 0; c < len; c++)
    {
      // get position
      int pos = term + c + len * (p + word * height);

      // get font symbol
      unsigned char font_symbol = (negative ? 255 - symbol_pointer[pos] : symbol_pointer[pos] );

      // convert data for window programming
      lcd_data[c] = LCD_WP_ConvertData(font_symbol & 0x0F);
      lcd_data[c + len] = LCD_WP_ConvertData((font_symbol & 0xF0) >> 4);
    }

    // send data
    LCD_SendData(lcd_data, 2 * len);
  }
  free(lcd_data);
  LCD_WP_Disable();
  return len;
}


/*-------------------------------------------------------------------*
 *  write any string fonts with window programming
 * ------------------------------------------------------------------*/

void LCD_WriteAnyStringFont(t_font_type font_type, unsigned char y, unsigned char x, char word[])
{
  unsigned char k = 0;
  unsigned char a = 0;
  unsigned char len = 0;
  unsigned char char_offset = 33;

  // change character offset
  if(font_type == f_4x6_p || font_type == f_4x6_n) char_offset = 48;

  // write each character of string
  do
  {
    // actual word
    k = word[a];

    // write font symbol
    len = LCD_WriteAnyFont(font_type, y, x, (k - char_offset));

    // add length of font
    x += len;
    a++;
    k = word[a];
  }
  while(k != 0);
}


/* ------------------------------------------------------------------*
 *            write any value maximum is defined below
 * ------------------------------------------------------------------*/

void LCD_WriteAnyValue(t_font_type font_type, unsigned char num, unsigned char y, unsigned char x, int value)
{
  char v[6] = {0x00};

  // safety
  if(num > 5) return;

  // extract digits
  for(unsigned char i = 0; i < num; i++)
  {
    v[num - 1 - i] = value % 10 + 48;
    value /= 10;
  }

  // write symbol
  LCD_WriteAnyStringFont(font_type, y, x, v);
}


/*-------------------------------------------------------------------*
 *  write any symbol
 * ------------------------------------------------------------------*/

void LCD_WriteAnySymbol(unsigned char row, unsigned char col, t_any_symbol any_symbol)
{
  unsigned char symbol = 0;
  unsigned char offset = 0;

  unsigned char *symbol_pointer = NULL;

  // get correct symbol pointer
  switch(any_symbol)
  {
    // 35 x 23 [8]
    case _n_pumpOff: case _n_mud: case _n_inflowPump: case _n_pump2:
    case _p_pumpOff: case _p_mud: case _p_inflowPump: case _p_pump2: 
      symbol_pointer = Symbols_35x23_bmp;
      offset = 0;
      break;

    // 29 x 17 [20]
    case _n_setDown: case _n_alarm: case _n_air: case _n_sensor: case _n_watch: case _n_compressor: case _n_circulate: case _n_cal: case _n_zone: case _n_level:
    case _p_setDown: case _p_alarm: case _p_air: case _p_sensor: case _p_watch: case _p_compressor: case _p_circulate: case _p_cal: case _p_zone: case _p_level:
      symbol_pointer = Symbols_29x17_bmp;
      offset = _p_pump2 + 1;
      break;

    // 19 x 19 [23]
    case _n_phosphor: case _n_pump: case _n_esc: case _n_plus: case _n_minus: case _n_arrow_up: case _n_arrow_down: case _n_ok: case _n_grad: case _n_sonic: case _n_arrow_redo:
    case _p_phosphor: case _p_pump: case _p_esc: case _p_plus: case _p_minus: case _p_arrow_up: case _p_arrow_down: case _p_ok: case _p_grad: case _p_sonic: case _p_arrow_redo: 
    case _line:
      symbol_pointer = Symbols_19x19_bmp;
      offset = _p_level + 1;
      break;

    // 34 x 21 [6]
    case _p_frame: case _p_escape: case _p_del: 
    case _n_frame: case _n_escape: case _n_del:
      symbol_pointer = Symbols_34x21_bmp;
      offset = _line + 1;
      break;

    // 39 x 16 [2]
    case _n_text_frame: 
    case _p_text_frame:
      symbol_pointer = Symbols_39x16_bmp;
      offset = _n_del + 1;
      break;

    // hecs [1]
    case _logo_hecs:
      symbol_pointer = Symbol_HECS;
      offset = _logo_hecs;
      break;

    // purator [1]
    case _logo_purator:
      symbol_pointer = Symbol_Purator;
      offset = _logo_purator;
      break;

    // todo
    // valve symbol
    case _n_valve:
    case _p_valve: 
      symbol_pointer = Symbols_35x23_bmp;
      offset = _logo_purator + 1;
      break;

    default: return;
  }

  // get length and height
  unsigned char len = symbol_pointer[0];
  unsigned char height = symbol_pointer[1];

  // set frame
  LCD_WP_SetFrame(row, col, 2 * height, len);

  // allocate memory
  unsigned char *lcd_data = malloc(2 * len);

  // pages
  for(unsigned char p = 0; p < height; p++)
  {
    // columns
    for(unsigned char c = 0; c < len; c++)
    {
      // get symbol
      symbol = symbol_pointer[2 + c + len * (p + (any_symbol - offset) * height)];

      // convert data for window programming
      lcd_data[c] = LCD_WP_ConvertData(symbol & 0x0F);
      lcd_data[c + len] = LCD_WP_ConvertData((symbol & 0xF0) >> 4);
    }

    // send data
    LCD_SendData(lcd_data, 2 * len);
  }
  free(lcd_data);
  LCD_WP_Disable();
}


/* ------------------------------------------------------------------*
 *            text button
 * ------------------------------------------------------------------*/

void LCD_Write_TextButton(unsigned char row, unsigned char col, t_text_buttons text, unsigned char pos)
{
  char t[7] = "";

  // write frame
  if(pos){ LCD_WriteAnySymbol(row, col, _p_text_frame); }
  else{ LCD_WriteAnySymbol(row, col, _n_text_frame); }

  switch(text)
  {
    case TEXT_BUTTON_auto: row += 1; col += 8; strcat(t, "Auto");  break;
    case TEXT_BUTTON_manual: row += 1; col += 2; strcat(t, "Manual");break;
    case TEXT_BUTTON_setup: row += 1; col += 5; strcat(t, "Setup"); break;
    case TEXT_BUTTON_data: row += 1; col += 8; strcat(t, "Data");  break;
    case TEXT_BUTTON_sonic: row += 1; col += 5; strcat(t, "Sonic"); break;
    case TEXT_BUTTON_shot: row += 1; col += 8; strcat(t, "Shot");  break;
    case TEXT_BUTTON_open_ventil: row += 1; col += 5; strcat(t, "OpenV"); break;
    case TEXT_BUTTON_boot: row += 1; col += 8; strcat(t, "Boot");  break;
    case TEXT_BUTTON_read: row += 1; col += 8; strcat(t, "Read");  break;
    case TEXT_BUTTON_write: row += 1; col += 5; strcat(t, "Write"); break;
  }

  // write text
  if(pos){ LCD_WriteAnyStringFont(f_6x8_p, row, col, t); }
  else{ LCD_WriteAnyStringFont(f_6x8_n, row, col, t); }
}


/* ------------------------------------------------------------------*
 *            DeathMan
 * ------------------------------------------------------------------*/

void LCD_DeathMan(struct PlantState *ps, unsigned char row, unsigned char col)
{
  if(ps->frame_counter->frame < TC_FPS_HALF){ LCD_FillSpace(row, col, 1, 4); }
  else{ LCD_ClrSpace(row, col, 1, 4); }
}