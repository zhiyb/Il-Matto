#ifndef TFT_H
#define TFT_H

#define FONT_WIDTH 6
#define FONT_HEIGHT 8
//#define TFT_SCROLL

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include "ili9341.h"
#include "ascii.h"
#include "colour.h"

extern struct tfthw {
	uint8_t zoom, orient, tabSize;
	uint16_t x, y, w, h, fgc, bgc;
} tft;

FILE *tftout(void);

// Defined as inline to execute faster

static inline void tft_init(void);

static inline void tft_setOrient(uint8_t o);
static inline uint8_t tft_getOrient(void);
static inline void tft_bmp(uint8_t e);
static inline void tft_setZoom(uint8_t z);
static inline void tft_setBGLight(uint8_t e);
static inline void tft_setTabSize(uint8_t t);
static inline uint8_t tft_getTabSize(void);
static inline void tft_setXY(uint16_t m, uint16_t n);
static inline void tft_setX(uint16_t n);
static inline uint16_t tft_getX(void);
static inline void tft_setY(uint16_t n);
static inline uint16_t tft_getY(void);
static inline void tft_setColour(uint16_t f, uint16_t b);
static inline void tft_setForeground(uint16_t c);
static inline uint8_t tft_getForeground(void);
static inline void tft_setBackground(uint16_t c);
static inline uint8_t tft_getBackground(void);

static inline void tft_clean(void);
static inline void tft_fill(uint16_t clr);
static inline void tft_shiftUp(uint16_t l);
static inline void tft_print_char(const char c);
static inline void tft_print_string(const char *str);
static inline void tft_print_int16(const int16_t i);
static inline void tft_print_uint16(const uint16_t i);
static inline void tft_rectangle(uint16_t x, uint16_t y, \
		uint16_t w, uint16_t h, uint16_t c);
static inline void tft_frame(uint16_t x, uint16_t y, \
		uint16_t w, uint16_t h, uint8_t s, uint16_t c);
static inline void tft_point(uint16_t x, uint16_t y, uint16_t c);
static inline void tft_line(uint16_t x0, uint16_t y0, \
		uint16_t x1, uint16_t y1, uint16_t c);

static inline void tft_start(void);
static inline void tft_write(uint16_t c);

// Internal use only

static inline void tft_next(void);
static inline void tft_tab(void);
static inline void tft_newline(void);
static inline void tft_all(void);
static inline void tft_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
static inline void tft_putch(char ch);

#define WIDTH FONT_WIDTH
#define HEIGHT FONT_HEIGHT
#define SIZE_H 320
#define SIZE_W 240
#define DEF_FGC 0xFFFF
#define DEF_BGC 0x0000
#define SWAP(x, y) { \
	(x) = (x) ^ (y); \
	(y) = (x) ^ (y); \
	(x) = (x) ^ (y); \
}

static inline void tft_setX(uint16_t n)
{
	tft.x = n;
}

static inline void tft_setY(uint16_t n)
{
	tft.y = n;
}

static inline void tft_setXY(uint16_t m, uint16_t n)
{
	tft.x = m;
	tft.y = n;
}

static inline uint16_t tft_getX(void)
{
	return tft.x;
}

static inline uint16_t tft_getY(void)
{
	return tft.y;
}

static inline void tft_setColour(uint16_t f, uint16_t b)
{
	tft.fgc = f;
	tft.bgc = b;
}

static inline void tft_setForeground(uint16_t c)
{
	tft.fgc = c;
}

static inline void tft_setBackground(uint16_t c)
{
	tft.bgc = c;
}

static inline uint8_t tft_getForeground(void)
{
	return tft.fgc;
}

static inline uint8_t tft_getBackground(void)
{
	return tft.bgc;
}

static inline uint8_t tft_getOrient(void)
{
	return tft.orient;
}

static inline void tft_setBGLight(uint8_t e)
{
	ili9341_setBGLight(e);
}

static inline void tft_setTabSize(uint8_t t)
{
	tft.tabSize = t;
}

static inline uint8_t tft_getTabSize(void)
{
	return tft.tabSize;
}

static inline void tft_all(void)
{
	ili9341_cmd(0x2A);			// Column Address Set
	ili9341_data(0x00);			// x
	ili9341_data(0x00);
	ili9341_data((tft.w - 1) / 0x0100);	// w
	ili9341_data((tft.w - 1) % 0x0100);
	ili9341_cmd(0x2B);			// Page Address Set
	ili9341_data(0x00);			// y
	ili9341_data(0x00);
	ili9341_data((tft.h - 1) / 0x0100);	// h
	ili9341_data((tft.h - 1) % 0x0100);
}

static inline void tft_fill(uint16_t clr)
{
	uint8_t ch = clr / 0x0100, cl = clr % 0x0100;
	uint16_t x = tft.w, y;
	tft_all();
	ili9341_cmd(0x2C);			// Memory Write
	while (x--) {
		y = tft.h;
		while (y--) {
			ili9341_data(ch);
			ili9341_data(cl);
		}
	}
}

