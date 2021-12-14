// --
// EADOGXL160-7 Display driver

#include<avr/io.h>
#include<inttypes.h>

#include "defines.h"

#include "twi_func.h"
#include "lcd_driver.h"
#include "symbols.h"


/* ==================================================================*
 *            FUNCTIONS     Config and Transfer
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  LCD init LcdInitMacro for details
 * ------------------------------------------------------------------*/

void LCD_Init(void)
{

  unsigned char LcdInitMacro[] = {Set_Com_End_H, Set_Com_End_L, Set_LCD_Mapping_Control, Set_Scroll_Line_LSB, Set_Scroll_Line_MSB, Set_Panel_Loading, Set_LCD_Bias_Ratio, Set_Vbias_Potentiometer_H, Set_Vbias_Potentiometer_L, Set_RAM_Address_Control, Set_Display_Enable};

  // set ports
  LCD_LED_DIR;
  LCD_RST_DIR;
  LCD_RST_OFF;
  LCD_Rst();

  // init macros
  while(LCD_SendCmd(LcdInitMacro, 11));
}


/*-------------------------------------------------------------------*
 *  lcd backlight
 * ------------------------------------------------------------------*/

void LCD_Backlight(t_FuncCmd cmd)
{
  static t_FuncCmd    state = _off;
  static unsigned int count = 0;

  switch(cmd)
  {
    // turn on
    case _on: LCD_LED_ON; state = _on; count = 0; break;

    // turn off
    case _off: LCD_LED_OFF; state = _off; break;

    // go into error state
    case _error: state = _error; break;

    // exe
    case _exe:

      //***LightAlwaysOn-Debug
      if(DEBUG) return;

      // light is on
      if(state == _on)
      {
        // counting up
        count++;

        //Ton = 3min
        if(count > BACKLIGHT_TON_FRAMES){count = 0; LCD_LED_OFF; state = _off;}
      }

      // error
      else if(state == _error)
      {
        count++;
        if(count > 400) LCD_LED_OFF;
        if(count > 2000){count = 0; LCD_LED_ON;}
      }             
      break;

    // reset
    case _reset: count = 0; break;

    // default
    default: break;
  }
}


/*-------------------------------------------------------------------*
 *  LCD_SendCmd
 * ------------------------------------------------------------------*/

unsigned char LCD_SendCmd(unsigned char* SCmd, unsigned char i)
{
  unsigned char twiErr = 0;

  // address, command, count of bytes
  twiErr = TWI_Master_WriteString(W_CMD, SCmd, i);

  // check if error occured
  if(twiErr == E_TWI_NO_DATA || twiErr == E_TWI_WAIT || twiErr == E_TWI_ARBLOST || twiErr == E_TWI_BUSERR  || twiErr == E_TWI_NO_SENT) return 1;

  return 0;
}


/*-------------------------------------------------------------------*
 *  LCD_SendData
 * ------------------------------------------------------------------*/

void LCD_SendData(unsigned char* SData, unsigned char i)
{
  TWI_Master_WriteString(W_DATA, SData, i);
}


/*-------------------------------------------------------------------*
 *  LCD_SetPageAddress
 * ------------------------------------------------------------------*/

void LCD_SetPageAddress(unsigned char PA)
{
  // page adress command
  unsigned char Cmd[] = {Page_Address + PA};

  // protection
  if((Cmd[0] < 0x60) || (Cmd[0] > 0x78)) return;

  // send
  while(LCD_SendCmd(Cmd, 1));
}


/*-------------------------------------------------------------------*
 *  LCD_SetColumnAddress
 * ------------------------------------------------------------------*/

