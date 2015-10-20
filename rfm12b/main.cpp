#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <tft.h>
#include <colours.h>
#include <rfm12_config.h>
#include <rfm12.h>
#include <uart.h>

FILE *tftout;

void init()
{
#if 1
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
	fputs("Initialised.\n", tftout);
	tft::setBGLight(true);

	static char buffer[200] = "\e[91mG\e[92mr\e[93mo\e[94mu\e[95mp \e[96m7, yz39g13 with jm15g13: \e[0m";
	const uint8_t len = strlen(buffer);
	char *ptr = buffer + len;
	int c;
	uint16_t txCount = 0, rxCount = 0;
	bool sent = true;
poll:
	if (sent == false) {
		sent = rfm12_tx(strlen(buffer) + 1, 0xaa, (uint8_t *)buffer) == RFM12_TX_ENQUEUED;
		if (sent) {
			printf("Sent: %u\n", txCount);
			fprintf(tftout, "Sent: %u\n", txCount);
			txCount++;
			puts(buffer);
			fputs(buffer, tftout);
			fputc('\n', tftout);
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
	if (rfm12_rx_status() == STATUS_COMPLETE) {
		printf_P(PSTR("\e[91mReceived %u: "), rxCount);
		fprintf_P(tftout, PSTR("Received %u: "), rxCount++);
		rxCount++;
		uint8_t len = rfm12_rx_len();
		uint8_t type = rfm12_rx_type();
		uint8_t *buffer = rfm12_rx_buffer();
		printf_P(PSTR("length %u, type 0x%02x\n\e[92m"), len, type);
		fprintf_P(tftout, PSTR("length %u, type 0x%02x\n"), len, type);
		*(buffer + len) = '\0';
		puts((char *)buffer);
		fputs((char *)buffer, tftout);
		fputc('\n', tftout);
		rfm12_rx_clear();
	}

	rfm12_tick();
	_delay_us(1);
	goto poll;

	return 1;
}
