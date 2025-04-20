#ifndef PWM__
#define PWM__

#include "adc.h"

void init_timer3(void);
void change_duty_cycle(int16_t analog_magnitude);
void enable_motor_control(void);
void disable_motor_control(void);

#endif // PWM__