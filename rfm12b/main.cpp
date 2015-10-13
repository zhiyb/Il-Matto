#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <tft.h>
#include <rfm12_config.h>
#include <rfm12.h>

//#define RFM12B_TX

void init()
{
	tft::init();
	stdout = tftout();
	_delay_ms(1000);
	rfm12_init();
	_delay_ms(1000);
	sei();
}

int main()
{
	init();
	tft::setBGLight(true);

	puts("Initialised.");
#ifdef RFM12B_TX
	static uint8_t teststr[] = "Hello, world!\r\n";
	uint16_t cnt = 0;
	for (;;) {
		printf("%02x, ", rfm12_tx(sizeof(teststr), 0xaa, teststr));
		rfm12_start_tx(0xaa, sizeof(teststr));
		rfm12_tick();
		printf("sent: %u\n", cnt++);
		for (uint16_t i = 0; i < 1000; i++) {
			_delay_us(500);
			rfm12_tick();
		}
	}
#else
	for (;;) {
		rfm12_tick();
		while (rfm12_rx_status() != STATUS_COMPLETE)
			rfm12_tick();
		printf("Received: ");
		uint8_t len = rfm12_rx_len();
		uint8_t type = rfm12_rx_type();
		uint8_t *buffer = rfm12_rx_buffer();
		printf("length %u, type 0x%02x\n", len, type);
		puts((char *)buffer);
		rfm12_rx_clear();
	}
#endif

	return 1;
}
