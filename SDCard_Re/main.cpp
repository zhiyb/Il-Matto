#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
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
	printf("SDCard detected, %swrite protected, ", sd.writeProtected() ? "" : "not ");

	uint8_t err;
	if ((err = sd.init()) != 0x00) {
		printf("Initialisation failed: %u, err: %u\n", err, sd.err());
		goto fin;
	}
	printf("%u GB\n", (uint16_t)(sd.size() / 1024 / 1024));

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

	setfs(&fs);
	DIR *dir = opendir("/");
	puts("\nReading root directory...");
	if (dir == NULL) {
		printf("opendir failed: %u\n", errno);
		goto fin;
	}

	DIR *dir2 = opendir("/Il Matto");
	puts("\nReading 'Il Matto' directory...");
	if (dir2 == NULL) {
		printf("opendir failed: %u\n", errno);
		goto fin;
	}
	struct dirent *ent;
	while ((ent = readdir(dir2)) != NULL)
		puts(ent->d_name);

	DIR *dir3 = opendir("/Il Matto/Testing");
	puts("\nReading 'Il Matto/Testing' directory...");
	if (dir3 == NULL) {
		printf("opendir failed: %u\n", errno);
		goto fin;
	}

	DIR *dir4 = opendir("/Il Matto/Testing/");
	puts("\nReading 'Il Matto/Testing' directory...");
	if (dir4 == NULL) {
		printf("opendir failed: %u\n", errno);
		goto fin;
	}

	closedir(dir);
	closedir(dir2);
	closedir(dir3);

	goto fin;

	return 1;
}
