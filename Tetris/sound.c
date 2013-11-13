#include <avr/io.h>
#include <util/delay.h>
#include "sound.h"

void Sound_init(void)
{
	Sound_DDR |= Sound_IO;
	Sound_PORT &= ~Sound_IO;
}

// Try to be more accuracy
void inline delay(uint8_t ms25)
{
	while (ms25--)
		_delay_us(25);
}

// (freq)kHz for (ms)ms
void Sound(uint8_t freq, uint8_t ms)
{
	uint8_t m;
	for (m = 0; m < freq * ms; m++) {
		Sound_PORT |= Sound_IO;
		delay(20 / freq);
		Sound_PORT &= ~Sound_IO;
		delay(20 / freq);
	}
}
