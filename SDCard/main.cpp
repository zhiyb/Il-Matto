#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "tft.h"
#include "sd.h"

void init(void)
{
	tft.init();
	tft /= tft.FlipLandscape;
	tft.clean();
	stdout = tftout();
	tft++;
}

int main(void)
{
	init();

start:
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
	puts("Initialise succeed!");
	printf("SD Card version: %d\n", sd.version() & 0x0F);
	printf("Address: %s\n", (sd.version() & 0xF0) ? \
			"Block address" : "Byte address");
	{
		class cidReg cid = sd.cid();
		printf("Manufacture ID: %u\n", cid.mid);
		printf("OEM/Application ID: %u\n", cid.oid);
		printf("Product name: %s\n", cid.pnm);
		printf("Product revision: %u\n", cid.prv);
		printf("Product serial number: %lu\n", cid.psn);
		printf("Maufacturing date: 20%02d-0%01d\n", \
				cid.mdt >> 4, cid.mdt & 0x0F);
		printf("CRC7 checksum: 0x%02X\n", cid.crc);
	}
finished:
	puts("Remove SD to run again...");
	while (sd.detect());
	goto start;

	return 1;
}
