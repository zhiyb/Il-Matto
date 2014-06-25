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
	circle(Out, false);
	circle(In, false);

start:
#if 1
	circle(Out, true);
	circle(In, true);
#else
	expand(true);
	shrink(true);
#endif
	goto start;
	return 1;
}
