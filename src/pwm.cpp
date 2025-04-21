#include "pwm.h"
#include "motor.h"

void init_timer3(void) {
    // Mode 7: Fast PWM, 10-bit (TOP = 0x3FF)
    // Non-inverting mode on OC3A (Clear OC3A on Compare Match, set OC3A at 0)
    TCCR3A = (1 << COM3A1) | (1 << WGM31) | (1 << WGM30);
    // WGM32 for Mode 7, No prescaling (CS30 == 1)
    TCCR3B = (1 << WGM32) | (1 << CS30);
	// Turn off all digital input registers to limit power consumption
	DIDR0 = 0xFF;
}

// Motor Control Constants
#define ADC_MAX 0x3FF       // 10-bit ADC maximum (1023)
#define ADC_CENTER (ADC_MAX / 2)  // Center point (511)
#define PWM_MAX 0x3FF       // 10-bit PWM maximum (1023)
#define DEAD_ZONE 0        // ±0 count neutral zone (adjust if needed)

// Duty Cycle Adjustment (Direction + Speed Control)
void change_duty_cycle(int16_t analog_magnitude) {
    uint16_t duty_cycle = 0;

	/*
	* Counter-clockwise Rotation (Left of Center)
	* analog_magnitude range: 0 to (ADC_CENTER - DEAD_ZONE - 1)
	*/
    if (analog_magnitude < (ADC_CENTER - DEAD_ZONE)) {
        duty_cycle = (ADC_CENTER - analog_magnitude) * 2;        
        // Set direction pins to move counter-clockwise (PH4 == 1, PH3 == 0)
        PORTH = (1 << PH4);

    // Clockwise
    } else if (analog_magnitude > (ADC_CENTER + DEAD_ZONE)) {
        // Calculate speed: Map [ADC_CENTER + DEAD_ZONE + 1, ADC_MAX] to [0, PWM_MAX]
        duty_cycle = (analog_magnitude - (ADC_CENTER + 1)) * 2;
        // Set direction pins to move clockwise (PH4 == 0, PH3 == 1)
        PORTH = (1 << PH3);
	// Stop
    } else {
        // Use bitwise operations to set direction pins low (brake).
        PORTH = 0;
    }

    // Set the PWM duty cycle register for Timer3 Output Compare A (OC3A = PE3)
    OCR3A = duty_cycle;
}


void enable_motor_control(void) {
	// Enable Timer3
    TCCR3A |= (1 << COM3A1); 
}

void disable_motor_control(void) {
    // Disconnect Timer3 PWM output from pin PE3 by disconnecting OC3A
    TCCR3A &= ~(1 << COM3A1); 

    // Turn off motor
	OCR3A = 0;  // Set duty cycle to 0
    PORTE = 0;  // Force PE3 low
}