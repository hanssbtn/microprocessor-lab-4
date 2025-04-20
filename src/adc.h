#ifndef ADC__
#define ADC__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

void init_adc_pins(void);
int16_t read_adc_pin(void);

#endif // ADC__