static inline void tft_clean(void)
{
	tft_fill(tft.bgc);
	tft.x = 0;
	tft.y = 0;
}

static inline void tft_start(void)
{
	ili9341_cmd(0x2C);
}

static inline void tft_write(uint16_t c)
{
	ili9341_data(c / 0x0100);
	ili9341_data(c % 0x0100);
}

static inline void tft_init(void)
{
	ili9341_init();
	tft.x = 0;
	tft.y = 0;
	tft.zoom = 1;
	tft.orient = Portrait;
	tft.tabSize = 4;
	tft.w = SIZE_W;
	tft.h = SIZE_H;
	tft.fgc = DEF_FGC;
	tft.bgc = DEF_BGC;
}

static inline void tft_bmp(uint8_t e)
{
	if (!e) {
		ili9341_setOrient(tft.orient);
		return;
	}
	switch (tft.orient) {
	case Landscape:
		ili9341_setOrient(BMPLandscape);
		break;
	case Portrait:
		ili9341_setOrient(BMPPortrait);
		break;
	case FlipLandscape:
		ili9341_setOrient(BMPFlipLandscape);
		break;
	case FlipPortrait:
		ili9341_setOrient(BMPFlipPortrait);
	}
}

static inline void tft_shiftUp(uint16_t l)
{
	// 0x2C Write, 0x2E Read, 0x3C / 0x3E Continue, 0x00 NOP
	uint8_t buff[tft.w * 2];
	uint16_t r;
	ili9341_cmd(0x2A);			// Column Address Set
	ili9341_data(0x00);
	ili9341_data(0x00);
	ili9341_data((tft.w - 1) / 0x0100);
	ili9341_data((tft.w - 1) % 0x0100);
	for (r = 0; r < tft.h - l; r++) {
		uint16_t b = tft.w * 2;
		//area(0, r + l, w, 1);
		ili9341_cmd(0x2B);		// Page Address Set
		ili9341_data((r + l) / 0x0100);
		ili9341_data((r + l) % 0x0100);
		ili9341_data((r + l) / 0x0100);
		ili9341_data((r + l) % 0x0100);
		ili9341_cmd(0x2E);		// Read
		ili9341_mode(1);		// Read mode
		ili9341_recv();
		while (b--) {
			buff[b] = ili9341_recv() & 0xF8;
			uint8_t g = ili9341_recv();
			buff[b--] |= g >> 5;
			buff[b] = (g << 3) & 0xE0;
			buff[b] |= ili9341_recv() >> 3;
		}
		ili9341_mode(0);		// Write mode

		b = tft.w * 2;
		//area(0, r, w, 1);
		ili9341_cmd(0x2B);		// Page Address Set
		ili9341_data(r / 0x0100);
		ili9341_data(r % 0x0100);
		ili9341_data(r / 0x0100);
		ili9341_data(r % 0x0100);
		ili9341_cmd(0x2C);		// Write
		while (b--)
			ili9341_data(buff[b]);
	}
	//area(0, h - l, w, l);
	ili9341_cmd(0x2B);		// Page Address Set
	ili9341_data((tft.h - l) / 0x0100);
	ili9341_data((tft.h - l) % 0x0100);
	ili9341_data((tft.h - 1) / 0x0100);
	ili9341_data((tft.h - 1) % 0x0100);
	ili9341_cmd(0x2C);
	while (r++ < tft.h) {
		uint16_t c = tft.w;
		while (c--) {
			ili9341_data(tft.bgc / 0x0100);
			ili9341_data(tft.bgc % 0x0100);
		}
	}
}

static inline void tft_setZoom(uint8_t z)
{
	tft.zoom = z;
}

static inline void tft_next(void)
{
	tft.x += WIDTH * tft.zoom;
	if (tft.x + WIDTH * tft.zoom > tft.w)
		tft_newline();
}

static inline void tft_tab(void)
{
	if (tft.x % (WIDTH * tft.zoom))
		tft.x -= tft.x % (WIDTH * tft.zoom);
	do
		tft_next();
	while ((tft.x / (WIDTH * tft.zoom)) % tft.tabSize);
}

static inline void tft_newline(void)
{
	tft.x = 0;
	tft.y += HEIGHT * tft.zoom;
	if (tft.y + HEIGHT * tft.zoom > tft.h) {
#ifdef TFT_SCROLL
		*this ^= HEIGHT * zoom;
		y -= HEIGHT * zoom;
#else
		tft_fill(tft.bgc);
		tft.y = 0;
#endif
	}
}

static inline void tft_print_char(const char c)
{
	switch (c) {
	case '\n':
		tft_newline();
		break;
	case '\t':
		tft_tab();
		break;
	default:
		if ((unsigned)c < ' ' || (unsigned)c > 127)
			break;
		tft_putch(c);
		tft_next();
	}
}

static inline void tft_print_string(const char *str)
{
	while (*str)
		tft_print_char(*str++);
}

