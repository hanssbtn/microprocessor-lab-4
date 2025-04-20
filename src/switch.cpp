#include "switch.h"

void init_switch_pin(void) {
	DDRD = 0;
	PORTD = (1 << PD0);
	EIMSK |= (1 << INT0); // enable external interrupts 
	EICRA = (EICRA | (1 << ISC00)) & ~(1 << ISC01);
}