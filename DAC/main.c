#include <avr/interrupt.h>
#include <util/delay.h>
#include "dac.h"

#define DAC_CHANNEL	0

int main(void)
{
	DDRB |= _BV(7);
	initDAC();
	sei();

	uint8_t dac = 0;
inc:
	setDAC(DAC_CHANNEL, dac);
	_delay_ms(10);
	if (++dac != 255)
		goto inc;
dec:
	setDAC(DAC_CHANNEL, dac);
	_delay_ms(10);
	if (--dac != 0)
		goto dec;
	goto inc;

	return 1;
}
