// --
// debug

#include "twi_func.h"
#include "lcd_driver.h"
#include "utils.h"


/* ------------------------------------------------------------------*
 *            debug two wire interface, i2c, for raspberry
 * ------------------------------------------------------------------*/

void debug_loog_twi_rpi(void)
{
  // test data
  unsigned char rpi_test_data[2] = { 0x00, 0xA5 };

  int a = 0;
  int b = 0;
  // const int a_lim = 30000;
  // const int b_lim = 500;

  const int a_lim = 100;
  const int b_lim = 5;
  const unsigned char addr = 0xFF;

  while(1)
  {
    WDT_RESET;
    a++;
    //LCD_WriteAnyValue(_f_6x8, 3, 0, 45, rpi_test_data[0], false);
    if(a > a_lim)
    {
      a = 0;
      b++;
      if(b > b_lim)
      {
        b = 0;
        // watchdog
        WDT_RESET;

        // write
        TWI_D_Master_WriteString(addr, rpi_test_data, 2);
        //LCD_WriteAnyValue(_f_6x8, 2, 0, 45, addr, false);
        //LCD_WriteAnyValue(_f_6x8, 3, 2, 45, rpi_test_data[0], false);
        rpi_test_data[0] += 1;
      }
    }
  }
}