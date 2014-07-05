#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "display.h"
#include "sd.h"
#include "ascii.h"
#include "uart.h"
#include "usb.h"
#include "tty.h"
#include "mbr.h"
#include "fat32.h"
#include "timer1.h"

using namespace display;

void init(void)
{
	usbDisconnect();
	disp.init();
	uart::init();
	timer1::init();
	stdout = disp.out();
	stderr = uart::out();
}

int main(void)
{
	init();
	class sdhw_t sd;
	uint8_t err;//, cnt;
	sei();

	fputs(TTY_WHITE "*** Hello, world! ***\n", stderr);
	disp.setColour(Red);
	disp.drawString(2, 0, 2,"Hello");
	disp.setColour(Green);
	disp.drawString(2, 16, 2, "World");
	disp.setColour(Orange);
	disp.drawEllipse(0, 0, 63, 31);
	disp.drawLine(0, 0, 63, 31);
	disp.drawLine(0, 31, 63, 0);
	disp.update();
	_delay_ms(500);

start:	{
	disp.setColour(None);
	disp.clear();
	if (!sd.detect()) {
		disp.update();
		while (!sd.detect());
	}
	fputs(TTY_BLUE "SDCard detected\n", stderr);
	disp.setColour(Green);
	puts("SDCard");

	if (sd.writeProtected()) {
		fputs(TTY_RED "Write protected\n", stderr);
		disp.setColour(Red);
		puts("Protected");
	}
	if ((err = sd.init()) != 0x00) {
		fprintf(stderr, TTY_RED "init() failed: %02X\n", err);
		goto failed;
	}
	fprintf(stderr, TTY_YELLOW "Size: %u MB\n", (uint16_t)(sd.size() / 1024));
	disp.setColour(Orange);
	printf("%u MB\n", (uint16_t)(sd.size() / 1024));
	
	class mbr_t mbr(&sd);
	if (mbr.err()) {
		fprintf(stderr, TTY_RED "MBR failed: %02X\n", mbr.err());
		goto failed;
	}
	for (err = 0; err < 4; err++)
		if (mbr.type(err) != mbr_t::FAT32 && \
				mbr.type(err) != mbr_t::FAT32_OLD)
			fprintf(stderr, TTY_RED "Partition %u (0x%02X) is not FAT32!\n", err, mbr.type(err));
		else
			break;
	if (err == 4)
		goto failed;
	fprintf(stderr, TTY_GREEN "Partition %u is FAT32\n", err);
	fat32_t fs(&sd, mbr.address(err));
	if (fs.err()) {
		fprintf(stderr, TTY_RED "Read FAT32 failed: %u\n", fs.err());
		goto failed;
	}
	op::setfs(&fs);

#if 0
	fputs(TTY_YELLOW "Reading '/BoardData' directory...\n", stderr);
	DIR *dir = op::opendir("/BoardData");
	if (dir == NULL) {
		fprintf(stderr, TTY_RED "opendir() failed: %u\n", errno);
		goto failed;
	}
	struct dirent *ent;
	while ((ent = op::readdir(dir)) != NULL)
		if (strcmp(ent->d_name, "Display.buff") == 0) {
			fputs(TTY_GREEN "Got buff file\n", stderr);
			break;
		}
		//fprintf(stderr, "%s%-25s\t|\t\%#02x\t|\t%lukB\n", ent->d_type & IS_DIR ? TTY_BLUE : TTY_GREEN, ent->d_name, ent->d_type, ent->d_size / 1024);

#if 0
	fputs(TTY_YELLOW "Testing max refresh speed...\n", stderr);
	timer1::start();
	cnt = 0;
	while (!timer1::over()) {
		disp.fill(Green);
		cnt++;
	}
	timer1::stop();
	fprintf(stderr, TTY_WHITE "CNT: %u\n", cnt);
#endif

#if 0
	fputs(TTY_YELLOW "Reading 'Display.buff' file by dirent...\n", stderr);
	timer1::start();
	cnt = 0;
	while (!timer1::over()) {
		FILE *fp = op::fopen(ent, "r");
		if (fp == NULL) {
			fprintf(stderr, TTY_RED "Open file failed: %u\n", errno);
			goto failed;
		}
		for (uint_t r = 0; r < BUFF_H; r++)
			for (uint_t c = 0; c < BUFF_W / 8; c++) {
				buff[r][c][BuffRed] = fgetc(fp);
				buff[r][c][BuffGreen] = fgetc(fp);
			}
		op::fclose(fp);
		cnt++;
	}
	timer1::stop();
	fprintf(stderr, TTY_WHITE "CNT: %u\n", cnt);
#endif
	op::closedir(dir);
#endif

	fputs(TTY_YELLOW "Changing to '/BoardData' directory...\n", stderr);
	if (op::chdir("/BoardData") != 0) {
		fprintf(stderr, TTY_RED "Change dir failed: %u\n", errno);
		goto failed;
	}

#if 0
	fputs(TTY_YELLOW "Reading 'Display.buff' file...\n", stderr);
	timer1::start();
	cnt = 0;
	while (!timer1::over()) {
		FILE *fp = op::fopen("Display.buff", "r");
		if (fp == NULL) {
			fprintf(stderr, TTY_RED "Open file failed: %u\n", errno);
			goto failed;
		}
		for (uint_t r = 0; r < BUFF_H; r++)
			for (uint_t c = 0; c < BUFF_W / 8; c++) {
				buff[r][c][BuffRed] = fgetc(fp);
				buff[r][c][BuffGreen] = fgetc(fp);
			}
		op::fclose(fp);
		cnt++;
	}
	timer1::stop();
	fprintf(stderr, TTY_WHITE "CNT: %u\n", cnt);
#endif

	fputs(TTY_YELLOW "Reading 'Movie.buffs' file...\n", stderr);
	{
		FILE *fp = op::fopen("Movie.buffs", "r");
		if (fp == NULL) {
			fprintf(stderr, TTY_RED "Open file failed: %u\n", errno);
			goto failed;
		}
		int ch;
		while ((ch = fgetc(fp)) != -1) {
			for (uint_t r = 0; r < BUFF_H; r++)
				for (uint_t c = 0; c < BUFF_W / 8; c++) {
					if (r != 0 || c != 0)
						ch = fgetc(fp);
					buff[r][c][BuffRed] = ch;
					//buff[r][c][BuffGreen] = fgetc(fp);
				}
			//_delay_ms(100);
		}
		op::fclose(fp);
	}

	}
	goto ret;
failed:
	disp.setColour(Red);
	printf("Failed");
ret:
	disp.update();
	while (sd.detect());
	fputs(TTY_BLUE "SDCard removed\n", stderr);
	goto start;

	while (1);
	return 1;
}
