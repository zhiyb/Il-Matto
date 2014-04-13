#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "tft.h"
#include "sd.h"
#include "disk.h"
#include "fat32.h"
#include "pwm.h"
#include "timer.h"

void melody_main(void);
void melody_init(void);

class disk mmc;
class fat32 fs;

void init(void)
{
	tft.init();
	stdout = tftout();
	//melody_init();
	pwm::init();
	timer0::init();
	tft++;
}

int main(void)
{
	init();

start:
	tft *= 1;
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
	printf("SD Card version: %u\n", sd.version() & 0x0F);
	printf("Address: %s\n", (sd.version() & 0xF0) ? \
			"Block address" : "Byte address");

	/*printf("Manufacture ID: %u\n", sd.cid().MID);
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
	if (fs.status != fs.OK) {
		printf("Read FAT32 failed with %02X\n", fs.status);
		goto finished;
	}
	{	// Read directory
		tft *= 1;
		char path[] = "";
		DIR *dir = fs.opendir(path);
		if (dir == NULL) {
			printf("Error open dir with %02X\n", fs.errno);
			goto finished;
		}
		struct dirent *d;
		while ((d = fs.readdir(dir)) != NULL) {
			printf("Read name: %.11s", d->d_name);
			printf(", type: 0x%02X\n", d->d_type);
		}
		fs.closedir(dir);
		//tft /= tft.Portrait;
#define FUNC 0
#if (FUNC == 2)
		timer0::set(8000);
		char wav[] = "ILMATT~1/WAV/TEST    WAV";
		class fat32_file *stream;
play:
		stream = fs.sfopen(wav);
		if (stream == NULL) {
			puts("File open filed!");
			goto finished;
		}
		printf("File size: %ld\n", stream->size);
		while (stream->remain) {
			uint8_t s = stream->sgetc();
			pwm::set(s, s);
			/*if (timer0::flag()) {
				_delay_ms(10);
				timer0::flag();
				continue;
			}*/
			while (!timer0::flag());
		}
		fs.fclose(stream);
		goto play;
#elif (FUNC == 1)
		fs.test();
		//tft.fill(conv::c32to16(0x0000FF));
		while (sd.detect()) {
			tft ^= 1;
			_delay_ms(10);
		}
		goto start;
		while (sd.detect());
		goto start;
#else
		char txt[] = "ILMATT~1/TXT/TEST    TXT";
		class fat32_file *stream = fs.fopen(txt);
		if (stream == NULL) {
			puts("File open filed!");
			goto finished;
		}
		printf("File size: %ld\n", stream->size);
		_delay_ms(200);
		tft.clean();
		char _c;
		while ((_c = stream->getc()) != EOF) {
			putchar(_c);
			//_delay_ms(2);
		}
		fs.fclose(stream);
#endif
	}
finished:
	printf("Remove SD to run again...");
	while (sd.detect());
	goto start;

	return 1;
}