void LCD_SetColumnAdress(unsigned char CA)
{
  unsigned char H = 0x00;
  unsigned char L = 0x00;
  unsigned char ColumnAddress[]  = {Column_LSB0, Column_MSB0};

  // protection
  if(CA > 160) return;

  L = (CA & 0x0F);
  H = (CA & 0xF0);
  H = (H >> 4);

  // column addresses
  ColumnAddress[0] = (Column_LSB0 + L );
  ColumnAddress[1] = (Column_MSB0 + H );

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
 *  LCD_WP_Page
 * --------------------------------------------------------------
 *  Window Programming, Set Start Page-Address and End Page-Address
 * ------------------------------------------------------------------*/

void LCD_WP_Page(unsigned char startPA, unsigned char endPA)
{
  unsigned char Cmd[] = {WPP0, startPA, WPP1, endPA};
  while(LCD_SendCmd(Cmd, 4));
}


/*-------------------------------------------------------------------*
 *  LCD_WP_Column
 * --------------------------------------------------------------
 *  Window Programming, Set Start Column-Address and End Column-Address
 * ------------------------------------------------------------------*/

void LCD_WP_Column(unsigned char startCA, unsigned char endCA)
{
  unsigned char Cmd[] = {WPC0, startCA, WPC1, endCA};
  while(LCD_SendCmd(Cmd, 4));
}



/* ==================================================================*
 *            FUNCTIONS     Commands
 * ==================================================================*/


/*-------------------------------------------------------------------*
 *  lcd software reset
 * ------------------------------------------------------------------*/

void LCD_Rst(void)
{
  unsigned char Cmd[] = {System_Reset};
  while(LCD_SendCmd(Cmd, 1));
}


/*-------------------------------------------------------------------*
 *  lcd hardware reset
 * ------------------------------------------------------------------*/

void LCD_HardwareRst(void)
{
  LCD_RST_DIR;
  LCD_RST_ON;
}


/*-------------------------------------------------------------------*
 *  LCD_Clean
 * ------------------------------------------------------------------*/

void LCD_Clean(void)
{
  unsigned char LcdData[80] = {0x00};

  // page and column address
  LCD_SetPageAddress(0);
  LCD_SetColumnAdress(0);

  LCD_WP_Enable();      //Window Programm Enable
  LCD_WP_Page(0, 25);     //Page Frame
  LCD_WP_Column(0,159);   //Column Frame

  // 25 pages
  for(unsigned char p = 0; p < 52; p++)
  {
    // 160 columns
    for(unsigned char c = 0; c < 2; c++)
    {
      LCD_SendData(LcdData, 80);
    }
  }
  LCD_WP_Disable();
}



/* ==================================================================*
 *            FUNCTIONS     Fill and Clear Space
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  LCD_FillSpace
 * ------------------------------------------------------------------*/

void LCD_FillSpace(unsigned char row, unsigned char col, unsigned char height, unsigned char len)
{
  unsigned char LcdData[160] = {0xFF};

  // fill array
  for(unsigned char p = 0; p < 160; p++) LcdData[p] = 0xFF;

  LCD_WP_SetFrame(row, col, height, len);
  
  // 25 pages
  for(unsigned char p = 0; p < height; p++) LCD_SendData(LcdData, (len));

  LCD_WP_Disable();
}


/*-------------------------------------------------------------------*
 *  LCD_ClrSpace
 * ------------------------------------------------------------------*/

void LCD_ClrSpace(unsigned char row, unsigned char col, unsigned char height, unsigned char len)
{
  unsigned char LcdData[160] = {0x00};

  // zero
  for(unsigned char p = 0; p < 160; p++) LcdData[p] = 0x00;

  LCD_WP_SetFrame(row, col, height, len);
  
  // 25 pages
  for(unsigned char p = 0; p < height; p++) LCD_SendData(LcdData, (len));

  LCD_WP_Disable();
}



/* ==================================================================*
 *            Font
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Font 6x8
 * ------------------------------------------------------------------*/
/*-------------------------------------------------------------------*
 *  LCD_WriteFont
 * --------------------------------------------------------------
 *  parameter:  unsigned char y - Start Page
 *  unsigned char x             - Start Collumn
 *  unsigend char word          - Word to write
 * --------------------------------------------------------------
 *  Write Font with WP
 * ------------------------------------------------------------------*/

void LCD_WriteFont(unsigned char row, unsigned char col, unsigned short word)
{
  unsigned char LcdData[40] = {0x00};

  // dimensions of font
  unsigned char len = Font_6X8[0];
  unsigned char height = Font_6X8[1];

  // set frame
  LCD_WP_SetFrame(row, col, height, len);

  // pages
  for(unsigned char p = 0; p < height; p++)
  {
    // columns
    for(unsigned char c = 0; c < len; c++)
    {
      LcdData[c] = LCD_ConvertWP(Font_6X8[8 + c + len * p + word * len * height] & 0x0F);
    }

    // send
    LCD_SendData(LcdData, len);

    // columns
    for(unsigned char a = 0; a < len; a++)
    {
      LcdData[a] = LCD_ConvertWP((Font_6X8[8 + a + len * p + ((word) * len * height)] & 0xF0) >> 4);
    }

    // send
    LCD_SendData(LcdData, len);
  }
  LCD_WP_Disable();
}


/* ------------------------------------------------------------------*
 *            Font 6x8 Neg
 * ------------------------------------------------------------------*/

void LCD_WriteFontNeg(unsigned char row, unsigned char col, unsigned short word)
{
  unsigned char LcdData[40] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  unsigned char len = Font_6X8_Neg[0];
  unsigned char height = Font_6X8_Neg[1];

  LCD_WP_SetFrame(row, col, height, len);

  for(unsigned char p = 0; p < height; p++)
  {
    for(unsigned char c = 0; c < len; c++)
    {
      L = (Font_6X8_Neg[8 + c + len * p + ((word) * len * height)] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }

    LCD_SendData(LcdData, len);

    for(unsigned char a = 0; a < len; a++)
    {
      H = (Font_6X8_Neg[8 + a + len * p + ((word) * len * height)] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }

    LCD_SendData(LcdData, len);
  }

  LCD_WP_Disable();
}


/* ------------------------------------------------------------------*
 *            Font 8x16 Num
 * ------------------------------------------------------------------*/

void LCD_WriteFontNum(unsigned char row, unsigned char col, unsigned char word)
{
  unsigned char LcdData[20] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  unsigned char len = Font_Numbers_8X16[0];
  unsigned char height = Font_Numbers_8X16[1];
  unsigned char n = 0;

  // negative / positive numbers
  if(word & 0xF0) n = 1;
  else n = 0;

  word = word & 0x0F;

  // frame set
  LCD_WP_SetFrame(row, col, height, len);

  // pages
  for(unsigned char p = 0; p < height; p++)
  {
    // column
    for(unsigned char c = 0; c < len; c++)
    {
      L = (Font_Numbers_8X16[2 + c + len * p + ((word) * len * height) + (n*10*len*height)] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }
    LCD_SendData(LcdData, len);

    // 160 columns
    for(unsigned char a = 0; a < len; a++)
    {
      H = (Font_Numbers_8X16[2 + a + len * p + ((word) * len * height) + (n*10*len*height)] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }
    LCD_SendData(LcdData, len);
  }

  LCD_WP_Disable();
}


/* ------------------------------------------------------------------*
 *            Font 4x6
 * ------------------------------------------------------------------*/

void LCD_WriteMyFont(unsigned char row, unsigned char col, unsigned char word)
{
  unsigned char LcdData[10] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  unsigned char len = FontNumbers_4X6[0];
  unsigned char height = FontNumbers_4X6[1];

  LCD_WP_SetFrame(row, col, height, len);

  for(unsigned char p = 0; p < height; p++)
  {
    for(unsigned char c = 0; c < len; c++)
    {
      L = (FontNumbers_4X6[2 + c + len * p + ((word) * len * height)] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }
    LCD_SendData(LcdData, len);

    for(unsigned char a = 0; a < len; a++)
    {
      H = (FontNumbers_4X6[2 + a + len * p + ((word) * len * height)] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }
    LCD_SendData(LcdData, len);
  }
  LCD_WP_Disable();
}


/* ------------------------------------------------------------------*
 *            Font 4x6 Neg
 * ------------------------------------------------------------------*/

void LCD_WriteMyFontNeg(unsigned char row, unsigned char col, unsigned char word)
{
  unsigned char LcdData[10] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  unsigned char len = FontNumbers_4X6_Neg[0];
  unsigned char height = FontNumbers_4X6_Neg[1];

  LCD_WP_SetFrame(row, col, height, len);

  for(unsigned char p = 0; p < height; p++)
  {
    for(unsigned char c = 0; c < len; c++)
    {
      L = (FontNumbers_4X6_Neg[2 + c + len * p + ((word)*len*height)] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }
    LCD_SendData(LcdData, len);

    for(unsigned char a = 0; a < len; a++)
    {
      H = (FontNumbers_4X6_Neg[2 + a + len * p + ((word)*len*height)] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }
    LCD_SendData(LcdData, len);
  }

  LCD_WP_Disable();
}




/* ==================================================================*
 *            String Font
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  LCD_WriteStringFont
 * --------------------------------------------------------------
 *  unsigned char y   - Start Page
 *  unsigned char x   - Start Collumn
 *  char      word[]  - String to write
 * --------------------------------------------------------------
 *  Write String Font with WP
 * ------------------------------------------------------------------*/

void LCD_WriteStringFont(unsigned char y, unsigned char x, char word[])
{
  unsigned char k=0;
  unsigned char a=0;

  do
  {
    k = word[a];
    LCD_WriteFont( y, x, (k-33));
    x = x + Font_6X8[0];
    a++;
    k = word[a];
  }
  while(k!=0);
}


/* ------------------------------------------------------------------*
 *            StringFont 6x8 Neg
 * ------------------------------------------------------------------*/

void LCD_WriteStringFontNeg(unsigned char y, unsigned char x, char word[])
{
  unsigned char k=0;
  unsigned char a=0;

  do
  {
    k = word[a];
    LCD_WriteFontNeg( y, x, (k-33));
    x = x + Font_6X8_Neg[0];
    a++;
    k = word[a];
  }
  while(k!=0);
}


/* ------------------------------------------------------------------*
 *            StringFont 4x6
 * ------------------------------------------------------------------*/

void LCD_WriteStringMyFont(unsigned char y, unsigned char x, char word[])
{
  unsigned char k=0;
  unsigned char a=0;

  do
  {
    k = word[a];
    LCD_WriteMyFont(y, x, k-48);
    x = x + FontNumbers_4X6[0];
    a++;
    k = word[a];
  }
  while(k!=0);
}



/* ==================================================================*
 *            Write Value 6x8
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Value 6x8     2Pos
 * ------------------------------------------------------------------*/

void LCD_WriteValue2(unsigned char y, unsigned char x, int value)
{
  char cValue[10] = {0x00};
  int con = 0;
  unsigned char i = 0;

  con = value;

  if(con > 99)  con = 99;
  if(con < 1)   con = 0;

  //--------------------------------------------------Hex2Bcd1Byte
  cValue[0] = (con / 10);
  cValue[1] = (con - (10 * cValue[0]));

  //--------------------------------------------------AsciiConversion
  for(i = 0; i < 2; i++) cValue[i] = cValue[i] + 48;
  LCD_WriteStringFont(y, x, cValue);
}



/* ------------------------------------------------------------------*
 *            Value 6x8 Neg   2Pos
 * ------------------------------------------------------------------*/

void LCD_WriteValueNeg2(unsigned char y, unsigned char x, int value)
{
  char cValue[10] = {0x00};
  int con = 0;
  unsigned char i = 0;

  con = value;

  if(con > 99) con = 99;
  if(con < 1) con = 0;

  //--------------------------------------------------Hex2Bcd1Byte
  cValue[0] = (con / 10);
  cValue[1] = (con - (10 * cValue[0]));

  //--------------------------------------------------AsciiConversion
  for(i = 0; i < 2; i++) cValue[i] = cValue[i] + 48;
  LCD_WriteStringFontNeg(y, x, cValue);
}



/* ------------------------------------------------------------------*
 *            Value 6x8 Neg   3Pos
 * ------------------------------------------------------------------*/

void LCD_WriteValue3(unsigned char y, unsigned char x, int value)
{
  char cValue[10] = {0};
  unsigned char i = 0;

  if(value > 999) value = 999;
  if(value < 1) value = 0;

  //--------------------------------------------------Hex2Bcd1Byte
  cValue[0] = (value / 100);
  cValue[1] = ((value - (100 * cValue[0])) / 10);
  cValue[2] = (value - ((cValue[0] * 100) + (cValue[1] * 10)));

  //--------------------------------------------------AsciiConversion
  for(i = 0; i < 3; i++) cValue[i] = cValue[i] + 48;
  LCD_WriteStringFont(y, x, cValue);
}



/* ------------------------------------------------------------------*
 *            Value 6x8 Neg 3Pos
 * ------------------------------------------------------------------*/

void LCD_WriteValueNeg3(unsigned char y, unsigned char x, int value)
{
  char cValue[10] = {0x00};
  int con = 0;
  unsigned char i = 0;

  con = value;

  if(con > 999) con = 999;
  if(con < 1) con = 0;

  //--------------------------------------------------Hex2Bcd1Byte
  cValue[0] = (con / 100);
  cValue[1] = ((con - (100*cValue[0])) / 10);
  cValue[2] = (con - ((cValue[0] * 100) + (cValue[1] * 10)));

  //--------------------------------------------------AsciiConversion
  for(i = 0; i < 3; i++) cValue[i] = cValue[i] + 48;
  LCD_WriteStringFontNeg(y, x, cValue);
}



/* ==================================================================*
 *            Value 4x6   2Pos
 * ==================================================================*/

void LCD_WriteValue2_MyFont(unsigned char y, unsigned char x, int value)
{
  char cValue[10] = {0x00};
  int con = 0;
  unsigned char i = 0;

  con = value;


  if(con>99) con = 99;
  if(con<1) con = 0;

  //--------------------------------------------------Hex2Bcd1Byte
  cValue[0] = (con / 10);
  cValue[1] = (con - (10 * cValue[0]));

  //--------------------------------------------------AsciiConversion
  for(i = 0; i < 2; i++) cValue[i] = cValue[i] + 48;
  LCD_WriteStringMyFont(y, x, cValue);
}


/* ------------------------------------------------------------------*
 *            Value 4x6     3Pos
 * ------------------------------------------------------------------*/

void LCD_WriteValue3_MyFont(unsigned char y, unsigned char x, int value)
{
  char cValue[10] = {0x00};
  int con = 0;
  unsigned char i = 0;

  con = value;

  if(con > 999) con = 999;
  if(con < 1) con = 0;

  //--------------------------------------------------Hex2Bcd1Byte
  cValue[0] = (con / 100);
  cValue[1] = ((con - (100 * cValue[0])) / 10);
  cValue[2] = (con - ((cValue[0] * 100) + (cValue[1] * 10)));

  //--------------------------------------------------AsciiConversion
  for(i = 0; i < 3; i++) cValue[i] = cValue[i] + 48;
  LCD_WriteStringMyFont(y, x, cValue);
}


/* ------------------------------------------------------------------*
 *            Value 4x6     4Pos
 * ------------------------------------------------------------------*/

void LCD_WriteValue4_MyFont(unsigned char y, unsigned char x, int value)
{
  char cValue[10] = {0x00};
  int con = 0;
  unsigned char i = 0;
  int v0 = 0;
  int v1 = 0;

  con = value;
  if(con > 60000) con = 60000;
  if(con < 1) con = 0;

  //--------------------------------------------------Hex2Bcd1Byte
  cValue[0] = (con / 1000);
  v0 = 1000 * cValue[0];

  cValue[1] = ((con - (v0)) / 100);
  v1 = 100 * cValue[1];
  cValue[2] = ((con - (v0 + v1)) / 10);
  cValue[3] = ((con - (v0 + v1 + (cValue[2] * 10))));

  //--------------------------------------------------AsciiConversion
  for(i = 0; i < 4; i++) cValue[i] = cValue[i] + 48;
  LCD_WriteStringMyFont(y, x, cValue);
}


/* ------------------------------------------------------------------*
 *            Value 4x6     5Pos
 * ------------------------------------------------------------------*/

void LCD_WriteValue5_MyFont(unsigned char y, unsigned char x, int value)
{
  char cValue[10] = {0x00};
  int con = 0;
  unsigned char i = 0;
  int v0 = 0;
  int v1 = 0;

  con = value;
  if(con > 60000) con = 60000;
  if(con < 1) con = 0;

  //--------------------------------------------------Hex2Bcd1Byte
  cValue[0] = (con / 10000);
  v0 = 10000 * cValue[0];

  cValue[1] = ((con - (v0)) / 1000);
  v1= 1000 * cValue[1];

  cValue[2] = ((con - (v0 + v1)) / 100);
  cValue[3] = ((con - (v0 + v1 + cValue[2]* 100)) / 10);
  cValue[4] = (con -  (v0 + v1 + cValue[2]* 100 + cValue[3] * 10));

  //--------------------------------------------------AsciiConversion
  for(i = 0; i < 5; i++) cValue[i] = cValue[i] + 48;
  LCD_WriteStringMyFont(y, x, cValue);
}



/* ==================================================================*
 *            Value 6x8       4Pos
 * ==================================================================*/

void LCD_WriteValue4(unsigned char y, unsigned char x, int value)
{
  char cValue[10] = {0x00};
  int con = 0;
  unsigned char i = 0;
  int v0 = 0;
  int v1 = 0;

  con = value;
  if(con > 60000) con = 60000;
  if(con < 1) con = 0;

  //--------------------------------------------------Hex2Bcd1Byte
  cValue[0] = (con / 1000);
  v0 = 1000 * cValue[0];

  cValue[1] = ((con - (v0)) / 100);
  v1 = 100 * cValue[1];
  cValue[2] = ((con - (v0 + v1)) / 10);
  cValue[3] = ((con - (v0 + v1 + (cValue[2] * 10))));

  //--------------------------------------------------AsciiConversion
  for(i = 0; i < 4; i++) cValue[i] = cValue[i] + 48;
  LCD_WriteStringFont(y, x, cValue);
}



/* ==================================================================*
 *            Symbols
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  LCD_WriteSymbols
 * --------------------------------------------------------------
 *  unsigned char y   - Start Page
 *  unsigned char x   - Start Collumn
 *  t_Symbol    sym   - Enum-Button to write
 * --------------------------------------------------------------
 *  Writes a negative Symbol
 * ------------------------------------------------------------------*/


void LCD_Write_Symbol_1(unsigned char row, unsigned char col, t_Symbols_35x23 sym)
{
  unsigned char LcdData[40] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  unsigned char len = Symbols_35x23_bmp[0];
  unsigned char height= Symbols_35x23_bmp[1];

  LCD_WP_SetFrame(row, col, height, len);

  for(unsigned char p = 0; p < height; p++)
  {
    for(unsigned char c = 0; c < len; c++)
    {
      L = (Symbols_35x23_bmp[2  + c + len * p + ((sym) * len * height)] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }

    LCD_SendData(LcdData, len);

    for(unsigned char a = 0; a < len; a++)
    {
      H = (Symbols_35x23_bmp[2  + a + len * p + ((sym) * len * height)] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }

    LCD_SendData(LcdData, len);
  }

  LCD_WP_Disable();
}


/* ------------------------------------------------------------------*
 *            Symbols 2 29x17
 * ------------------------------------------------------------------*/

void LCD_Write_Symbol_2(unsigned char row, unsigned char col, t_Symbols_29x17 sym)
{
  unsigned char LcdData[40] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  unsigned char len = Symbols_29x17_bmp[0];
  unsigned char height = Symbols_29x17_bmp[1];

  LCD_WP_SetFrame(row, col, height, len);

  for(unsigned char p = 0; p < height; p++)
  {
    for(unsigned char c = 0; c < len; c++)
    {
      L = (Symbols_29x17_bmp[2  + c + len * p + ((sym) * len * height)] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }

    LCD_SendData(LcdData, len);

    for(unsigned char a = 0; a < len; a++)
    {
      H = (Symbols_29x17_bmp[2  + a + len * p + ((sym) * len * height)] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }

    LCD_SendData(LcdData, len);
  }

  LCD_WP_Disable();
}


/* ------------------------------------------------------------------*
 *            Symbols 3 19x19
 * ------------------------------------------------------------------*/

void LCD_Write_Symbol_3(unsigned char row, unsigned char col, t_Symbols_19x24 sym)
{
  unsigned char LcdData[40] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  unsigned char len = Symbols_19x19_bmp[0];
  unsigned char height = Symbols_19x19_bmp[1];

  LCD_WP_SetFrame(row, col, height, len);

  for(unsigned char p = 0; p < height; p++)
  {
    for(unsigned char c = 0; c < len; c++)
    {
      L = (Symbols_19x19_bmp[2  + c + len * p + ((sym) * len * height)] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }

    LCD_SendData(LcdData, len);

    for(unsigned char a = 0; a < len; a++)
    {
      H = (Symbols_19x19_bmp[2  + a + len * p + ((sym) * len * height)] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }

    LCD_SendData(LcdData, len);
  }

  LCD_WP_Disable();
}



/* ==================================================================*
 *            Pin
 * ==================================================================*/

void LCD_Write_Pin(unsigned char row, unsigned char col, t_pinSymbols sym, unsigned char num)
{
  unsigned char LcdData[40] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  unsigned char len = Pin_34x21_bmp[0];
  unsigned char height = Pin_34x21_bmp[1];

  LCD_WP_SetFrame(row, col, height, len);

  for(unsigned char p = 0; p < height; p++)
  {
    for(unsigned char c = 0; c < len; c++)
    {
      L = (Pin_34x21_bmp[2  + c + len * p + ((sym) * len * height)] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }

    LCD_SendData(LcdData, len);

    for(unsigned char a = 0; a < len; a++)
    {
      H = (Pin_34x21_bmp[2  + a + len * p + ((sym) * len * height)] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }

    LCD_SendData(LcdData, len);
  }

  if(num < 0x20) LCD_WriteFontNum((row + 1), (col + 13), num);

  LCD_WP_Disable();
}



/* ==================================================================*
 *            Text
 * ==================================================================*/

void LCD_Write_TextButton(unsigned char row, unsigned char col, t_textButtons text, unsigned char pos)
{
  unsigned char LcdData[40] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  unsigned char len = textButton_39x16_bmp[0];
  unsigned char height = textButton_39x16_bmp[1];

  LCD_WP_SetFrame(row, col, height, len);

  for(unsigned char p = 0; p < height; p++)
  {
    for(unsigned char c = 0; c < len; c++)
    {
      L = (textButton_39x16_bmp[2  + c + len*p + ((pos)*len*height)] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }

    // write low page
    LCD_SendData(LcdData, len);

    for(unsigned char a = 0; a < len; a++)
    {
      H = (textButton_39x16_bmp[2  + a + len*p + ((pos)*len*height)] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }

    // write high page
    LCD_SendData(LcdData, len);
  }

  // positive text
  if(pos)
  {
    switch(text)
    {
      case Auto:    LCD_WriteStringFont((row+1), (col+8), "Auto");  break;
      case Manual:  LCD_WriteStringFont((row+1), (col+2), "Manual");break;
      case Setup:   LCD_WriteStringFont((row+1), (col+5), "Setup"); break;
      case Data:    LCD_WriteStringFont((row+1), (col+8), "Data");  break;
      case Sonic:   LCD_WriteStringFont((row+1), (col+5), "Sonic"); break;
      case Shot:    LCD_WriteStringFont((row+1), (col+8), "Shot");  break;
      case OpenV:   LCD_WriteStringFont((row+1), (col+5), "OpenV"); break;
      case Boot:    LCD_WriteStringFont((row+1), (col+8), "Boot");  break;
      case Read:    LCD_WriteStringFont((row+1), (col+8), "Read");  break;
      case Write:   LCD_WriteStringFont((row+1), (col+5), "Write"); break;
    }
  }
  else                  //negative Text
  {
    switch(text)
    {
      case Auto:    LCD_WriteStringFontNeg((row+1), (col+8), "Auto");   break;
      case Manual:  LCD_WriteStringFontNeg((row+1), (col+2), "Manual"); break;
      case Setup:   LCD_WriteStringFontNeg((row+1), (col+5), "Setup");  break;
      case Data:    LCD_WriteStringFontNeg((row+1), (col+8), "Data");   break;
      case Sonic:   LCD_WriteStringFontNeg((row+1), (col+5), "Sonic");  break;
      case Shot:    LCD_WriteStringFontNeg((row+1), (col+8), "Shot");   break;
      case OpenV:   LCD_WriteStringFontNeg((row+1), (col+5), "OpenV");  break;
      case Boot:    LCD_WriteStringFontNeg((row+1), (col+8), "Boot");   break;
      case Read:    LCD_WriteStringFontNeg((row+1), (col+8), "Read");   break;
      case Write:   LCD_WriteStringFontNeg((row+1), (col+5), "Write");  break;
    }
  }

  LCD_WP_Disable();
}


/* ------------------------------------------------------------------*
 *            Text Purator
 * ------------------------------------------------------------------*/

void LCD_Write_Purator(unsigned char row, unsigned char col)
{
  unsigned char LcdData[160] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  unsigned char len = Text_Purator[0];
  unsigned char height = Text_Purator[1];

  LCD_WP_SetFrame(row, col, height, len);

  for(unsigned char p = 0; p < height; p++)
  {
    for(unsigned char c = 0; c < len; c++)
    {
      L = (Text_Purator[2  + c + len*p] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }

    LCD_SendData(LcdData, len);

    for(unsigned char a = 0; a < len; a++)
    {
      H = (Text_Purator[2  + a + len*p] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }

    LCD_SendData(LcdData, len);
  }

  LCD_WP_Disable();
}


/* ------------------------------------------------------------------*
 *            Text HECS
 * ------------------------------------------------------------------*/

void LCD_Write_HECS(unsigned char row, unsigned char col)
{
  unsigned char LcdData[80] = {0x00};
  unsigned char H = 0x00;
  unsigned char L = 0x00;

  // width in dots and height in Bytes
  unsigned char len = Text_HECS[0];
  unsigned char height = Text_HECS[1];

  LCD_WP_SetFrame(row, col, height, len);

  for(unsigned char p = 0; p < height; p++)
  {
    for(unsigned char c = 0; c < len; c++)
    {
      L = (Text_HECS[2  + c + len*p] & 0x0F);
      LcdData[c] = LCD_ConvertWP(L);
    }

    LCD_SendData(LcdData, len);

    for(unsigned char a = 0; a < len; a++)
    {
      H = (Text_HECS[2  + a + len*p] & 0xF0);
      H = (H >> 4);
      LcdData[a] = LCD_ConvertWP(H);
    }

    LCD_SendData(LcdData, len);
  }

  LCD_WP_Disable();
}


/* ------------------------------------------------------------------*
 *            DeathMan
 * ------------------------------------------------------------------*/

void LCD_DeathMan(unsigned char row, unsigned char col)
{
  static unsigned char state = 1;

  if(state)
  {
    LCD_FillSpace(row, col, 1, 4);
    state = 0;
  }
  else
  {
    LCD_ClrSpace(row, col, 1, 4);
    state = 1;
  }
}



/* ==================================================================*
 *            Built-in-Functions
 * ==================================================================*/

/*-------------------------------------------------------------------*
 *  LCD_WP_SetFrame
 * --------------------------------------------------------------
 *  parameter:  unsigned char row   - Start Page
 *        unsigned char col   - Start Collumn
 * --------------------------------------------------------------
 *  return:     void
 *  Descirption:
 * --------------------------------------------------------------
 *  Writes a negative Symbol
 * ------------------------------------------------------------------*/


void LCD_WP_SetFrame(unsigned char row, unsigned char col, unsigned char height, unsigned char len)
{ 
  // page and column address
  LCD_SetPageAddress(row);
  LCD_SetColumnAdress(col);

  // set frame
  LCD_WP_Enable();
  LCD_WP_Page(row, (row + (height * 2)));
  LCD_WP_Column(col, (col + len - 1));
}



/*-------------------------------------------------------------------*
 *  LCD_ConvertWP
 * --------------------------------------------------------------
 *  Converts a Nibble to the LED Standards of the Display:
 *  Each Pixel in the H is converted to 2 Bits, 11 Px On - 00 Px Off
 * ------------------------------------------------------------------*/

unsigned char LCD_ConvertWP(unsigned char con)
{
  unsigned char convert = 0;

  if((con & 0x01) == 0x01) convert = convert + 0x03;
  if((con & 0x02) == 0x02) convert = convert + 0x0C;
  if((con & 0x04) == 0x04) convert = convert + 0x30;
  if((con & 0x08) == 0x08) convert = convert + 0xC0;

  return convert;
}