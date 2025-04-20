#include "segment.h"
#include "timer.h"

static const uint8_t segment_map[] PROGMEM = {
    0x6F, // 9: segments a,b,c,d,f,  g
    0x7F, // 8: segments a,b,c,d,e,f,g
    0x07, // 7: segments a,b,c
    0x7D, // 6: segments a,  c,d,e,f,g
    0x6D, // 5: segments a,  c,d,  f,g
    0x66, // 4: segments b,  c,    f,g
    0x4F, // 3: segments a,b,c,d,    g
    0x5B, // 2: segments a,b,  d,e,  g
    0x06, // 1: segments   b,c
    0x3F, // 0: segments a,b,c,d,e,f
};

void init_segment_pins(void) {
	DDRC = 0x7F;
	PORTC = 0;
}

void countdown(void) {
	for (int i = 0; i < 10; ++i) {
		PORTC = pgm_read_byte(&segment_map[i]);
		delay_1s();
	}
	PORTC = 0;
} 