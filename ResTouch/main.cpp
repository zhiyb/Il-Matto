#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include "restouch.h"

class tft_t tft;

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft.init();
	tft.setOrient(tft.FlipLandscape);
	tft.setBackground(0x667F);
	tft.setForeground(0x0000);
	tft.clean();
	stdout = tftout(&tft);
	ResTouch::init();
	tft.setBGLight(true);
}

int main(void)
{
	init();

start:
	tft.clean();
	tft.setZoom(1);
	puts("*** Touch library testing ***");
	tft.setZoom(2);
	ResTouch::mode(ResTouch::Detection);

loop:
	if (ResTouch::function(ResTouch::Detection)) {
		uint16_t x, y;
		ResTouch::mode(ResTouch::ReadX);
		x = ResTouch::function(ResTouch::ReadX);
		ResTouch::mode(ResTouch::ReadY);
		y = ResTouch::function(ResTouch::ReadY);
		ResTouch::mode(ResTouch::Detection);
		if (ResTouch::function(ResTouch::Detection))
			printf("TE: %u\t%u\n", x, y);
	}
	goto loop;
	goto start;

	return 1;
}
