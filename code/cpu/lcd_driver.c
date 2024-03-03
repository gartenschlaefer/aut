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
 *  write any string fonts with window programming
 * ------------------------------------------------------------------*/

void LCD_WriteAnyStringFont(t_font_type font_type, unsigned char y, unsigned char x, char *string, bool negative)
{
  unsigned char len = 0;
  const unsigned char char_offset = ((font_type == _f_4x6) ? 48 : 33);

  // write each character of the string
  while(*string)
  {  
    len = LCD_WriteAnyFont(font_type, y, x, (*string - char_offset), negative);
    x += len;
    string++;
  }
}


/* ------------------------------------------------------------------*
 *            write any value maximum is defined below
 * ------------------------------------------------------------------*/

void LCD_WriteAnyValue(t_font_type font_type, unsigned char num, unsigned char y, unsigned char x, int value, bool negative)
{
  char v[6] = {0x00};

  // safety
  if(num > 5){ return; }

  // extract digits
  for(unsigned char i = 0; i < num; i++)
  {
    v[num - 1 - i] = value % 10 + 48;
    value /= 10;
  }

  // write symbol
  LCD_WriteAnyStringFont(font_type, y, x, v, negative);
}


/*-------------------------------------------------------------------*
 *  write any font with window programming
 * ------------------------------------------------------------------*/

unsigned char LCD_WriteAnyFont(t_font_type font_type, unsigned char row, unsigned char col, unsigned char letter, bool negative)
{
  const unsigned char *symbol_pointer = NULL;
  const unsigned char *inv_mask_pointer = NULL;

  switch(font_type)
  {
    // 6x8
    case _f_6x8: 
      symbol_pointer = Font_6x8; 
      letter += 1; 
      break;

    // 4x6
    case _f_4x6: 
      symbol_pointer = Font_Numbers_4x6;
      inv_mask_pointer = (negative ? Font_Numbers_4x6_inv_mask : NULL);
      break;

    // 6x12
    case _f_8x16: 
      symbol_pointer = Font_Numbers_6x12; 
      break;

    default: return 0;
  }

  // send data and get length
  unsigned char len = LCD_send_symbol_data_from_pointer(row, col, symbol_pointer, inv_mask_pointer, letter, 0, negative);

  // // get length and height (height in bytes)
  // unsigned char len = symbol_pointer[0];
  // unsigned char height = (unsigned char)(symbol_pointer[1] / 8) + (symbol_pointer[1] % 8 ? 1 : 0);

  // // allocate memory
  // unsigned char *lcd_data = malloc(2 * len);

  // // set frame
  // LCD_WP_SetFrame(row, col, 2 * height, len);

  // // pages
  // for(unsigned char p = 0; p < height; p++)
  // {
  //   // columns
  //   for(unsigned char c = 0; c < len; c++)
  //   {
  //     // get position
  //     int pos = term + c + len * (p + letter * height);

  //     // get font symbol
  //     unsigned char font_symbol = (negative ? 255 - symbol_pointer[pos] : symbol_pointer[pos]);

  //     // convert data for window programming
  //     lcd_data[c] = LCD_WP_ConvertData(font_symbol & 0x0F);
  //     lcd_data[c + len] = LCD_WP_ConvertData((font_symbol & 0xF0) >> 4);
  //   }

  //   // send data
  //   LCD_SendData(lcd_data, 2 * len);
  // }
  // free(lcd_data);
  // LCD_WP_Disable();
  return len;
}


/*-------------------------------------------------------------------*
 *            write any symbol
 * ------------------------------------------------------------------*/

void LCD_WriteAnySymbol(unsigned char row, unsigned char col, t_any_symbol any_symbol, bool negative)
{
  unsigned char offset = 0;
  unsigned char inv_mask_pos = 0;
  const unsigned char *symbol_pointer = NULL;
  const unsigned char *inv_mask_pointer = NULL;

  // get correct symbol pointer
  switch(any_symbol)
  {
    // 35x24
    case _s_pump_off:  case _s_inflow_pump: case _s_pump2:
      symbol_pointer = Symbols_35x24;
      inv_mask_pointer = (negative && any_symbol != _s_pump2 ? Symbols_35x24_inv_mask : NULL);
      offset = _s_pump_off;
      break;

    // 29x20
    case _s_set_down: case _s_circulate: case _s_air: case _s_zone:
      symbol_pointer = Symbols_29x20;
      inv_mask_pointer = (negative ? Symbols_29x20_inv_mask : NULL);
      offset = _s_set_down;
      break;

    // 29x20
    case _s_cal: case _s_watch: case _s_alarm:
      symbol_pointer = Symbols_29x20;
      inv_mask_pointer = (negative ? Symbols_29x20_inv_mask : NULL);
      inv_mask_pos = 1;
      offset = _s_set_down;
      break;

    // 29x20
    case _s_sensor: case _s_compressor: case _s_level:
      symbol_pointer = Symbols_29x20;
      offset = _s_set_down;
      break;

    // 29x24
    case _s_mud:
      symbol_pointer = Symbols_29x24;
      inv_mask_pointer = (negative ? Symbols_29x24_inv_mask : NULL);
      offset = _s_mud;
      break;

    // 19x19
    case _s_phosphor: case _s_esc: case _s_plus: case _s_minus: case _s_arrow_up: case _s_arrow_down: case _s_ok: case _s_grad: case _s_sonic: case _s_arrow_redo:
      symbol_pointer = Symbols_19x20;
      inv_mask_pointer = (negative ? Symbols_19x20_inv_mask : NULL);
      offset = _s_phosphor;
      break;

    // 19x24
    case _s_pump:
      symbol_pointer = Symbols_15x24;
      offset = _s_pump;
      break;

    // 33x20
    case _s_frame: case _s_escape: case _s_del:
      symbol_pointer = Symbols_33x20;
      inv_mask_pointer = (negative ? Symbols_33x20_inv_mask : NULL);
      offset = _s_frame;
      break;

    // 39x16 [2]
    case _s_text_frame: 
      symbol_pointer = Symbols_39x16;
      offset = _s_text_frame;
      break;

    // hecs [1]
    case _s_logo_hecs:
      symbol_pointer = Symbol_HECS;
      offset = _s_logo_hecs;
      break;

    // purator [1]
    case _s_logo_purator:
      symbol_pointer = Symbol_Purator;
      offset = _s_logo_purator;
      break;

    // todo:
    // valve symbol
    case _s_valve:
      symbol_pointer = Symbols_29x20;
      offset = _s_valve;
      break;

    default: return;
  }

  // send data
  LCD_send_symbol_data_from_pointer(row, col, symbol_pointer, inv_mask_pointer, (any_symbol - offset), inv_mask_pos, negative);
}


