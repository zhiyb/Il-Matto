#include <avr/io.h>
#include <stdlib.h>
#include "ascii.h"
#include "ili9341.h"
#include "tft.h"

#define WIDTH 6
#define HEIGHT 8
#define SIZE_H 320
#define SIZE_W 240
#define DEF_FC 0xFFFF
#define DEF_BC 0x0000
#define SWAP(x, y) { \
	(x) = (x) ^ (y); \
	(y) = (x) ^ (y); \
	(x) = (x) ^ (y); \
}

#include <avr/pgmspace.h>

class tfthw tft;

tfthw::tfthw(void)
{
	x = 0;
	y = 0;
	zoom = 1;
	orient = Portrait;
	w = SIZE_W;
	h = SIZE_H;
	fc = DEF_FC;
	bc = DEF_BC;
}

class tfthw& tfthw::operator/=(uint8_t o)
{
	setOrient(o);
	return *this;
}

class tfthw& tfthw::operator*=(uint8_t z)
{
	zoom = z;
	return *this;
}

class tfthw& tfthw::operator++(int x)
{
	_setBGLight(true);
	return *this;
}

class tfthw& tfthw::operator--(int x)
{
	_setBGLight(false);
	return *this;
}

class tfthw& tfthw::operator<<(const int16_t i)
{
	uint16_t p = 10000, n = abs(i);
	if (i < 0)
		*this << '-';
	while ((p != 1) && (n / p == 0))
		p /= 10;
	while (p != 0) {
		*this << (char)((n / p) % 10 + '0');
		p /= 10;
	}
	return *this;
}

class tfthw& tfthw::operator<<(const uint16_t i)
{
	uint16_t p = 10000;
	while ((p != 1) && (i / p == 0))
		p /= 10;
	while (p != 0) {
		*this << (char)((i / p) % 10 + '0');
		p /= 10;
	}
	return *this;
}

class tfthw& tfthw::operator<<(const int32_t i)
{
	uint32_t p = 1000000000, n = labs(i);
	if (i < 0)
		*this << '-';
	while ((p != 1) && (n / p == 0))
		p /= 10;
	while (p != 0) {
		*this << (char)((n / p) % 10 + '0');
		p /= 10;
	}
	return *this;
}

class tfthw& tfthw::operator<<(const uint32_t i)
{
	uint32_t p = 1000000000;
	while ((p != 1) && (i / p == 0))
		p /= 10;
	while (p != 0) {
		*this << (char)((i / p) % 10 + '0');
		p /= 10;
	}
	return *this;
}

class tfthw& tfthw::operator<<(const char c)
{
	if (c == '\n')
		newline();
	else {
		putbigch(x, y, zoom, c, fc, bc);
		next();
	}
	return *this;
}

class tfthw& tfthw::operator<<(const char *str)
{
	while (*str) {
		*this << *str;
		str++;
	}
	return *this;
}

int tftputch(const char c, FILE *stream)
{
	tft << c;
	return 0;
}

FILE *tftout(void)
{
	static FILE *out = NULL;
	if (out == NULL)
		out = fdevopen(tftputch, NULL);
	return out;
}

void tfthw::next(void)
{
	x += WIDTH * zoom;
	if (x + WIDTH * zoom > w)
		newline();
}

void tfthw::setBGLight(bool e)
{
	_setBGLight(e);
}

void tfthw::setOrient(uint8_t o)
{
	_setOrient(o);
	switch (o) {
	case Landscape:
	case FlipLandscape:
		w = SIZE_H;
		h = SIZE_W;
		break;
	case Portrait:
	case FlipPortrait:
		w = SIZE_W;
		h = SIZE_H;
	}
	orient = o;
}

void tfthw::newline(void)
{
	x = 0;
	y += HEIGHT * zoom;
	if (y + HEIGHT * zoom > h) {
		fill(bc);
		y = 0;
	}
}

void tfthw::putbigint32(uint16_t x, uint16_t y, uint8_t z, int32_t i, \
		uint16_t fgc, uint16_t bgc)
{
	uint32_t p = 1000000000;
	if (i < 0) {
		putbigch(x, y, z, '-', fgc, bgc);
		x += WIDTH * z;
		i = abs(i);
	}
	while ((p != 1) && (i / p == 0))
		p /= 10;
	while (p != 0) {
		putbigch(x, y, z, (i / p) % 10 + '0', fgc, bgc);
		x += WIDTH * z;
		p /= 10;
	}
}

void tfthw::putint32(uint16_t x, uint16_t y, int32_t i, \
		uint16_t fgc, uint16_t bgc)
{
	uint32_t p = 1000000000;
	if (i < 0) {
		putch(x, y, '-', fgc, bgc);
		x += WIDTH;
		i = abs(i);
	}
	while ((p != 1) && (i / p == 0))
		p /= 10;
	while (p != 0) {
		putch(x, y, (i / p) % 10 + '0', fgc, bgc);
		x += WIDTH;
		p /= 10;
	}
}

