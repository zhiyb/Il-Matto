#include <avr/interrupt.h>
#include <util/delay.h>
#include "led.h"

int main(void)
{
	led::init();

loop:
	led::send(0x55);
	_delay_ms(1);
	led::send(0xAA);
	_delay_ms(1);
	goto loop;
	uint8_t data = 1;
	for (uint8_t i = 0; i < 8; i++) {
		led::send(data);
		data <<= 1;
		_delay_ms(1);
	}
	goto loop;

	return 0;
}
