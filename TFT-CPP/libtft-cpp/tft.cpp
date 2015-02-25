/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

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
	//setTopMask(0);
	//setBottomMask(0);
	d.tfa = 0;
	d.bfa = 0;
	d.vsp = 0;
	setTopMask(0);
	setBottomMask(0);
	setTransform(false);
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

void tft_t::rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c)
{
	if (!h || !w)
		return;
	if (transform()) {
		uint16_t yt = vsTransformBack(y);
		if ((int16_t)yt < (int16_t)topEdge() && \
			(int16_t)(yt + h) >= (int16_t)topEdge()) {	// Top edge clipping
			h -= topEdge() - yt;
			y = upperEdge();
			yt = vsTransformBack(y);
		} else if (yt < bottomEdge() && yt + h >= bottomEdge())	// Bottom edge clipping
			h = bottomEdge() - yt;
		if (y + h > bottomEdge())				// Transform edge split
			if (y < bottomEdge()) {
				rectangle(x, y, w, bottomEdge() - y, c);
				rectangle(x, topFixedArea(), w, h - (bottomEdge() - y), c);
				return;
			}

		if (yt < topMask()) {
			if (yt + h < topMask())
				return;
			h -= topMask() - yt;
			y = vsTransform(topMask());
		}

		uint16_t bMask = height() - bottomMask();
		if (yt >= bMask)
			return;
		if (yt + h > bMask)
			h -= yt + h - bMask;
	}

	area(x, y, w, h);
	start();
	while (h--)
		for (uint16_t xx = x; xx < w; xx++)
			write16(c);
}

void tft_t::putch(char ch)
{
	// Display y coordinate, start y coordinate
	uint16_t yy = y(), y0 = y();
	uint8_t h = FONT_HEIGHT * zoom();
	uint8_t start = 0, end = h;

	if (transform()) {
		uint16_t yt = vsTransformBack(yy);
		if ((int16_t)yt < (int16_t)topEdge() && \
			(int16_t)(yt + h) >= (int16_t)topEdge()) {	// Top edge clipping
			start = topEdge() - yt;
			yy = upperEdge();
			y0 = yy - start;
			yt = vsTransformBack(yy);
		} else if (yt < bottomEdge() && yt + h >= bottomEdge())	// Bottom edge clipping
			end = bottomEdge() - yt;

		if (yt < topMask()) {
			if (yt + end - start < topMask())
				return;
			yy = vsTransform(topMask());
			start += topMask() - yt;
			y0 = yy - start;
		}

		uint16_t bMask = height() - bottomMask();
		if (yt >= bMask)
			return;
		if (yt + end - start > bMask)
			end -= yt + end - start - bMask;
	}

	area(x(), yy, WIDTH * zoom(), h);
	this->start();
	bool transformed = false;
	for (uint8_t i = start; i < end; i++) {
		if (transform() && !transformed && y0 < bottomEdge() && y0 + i >= bottomEdge()) {
			area(x(), topFixedArea(), WIDTH * zoom(), h);
			this->start();
			transformed = true;
		}
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
	setX(0);
	setY(0);
	d.orient = o;
	_setOrient(o);
}

uint16_t tft_t::vsTransform(uint16_t y) const
{
	if (y < topEdge() || y >= bottomEdge())
		return y;
	y -= topEdge();		// Relative to upperEdge
	y += upperEdge();	// Relative to 0
	if (y >= bottomEdge())	// Transform edge
		y -= vsHeight();
	return y;
}

uint16_t tft_t::vsTransformBack(uint16_t y) const
{
	if (y < topEdge() || y >= bottomEdge())
		return y;
	if (y < upperEdge())
		y += vsHeight();
	y -= upperEdge();	// Relative to upperEdge
	y += topEdge();		// Relative to 0
	return y;
}

void tft_t::bmp(bool e)
{
	if (e)
		_setOrient(orient() + BMPLandscape);
	else
		_setOrient(orient());
}

void tft_t::setVerticalScrolling(const uint16_t vsp)
{
	cmd(0x37);	// Vertical Scrolling Start Address
	write16(vsp);
	d.vsp = vsp;
}

void tft_t::setVerticalScrollingArea(const uint16_t tfa, const uint16_t bfa)
{
	uint16_t vsa = SIZE_H - tfa - bfa;
	cmd(0x33);	// Vertical Scrolling Definition
	write16(tfa);	// Top Fixed Area
	write16(vsa);	// Vertical Scrolling Area
	write16(bfa);	// Bottom Fixed Area
	d.tfa = tfa;
	d.bfa = bfa;
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
