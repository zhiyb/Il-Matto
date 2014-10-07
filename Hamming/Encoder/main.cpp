#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "tft.h"
#include "conv.h"

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft.init();
	tft /= tft.Portrait;
	tft.setBackground(0x0000);
	tft.setForeground(0x667F);
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
		uint8_t exp = ((num & 0x08) << 4) | ((num & 0x04) << 3) | ((num & 0x02) << 2) | ((num & 0x01) << 1);
		exp |= ((num & 0x02) << 5) ^ ((num & 0x04) << 4) ^ ((num & 0x08) << 3);
		exp |= ((~num & 0x01) << 4) ^ ((num & 0x02) << 3) ^ ((num & 0x04) << 2);
		exp |= ((~num & 0x01) << 2) ^ ((num & 0x02) << 1) ^ ((num & 0x08) >> 1);
		exp |= (~num & 0x01) ^ ((num & 0x04) >> 2) ^ ((num & 0x08) >> 3);
		tft.setForeground(conv::c32to16(data == exp ? 0x0022B14C : 0x00ED1C24));
		printf("Num: %u, data: 0x%02X\n", num, data);
		//_delay_ms(1000);
	}

	while (1);
	goto start;

	return 1;
}
