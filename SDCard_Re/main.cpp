#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include <math.h>
#include "tft.h"
#include "sd.h"
#include "mbr.h"
#include "fat32.h"
#include "conv.h"
#include "dirent.h"

#define PI	3.1415927

class sdhw_t sd;

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft.init();
	tft /= tft.FlipLandscape;
	tft.setForeground(conv::c32to16(0x00FF00));
	tft.setBackground(0);
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

	puts("\nReading '/Il Matto/Testing' directory...");
	DIR *dir = opendir("/Il Matto/Testing");
	if (dir == NULL) {
		printf("opendir failed: %u\n", errno);
		goto fin;
	}
	uint16_t bgc = tft.getForeground();
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_type & IS_DIR)
			tft.setForeground(conv::c32to16(0x0000FF));
		else
			tft.setForeground(conv::c32to16(0xFFFFFF));
		puts(ent->d_name);
	}
	closedir(dir);
	tft.setForeground(bgc);

	_delay_ms(2000);
	tft.clean();
	tft.line(0, tft.height() / 2, tft.width(), tft.height() / 2, conv::c32to16(0xFF0000));
	tft.line(tft.width() / 2, 0, tft.width() / 2, tft.height(), conv::c32to16(0xFF0000));
	float _y = 0;
	for (uint16_t i = 0; i < tft.width(); i++) {
		float x = PI * 6 * i / tft.width() - PI * 3;
		float y = sin(x) + x / 4 / PI;
		y = -y;
		if (y > 2)
			y = 2;
		if (-y > 2)
			y = -2;
		y *= tft.height() / 4;
		if (i == 0)
			tft.point(0, tft.height() / 2 + y, conv::c32to16(0x66CCFF));
		else
			tft.line(i - 1, tft.height() / 2 + _y, i, tft.height() / 2 + y, conv::c32to16(0x66CCFF));
		_y = y;
	}

	goto fin;

	return 1;
}
