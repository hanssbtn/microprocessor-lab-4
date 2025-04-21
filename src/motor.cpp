#include "motor.h"
#include "pwm.h"

/*
* Key Features:
*   - Bi-directional DC motor control
*   - GPIO initialization for L293D driver
*   - Integrated with Timer3 PWM for speed control
* Hardware Connections:
*   - PH3: IN2 (Direction Control)
*   - PH4: IN1 (Direction Control)
*   - PE3: EN1 (PWM Speed Control via OC3A)
* Critical Dependencies:
*   - pwm.h for Timer3 PWM generation
*   - motor.h for pin definitions
*/

// Motor Pin Initialization
void init_motor_pins(void) {
	/*
	 * DDRH = (1 << DDH3) | (1 << DDH4);
	 * Data Direction Register H - Configure Direction Pins as Outputs
	 * 
	 * Register Breakdown:
	 * DDRH: [DDH7][DDH6][DDH5][DDH4][DDH3][DDH2][DDH1][DDH0]
	 * 
	 * Bit Settings:
	 * - DDH3:1 → PH3 (IN2) as output
	 * - DDH4:1 → PH4 (IN1) as output
	 * 
	 * L293D Interface:
	 * IN1 | IN2 | Motor Action
	 * ----|-----|------------
	 *  0  |  0  | Fast motor stop (brake)
	 *  0  |  1  | Clockwise rotation
	 *  1  |  0  | Counter-clockwise rotation
	 *  1  |  1  | Fast motor stop (brake)
	 * 
	 * Why Set Both Direction Pins?
	 * - Ensures clean startup state
	 * - Prevents floating inputs on L293D
	 */
	DDRH = (1 << DDH3) | (1 << DDH4);

	/*
	 * DDRE = (1 << DDE3);
	 * Data Direction Register E - Configure Enable Pin as Output
	 * 
	 * Hardware Connection:
	 * - PE3 (OC3A) serves dual purpose:
	 *   * GPIO when PWM disabled
	 *   * PWM output when enabled
	 * 
	 * Critical Design Choice:
	 * - Must be output regardless of PWM state
	 * - DDR bits must be set before using PWM
	 */
	DDRE = (1 << DDE3);

	/*
	 * PORTH = 0;
	 * PORT H Data Register - Initialize Direction Pins Low
	 * 
	 * Safety Implications:
	 * - IN1=IN2=0 → L293D in brake mode
	 * - Prevents accidental motor movement during startup
	 * 
	 * Electrical Consideration:
	 * - 0V on both inputs when MCU resets
	 * - L293D outputs will be high impedance
	 */
	PORTH = 0;

	/*
	 * PORTE = 0;
	 * PORT E Data Register - Initialize Enable Pin Low
	 * 
	 * Why Important?
	 * - PE3 low ensures L293D disabled at startup
	 * - Even if PWM accidentally enabled, duty cycle=0
	 * - Meets project requirement: "Motor off initially"
	 */
	PORTE = 0;
}

/* 
 * NOTE:
 *
 * HARDWARE ANALYSIS:
 * 
 * 1. L293D DRIVER TRUTH TABLE:
 *    +---------+---------+---------------------+------------------+
 *    |   IN1   |   IN2   | ENABLE (PWM) State  | Motor Behavior   |
 *    +---------+---------+---------------------+------------------+
 *    |    0    |    0    |         X           | Brake (Coast)    |
 *    |    0    |    1    |        High         | Clockwise        |
 *    |    0    |    1    |        PWM          | Speed Control CW |
 *    |    1    |    0    |        High         | Counter-CW       |
 *    |    1    |    0    |        PWM          | Speed Control CCW|
 *    |    1    |    1    |         X           | Brake (Active)   |
 *    +---------+---------+---------------------+------------------+
 * 
 * 2. CURRENT HANDLING:
 *    - ATmega2560 GPIO pins can source/sink 20mA max
 *    - L293D input current ~100μA (negligible load)
 *    - Always keep unused GPIOs as inputs to reduce power
 * 
 * 3. NOISE MITIGATION:
 *    - 100nF ceramic capacitor between L293D VCC and GND
 *    - Separate power supply for motors if >500mA
 *    - Twisted pair wiring for motor connections
 * 
 * 4. FAIL-SAFE MECHANISMS:
 *    - All control pins initialized low
 *    - PWM starts disabled
 *    - Direction pins explicitly set before enabling
 * 
 * 5. POWER SEQUENCING:
 *    1. Initialize GPIO states
 *    2. Configure PWM timer
 *    3. Enable PWM only after full initialization
 *    4. Change directions only when PWM disabled
 */