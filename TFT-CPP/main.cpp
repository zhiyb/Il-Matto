#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <tft.h>
#ifdef TFT_READ_AVAILABLE
#include "capture.h"
#endif

extern void test(void);

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft::init();
#ifdef TFT_READ_AVAILABLE
	capture::init();
	capture::enable();
#endif
	stdout = tftout();
	sei();
}

int main(void)
{
	init();
	test();

	return 1;
}
