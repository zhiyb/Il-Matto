#include <avr/io.h>
#include <ctype.h>
#include "ascii.h"
#include "ili9341.h"
#include "tft.h"

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

#include <avr/pgmspace.h>

tft_t::tft_t(void)
{
	setX(0);
	setY(0);
	setZoom(1);
	d.orient = Portrait;
	setTabSize(4);
	setWidth(SIZE_W);
	setHeight(SIZE_H);
	setForeground(DEF_FGC);
	setBackground(DEF_BGC);
}

void tft_t::frame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint8_t s, uint16_t c)
{
	rectangle(x, y, w - s, s, c);
	rectangle(x + w - s, y, s, h - s, c);
	rectangle(x, y + s, s, h - s, c);
	rectangle(x + s, y + h - s, w - s, s, c);
}

void tft_t::line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, \
		uint16_t c)
{
	if (x0 > width())
		x0 = width() - 1;
	if (x1 > width())
		x1 = width() - 1;
	if (y0 > height())
		y0 = height() - 1;
	if (y1 > height())
		y1 = height() - 1;
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
			point(x0 + dx * (y - y0) / dy * \
					(x0 > x1 ? -1 : 1), y, c);
	} else {
		if (x0 > x1) {
			SWAP(x0, x1);
			SWAP(y0, y1);
		}
		for (uint16_t x = x0; x <= x1; x++)
			point(x, y0 + dy * (x - x0) / dx * \
					(y0 > y1 ? -1 : 1), c);
	}
}

void tft_t::rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint16_t c)
{
	area(x, y, w, h);
	cmd(0x2C);			// Memory Write
	while (w--)
		for (y = 0; y < h; y++)
			write16(c);
}

void tft_t::putch(char ch)
{
	area(x(), y(), WIDTH * zoom(), HEIGHT * zoom());
	cmd(0x2C);			// Memory Write
	for (uint8_t i = 0; i < HEIGHT * zoom(); i++) {
		unsigned char c;
		c = pgm_read_byte(&(ascii[ch - ' '][i / zoom()]));
		for (uint8_t j = 0; j < WIDTH * zoom(); j++) {
			if (c & 0x80)
				write16(foreground());
			else
				write16(background());
			if (j % zoom() == zoom() - 1)
				c <<= 1;
		}
	}
}

void tft_t::setOrient(uint8_t o)
{
	_setOrient(o);
	switch (o) {
	case Landscape:
	case FlipLandscape:
		setWidth(SIZE_H);
		setHeight(SIZE_W);
		break;
	case Portrait:
	case FlipPortrait:
		setWidth(SIZE_W);
		setHeight(SIZE_H);
	}
	d.orient = o;
	setX(0);
	setY(0);
}

void tft_t::bmp(bool e)
{
	if (!e) {
		_setOrient(orient());
		return;
	}
	_setOrient(orient() + BMPLandscape - Landscape);
}

static tft_t *tft;

inline int tftputch(const char c, FILE *stream)
{
	(*tft) << c;
	return 0;
}

FILE *tftout(tft_t *hw)
{
	static FILE *out = NULL;
	tft = hw;
	if (out == NULL)
		out = fdevopen(tftputch, NULL);
	return out;
}