/* ------------------------------------------------------------------*
 *            text button
 * ------------------------------------------------------------------*/

void LCD_Write_TextButton(unsigned char row, unsigned char col, t_text_buttons text, bool negative)
{
  char t[7] = "";

  // write frame
  LCD_WriteAnySymbol(row, col, _s_text_frame, negative);

  switch(text)
  {
    case TEXT_BUTTON_auto: row += 1; col += 8; strcat(t, "Auto");  break;
    case TEXT_BUTTON_manual: row += 1; col += 2; strcat(t, "Manual");break;
    case TEXT_BUTTON_setup: row += 1; col += 5; strcat(t, "Setup"); break;
    case TEXT_BUTTON_data: row += 1; col += 8; strcat(t, "Data");  break;
    case TEXT_BUTTON_sonic: row += 1; col += 5; strcat(t, "Sonic"); break;
    case TEXT_BUTTON_shot: row += 1; col += 8; strcat(t, "Shot");  break;
    case TEXT_BUTTON_open_valve: row += 1; col += 5; strcat(t, "OpenV"); break;
    case TEXT_BUTTON_boot: row += 1; col += 8; strcat(t, "Boot");  break;
    case TEXT_BUTTON_read: row += 1; col += 8; strcat(t, "Read");  break;
    case TEXT_BUTTON_write: row += 1; col += 5; strcat(t, "Write"); break;
  }

  // write text
  LCD_WriteAnyStringFont(_f_6x8, row, col, t, negative);
}


/* ------------------------------------------------------------------*
 *            DeathMan
 * ------------------------------------------------------------------*/

void LCD_DeathMan(struct PlantState *ps, unsigned char row, unsigned char col)
{
  if(ps->frame_counter->frame < TC_FPS_HALF){ LCD_FillSpace(row, col, 1, 4); }
  else{ LCD_ClrSpace(row, col, 1, 4); }
}


/*-------------------------------------------------------------------*
 *            send data from pointer
 * ------------------------------------------------------------------*/

unsigned char LCD_send_symbol_data_from_pointer(unsigned char row, unsigned char col, const unsigned char *symbol_pointer, const unsigned char *inv_mask_pointer, unsigned char symbol_pos, unsigned char inv_mask_pos, bool negative)
{
  // get length and height (height in bytes)
  unsigned char l_px = *symbol_pointer++;
  unsigned char h_px = *symbol_pointer++;
  unsigned char h_page = (unsigned char)(h_px / 4) + (h_px % 4 ? 1 : 0);
  unsigned char h_byte = (unsigned char)(h_px / 8) + (h_px % 8 ? 1 : 0);

  // set frame
  LCD_WP_SetFrame(row, col, h_page, l_px);

  // allocate memory (for writing two pages)
  unsigned char *lcd_data = malloc(2 * l_px);

  // get to symbol position in array
  symbol_pointer += (l_px * h_byte * symbol_pos);

  // inv mask pointer
  if(inv_mask_pointer)
  { 
    inv_mask_pointer += 2;
    if(inv_mask_pos){ inv_mask_pointer += inv_mask_pos * (l_px * h_byte); }
  }

  // pages
  for(unsigned char p = 0; p < h_byte; p++)
  {
    bool send_two_pages = ((p * 2 + 2) <= h_page ? true : false);

    // columns
    for(unsigned char c = 0; c < l_px; c++)
    {
      // symbol
      unsigned char symbol = *symbol_pointer++;

      // inversion?
      if(negative)
      {
        // with mask or without
        if(inv_mask_pointer){ symbol ^= *inv_mask_pointer; inv_mask_pointer++; }
        else{ symbol = ~symbol; }
      }

      // convert data for window programming
      lcd_data[c] = LCD_WP_ConvertData(symbol & 0x0F);

      // send other page
      if(send_two_pages){ lcd_data[c + l_px] = LCD_WP_ConvertData((symbol & 0xF0) >> 4); }
    }

    // send data
    LCD_SendData(lcd_data, (send_two_pages ? 2 : 1) * l_px);
  }
  
  // free allocated memory
  free(lcd_data);
  LCD_WP_Disable();

  return l_px;
}