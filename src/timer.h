#ifndef TIMER__
#define TIMER__

#include <avr/io.h>
#include <avr/interrupt.h>

void init_timer0(void);
void init_timer1(void);
void delay_1ms(void);
void delay_10ms(void);
void delay_1us(void);
void delay_1s(void);

#endif // TIMER__
