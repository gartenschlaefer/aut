// --
// EADOGXL160-7 Display driver

// Include guard
#ifndef LCD_DRIVER_H   
#define LCD_DRIVER_H


/* ==================================================================*
 *            Defines
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Backlight 30000 -> ca. 3min
 * ------------------------------------------------------------------*/

#define BACKLIGHT_TON_FRAMES (3000)
#define BACKLIGHT_ERROR_ON (400)
#define BACKLIGHT_ERROR_OFF (2000)

/* ------------------------------------------------------------------*
 *            PORT
 * ------------------------------------------------------------------*/

#define LCD_RST_DIR   (PORTC.DIRSET = PIN2_bm)
#define LCD_LED_DIR   (PORTC.DIRSET = PIN3_bm)

#define LCD_RST_OFF   (PORTC.OUTSET = PIN2_bm)
#define LCD_RST_ON    (PORTC.OUTCLR = PIN2_bm)

#define LCD_LED_ON    (PORTC.OUTSET = PIN3_bm)
#define LCD_LED_OFF   (PORTC.OUTCLR = PIN3_bm)


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

#define Set_All_Pixels_On_Off          0xA4

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

#define W_CMD         (0x78>>1)
#define W_DATA        (0x7A>>1)
#define R_DATA        (0x7B>>1)
#define R_STATUS      (0x79>>1)



/* ------------------------------------------------------------------*
 *            Symbols
 * ------------------------------------------------------------------*/

typedef enum
{
  n_pumpOff,  n_mud,  n_inflowPump, n_pump2,
  p_pumpOff,  p_mud,  p_inflowPump, p_pump2
}t_Symbols_35x23;


typedef enum
{
  n_setDown,    n_alarm,      n_air,      n_sensor, n_watch,
  n_compressor, n_circulate,  n_cal,      n_zone,   n_level,
  p_setDown,    p_alarm,      p_air,      p_sensor, p_watch,
  p_compressor, p_circulate,  p_cal,      p_zone,   p_level
}t_Symbols_29x17;


typedef enum
{
  n_phosphor, n_pump,       n_esc,        n_plus,
  n_minus,    n_arrowUp,    n_arrowDown,  n_ok,   n_grad,
  n_sonic,    n_arrowRedo,
  p_phosphor, p_pump,       p_esc,        p_plus,
  p_minus,    p_arrowUp,    p_arrowDown,  p_ok,
  p_line,     p_grad,       p_sonic,      p_arrowRedo
}t_Symbols_19x24;


typedef enum
{
  frame,      p_escape,   p_del,
  black,      n_escape,   n_del
}t_pinSymbols;


typedef enum
{
  Auto,   Manual, Setup,  Data,
  Sonic,  Shot,   OpenV,  Boot,
  Read,   Write
}t_textButtons;



/* ------------------------------------------------------------------*
 *            Setup Page Symbols
 * ------------------------------------------------------------------*/

typedef enum
{
  sn_circulate, sn_air,     sn_setDown,   sn_pumpOff,
  sn_mud,     sn_compressor,  sn_phosphor,  sn_inflowPump,
  sn_cal,     sn_alarm,     sn_watch,   sn_zone,
  sp_circulate, sp_air,     sp_setDown,   sp_pumpOff,
  sp_mud,     sp_compressor,  sp_phosphor,  sp_inflowPump,
  sp_cal,     sp_alarm,     sp_watch,   sp_zone,
}t_SetupSym;



/* ------------------------------------------------------------------*
 *            Control Buttons
 * ------------------------------------------------------------------*/

typedef enum
{
  sn_plus,    sn_minus,   sn_esc,   sn_ok,
  sp_plus,    sp_minus,   sp_esc,   sp_ok
}t_CtrlButtons;



/* ------------------------------------------------------------------*
 *            Date Time Page
 * ------------------------------------------------------------------*/

typedef enum
{
  n_h,    n_min,    n_day,    n_month,    n_year,
  p_h,    p_min,    p_day,    p_month,    p_year
}t_DateTime;





/* ==================================================================*
 *            FUNCTIONS - API
 * ==================================================================*/

/* ------------------------------------------------------------------*
 *            Config and Transfer
 * ------------------------------------------------------------------*/

