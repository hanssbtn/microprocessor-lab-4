#include "timer.h"

void init_timer0(void) {
	// Turn off timer0 by zeroing out the TCCR0B register
	TCCR0B = 0;
	// Change mode to CTC
	TCCR0A |= (1 << WGM01);
	// Set appropriate OCR0A value
	OCR0A = 250;
}

void init_timer1(void) {
	TCCR1B = (1 << WGM12); // Turn off timer1
	TCCR1A = 0;
	// Target delay 1 s = 1/(CPU Frequency) * OCR1A * Prescaler
	// OCR1A = 1 * 16 * 10 ^ 6 / 256
	OCR1A = 62500;
}

void delay_10ms(void) {
	// Set prescaler to 64
	TCCR0B |= (1 << CS01) | (1 << CS00);
	TCNT0 = 0; // Clear timer counter before use
	for (unsigned int count = 0; count < 10; ) {
		while (!(TIFR0 & (1 << OCF0A)));
		// Increment `count` by 1 when the timer has been cleared
		count++;
		TIFR0 |= (1 << OCF0A); // Clear timer compare flag just in case
	}
	TCCR0B = 0; // Turn off timer0
	TIFR0 |= (1 << OCF0A); // Clear timer compare flag just in case
}

void delay_1ms(void) {
	// Set prescaler to 64
	TCCR0B |= (1 << CS01) | (1 << CS00);
	TCNT0 = 0; // Clear timer counter before use
	while (!(TIFR0 & (1 << OCF0A)));
	TIFR0 |= (1 << OCF0A); // Clear timer compare flag just in case
	TCCR0B = 0; // Turn off timer0
	TIFR0 |= (1 << OCF0A); // Clear timer compare flag just in case
}

void delay_1us(void){
	// Set prescaler to 8
	TCCR0B |= 1 << CS01;
	OCR0A = 2; // Set OCR0A to appropriate value
	TCNT0 = 0; // Clear timer counter just in case
	while (!(TIFR0 & (1 << OCF0A))); // Wait until timer0 compare A flag is set
	TCCR0B = 0; // Turn off timer0
	TIFR0 |= 1 << OCF0A; // Clear timer compare flag just in case
}

void delay_1s(void) {
	TCNT1 = 0;
	// Set prescaler to 256
	TCCR1B |= 1 << CS12;
	while (!(TIFR1 & (1 << OCF1A)));
	TIFR1 |= 1 << OCF1A;
	TCCR1B = 1 << WGM12;
}