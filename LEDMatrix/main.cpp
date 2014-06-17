#include <avr/interrupt.h>
#include "led.h"

int main(void)
{
	led::init();
	sei();
	for (uint8_t j = 0; j < LED_H; j++)
		for (uint8_t k = 0; k < LED_W / 8; k++) {
			led::buff[j][k][led::Red] = 0x55;
			led::buff[j][k][led::Green] = 0xAA;
		}
	while (1);
	return 1;
}
