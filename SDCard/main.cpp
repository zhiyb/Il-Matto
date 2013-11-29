#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "tft.h"
#include "sd.h"
#include "disk.h"
#include "fat32.h"

void melody_main(void);
void melody_init(void);

class disk mmc;
class fat32 fs;

void init(void)
{
	tft.init();
	stdout = tftout();
	melody_init();
	tft++;
}

int main(void)
{
	init();

start:
	tft *= 2;
	tft /= tft.FlipLandscape;
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
		printf("Initialise failed with %02X!\n", res);
		goto finished;
	}
	/*printf("SD Card version: %u\n", sd.version() & 0x0F);
	printf("Address: %s\n", (sd.version() & 0xF0) ? \
			"Block address" : "Byte address");

	printf("Manufacture ID: %u\n", sd.cid().MID);
	printf("OEM/Application ID: %u\n", sd.cid().OID);
	printf("Product name: %.5s\n", sd.cid().PNM);
	printf("Serial number: %lu\n", sd.cid().PSN);
	printf("Maufacture date: 20%02u-0%01u\n", \
			sd.cid().MDT >> 4, sd.cid().MDT & 0x0F);
	printf("CSD register version: %u\n", sd.csd().CSD_STRUCTURE + 1);

	_delay_ms(3000);
	tft *= 2;
	tft.clean();*/
	printf("SD Size: %luMB\n", sd.size() / 1024);
	mmc.init();
	if (mmc.status() != mmc.SUCCEED) {
		printf("Disk read failed with %02X\n", mmc.status());
		goto finished;
	}
	if (mmc.part(0).type() != 0x0B && mmc.part(0).type() != 0x0C) {
		puts("Partition 1 is not FAT32");
		goto finished;
	}
	puts("Partition 1 is FAT32");
	printf("Start at sector %lu\n", mmc.part(0).begin());
	fs.init(mmc.part(0));
	if (fs.status() != fs.OK) {
		printf("Read FAT32 failed with %02X\n", fs.status());
		goto finished;
	}
	{	// Read directory
		tft *= 1;
		char path[] = "";
		DIR *dir = fs.opendir(path);
		if (dir == NULL) {
			printf("Error open dir with %02X\n", fs.errno());
			goto finished;
		}
		struct dirent *d;
		while ((d = fs.readdir(dir)) != NULL) {
			printf("Read name: %.11s", d->d_name);
			printf(", type: 0x%02X\n", d->d_type);
		}
		fs.closedir(dir);
		//tft /= tft.Portrait;
		fs.test();
		while (!(SD_PIN & SD_CD))
			melody_main();
		goto start;
	}
finished:
	printf("Remove SD to run again...");
	while (sd.detect());
	goto start;

	return 1;
}
