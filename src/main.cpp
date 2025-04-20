#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"
#include "switch.h"
#include "timer.h"
#include "pwm.h"
#include "motor.h"
#include "segment.h"

#define BEFORE_PRESS 0
#define PRESSED 1
#define RELEASED 2
#define AFTER_RELEASE 3

volatile char state = BEFORE_PRESS;

int16_t main(void) {
	init_timer0();
	init_timer1();
	init_timer3(); 
    init_adc_pins();    // Sets up ADC on PF0 (analog pin 0)
    init_motor_pins();  // Sets up PH3, PH4, PE3 direction
    init_switch_pin();  // Sets up INT0 (PD0)
	init_segment_pins();
	sei();
	while (1) {
		switch (state) {
			case PRESSED: {
				state = RELEASED;
				delay_1ms();
			} break;
			case AFTER_RELEASE: {
				// Disable global interrupts to prevent button from 
				// being pressed here as it could mess up the state, 
				// such as preventing proper re-init of the ADC
				cli();
				disable_motor_control();
				countdown();
				enable_motor_control();
				state = BEFORE_PRESS;
				// Re-enable global interrupt to re-enable button press
				sei();
				delay_10ms();
			} break;
			default:
				break;
		}
		change_duty_cycle(read_adc_pin());
	}
	return 0;
}

ISR(INT0_vect) {
	switch (state) {
		case BEFORE_PRESS:
			state = PRESSED;
			break;
		case RELEASED:
			state = AFTER_RELEASE;
			break;
		default:
			break;
	}
}