void          LCD_Init            (void);
unsigned char LCD_SendCmd         (unsigned char* SCmd, unsigned char i);
void          LCD_SendData        (unsigned char* SData, unsigned char i);
void          LCD_SetPageAddress  (unsigned char PA);
void          LCD_SetColumnAdress (unsigned char CA);

/* ------------------------------------------------------------------*
 *            Commands
 * ------------------------------------------------------------------*/

void LCD_Rst            (void);
void LCD_HardwareRst    (void);
void LCD_Clean          (void);

void LCD_Backlight      (t_FuncCmd cmd);


/* ------------------------------------------------------------------*
 *            Window Programming
 * ------------------------------------------------------------------*/

void LCD_WP_Enable  (void);
void LCD_WP_Disable (void);
void LCD_WP_Column  (unsigned char startCA, unsigned char endCA);
void LCD_WP_Page    (unsigned char startPA, unsigned char endPA);


/* ------------------------------------------------------------------*
 *            Write Font
 * ------------------------------------------------------------------*/

void LCD_WriteFont      (unsigned char row, unsigned char col, unsigned short word);
void LCD_WriteFontNeg   (unsigned char row, unsigned char col, unsigned short word);
void LCD_WriteFontNum   (unsigned char row, unsigned char col, unsigned char word);
void LCD_WriteMyFont    (unsigned char row, unsigned char col, unsigned char word);
void LCD_WriteMyFontNeg (unsigned char row, unsigned char col, unsigned char word);

void LCD_WriteStringFont      (unsigned char y, unsigned char x, char word[]);
void LCD_WriteStringFontNeg   (unsigned char y, unsigned char x, char word[]);
void LCD_WriteStringMyFont    (unsigned char y, unsigned char x, char word[]);


/* ------------------------------------------------------------------*
 *            Write Value
 * ------------------------------------------------------------------*/

void LCD_WriteValue2  (unsigned char y, unsigned char x, int value);
void LCD_WriteValueNeg2 (unsigned char y, unsigned char x, int value);
void LCD_WriteValue3  (unsigned char y, unsigned char x, int value);
void LCD_WriteValueNeg3 (unsigned char y, unsigned char x, int value);
void LCD_WriteValue4  (unsigned char y, unsigned char x, int value);

void LCD_WriteValue2_MyFont(unsigned char y, unsigned char x, int value);
void LCD_WriteValue3_MyFont(unsigned char y, unsigned char x, int value);
void LCD_WriteValue4_MyFont(unsigned char y, unsigned char x, int value);
void LCD_WriteValue5_MyFont(unsigned char y, unsigned char x, int value);


/* ------------------------------------------------------------------*
 *            Write Symbols
 * ------------------------------------------------------------------*/

void LCD_Write_Symbol_1 (unsigned char row, unsigned char col, t_Symbols_35x23 sym);
void LCD_Write_Symbol_2 (unsigned char row, unsigned char col, t_Symbols_29x17 sym);
void LCD_Write_Symbol_3 (unsigned char row, unsigned char col, t_Symbols_19x24 sym);


/* ------------------------------------------------------------------*
 *            Write Pin + Text
 * ------------------------------------------------------------------*/

void LCD_Write_Pin      (unsigned char row, unsigned char col, t_pinSymbols sym, unsigned char num);
void LCD_Write_TextButton (unsigned char row, unsigned char col, t_textButtons text, unsigned char pos);
void LCD_Write_Purator    (unsigned char row, unsigned char col);
void LCD_Write_HECS     (unsigned char row, unsigned char col);

void LCD_DeathMan     (unsigned char row, unsigned char col);


/* ------------------------------------------------------------------*
 *            Built in Functions
 * ------------------------------------------------------------------*/

unsigned char   LCD_ConvertWP (unsigned char con);
void      LCD_WP_SetFrame (unsigned char row, unsigned char col, unsigned char height, unsigned char len);

void      LCD_FillSpace (unsigned char row, unsigned char col, unsigned char height, unsigned char len);
void      LCD_ClrSpace  (unsigned char row, unsigned char col, unsigned char height, unsigned char len);

#endif


