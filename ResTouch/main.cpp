#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include "restouch.h"

tft_t tft;
ResTouch touch(&tft);

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft.init();
	tft.setOrient(tft.FlipLandscape);
	tft.setBackground(0x667F);
	tft.setForeground(0x0000);
	stdout = tftout(&tft);
	touch.init();
	tft.setBGLight(true);
	touch.calibrate();
	tft.clean();
}

int main(void)
{
	init();

start:
	tft.clean();
	tft.setZoom(1);
	puts("*** Touch library testing ***");
	tft.setZoom(2);

loop:
	if (touch.detect()) {
		ResTouch::coord_t res = touch.read();
		if (touch.detect())
			tft.point(res.x, res.y, 0xFFFF);
			//printf("TE: %u\t%u\n", res.x, res.y);
	}
	goto loop;
	goto start;

	return 1;
}
