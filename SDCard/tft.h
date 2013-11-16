#ifndef TFT_H
#define TFT_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include "ili9341.h"
#include "ascii.h"

class tfthw: public ili9341
{
public:
	tfthw(void);

	inline class tfthw& operator<<(const char c);
	inline class tfthw& operator<<(const char *str);
	inline class tfthw& operator<<(const int16_t i);
	inline class tfthw& operator<<(const uint16_t i);
	inline class tfthw& operator<<(const int32_t i);
	inline class tfthw& operator<<(const uint32_t i);
	inline class tfthw& operator++(int x);
	inline class tfthw& operator--(int x);
	inline class tfthw& operator*=(uint8_t z);
	inline class tfthw& operator/=(uint8_t o);

	inline void setX(uint16_t n) {x = n;}
	inline void setY(uint16_t n) {y = n;}
	inline void setXY(uint16_t m, uint16_t n) {x = m; y = n;}
	inline uint16_t getX(void) {return x;}
	inline uint16_t getY(void) {return y;}
	inline void setForeground(uint16_t c) {fgc = c;}
	inline void setBackground(uint16_t c) {bgc = c;}
	inline uint8_t getForeground(void) {return fgc;}
	inline uint8_t getBackground(void) {return bgc;}
	void setOrient(uint8_t o);
	inline uint8_t getOrient(void) {return orient;}
	inline void setBGLight(bool e) {_setBGLight(e);}
	inline void clean(void) {fill(bgc); x = 0; y = 0;}
	void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, \
		uint16_t c);
	void frame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint8_t s, uint16_t c);
	inline void fill(uint16_t clr);
	void putch(char ch);
	void rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint16_t c);
	inline void point(uint16_t x, uint16_t y, uint16_t c);

protected:
	inline void area(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	inline void all();
	inline void newline(void);
	inline void next(void);

	uint8_t zoom, orient;
	uint16_t x, y, w, h, fgc, bgc;
};

FILE *tftout(void);

extern class tfthw tft;

// Defined as inline to execute faster

#define WIDTH 6
#define HEIGHT 8
#define SIZE_H 320
#define SIZE_W 240
#define DEF_FGC 0xFFFF
#define DEF_BGC 0x0000
#define SWAP(x, y) { \
	(x) = (x) ^ (y); \
	(y) = (x) ^ (y); \
	(x) = (x) ^ (y); \
}

inline class tfthw& tfthw::operator/=(uint8_t o)
{
	setOrient(o);
	return *this;
}

inline class tfthw& tfthw::operator*=(uint8_t z)
{
	zoom = z;
	return *this;
}

inline class tfthw& tfthw::operator++(int x)
{
	_setBGLight(true);
	return *this;
}

inline class tfthw& tfthw::operator--(int x)
{
	_setBGLight(false);
	return *this;
}

inline class tfthw& tfthw::operator<<(const int16_t i)
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

inline class tfthw& tfthw::operator<<(const uint16_t i)
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

inline class tfthw& tfthw::operator<<(const char c)
{
	if (c == '\n')
		newline();
	else {
		putch(c);
		next();
	}
	return *this;
}

inline class tfthw& tfthw::operator<<(const char *str)
{
	while (*str) {
		*this << *str;
		str++;
	}
	return *this;
}

inline void tfthw::point(uint16_t x, uint16_t y, uint16_t c)
{
	area(x, y, 1, 1);
	send(1, 0x2C);			// Memory Write
	send(0, c / 0x0100);
	send(0, c % 0x0100);
}

inline void tfthw::newline(void)
{
	x = 0;
	y += HEIGHT * zoom;
	if (y + HEIGHT * zoom > h) {
		fill(bgc);
		y = 0;
	}
}

inline void tfthw::fill(uint16_t clr)
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

inline void tfthw::area(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
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

inline void tfthw::all()
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

inline void tfthw::next(void)
{
	x += WIDTH * zoom;
	if (x + WIDTH * zoom > w)
		newline();
}

#undef WIDTH
#undef HEIGHT
#undef SIZE_H
#undef SIZE_W
#undef DEF_FGC
#undef DEF_BGC
#undef SWAP

#endif
