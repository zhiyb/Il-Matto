#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "tft.h"

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft.init();
	tft /= tft.Portrait;
	tft.setBackground(0x667F);
	tft.setForeground(0x0000);
	tft.clean();
	stdout = tftout();
	tft++;

	PORTD = 0xE0;
	DDRD = 0x1F;
	DDRB = 0x01;
	PORTB = 0x00;
}

int main(void)
{
	init();

start:
	tft.clean();
	tft *= 2;
	puts("Hamming encoder");

	for (uint8_t  num = 0; num < 16; num++) {
		PORTD = 0xE0;
		PORTD |= 0x10 | num;
		uint8_t data = 0;
		uint8_t i = 0;
		while (i < 8) {
			PINB |= 1;
			PINB |= 1;
			if (i == 0 && !(PIND & _BV(5))) {
				puts("Loop waiting");
				continue;
			}
			data >>= 1;
			data |= (PIND & _BV(6)) ? 0x80 : 0x00;
			i++;
		}
		PORTD = 0xE0;
		//	PINB |= 1;
		//	PINB |= 1;
		printf("Num: %u, data: 0x%02X\n", num, data);
		//_delay_ms(1000);
	}

	while (1);
	goto start;

	return 1;
}
