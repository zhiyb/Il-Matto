#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "tft.h"
#include "sd.h"
#include "mbr.h"
#include "fat32.h"
#include "conv.h"
#include "dirent.h"

class sdhw_t sd;

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft.init();
	tft /= tft.FlipLandscape;
	tft.setBackground(conv::c32to16(0x66CCFF));
	tft.setForeground(conv::c32to16(~(uint32_t)0x66CCFF & 0xFFFFFF));
	tft.clean();
	stdout = tftout();
	tft++;
}

int main(void)
{
	init();
	goto start;

fin:
	puts("Test finished, remove SDCard to run again...");
	while (sd.detect());
start:
	tft.clean();
	tft *= 1;
	puts("*** SDCard library test ***");
	puts("Please insert SDCard...");
	while (!sd.detect());
	puts("SDCard detected!");
	puts(sd.writeProtected() ? "Write protected!" : "Not write protected!");

	uint8_t errno;
	if ((errno = sd.init()) != 0x00) {
		printf("Initialisation failed: %u, errno: %u\n", errno, sd.err());
		goto fin;
	}
	printf("SDCard size: %u GB\n", (uint16_t)(sd.size() / 1024 / 1024));

	class mbr_t mbr(&sd);
	if (mbr.err()) {
		printf("Read MBR failed: %u\n", mbr.err());
		goto fin;
	}
	if (mbr.type(0) != mbr_t::FAT32) {
		printf("Partition 1 (0x%02X) is not FAT32!\n", mbr.type(0));
		goto fin;
	}
	puts("Partition 1 is FAT32, reading...");

	fat32_t fs(&sd, mbr.address(0));
	if (fs.err()) {
		printf("Read FAT32 file system failed: %u\n", fs.err());
		goto fin;
	}

	putchar(fs.chainRead(fs.rootClus));
	for (uint8_t i = 1; i < 11; i++)
		putchar(fs.chainRead());
	putchar('\n');
	fs.chainReadClose();

	goto fin;

	return 1;
}
