#include <avr/interrupt.h>
#include <util/delay.h>
#include "dac.h"

int main(void)
{
	DDRB |= _BV(7);
	init_dac();
	sei();

	uint8_t dac = 0;
inc:
	set_dac(dac);
	_delay_ms(10);
	if (++dac != 255)
		goto inc;
dec:
	set_dac(dac);
	_delay_ms(10);
	if (--dac != 0)
		goto dec;
	goto inc;

	return 1;
}
