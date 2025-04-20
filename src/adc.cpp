#include "adc.h"

int16_t read_adc_pin(void) {
    // Trigger conversion start
    ADCSRA |= (1 << ADSC);

    // Wait until conversion finishes (hardware clears ADSC) 
    while (ADCSRA & (1 << ADSC));
	// ADCH is automatically casted to int when shifting
    return static_cast<int16_t>(ADC);
}

void init_adc_pins(void) {
	// Set pin PF0 as input
    DDRF &= ~(1 << DDF0);
    // Disable pull-up resistor on PF0 (optional but good practice for ADC)
    PORTF &= ~(1 << PORTF0);

    // Set reference to AVCC (5V) (REFS0 == 1), right-adjust result (ADLAR == 0)
    // Select ADC0 channel (MUX4:0 = 00000)
    ADMUX = (1 << REFS0);
	
    // Set ADC prescaler to 128 (ADPS2=1, ADPS1=1, ADPS0=1)
    // 16MHz / 128 = 125kHz ADC clock
    int pre = (1 << ADPS0) \
		| (1 << ADPS1) \
		| (1 << ADPS2) \
		;

    // Enable ADC (ADEN), start first conversion (ADSC), set prescaler
    ADCSRA = (1 << ADEN) | (1 << ADSC) | pre;

    // Wait for the first conversion to complete.
	// IMPORTANT: This is needed to fully initialize the ADC.
    while (ADCSRA & (1 << ADSC));
}