void tfthw::frame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint8_t s, uint16_t c)
{
	rectangle(x, y, w - s, s, c);
	rectangle(x + w - s, y, s, h - s, c);
	rectangle(x, y + s, s, h - s, c);
	rectangle(x + s, y + h - s, w - s, s, c);
}

void tfthw::fill(uint16_t clr)
{
	uint16_t x, y;
	all();
	send(1, 0x2C);			// Memory Write
	for (x = 0; x < w; x++)
		for (y = 0; y < h; y++) {
			send(0, clr / 0x0100);
			send(0, clr % 0x0100);
		}
}

void tfthw::line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, \
		uint16_t c)
{
	if (x0 == x1) {
		if (y0 > y1)
			SWAP(y0, y1);
		rectangle(x0, y0, 1, y1 - y0, c);
		return;
	}
	if (y0 == y1) {
		if (x0 > x1)
			SWAP(x0, x1);
		rectangle(x0, y0, x1 - x0, 1, c);
		return;
	}
	uint16_t dx = abs(x1 - x0), dy = abs(y1 - y0);
	if (dx < dy) {
		if (y0 > y1) {
			SWAP(x0, x1);
			SWAP(y0, y1);
		}
		for (uint16_t y = y0; y <= y1; y++)
			point(x0 + x1 * (y - y0) / dy - \
					x0 * (y - y0) / dy, y, c);
	} else {
		if (x0 > x1) {
			SWAP(x0, x1);
			SWAP(y0, y1);
		}
		for (uint16_t x = x0; x <= x1; x++)
			point(x, y0 + y1 * (x - x0) / dx - \
					y0 * (x - x0) / dx, c);
	}
}

void tfthw::point(uint16_t x, uint16_t y, uint16_t c)
{
	area(x, y, 1, 1);
	send(1, 0x2C);			// Memory Write
	send(0, c / 0x0100);
	send(0, c % 0x0100);
}

void tfthw::all()
{
	send(1, 0x2A);			// Column Address Set
	send(0, 0x00);			// x
	send(0, 0x00);
	send(0, (w - 1) / 0x0100);	// w
	send(0, (w - 1) % 0x0100);
	send(1, 0x2B);			// Page Address Set
	send(0, 0x00);			// y
	send(0, 0x00);
	send(0, (h - 1) / 0x0100);	// h
	send(0, (h - 1) % 0x0100);
}

void tfthw::area(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	send(1, 0x2A);			// Column Address Set
	send(0, x / 0x0100);
	send(0, x % 0x0100);
	send(0, (x + w - 1) / 0x0100);
	send(0, (x + w - 1) % 0x0100);
	send(1, 0x2B);			// Page Address Set
	send(0, y / 0x0100);
	send(0, y % 0x0100);
	send(0, (y + h - 1) / 0x0100);
	send(0, (y + h - 1) % 0x0100);
}

void tfthw::rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint16_t c)
{
	area(x, y, w, h);
	send(1, 0x2C);			// Memory Write
	while (w--)
		for (y = 0; y < h; y++) {
			send(0, c / 0x0100);
			send(0, c % 0x0100);
		}
}

void tfthw::putch(uint16_t x, uint16_t y, char ch, \
		uint16_t fgc, uint16_t bgc)
{
	unsigned char c;
	uint8_t i, j;
	area(x, y, WIDTH, HEIGHT);
	send(1, 0x2C);			// Memory Write
	for (i = 0; i < HEIGHT; i++) {
		c = pgm_read_byte(&(ascii[ch - ' '][i]));
		for (j = 0; j < WIDTH; j++) {
			if (c & 0x80) {
				send(0, fgc / 0x0100);
				send(0, fgc % 0x0100);
			} else {
				send(0, bgc / 0x0100);
				send(0, bgc % 0x0100);
			}
			c <<= 1;
		}
	}
}

void tfthw::putbigch(uint16_t x, uint16_t y, uint8_t z, char ch, \
		uint16_t fgc, uint16_t bgc)
{
	unsigned char c;
	uint8_t i, j;
	area(x, y, WIDTH * z, HEIGHT * z);
	send(1, 0x2C);			// Memory Write
	for (i = 0; i < HEIGHT * z; i++) {
		c = pgm_read_byte(&(ascii[ch - ' '][i / z]));
		for (j = 0; j < WIDTH * z; j++) {
			if (c & 0x80) {
				send(0, fgc / 0x0100);
				send(0, fgc % 0x0100);
			} else {
				send(0, bgc / 0x0100);
				send(0, bgc % 0x0100);
			}
			if (j % z == z - 1)
				c <<= 1;
		}
	}
}

void tfthw::puts(uint16_t x, uint16_t y, char *str, \
		uint16_t fgc, uint16_t bgc)
{
	while (*str != '\0') {
		putch(x, y, *str++, fgc, bgc);
		x += WIDTH;
	}
}

void tfthw::putbigs(uint16_t x, uint16_t y, uint8_t z, char *str, \
		uint16_t fgc, uint16_t bgc)
{
	while (*str != '\0') {
		putbigch(x, y, z, *str++, fgc, bgc);
		x += WIDTH * z;
	}
}
