/*
sets the output
 */

// Relay Output
#define SIGLAMP_SET    (PORTA |= (1 << PA0))
#define SIGLAMP_RESET  (PORTA &= ~(1 << PA0))

// FU Output
#define FU1_SET_OPEN      (PORTA |= (1 << PA1))
#define FU1_RESET_OPEN    (PORTA &= ~(1 << PA1))
#define FU2_SET_CLOSE     (PORTA |= (1 << PA2))
#define FU2_RESET_CLOSE   (PORTA &= ~(1 << PA2))
#define FU4_SET_RST       (PORTA |= (1 << PA4))
#define FU4_RESET_RST     (PORTA &= ~(1 << PA4))

// Switch Input
#define S7_RESERVE    (!(PINB & (1 << PB2)))
#define S6_ESTOP      (!(PINB & (1 << PB3)))
#define S5_GATE_CLOSE (!(PINB & (1 << PB4)))
#define S5_GATE_OPEN  (!(PINB & (1 << PB5)))
#define S4_MANUAL     (!(PINB & (1 << PB6)))
#define S4_AUTO       (!(PINB & (1 << PB7)))

#define S3_RESERVE  (!(PINC & (1 << PC0)))
#define S2_END      (!(PINC & (1 << PC1)))
#define S1_END      (!(PINC & (1 << PC2)))




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
void Gate_algorithm(struct Gate *Gate1);
