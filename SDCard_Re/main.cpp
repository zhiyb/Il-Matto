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
	puts("*** SDCard library test ***");
	puts("Please insert SDCard...");
	while (!sd.detect());
	puts("SDCard detected!");
	puts(sd.writeProtected() ? "Write protected!" : "Not write protected!");
	printf("Initialisation result: %u, errno: %u\n", sd.init(), sd.err());
	printf("SDCard size: %u GB\n", (uint16_t)(sd.size() / 1024 / 1024));
	class mbr_t mbr(&sd);
	printf("Read MBR result: %u\n", mbr.errno);
	for (uint8_t i = 0; i < 4; i++)
		printf("Partition %u type: 0x%02X\n", i, mbr.type[i]);
	hw_t *hw = &sd;
	puts("hw read attempt 1.");
	if (!hw->dataAddress(hw->Read, mbr.addr[0]))
		printf("Initialise hw read failed: %u\n", sd.err());
	if (!hw->dataStop(hw->Read))
		printf("Stop hw read failed: %u\n", sd.err());
	puts("hw read attempt 2.");
	if (!hw->dataAddress(hw->Read, mbr.addr[0]))
		printf("Initialise hw read failed: %u\n", sd.err());
	if (!hw->dataStop(hw->Read))
		printf("Stop hw read failed: %u\n", sd.err());
	puts("Test finished, remove SDCard to run again...");
	while (sd.detect());
	goto start;

	return 1;
}
