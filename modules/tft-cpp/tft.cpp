/*
 * Author: Yubo Zhi (normanzyb@gmail.com)
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <ctype.h>
#include <stdlib.h>
#include "ascii.h"
#include "tft_hw.h"
#include "tft.h"

#define DEF_FGC	0xFFFF
#define DEF_BGC	0x0000

namespace tft
{
	template <typename Type>
	static inline void swap(Type &a, Type &b);

	uint8_t zoom, orient, tabSize;
	uint16_t x, y, width, height;
	uint16_t foreground, background;
	const struct font_t *font = &fonts;

#ifdef TFT_VERTICAL_SCROLLING
	struct tft_vs_t d;
#endif
}

template <typename Type>
inline void tft::swap(Type &a, Type &b)
{
	Type tmp = a;
	a = b;
	b = tmp;
}

bool tft::setFont(uint8_t w, uint8_t h)
{
	if (font->width == w && font->height == h)
		return true;
	const struct font_t *ptr = &fonts;
	while (ptr != 0)
		if (ptr->width == w && ptr->height == h) {
			font = ptr;
			return true;
		} else
			ptr = ptr->next;
	return false;
}

#ifdef TFT_READ_AVAILABLE
inline void tft::shiftUp(const uint16_t l)
{
	using namespace tfthw;

	uint8_t buff[width * 2];
	uint16_t r;
	tfthw::setColumn(0, width - 1);
	for (r = 0; r < height - l; r++) {
		uint16_t b = width * 2;
		//area(0, r + l, w, 1);
		setPage(r + l, r + l);
		memRead();
		mode(true);	// Read mode
		read();
		while (b--) {	// RGB 888 mode
			buff[b] = read() & 0xF8;
			uint8_t g = read();
			buff[b--] |= g >> 5;
			buff[b] = (g << 3) & 0xE0;
			buff[b] |= read() >> 3;
		}
		mode(false);		// Write mode

		b = width * 2;
		//area(0, r, w, 1);
		setPage(r, r);
		memWrite();
		while (b--)
			write(buff[b]);
	}
	//area(0, h - l, w, l);
	setPage(height - l, height - 1);
	memWrite();
	while (r++ < height)
		for (uint16_t c = width; c; c--)
			write16(background);
}
#endif

void tft::init()
{
	tfthw::init();
	x = 0;
	y = 0;
#ifdef TFT_VERTICAL_SCROLLING
	d.tfa = 0;
	d.bfa = 0;
	d.vsp = vsMaximum();
	setTopMask(0);
	setBottomMask(0);
	setTransform(false);
#endif
	zoom = 1;
	setOrient(TFT_DEF_ORIENT);
	tabSize = 4;
	//width = SIZE_W;
	//height = SIZE_H;
	foreground = DEF_FGC;
	background = DEF_BGC;
}

void tft::putString(const char *str, bool progMem)
{
#ifdef TFT_VERTICAL_SCROLLING
	uint16_t xt = 0;
	bool clip = transform() && !portrait();
	if (clip) {
		xt = vsTransformBack(x);
		clip = xt < bottomEdge();
	}
#endif

	char c;
	while ((c = progMem ? pgm_read_byte(str++) : *str++) != '\0') {
		putChar(c);
#ifdef TFT_VERTICAL_SCROLLING
		if (clip) {
			xt += FONT_WIDTH * zoom;
			if (xt >= bottomEdge())
				break;
		}
#endif
	}
}

void tft::frame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint8_t s, uint16_t c)
{
	rectangle(x, y, w - s, s, c);
	rectangle(x + w - s, y, s, h - s, c);
	rectangle(x, y + s, s, h - s, c);
	rectangle(x + s, y + h - s, w - s, s, c);
}

void tft::line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, \
		uint16_t c)
{
	if (x0 > width)
		x0 = width - 1;
	if (x1 > width)
		x1 = width - 1;
	if (y0 > height)
		y0 = height - 1;
	if (y1 > height)
		y1 = height - 1;
	if (x0 == x1) {
		if (y0 > y1)
			swap(y0, y1);
		rectangle(x0, y0, 1, y1 - y0 + 1, c);
		return;
	}
	if (y0 == y1) {
		if (x0 > x1)
			swap(x0, x1);
		rectangle(x0, y0, x1 - x0 + 1, 1, c);
		return;
	}
	uint16_t dx = abs(x1 - x0), dy = abs(y1 - y0);
	if (dx < dy) {
		if (y0 > y1) {
			swap(x0, x1);
			swap(y0, y1);
		}
		for (uint16_t y = y0; y <= y1; y++)
			point(x0 + dx * (y - y0) / dy * \
					(x0 > x1 ? -1 : 1), y, c);
	} else {
		if (x0 > x1) {
			swap(x0, x1);
			swap(y0, y1);
		}
		for (uint16_t x = x0; x <= x1; x++)
			point(x, y0 + dy * (x - x0) / dx * \
					(y0 > y1 ? -1 : 1), c);
	}
}

void tft::rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c)
{
	if ((int16_t)w <= 0 || (int16_t)h <= 0)
		return;

#ifdef TFT_VERTICAL_SCROLLING
	uint16_t yt, bMask;
	if (!transform())
		goto disp;

	if (!portrait()) {
		swap(x, y);
		swap(w, h);
	}

	yt = vsTransformBack(y);
	if ((int16_t)yt < (int16_t)topEdge() && \
		(int16_t)(yt + h) >= (int16_t)topEdge()) {	// Top edge clipping
		h -= topEdge() - yt;
		y = upperEdge();
		yt = vsTransformBack(y);
	} else if (yt < bottomEdge() && yt + h >= bottomEdge())	// Bottom edge clipping
		h = bottomEdge() - yt;
	if (y + h > bottomEdge())				// Transform edge split
		if (y < bottomEdge()) {
			if (!portrait()) {
				rectangle(y, x, bottomEdge() - y, w, c);
				rectangle(topEdge(), x, h - (bottomEdge() - y), w, c);
			} else {
				rectangle(x, y, w, bottomEdge() - y, c);
				rectangle(x, topEdge(), w, h - (bottomEdge() - y), c);
			}
			return;
		}

	if (yt < topMask()) {
		if (yt + h < topMask())
			return;
		h -= topMask() - yt;
		y = vsTransform(topMask());
	}

	bMask = vsMaximum() - bottomMask();
	if (yt >= bMask)
		return;
	if (yt + h > bMask)
		h -= yt + h - bMask;

	if (!portrait()) {
		tfthw::area(y, x, h, w);
		goto draw;
	}

disp:
#endif
	if ((int16_t)x < 0) {
		w -= -(int16_t)x;
		x = 0;
	}
	if ((int16_t)y < 0) {
		h -= -(int16_t)y;
		y = 0;
	}
	if ((int16_t)w <= 0 || (int16_t)h <= 0)
		return;
	if (x + w > width)
		w = width - x;
	if (y + h > height)
		h = height - y;
	if ((int16_t)w <= 0 || (int16_t)h <= 0)
		return;

	tfthw::area(x, y, w, h);
#ifdef TFT_VERTICAL_SCROLLING
draw:
#endif
	tfthw::memWrite();
	while (h--)
		for (uint16_t xx = 0; xx < w; xx++)
			tfthw::write16(c);
}

void tft::setOrient(uint8_t o)
{
	width = TFT_SIZE_WIDTH;
	height = TFT_SIZE_HEIGHT;
	if (o & Portrait)
		swap(width, height);
	x = 0;
	y = 0;
	orient = o;
	tfthw::setOrient(o);
}

#ifdef TFT_VERTICAL_SCROLLING
uint16_t tft::vsTransform(uint16_t y)
{
#ifdef TFT_CHECKING
	if ((int16_t)y < 0)
		return y;
#endif
	if (y < topEdge() || y >= bottomEdge())
		return y;
	y -= topEdge();		// Relative to upperEdge
	y += upperEdge();	// Relative to 0
	if (y >= bottomEdge())	// Transform edge
		y -= vsHeight();
	return y;
}

uint16_t tft::vsTransformBack(uint16_t y)
{
#ifdef TFT_CHECKING
	if ((int16_t)y < 0)
		return y;
#endif
	if (y < topEdge() || y >= bottomEdge())
		return y;
	if (y < upperEdge())
		y += vsHeight();
	y -= upperEdge();	// Relative to upperEdge
	y += topEdge();		// Relative to 0
	return y;
}

void tft::setVerticalScrolling(const uint16_t vsp)
{
	tfthw::setVSStart(flipped() ? vsMaximum() - vsp : vsp);
	d.vsp = vsp;
}

void tft::setVerticalScrollingArea(const uint16_t tfa, const uint16_t bfa)
{
	uint16_t vsa = vsMaximum() - tfa - bfa;
	if (flipped())
		tfthw::setVSDefinition(bfa, vsa, tfa);
	else
		tfthw::setVSDefinition(tfa, vsa, bfa);
	d.tfa = tfa;
	d.bfa = bfa;
}
#endif

void tft::newline()
{
	using namespace tfthw;
	x = 0;
	y += FONT_HEIGHT * zoom;
	if (y + FONT_HEIGHT * zoom > height) {
#ifdef TFT_SCROLL
		shiftUp(FONT_HEIGHT * zoom);
		y -= FONT_HEIGHT * zoom;
#else
		clean();
#endif
	}
}

void tft::drawChar(char ch)
{
#ifdef TFT_FONTS
#ifdef ASCII_STRIPPED
	drawImage2(font->ptr + ((uint8_t)ch - font->offset) * font->size, x, y, FONT_WIDTH, FONT_HEIGHT, true);
#else
	drawImage2Aligned(font->ptr + ((uint8_t)ch - font->offset) * font->size, x, y, FONT_WIDTH, FONT_HEIGHT, true);
#endif
#else
	using namespace tfthw;
#ifdef TFT_CHECKING
	if ((int16_t)x() >= (int16_t)width() || (int16_t)y() >= (int16_t)height())
		return;
#endif
	if ((int16_t)(x + FONT_WIDTH * zoom) < 0)
		return;
	uint8_t h = FONT_HEIGHT * zoom, w = FONT_WIDTH * zoom;
#ifdef TFT_VERTICAL_SCROLLING
	// Display coordinate, start coordinate
	uint16_t xx = x;
	uint16_t yy = y;
	uint8_t xStart = 0, xStop = w;
	uint8_t yStart = 0, yStop = h;

	if (transform()) {
		if (landscape()) {
			yy = xx;
			yStop = xStop;
			swap(w, h);
		}

		uint16_t yt = vsTransformBack(yy);

#if 0
		if ((int16_t)yt < (int16_t)topEdge() && \
			(int16_t)(yt + h) >= (int16_t)topEdge()) {	// Top edge clipping
			yStart = topEdge() - yt;
			yy = upperEdge();
			yt = vsTransformBack(yy);
		} else if (yt < bottomEdge() && yt + h >= bottomEdge())	// Bottom edge clipping
			yStop = bottomEdge() - yt;
#endif

		// Top mask clipping
		if (yt < topMask()) {
			if (yt + yStop - yStart < topMask())
				return;
			yStart += topMask() - yt;
			yt = topMask();
			yy = vsTransform(yt);
		}

		// Bottom mask clipping
		uint16_t bMask = vsMaximum() - bottomMask();
		if (yt >= bMask)
			return;
		if (yt + yStop - yStart > bMask)
			yStop -= yt + yStop - yStart - bMask;

		if (landscape()) {
			swap(w, h);
			xx = yy;
			xStart = yStart;
			xStop = yStop;
			yy = y;
			yStart = 0;
			yStop = h;
		}
	}

	bool xTransform = transform() && !portrait() && xx < bottomEdge() && xx + xStop - xStart > bottomEdge();
	bool yTransform = transform() && portrait() && yy < bottomEdge() && yy + yStop - yStart > bottomEdge();
	uint8_t xEnd = xTransform ? bottomEdge() - xx : xStop;
draw:
	area(xx, yy, xEnd - xStart, h);
#else
	area(x, y, w, h);
#endif
	memWrite();
#ifdef TFT_VERTICAL_SCROLLING
	for (uint8_t yi = yStart; yi < yStop; yi++) {
		if (yTransform && yy + yi - yStart == bottomEdge()) {
			area(x, topEdge(), w, h);
			memWrite();
			yTransform = false;
		}
#else
	for (uint8_t yi = 0; yi < h; yi++) {
#endif
		unsigned char c;
#ifdef TFT_VERTICAL_SCROLLING
		c = pgm_read_byte(font->ptr + ((uint8_t)ch - font->offset) * font->size + yi / zoom) << (xStart / zoom);
		for (uint8_t xi = xStart; xi < xEnd; xi++) {
#else
		c = pgm_read_byte(font->ptr + ((uint8_t)ch - font->offset) * font->size + yi / zoom);
		for (uint8_t xi = 0; xi < w; xi++) {
#endif
			if (c & 0x80)
				write16(foreground);
			else
				write16(background);
			if ((xi + 1) % zoom == 0)
				c <<= 1;
		}
	}
#ifdef TFT_VERTICAL_SCROLLING
	if (xTransform) {
		xx = topEdge();
		xStart = xEnd;
		xEnd = xStop;
		xTransform = false;
		goto draw;
	}
#endif
#endif
}

void tft::drawImage2(const uint8_t *ptr, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool progMem)
{
	using namespace tfthw;
	uint8_t i = 0, c = 0;
	// TODO: zooming support

#ifdef TFT_VERTICAL_SCROLLING
	uint16_t xt = 0, yt = 0, bMask;
	uint8_t xs = 0, xe = 0;

	if (!transform())
		goto disp;
	if (landscape())
		goto landscape;

	yt = vsTransformBack(y);

	// Top mask clipping
	if (yt < topMask()) {
		if (yt + h <= topMask())
			return;
		uint16_t s = topMask() - yt;
		h -= s;
		yt = topMask();
		ptr += s * w / 8;
		i = (s * w) % 8;
		c = *ptr << i;
	}

	// Bottom mask clipping
	bMask = vsMaximum() - bottomMask();
	if (yt >= bMask)
		return;
	if (yt + h > bMask)
		h -= yt + h - bMask;

	// TODO: topEdge & bottomEdge clipping may need to implemented

	y = vsTransform(yt);
	goto disp;

landscape:
	xt = vsTransformBack(x);

	// Top mask clipping
	if (xt < topMask()) {
		if (xt + w <= topMask())
			return;
		xs = topMask() - xt;
		xt = topMask();
	}

#if 0
	// Bottom mask clipping
	bMask = vsMaximum() - bottomMask();
	if (xt >= bMask)
		return;
	if (xt + w > bMask)
		xe = xt + w - bMask;
#endif

	// TODO: topEdge & bottomEdge clipping may need to implemented

	x = vsTransform(xt);
	w -= xs + xe;
#endif

#ifdef TFT_VERTICAL_SCROLLING
disp:
#endif
	area(x, y, w, h);
	memWrite();
#ifdef TFT_VERTICAL_SCROLLING
	//bool xTransform = transform() && !portrait() && x < bottomEdge() && x + w - xs - xe > bottomEdge();
	bool yTransform = transform() && portrait() && y < bottomEdge() && y + h > bottomEdge();
#endif
	for (uint8_t yy = 0; yy < h; yy++) {
#ifdef TFT_VERTICAL_SCROLLING
		if (yTransform && y + yy == bottomEdge()) {
			area(x, topEdge(), w, h);
			memWrite();
			yTransform = false;
		}
		uint8_t pi = i;
		for (uint8_t xx = 0; xx < xs; xx++) {
			if (i % 8 == 0)
				c = progMem ? pgm_read_byte(ptr++) : *ptr++;
			i++;
		}
		if (xs) {
			//i += xs;
			//ptr += (i + 1) / 8;
			i = pi + xs;
			i %= 8;
			if (i)
				c <<= i;
		}
#endif
		for (uint8_t xx = 0; xx < w; xx++) {
			if (i++ == 0)
				c = progMem ? pgm_read_byte(ptr++) : *ptr++;
			if (c & 0x80)
				write16(foreground);
			else
				write16(background);
			if (i == 8)
				i = 0;
			else
				c <<= 1;
		}
	}
}

void tft::drawImage2Aligned(const uint8_t *ptr, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool progMem)
{
	using namespace tfthw;
	uint8_t i, c = 0;
	// TODO: zooming support
	// TODO: vertical scrolling support

	area(x, y, w, h);
	memWrite();
	for (uint8_t yy = 0; yy < h; yy++) {
		i = 0;
		for (uint8_t xx = 0; xx < w; xx++) {
			if (i++ == 0)
				c = progMem ? pgm_read_byte(ptr++) : *ptr++;
			if (c & 0x80)
				write16(foreground);
			else
				write16(background);
			if (i == 8)
				i = 0;
			else
				c <<= 1;
		}
	}
}

static int tftputchar(const char c, FILE *stream)
{
	tft::putChar(c);
	return 0;
}

FILE *tft::devout()
{
	static FILE *out = NULL;
	if (out == NULL)
		out = fdevopen(tftputchar, NULL);
	return out;
}

void tft::fill(uint16_t clr)
{
	using namespace tfthw;
	uint8_t ch = clr / 0x0100, cl = clr % 0x0100;
	uint16_t x = width, y;
	all();
	memWrite();
	while (x--) {
		y = height;
		while (y--) {
			data(ch);
			data(cl);
		}
	}
}
