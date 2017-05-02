/*
 */

#include <avr/io.h>

#include "basic.h"
#include "output.h"

int main(void)
{
  OUT_init();

  while(1)
  {
    if(S5_DOOR_CLOSE)
    {
      OUT_setSignalLamp();
    }
    else
    {
      OUT_resetSignalLamp();
    }
  }

  return 0;
}
