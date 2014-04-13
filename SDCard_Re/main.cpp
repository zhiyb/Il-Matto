#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "tft.h"
#include "sd.h"

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft.init();
	tft /= tft.FlipLandscape;
	tft.setBackground(0x667F);
	tft.setForeground(0x0000);
	tft.clean();
	stdout = tftout();
	tft++;
}

int main(void)
{
	init();

start:
	tft.clean();
	tft *= 1;
	puts("*** SDCard library testing ***");
	puts("Please insert SDCard!");
	while (!sd.detect());
	puts("SDCard detected!");
	puts(sd.writeProtected() ? "Write protected!" : "Not write protected!");
	printf("Initialisation result: %u, errno: %u\n", sd.init(), sd.err());
	printf("SDCard size: %u GB\n", (uint16_t)(sd.size() / 1024 / 1024));
	while (sd.detect());
	goto start;

	return 1;
}
