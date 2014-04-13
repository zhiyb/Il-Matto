#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "tft.h"
#include "sd.h"
#include "mbr.h"
#include "fat32.h"

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
	puts("*** SDCard library test ***");
	puts("Please insert SDCard...");
	while (!sd.detect());
	puts("SDCard detected!");
	puts(sd.writeProtected() ? "Write protected!" : "Not write protected!");
	printf("Initialisation result: %u, errno: %u\n", sd.init(), sd.err());
	printf("SDCard size: %u GB\n", (uint16_t)(sd.size() / 1024 / 1024));
	class mbr_t mbr(&sd);
	printf("Read MBR result: %u\n", mbr.err());
	printf("Partition 1 type: 0x%02X\n", mbr.type(0));
	puts("Test finished, remove SDCard to run again...");
	while (sd.detect());
	goto start;

	return 1;
}
