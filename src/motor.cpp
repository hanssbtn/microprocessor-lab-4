#include "motor.h"
#include "pwm.h"

// motor pin IN1 = PH4
// motor pin IN2 = PH3
// motor pin EN1 = PE3

void init_motor_pins(void) {
	DDRH = (1 << DDH3) | (1 << DDH4);
	DDRE = (1 << DDE3);
	PORTH = 0;
	PORTE = 0;
}