static inline void tft_print_int16(const int16_t i)
{
	uint16_t p = 10000, n = abs(i);
	if (i < 0)
		tft_print_char('-');
	while ((p != 1) && (n / p == 0))
		p /= 10;
	while (p != 0) {
		tft_print_char((char)((n / p) % 10 + '0'));
		p /= 10;
	}
}

static inline void tft_print_uint16(const uint16_t i)
{
	uint16_t p = 10000;
	while ((p != 1) && (i / p == 0))
		p /= 10;
	while (p != 0) {
		tft_print_char((char)((i / p) % 10 + '0'));
		p /= 10;
	}
}

static inline void tft_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	ili9341_cmd(0x2A);			// Column Address Set
	ili9341_data(x / 0x0100);
	ili9341_data(x % 0x0100);
	ili9341_data((x + w - 1) / 0x0100);
	ili9341_data((x + w - 1) % 0x0100);
	ili9341_cmd(0x2B);			// Page Address Set
	ili9341_data(y / 0x0100);
	ili9341_data(y % 0x0100);
	ili9341_data((y + h - 1) / 0x0100);
	ili9341_data((y + h - 1) % 0x0100);
}

static inline void tft_point(uint16_t x, uint16_t y, uint16_t c)
{
	tft_area(x, y, 1, 1);
	ili9341_cmd(0x2C);			// Memory Write
	ili9341_data(c / 0x0100);
	ili9341_data(c % 0x0100);
}

static inline void tft_setOrient(uint8_t o)
{
start:
	switch (o) {
	case Landscape:
	case FlipLandscape:
		tft.w = SIZE_H;
		tft.h = SIZE_W;
		break;
	case Portrait:
	case FlipPortrait:
		tft.w = SIZE_W;
		tft.h = SIZE_H;
		break;
	default:
		o = 0;
		goto start;
	}
	ili9341_setOrient(o);
	tft.orient = o;
	tft.x = 0;
	tft.y = 0;
}

static inline void tft_rectangle(uint16_t x, uint16_t y, \
		uint16_t w, uint16_t h, uint16_t c)
{
	tft_area(x, y, w, h);
	ili9341_cmd(0x2C);			// Memory Write
	while (w--)
		for (y = 0; y < h; y++) {
			ili9341_data(c / 0x0100);
			ili9341_data(c % 0x0100);
		}
}

static inline void tft_frame(uint16_t x, uint16_t y, \
		uint16_t w, uint16_t h, uint8_t s, uint16_t c)
{
	tft_rectangle(x, y, w - s, s, c);
	tft_rectangle(x + w - s, y, s, h - s, c);
	tft_rectangle(x, y + s, s, h - s, c);
	tft_rectangle(x + s, y + h - s, w - s, s, c);
}

static inline void tft_putch(char ch)
{
	uint8_t i;
	tft_area(tft.x, tft.y, WIDTH * tft.zoom, HEIGHT * tft.zoom);
	ili9341_cmd(0x2C);			// Memory Write
	for (i = 0; i < HEIGHT * tft.zoom; i++) {
		unsigned char c;
		uint8_t j;
		c = pgm_read_byte(&(ascii[ch - ' '][i / tft.zoom]));
		for (j = 0; j < WIDTH * tft.zoom; j++) {
			if (c & 0x80) {
				ili9341_data(tft.fgc / 0x0100);
				ili9341_data(tft.fgc % 0x0100);
			} else {
				ili9341_data(tft.bgc / 0x0100);
				ili9341_data(tft.bgc % 0x0100);
			}
			if (j % tft.zoom == tft.zoom - 1)
				c <<= 1;
		}
	}
}

static inline void tft_line(uint16_t x0, uint16_t y0, \
		uint16_t x1, uint16_t y1, uint16_t c)
{
	if (x0 == x1) {
		if (y0 > y1)
			SWAP(y0, y1);
		tft_rectangle(x0, y0, 1, y1 - y0, c);
		return;
	}
	if (y0 == y1) {
		if (x0 > x1)
			SWAP(x0, x1);
		tft_rectangle(x0, y0, x1 - x0, 1, c);
		return;
	}
	uint16_t dx = abs(x1 - x0), dy = abs(y1 - y0);
	if (dx < dy) {
		uint16_t y;
		if (y0 > y1) {
			SWAP(x0, x1);
			SWAP(y0, y1);
		}
		for (y = y0; y <= y1; y++)
			tft_point(x0 + x1 * (y - y0) / dy - \
					x0 * (y - y0) / dy, y, c);
	} else {
		uint16_t x;
		if (x0 > x1) {
			SWAP(x0, x1);
			SWAP(y0, y1);
		}
		for (x = x0; x <= x1; x++)
			tft_point(x, y0 + y1 * (x - x0) / dx - \
					y0 * (x - x0) / dx, c);
	}
}

#undef WIDTH
#undef HEIGHT
#undef SIZE_H
#undef SIZE_W
#undef DEF_FGC
#undef DEF_BGC
#undef SWAP

#endif
