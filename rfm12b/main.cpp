#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
//#include <tft.h>
//#include <colours.h>
#include <rfm12_config.h>
#include <rfm12.h>
#include <uart.h>
#include "mac_layer.h"
#include "tcp_layer.h"

//FILE *tftout;

void init()
{
#if 0
	tft::init();
	tftout = tft::devout();
#endif
#if 1
	uart_init();
	stdout = uart_io();
	stdin = uart_io();
#endif
	_delay_ms(1000);
	rfm12_init();
	_delay_ms(1000);
	sei();
}

int main()
{
	init();
	puts("Initialised.");
	//fputs("Initialised.\n", tftout);
	//tft::setBGLight(true);

	static char buffer[200] = "\e[96mGroup 7, yz39g13 with jm15g13: \e[0m";
	const uint8_t len = strlen(buffer);
	char *ptr = buffer + len;
	int c;
	uint16_t txCount = 0, rxCount = 0;
	bool sent = true;
poll:
	if (sent == false) {
		sent = write(~MACHINE_ADDRESS, strlen(buffer) + 1, (uint8_t *)buffer) == RFM12_TX_ENQUEUED;
		if (sent) {
			printf("Sent (0x%02x): %u\n", MACHINE_ADDRESS, txCount);
			//fprintf(tftout, "Sent: %u\n", txCount);
			txCount++;
			puts(buffer);
			//fputs(buffer, tftout);
			//fputc('\n', tftout);
		}
	}

	// If something is received through UART
	if ((c = uart_read_unblocked()) != -1) {
		putchar(c);
		*ptr++ = c;
		if (c == '\n' || c == '\r') {
			*ptr++ = '\n';
			*ptr = 0;
			sent = false;
			ptr = buffer + len;
		}
	}

	// If something is received through rfm12b
	if (available()) {
		printf_P(PSTR("\e[91mReceived %u: "), rxCount);
		//fprintf_P(tftout, PSTR("Received %u: "), rxCount++);
		rxCount++;
		uint8_t type = rfm12_rx_type();
		static uint8_t buffer[RFM12_RX_BUFFER_SIZE];
		uint8_t src, len;
		read(&src, &len, buffer);

		printf_P(PSTR("length %u, type 0x%02x, from 0x%02x\n\e[92m"), len, type, src);
		//fprintf_P(tftout, PSTR("length %u, type 0x%02x\n"), len, type);
		*(buffer + len) = '\0';
		puts((char *)buffer);
		//fputs((char *)buffer, tftout);
		//fputc('\n', tftout);
		//rfm12_rx_clear();
	}

	//rfm12_tick();
	mac_tick(MAC_CSMA);
	_delay_us(100);
	goto poll;

	return 1;
}
