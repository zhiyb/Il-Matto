#include <avr/io.h>
#include <avr/interrupt.h>
#include "led.h"

volatile uint8_t led::buff[LED_H][LED_W / 8][2];
uint8_t row;

void led::init(void)
{
	row = 0;
	fill(false, false);

	MCUCR |= 0x80;			// Disable JTAG
	MCUCR |= 0x80;

	LED_DPORT = 0;
	LED_CPORT &= ~(LED_EN | LED_STB | LED_CLK);
	LED_DDDR |= LED_LMASK | LED_DMASK;
	LED_CDDR |= LED_EN | LED_STB | LED_CLK;

	TCCR0A = _BV(WGM01);
	TCCR0B = 0;
	TCNT0 = 0;
	//OCR0A = 137;					// 85Hz
	OCR0A = 50;					// 85Hz * 2.5
	TIMSK0 = _BV(OCIE0A);
	TIFR0 = 0xFF;
	TCCR0B = _BV(CS01) | _BV(CS00);
}

void led::fill(bool r, bool g)
{
	for (uint8_t j = 0; j < LED_H; j++)
		for (uint8_t k = 0; k < LED_W / 8; k++) {
			buff[j][k][Red] = r ? 0xFF : 0x00;
			buff[j][k][Green] = g ? 0xFF : 0x00;
		}
}

ISR(TIMER0_COMPA_vect)
{
	using namespace led;

	uint8_t d[2][2];
	for (int8_t i = 0; i != LED_W / 8; i++) {
		d[0][Red] = buff[row][i][Red];
		d[0][Green] = buff[row][i][Green];
		d[1][Red] = buff[row + 16][i][Red];
		d[1][Green] = buff[row + 16][i][Green];
		for (uint8_t j = 0; j < 8; j++) {
			LED_DPORT &= LED_LMASK;
			LED_DPIN = ((d[0][Red] & 1) ? 0 : LED_RU) | ((d[0][Green] & 1) ? 0 : LED_GU) | ((d[1][Red] & 1) ? 0 : LED_RD) | ((d[1][Green] & 1) ? 0 : LED_GD);
			LED_CPIN = LED_CLK;
			d[0][Red] >>= 1;
			d[0][Green] >>= 1;
			d[1][Red] >>= 1;
			d[1][Green] >>= 1;
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
