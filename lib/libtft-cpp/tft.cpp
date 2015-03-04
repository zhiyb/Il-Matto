/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
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

tft_t::tft_t(void)
{
	setX(0);
	setY(0);
#ifdef TFT_VERTICALSCROLLING
	d.tfa = 0;
	d.bfa = 0;
	d.vsp = SIZE_H;
	setTopMask(0);
	setBottomMask(0);
	setTransform(false);
#endif
	setZoom(1);
	d.orient = Portrait;
	setTabSize(4);
	setWidth(SIZE_W);
	setHeight(SIZE_H);
	setForeground(DEF_FGC);
	setBackground(DEF_BGC);
}

void tft_t::putString(const char *str, bool progMem)
{
#ifdef TFT_VERTICALSCROLLING
	uint16_t xt = 0;
	bool clip = transform() && !portrait();
	if (clip) {
		xt = vsTransformBack(x());
		clip = xt < bottomEdge();
	}
#endif

	char c;
	while ((c = progMem ? pgm_read_byte(str++) : *str++) != '\0') {
		*this << c;
#ifdef TFT_VERTICALSCROLLING
		if (clip) {
			xt += FONT_WIDTH * zoom();
			if (xt >= bottomEdge())
				break;
		}
#endif
	}
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

#ifdef TFT_VERTICALSCROLLING
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
		area(y, x, h, w);
		goto draw;
	}

disp:
#endif
	area(x, y, w, h);
#ifdef TFT_VERTICALSCROLLING
draw:
#endif
	start();
	while (h--)
		for (uint16_t xx = 0; xx < w; xx++)
			write16(c);
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

#ifdef TFT_VERTICALSCROLLING
uint16_t tft_t::vsTransform(uint16_t y) const
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

uint16_t tft_t::vsTransformBack(uint16_t y) const
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
#endif

void tft_t::bmp(bool e)
{
	if (e)
		_setOrient(orient() + BMPLandscape);
	else
		_setOrient(orient());
}

#ifdef TFT_VERTICALSCROLLING
void tft_t::setVerticalScrolling(const uint16_t vsp)
{
	cmd(0x37);	// Vertical Scrolling Start Address
	write16(flipped() ? vsMaximum() - vsp : vsp);
	d.vsp = vsp;
}

void tft_t::setVerticalScrollingArea(const uint16_t tfa, const uint16_t bfa)
{
	uint16_t vsa = SIZE_H - tfa - bfa;
	cmd(0x33);	// Vertical Scrolling Definition
	if (flipped()) {
		write16(bfa);	// Top Fixed Area
		write16(vsa);	// Vertical Scrolling Area
		write16(tfa);	// Bottom Fixed Area
	} else {
		write16(tfa);	// Top Fixed Area
		write16(vsa);	// Vertical Scrolling Area
		write16(bfa);	// Bottom Fixed Area
	}
	d.tfa = tfa;
	d.bfa = bfa;
}
#endif

void tft_t::putch(char ch)
{
#ifdef TFT_CHECKING
	if ((int16_t)x() >= (int16_t)width() || (int16_t)y() >= (int16_t)height())
		return;
#endif
	if ((int16_t)(x() + FONT_WIDTH * zoom()) < 0)
		return;
	uint8_t h = FONT_HEIGHT * zoom(), w = FONT_WIDTH * zoom();
#ifdef TFT_VERTICALSCROLLING
	// Display coordinate, start coordinate
	uint16_t xx = x();
	uint16_t yy = y();
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
			yy = y();
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
	area(x(), y(), w, h);
#endif
	start();
#ifdef TFT_VERTICALSCROLLING
	for (uint8_t yi = yStart; yi < yStop; yi++) {
		if (yTransform && yy + yi - yStart == bottomEdge()) {
			area(x(), topEdge(), w, h);
			start();
			yTransform = false;
		}
#else
	for (uint8_t yi = 0; yi < h; yi++) {
#endif
		unsigned char c;
#ifdef TFT_VERTICALSCROLLING
		c = pgm_read_byte(&(ascii[ch - ' '][yi / zoom()])) << (xStart / zoom());
		for (uint8_t xi = xStart; xi < xEnd; xi++) {
#else
		c = pgm_read_byte(&(ascii[ch - ' '][yi / zoom()]));
		for (uint8_t xi = 0; xi < w; xi++) {
#endif
			if (c & 0x80)
				write16(foreground());
			else
				write16(background());
			if ((xi + 1) % zoom() == 0)
				c <<= 1;
		}
	}
#ifdef TFT_VERTICALSCROLLING
	if (xTransform) {
		xx = topEdge();
		xStart = xEnd;
		xEnd = xStop;
		xTransform = false;
		goto draw;
	}
#endif
}

void tft_t::drawImage2(const uint8_t *ptr, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool progMem)
{
	uint8_t i = 0, c = 0;
	// TODO: zooming support

#ifdef TFT_VERTICALSCROLLING
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

#ifdef TFT_VERTICALSCROLLING
disp:
#endif
	area(x, y, w, h);
	start();
#ifdef TFT_VERTICALSCROLLING
	//bool xTransform = transform() && !portrait() && x < bottomEdge() && x + w - xs - xe > bottomEdge();
	bool yTransform = transform() && portrait() && y < bottomEdge() && y + h > bottomEdge();
#endif
	for (uint8_t yy = 0; yy < h; yy++) {
#ifdef TFT_VERTICALSCROLLING
		if (yTransform && y + yy == bottomEdge()) {
			area(x, topEdge(), w, h);
			start();
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
				write16(foreground());
			else
				write16(background());
			if (i == 8)
				i = 0;
			else
				c <<= 1;
		}
	}
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
