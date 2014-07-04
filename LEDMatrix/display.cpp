#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "display.h"
#include "lcd.h"
#include "led.h"
#include "ascii.h"

using namespace display;

volatile uint_t display::buff[BUFF_H][BUFF_W / 8][2];
bool display::lcdOutput;

Display disp;

Display::Display(void)
{
	x = 0;
	y = 0;
	zoom = 1;
	tabSize = 4;
	clr = None;
}

void Display::init(void)
{
	lcdOutput = 1;
	disp.fill(None);

	MCUCR |= 0x80;			// Disable JTAG
	MCUCR |= 0x80;

	CHECK_DDR &= ~CHECK_BIT;
	CHECK_PORT |= CHECK_BIT;
	lcdOutput = CHECK_PIN & CHECK_BIT;

	if (lcdOutput)
		lcd::init();
	else
		led::init();
}

void Display::update(void)
{
	if (lcdOutput)
		lcd::update();
}

void Display::fill(const uint_t clr)
{
	for (uint_t j = 0; j < LED_H; j++)
		for (uint_t k = 0; k < LED_W / 8; k++) {
			buff[j][k][BuffRed] = (clr & AllRed) ? 0xFF : 0x00;
			buff[j][k][BuffGreen] = (clr & AllGreen) ? 0xFF : 0x00;
		}
}

void Display::drawChar(const uint_t x, const uint_t y, const uint_t zoom, const char ch)
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

void Display::drawChar(const char ch)
{
	drawChar(x, y, zoom, ch);
}

void Display::drawPoint(const uint_t x, const uint_t y, const uint_t clr)
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

void Display::drawString(const uint_t x, const uint_t y, const uint_t zoom, const char *str)
{
	uint_t dx = x;
	while (*str) {
		drawChar(dx, y, zoom, *str++);
		dx += FONT_W * zoom;
	}
}

void Display::drawEllipse(uint_t xx, uint_t yy, int_t w, int_t h)
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

void Display::drawLine(uint_t x0, uint_t y0, uint_t x1, uint_t y1)
{
	// Bresenham's line algorithm (http://en.wikipedia.org/wiki/Bresenham's_line_algorithm)
	uint_t dx = abs(x1 - x0), dy = abs(y1 - y0);
	int_t sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, err = dx - dy;

	while (true) {
		drawPoint(x0, y0, clr);
		if (x0 == x1 && y0 == y1)
			break;
		int e2 = 2 * err;
		if (e2 > -dy) {
			err = err - dy;
			x0 = x0 + sx;
		}
		if (x0 == x1 && y0 == y1) {
			drawPoint(x0, y0, clr);
			break;
		}
		if (e2 < dx) {
			err = err + dx;
			y0 = y0 + sy;
		}
	}
}

void Display::newline(void)
{
	x = 0;
	y += FONT_H * zoom;
	if (y + FONT_H * zoom > BUFF_H) {
#ifdef TFT_SCROLL
		*this ^= BUFF_H * zoom;
		y -= BUFF_H * zoom;
#else
		clear();
		y = 0;
#endif
	}
}

void Display::next(void)
{
	x += FONT_W * zoom;
	if (x + FONT_W * zoom > BUFF_W)
		newline();
}

void Display::tab(void)
{
	if (x % (FONT_W * zoom))
		x -= x % (FONT_W * zoom);
	do
		next();
	while ((x / (FONT_W * zoom)) % tabSize);
}


Display& Display::operator<<(const char c)
{
	switch (c) {
	case '\n':
		newline();
		break;
	case '\t':
		tab();
		break;
	default:
		if ((unsigned)c < ' ' || (unsigned)c > 127)
			break;
		drawChar(c);
		next();
	}
	return *this;
}

static int putch(const char c, FILE *stream)
{
	disp << c;
	return 0;
}

FILE *Display::out(void)
{
	static FILE *out = NULL;
	if (out == NULL)
		out = fdevopen(putch, NULL);
	return out;
}
