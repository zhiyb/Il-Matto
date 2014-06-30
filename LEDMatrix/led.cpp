#include <avr/io.h>
#include <avr/interrupt.h>
#include "led.h"
#include "ascii.h"

volatile uint_t led::buff[LED_H][LED_W / 8][2];
uint_t row;

void led::init(void)
{
	row = 0;
	fill(None);

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

void led::fill(const uint_t clr)
{
	for (uint_t j = 0; j < LED_H; j++)
		for (uint_t k = 0; k < LED_W / 8; k++) {
			buff[j][k][BuffRed] = (clr & AllRed) ? 0xFF : 0x00;
			buff[j][k][BuffGreen] = (clr & AllGreen) ? 0xFF : 0x00;
		}
}

void led::drawChar(const uint_t x, const uint_t y, const uint_t zoom, const uint_t clr, const char ch)
{
	for (uint_t dy = 0; dy < FONT_H * zoom; dy++) {
		unsigned char c = pgm_read_byte(&(ascii[ch - ' '][dy / zoom]));
		for (uint_t dx = 0; dx < FONT_W * zoom && x + dx < LED_W; dx++) {
			if (c & 0x80)
				drawPoint(x + dx, y + dy, clr & FGC);
			else
				drawPoint(x + dx, y + dy, clr & BGC);
			if (dx % zoom == zoom - 1)
				c <<= 1;
		}
	}
}

void led::drawPoint(const uint_t x, const uint_t y, const uint_t clr)
{
	if (clr & AllRed)
		buff[y][x / 8][BuffRed] |= 1 << x % 8;
	else
		buff[y][x / 8][BuffRed] &= ~(1 << x % 8);
	if (clr & AllGreen)
		buff[y][x / 8][BuffGreen] |= 1 << x % 8;
	else
		buff[y][x / 8][BuffGreen] &= ~(1 << x % 8);
}

void led::drawString(const uint_t x, const uint_t y, const uint_t zoom, const uint_t clr, const char *str)
{
	uint_t dx = x;
	while (*str) {
		drawChar(dx, y, zoom, clr, *str++);
		dx += FONT_W * zoom;
	}
}

void led::drawEllipse(uint_t xx, uint_t yy, int_t w, int_t h, const uint_t clr)
{
	// midpoint, 1/4 ellipse
	if (w == 0 || h == 0)
		return;
	if (w < 0) {		// negative width
		w = -w;
		xx -= w;
	}
	if (h < 0) {		// negative height
		h = -h;
		yy -= h;
	}

	int_t x, y;
	float d1, d2;
	float a2=(w/2)*(w/2),  b2=(h/2)*(h/2);
	xx += w/2;
	yy += h/2;
	x = 0;
	y = int_t(h/2);
	d1 = b2 - a2*(h/2) + 0.25*a2;
	drawPoint(xx+x, yy-y, clr);
	drawPoint(xx-x, yy-y, clr);
	drawPoint(xx-x, yy+y, clr);
	drawPoint(xx+x, yy+y, clr);
	while ( a2*(y-0.5) > b2*(x+0.5) ) {		// region 1
		if ( d1 < 0 ) {
			d1 = d1 + b2*(3.0+2*x);
			x++;
		} else {
			d1 = d1 + b2*(3.0+2*x) + 2.0*a2*(1-y);
			x++;
			y--;
		}
		drawPoint(xx+x, yy-y, clr);
		drawPoint(xx-x, yy-y, clr);
		drawPoint(xx-x, yy+y, clr);
		drawPoint(xx+x, yy+y, clr);
	}
	d2 = b2*(x+0.5)*(x+0.5) + a2*(y-1)*(y-1) - a2*b2;
	while ( y > 0 ) {				// region 2
		if ( d2 < 0 ) {
			d2 = d2 + 2.0*b2*(x+1) + a2*(3-2*y);
			x++;
			y--;
		} else {
			d2 = d2 + a2*(3-2*y);
			y--;
		}
		drawPoint(xx+x, yy-y, clr);
		drawPoint(xx-x, yy-y, clr);
		drawPoint(xx-x, yy+y, clr);
		drawPoint(xx+x, yy+y, clr);
	}
}

void led::drawLine(uint_t x1, uint_t y1, uint_t x2, uint_t y2, const uint_t clr)
{
	;
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
