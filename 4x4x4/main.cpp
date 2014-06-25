#include <avr/interrupt.h>
#include <util/delay.h>
#include "led.h"
#include "effect.h"

using namespace effect;

int main(void)
{
	led::init();
	sei();

	led::enable(true);

	scan();
	expand(false);
	shrink(false);

start:
	expand(true);
	shrink(true);
	delay();
	goto start;
	return 1;
}
