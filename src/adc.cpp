#include "adc.h"

/*
 * Implements ALL ADC requirements from project spec:
 * 1. Initializes ADC on A0 (PF0)
 * 2. Uses AVCC reference (5V)
 * 3. Right-adjusted 10-bit results
 * 4. Proper prescaling for 125kHz ADC clock
 */

/*
 * Reads the current analog value from ADC pin (A0/PF0)
 * Return: 10-bit value (0-1023) representing 0V-5V
 * 
 */
int16_t read_adc_pin(void) {
    /*
     * ADCSRA |= (1 << ADSC);
     * ADC Control and Status Register A - Start Conversion
     * 
     * Register Breakdown:
     * ADCSRA: [ADEN][ADSC][ADATE][ADIF][ADIE][ADPS2][ADPS1][ADPS0]
     * 
     * Binary Operation:
     * (1 << ADSC) = 0b01000000
     * |= sets only this bit (preserves other settings)
     * 
     * Hardware Behavior:
     * - Sets the ADC Start Conversion bit
     * - Begins analog-to-digital conversion process
     * - Conversion takes 13 ADC clock cycles (104µs @ 125kHz)
     */
    ADCSRA |= (1 << ADSC);

    /*
     * while (ADCSRA & (1 << ADSC));
     * Wait for conversion completion
     * 
     * Hardware Behavior:
     * - ADSC bit automatically clears when conversion finishes
     * - This creates a blocking wait (CPU spins until done)
     * - Better than delay() as it responds immediately when done
     * 
     * Why not use interrupts?
     * - For simple polling reads, this is more efficient
     * - Avoids ISR overhead for single conversion
     */
    while (ADCSRA & (1 << ADSC));

    /*
     * return static_cast<int16_t>(ADC);
     * Reads the ADC Data Register
     * 
     * Register Structure:
     * ADC: [ADCH (high byte)][ADCL (low byte)]
     * 
     * Right-adjusted 10-bit mode:
     * - ADCL contains bits 0-7
     * - ADCH contains bits 8-9
     * - Bits 10-15 are always 0
     * 
     * Voltage Calculation:
     * Digital Value = (Vin * 1023) / Vref
     * Where Vref = 5V (AVCC)
     */
    return static_cast<int16_t>(ADC);
}

/*
 * Initializes ADC hardware and configures A0 (PF0) as analog input
 */
void init_adc_pins(void) {
    /*
     * DDRF &= ~(1 << DDF0);
     * Data Direction Register F - Sets PF0 as input
     * 
     * Binary Operation:
     * ~(1 << DDF0) = 0b11111110
     * &= clears only bit 0 (DDF0)
     * 
     * Hardware Impact:
     * - Configures PF0 in high-impedance input mode
     * - Essential for analog input (output mode would damage ADC)
     */
    DDRF &= ~(1 << DDF0);

    /*
     * PORTF &= ~(1 << PORTF0);
     * PORTF Data Register - Disables pull-up on PF0
     * 
     * Why disable pull-up?
     * - Analog inputs should NEVER use pull-up resistors
     * - Pull-up would distort voltage measurements
     * - External potentiometer provides proper impedance
     */
    PORTF &= ~(1 << PORTF0);

    /*
     * ADMUX = (1 << REFS0);
     * ADC Multiplexer Register - Configuration
     * 
     * Register Breakdown:
     * ADMUX: [REFS1][REFS0][ADLAR][MUX4][MUX3][MUX2][MUX1][MUX0]
     * 
     * Settings Applied:
     * - REFS0=1: Use AVCC (5V) as reference (Requirement: 0V-5V range)
     * - ADLAR=0: Right-adjusted result (normal 10-bit mode)
     * - MUX4:0=00000: Select ADC0 channel (PF0)
     * 
     * Binary Math:
     * (1 << REFS0) = 0b01000000
     */
    ADMUX = (1 << REFS0);

    /*
     * ADC Prescaler Configuration
     * int pre = (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
     * 
     * Clock Division:
     * - ADPS2:0 = 111 → Division factor 128
     * - 16MHz/128 = 125kHz ADC clock
     * 
     * Why 125kHz?
     * - ATmega2560 ADC works best at 50-200kHz
     * - Slower = better accuracy (less noise)
     * - Faster = quicker conversions
     * - 125kHz is ideal balance (Requires 13 cycles = 104µs/conversion)
     */
    int pre = (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

    /*
     * ADCSRA = (1 << ADEN) | (1 << ADSC) | pre;
     * ADC Control and Status Register A - Full configuration
     * 
     * Settings Applied:
     * - ADEN=1: Enable ADC
     * - ADSC=1: Start initial conversion
     * - Prescaler: Set to 128 (from 'pre' calculation)
     * 
     * Why initial conversion?
     * - First conversion after enable is often inaccurate
     * - Warming up the ADC circuitry
     * - Project Requirement: "First conversion needed for initialization"
     */
    ADCSRA = (1 << ADEN) | (1 << ADSC) | pre;

    /*
     * while (ADCSRA & (1 << ADSC));
     * Wait for initial conversion to complete
     * 
     * Critical Detail:
     * - Ensures ADC is fully initialized before use
     * - Prevents reading unstable values
     * - Matches project requirement about first conversion
     */
    while (ADCSRA & (1 << ADSC));
}

/* 
 * NOTES:
 * 
 * 1. POTENTIOMETER INTERACTION:
 *    - Potentiometer forms voltage divider between 5V and GND
 *    - Wiper connects to PF0 (ADC0)
 *    - Voltage at wiper: Vout = (R2 / (R1 + R2)) * Vin
 *    - ADC converts this to digital value (0-1023)
 * 
 * 2. ADC TIMING:
 *    Conversion Time = 13 ADC clocks = 13 * (1/125kHz) = 104µs
 *    Maximum Sampling Rate = 1/104µs ≈ 9.6kHz
 * 
 * 3. NOISE CONSIDERATIONS:
 *    - 100nF capacitor near AREF pin recommended
 *    - Keep analog traces short
 *    - Avoid digital signals near analog inputs
 * 
 * 4. RESOLUTION:
 *    10-bit = 1024 steps
 *    Voltage per step = 5V / 1024 ≈ 4.88mV
 */