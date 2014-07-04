#include <avr/io.h>
#include <avr/interrupt.h>
#include "led.h"
#include "display.h"

uint_t row;

using namespace display;

void led::init(void)
{
	row = 0;

	LED_DPORT = 0;
	LED_CPORT &= ~(LED_EN | LED_STB | LED_CLK);
	LED_DDDR |= LED_LMASK | LED_DMASK;
	LED_CDDR |= LED_EN | LED_STB | LED_CLK;

	TCCR0A = _BV(WGM01);
	TCCR0B = 0;
	TCNT0 = 0;
	OCR0A = 137;					// 85Hz
	//OCR0A = 50;					// 85Hz * 2.5
	TIMSK0 = _BV(OCIE0A);
	TIFR0 = 0xFF;
	TCCR0B = _BV(CS01) | _BV(CS00);
}

ISR(TIMER0_COMPA_vect)
{
	using namespace led;

	uint_t d[2][2];
	for (int_t i = 0; i != LED_W / 8; i++) {
		d[0][BuffRed] = buff[row][i][BuffRed];
		d[0][BuffGreen] = buff[row][i][BuffGreen];
		d[1][BuffRed] = buff[row + 16][i][BuffRed];
		d[1][BuffGreen] = buff[row + 16][i][BuffGreen];
		for (uint_t j = 0; j < 8; j++) {
			LED_DPORT &= LED_LMASK;
			LED_DPIN = ((d[0][BuffRed] & 1) ? 0 : LED_RU) | ((d[0][BuffGreen] & 1) ? 0 : LED_GU) | ((d[1][BuffRed] & 1) ? 0 : LED_RD) | ((d[1][BuffGreen] & 1) ? 0 : LED_GD);
			LED_CPIN = LED_CLK;
			d[0][BuffRed] >>= 1;
			d[0][BuffGreen] >>= 1;
			d[1][BuffRed] >>= 1;
			d[1][BuffGreen] >>= 1;
			LED_CPIN = LED_CLK;
		}
	}
	LED_CPIN = LED_EN;
	LED_CPIN = LED_STB;
	LED_DPORT = LED_L(row);
	row = row == 15 ? 0 : row + 1;
	LED_CPIN = LED_STB;
	LED_CPIN = LED_EN;
}
