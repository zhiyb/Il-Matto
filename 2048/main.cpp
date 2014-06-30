#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "tft.h"
#include "2048.h"

void display(game2048& game)
{
	tft.clean();
	tft *= 1;
	while (1);
}

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft.init();
	tft /= tft.FlipLandscape;
	tft.setBackground(0x0000);
	tft.setForeground(0xFFFF);
	tft.clean();
	stdout = tftout();
	tft++;
}

int main(void)
{
	init();
	class game2048 game;

start:
	display(game);
	goto start;

	return 1;
}
