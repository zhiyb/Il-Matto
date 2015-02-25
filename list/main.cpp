#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include <list.h>
#include "menu.h"

tft_t tft;
list l(&tft);

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft.init();
	tft.setOrient(tft.Portrait);
	tft.setBackground(0x0000);
	tft.setForeground(0x667F);
	tft.clean();
	stdout = tftout(&tft);
	tft.setBGLight(true);
}

int main(void)
{
	init();

	tft.clean();
	tft.setZoom(2);
	puts("*** List ***");
	tft.setForeground(0x0000);

	l.refresh();
	l.display(&menuRoot);
	tft.rectangle(0, tft.bottomEdge(), tft.width(), 1, 0xF800);
	tft.rectangle(0, tft.topEdge() - 1, tft.width(), 1, 0xF800);

	uint16_t max = l.maxScroll(), v = 0;
	v = max - 1;
inc:
	l.setScroll(v);
	_delay_ms(10);
	if (++v < max)
		goto inc;
	_delay_ms(1000);
dec:
	l.setScroll(v);
	_delay_ms(10);
	if (--v > 0)
		goto dec;
	_delay_ms(1000);
	goto inc;

	return 1;
}
