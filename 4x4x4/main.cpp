#include <avr/interrupt.h>
#include <util/delay.h>
#include "led.h"

using namespace led;

int main(void)
{
	led::init();
	sei();

	//led::fill(true);
	led::buff[0] = 0x5A5A;
	_delay_ms(1000);
	led::buff[3] = 0xA5A5;

	while (1);
	return 1;
}
