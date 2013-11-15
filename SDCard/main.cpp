#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "tft.h"
#include "sd.h"
#include "disk.h"
#include "fat32.h"

class disk mmc;
class fat32 fs;

void init(void)
{
	tft.init();
	tft /= tft.FlipLandscape;
	stdout = tftout();
	tft++;
}

int main(void)
{
	init();

start:
	tft *= 2;
	tft.clean();
	puts("*** SD library test ***");
	puts("Waiting for SD insert...");
	while (!sd.detect());
	puts("SD Card detected!");
	if (sd.writeProtected())
		puts("Write-protected!");
	else
		puts("Not write-protected.");
	uint8_t res;
	if ((res = sd.init()) != 0) {
		printf("Initialise failed with %d!\n", res);
		goto finished;
	}
	printf("SD Card version: %d\n", sd.version() & 0x0F);
	printf("Address: %s\n", (sd.version() & 0xF0) ? \
			"Block address" : "Byte address");

	printf("Manufacture ID: %u\n", sd.cid().MID);
	printf("OEM/Application ID: %u\n", sd.cid().OID);
	printf("Product name: %.5s\n", sd.cid().PNM);
	printf("Serial number: %lu\n", sd.cid().PSN);
	printf("Maufacture date: 20%02u-0%01u\n", \
			sd.cid().MDT >> 4, sd.cid().MDT & 0x0F);

	printf("CSD register version: %u\n", sd.csd().CSD_STRUCTURE + 1);
	printf("Size: %luMB\n", sd.size() / 1024);

	_delay_ms(3000);
	tft *= 2;
	tft.clean();
	mmc.init();
	if (mmc.status() != mmc.SUCCEED) {
		printf("Disk read failed with %u\n", mmc.status());
		goto finished;
	}
	puts("Disk structure read.");
	if (mmc.part(0).type() != 0x0B && mmc.part(0).type() != 0x0C) {
		puts("Partition 1 is not FAT32");
		goto finished;
	}
	puts("Partition 1 is FAT32");
	printf("Start at sector %lu\n", mmc.part(0).begin());
	fs.init(mmc.part(0));
	if (fs.status() != fs.OK) {
		printf("Read FAT32 failed with %d\n", fs.status());
		goto finished;
	}
finished:
	printf("Remove SD to run again...");
	while (sd.detect());
	goto start;

	return 1;
}
