#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <tft.h>
#include <uart0.h>
#include <escape.h>
#include "spiflash.h"

void init()
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft::init();
	//stdout = tft::devout();
	SPIFlash::init();
	uart0_init();
	stdout = uart0_fd();
	sei();

	tft::setBGLight(true);
}

int main()
{
	using namespace SPIFlash;

	::init();
	puts_P(PSTR(ESC_DEFAULT ESC_FF ESC_CYAN "Hello, world!"));

	static char buffer[256];

loop:
	uart0_write_string(ESC_YELLOW "> " ESC_DEFAULT);
	uart0_readline(buffer, sizeof(buffer));
	if (strncmp_P(buffer, PSTR("erase "), 6) == 0) {
		// erase [4k/32k/64k] addr
		char *str = buffer + 6;
		uint32_t addr;
		if (strncmp_P(str, PSTR("4k "), 3) == 0) {
			*(str + 2) = '\0';
			sscanf(str + 3, "%li", &addr);
			SPIFlash::erase4k(addr);
		} else if (strncmp_P(str, PSTR("32k "), 4) == 0) {
			*(str + 3) = '\0';
			sscanf(str + 4, "%li", &addr);
			SPIFlash::erase32k(addr);
		} else if (strncmp_P(str, PSTR("64k "), 4) == 0) {
			*(str + 3) = '\0';
			sscanf(str + 4, "%li", &addr);
			SPIFlash::erase64k(addr);
		}
		printf_P(PSTR(ESC_RED "Erasing %s bytes at 0x%06lx.\n"), str, addr);
	} else if (strncmp_P(buffer, PSTR("dump "), 5) == 0) {
		// dump address
		uint32_t addr;
		sscanf_P(buffer + 5, PSTR("%li"), &addr);
		addr &= ~(uint32_t)(SPIFLASH_PAGE_SIZE - 1);
		readStart(addr);
		uint8_t i = 0;
		printf_P(PSTR(ESC_MAGENTA " Address  " ESC_CYAN));
		do {
			printf_P(PSTR("%2x "), i);
			if (i % 16 == 7)
				putchar(' ');
		} while (++i != 16);
		putchar('\n');
		i = 0;
		do {
			printf_P(PSTR(ESC_RED "0x%06lx  " ESC_GREEN), addr);
			do {
				printf_P(PSTR("%02x "), readByte());
				if (i % 16 == 7)
					putchar(' ');
			} while (++i % 16);
			addr += 16;
			putchar('\n');
		} while (i != 0);
		readStop();
	} else if (strncmp_P(buffer, PSTR("fill "), 5) == 0) {
		// fill address value
		uint32_t addr;
		uint8_t v;
		sscanf_P(buffer + 5, PSTR("%li%i"), &addr, &v);
		addr &= ~(uint32_t)(SPIFLASH_PAGE_SIZE - 1);
		printf_P(PSTR(ESC_RED "Filling page 0x%06lx with 0x%02x.\n"), addr, v);
		programStart(addr);
		uint8_t i = 0;
		do {
			writeByte(v);
		} while (++i != 0);
		programStop();
	} else /*if (strcmp_P(buffer, PSTR("program")) == 0) {
		char *ptr = buffer;
		uint8_t checksum = 0;
		uint8_t i = 0;
		do
			checksum += *ptr++ = uart0_read();
		while (++i != 0);
		uart0_read_data((char *)&i, 1);
		if (checksum != i)
			uart0_write('f');
		else {
			uart0_write('w');
			SPIFlash::writeData(addr, buffer, 256);
		}
	} else*/ if (strncmp_P(buffer, PSTR("status "), 7) == 0) {
		// status r [1-3]
		// status w [1-3] value
		char *str = buffer + 7;
		if (strncmp_P(str, PSTR("w "), 2) == 0) {
			uint8_t n, v;
			sscanf_P(str + 2, PSTR("%i%i"), &n, &v);
			if (n < 1 || n > 3)
				goto loop;
			SPIFlash::writeStatus(n, v);
			printf_P(PSTR(ESC_RED "Status %u: 0x%02x\n"), n, SPIFlash::readStatus(n));
		} else if (strncmp_P(str, PSTR("r "), 2) == 0) {
			uint8_t n;
			sscanf_P(str + 2, PSTR("%i"), &n);
			if (n < 1 || n > 3)
				goto loop;
			printf_P(PSTR(ESC_GREEN "Status %u: 0x%02x\n"), n, SPIFlash::readStatus(n));
		}
	}
	while (SPIFlash::checkBusy());
	goto loop;
	return 1;
}
