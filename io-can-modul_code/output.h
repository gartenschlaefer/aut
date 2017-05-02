/*
 */

#define S5_DOOR_OPEN  (!(PINB & (1<<PB4)))
#define S5_DOOR_CLOSE (!(PINB & (1<<PB5)))

void OUT_init(void);
void OUT_setSignalLamp(void);
void OUT_resetSignalLamp(void);
