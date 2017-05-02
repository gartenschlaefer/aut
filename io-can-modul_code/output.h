/*
 */

#define FU1_SET_OPEN      (PORTA |= (1 << PA1))
#define FU1_RESET_OPEN    (PORTA &= ~(1 << PA1))

#define FU2_SET_CLOSE     (PORTA |= (1 << PA2))
#define FU2_RESET_CLOSE   (PORTA &= ~(1 << PA2))

#define FU4_SET_RST       (PORTA |= (1 << PA4))
#define FU4_RESET_RST     (PORTA &= ~(1 << PA4))

#define S5_GATE_CLOSE (!(PINB & (1 << PB4)))
#define S5_GATE_OPEN  (!(PINB & (1 << PB5)))

//*-*Time To Move
#define TIME_GATE_TO_MOVE   5

struct Gate {
   unsigned char switch_open;
   unsigned char switch_close;
   unsigned char move_open;
   unsigned char move_close;
   unsigned char end_cycle;
   int time_wait;
};

void OUT_init(void);
void OUT_setSignalLamp(void);
void OUT_resetSignalLamp(void);
void OUT_setFuGateOpen(void);
void OUT_resetFuGateOpen(void);

void Gate_init(struct Gate *MyGate);
