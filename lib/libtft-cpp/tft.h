/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef TFT_H
#define TFT_H

// TFT is connected using port BD instead of port AC
//#define TFT_USE_PORT_BD
// Use shifting up instead of clean screen after screen full
//#define TFT_SCROLL
// Enable vertical scrolling functions
//#define TFT_VERTICALSCROLLING
// Enable (unnecessary) draw in range checking
//#define TFT_CHECKING

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include "ili9341.h"
#include "ascii.h"

#define TFT_SIZE_HEIGHT	320
#define TFT_SIZE_WIDTH	240

class tft_t: public ili9341
{
public:
	tft_t(void);

	inline class tft_t& operator<<(const char c);
	inline class tft_t& operator<<(const char *str);

	inline void setX(uint16_t x) {d.x = x;}
	inline void setY(uint16_t y) {d.y = y;}
	inline void setXY(uint16_t x, uint16_t y) {setX(x); setY(y);}
	inline uint16_t x(void) const {return d.x;}
	inline uint16_t y(void) const {return d.y;}
	inline uint16_t width(void) const {return d.w;}
	inline uint16_t height(void) const {return d.h;}
	inline void setZoom(const uint8_t zoom) {d.zoom = zoom;}
	inline uint8_t zoom(void) const {return d.zoom;}
	inline void setForeground(uint16_t c) {d.fgc = c;}
	inline void setBackground(uint16_t c) {d.bgc = c;}
	inline uint16_t foreground(void) const {return d.fgc;}
	inline uint16_t background(void) const {return d.bgc;}
	void bmp(bool e);
	void setOrient(uint8_t o);
	inline uint8_t orient(void) const {return d.orient;}
	inline void setBGLight(bool e) {_setBGLight(e);}
	inline void setTabSize(uint8_t t) {d.tabSize = t;}
	inline uint8_t tabSize(void) const {return d.tabSize;}
	inline bool flipped(void) const {return orient() == FlipPortrait || orient() == FlipLandscape;}
	inline bool portrait(void) const {return orient() == Portrait || orient() == FlipPortrait;}
	void putString(const char *str, bool progMem = false);

#ifdef TFT_VERTICALSCROLLING
	// Vertical scrolling related functions
	// Vertical scrolling pointer
	void setVerticalScrolling(const uint16_t vsp);
	// Top fixed area, bottom fixed area
	void setVerticalScrollingArea(const uint16_t tfa, const uint16_t bfa);
	// Vertical scrolling mode drawing auto transform
	inline bool transform(void) const {return d.tf;}
	inline void setTransform(const bool on) {d.tf = on;}
	// Vertical scrolling mode helper functions
	inline uint16_t vsMaximum(void) const;
	inline uint16_t topFixedArea(void) const {return d.tfa;}
	inline uint16_t bottomFixedArea(void) const {return d.bfa;}
	inline uint16_t vsHeight(void) const {return bottomEdge() - topFixedArea();}
	inline uint16_t topEdge(void) const {return topFixedArea();}
	inline uint16_t bottomEdge(void) const {return vsMaximum() - bottomFixedArea();}
	inline uint16_t upperEdge(void) const {return d.vsp;}
	inline uint16_t lowerEdge(void) const {return upperEdge() == topFixedArea() ? bottomEdge() : upperEdge();}
	// Vertical scrolling mode drawing coordinate transform
	uint16_t vsTransform(uint16_t y) const;
	uint16_t vsTransformBack(uint16_t y) const;
	// Refresh region mask, not transformed
	inline uint16_t topMask(void) const {return d.topMask;}
	inline void setTopMask(const uint16_t lm) {d.topMask = lm;}
	inline uint16_t bottomMask(void) const {return d.bottomMask;}
	inline void setBottomMask(const uint16_t lm) {d.bottomMask = lm;}
#endif

	inline void clean(void) {fill(background()); setX(0); setY(0);}
	void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, \
		uint16_t c);
	void frame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint8_t s, uint16_t c);
	inline void fill(uint16_t clr);
	void putch(char ch);
	void rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint16_t c);
	inline void point(uint16_t x, uint16_t y, uint16_t c);
	inline void shiftUp(const uint16_t l);

	inline void area(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	inline void all(void) {area(0, 0, width(), height());}
	static inline void start(void) {cmd(0x2C);}	// Memory write
	static inline void write16(uint16_t c) {data(c / 0x0100); data(c % 0x0100);}

private:
	inline void setWidth(const uint16_t w) {d.w = w;}
	inline void setHeight(const uint16_t h) {d.h = h;}

	inline void newline(void);
	inline void next(void);
	inline void tab(void);

	template <typename Type>
	static inline void swap(Type &a, Type &b);

	struct {
		uint8_t zoom, orient, tabSize;
		uint16_t x, y, w, h, fgc, bgc;
#ifdef TFT_VERTICALSCROLLING
		uint16_t vsp, tfa, bfa, topMask, bottomMask;
		bool tf;
#endif
	} d;
};

FILE *tftout(tft_t *hw);

// Defined as inline to execute faster

template <typename Type>
inline void tft_t::swap(Type &a, Type &b)
{
	Type tmp = a;
	a = b;
	b = tmp;
}

#ifdef TFT_VERTICALSCROLLING
inline uint16_t tft_t::vsMaximum(void) const
{
	return TFT_SIZE_HEIGHT;
}
#endif

inline void tft_t::shiftUp(const uint16_t l)
{
	// 0x2C Write, 0x2E Read, 0x3C / 0x3E Continue, 0x00 NOP
	uint8_t buff[width() * 2];
	uint16_t r;
	cmd(0x2A);			// Column Address Set
	write16(0);
	write16(width() - 1);
	for (r = 0; r < height() - l; r++) {
		uint16_t b = width() * 2;
		//area(0, r + l, w, 1);
		cmd(0x2B);		// Page Address Set
		write16(r + l);
		write16(r + l);
		cmd(0x2E);		// Read
		mode(true);		// Read mode
		recv();
		while (b--) {
			buff[b] = recv() & 0xF8;
			uint8_t g = recv();
			buff[b--] |= g >> 5;
			buff[b] = (g << 3) & 0xE0;
			buff[b] |= recv() >> 3;
		}
		mode(false);		// Write mode

		b = width() * 2;
		//area(0, r, w, 1);
		cmd(0x2B);		// Page Address Set
		write16(r);
		write16(r);
		start();
		while (b--)
			data(buff[b]);
	}
	//area(0, h - l, w, l);
	cmd(0x2B);		// Page Address Set
	write16(height() - l);
	write16(height() - 1);
	start();
	while (r++ < height())
		for (uint16_t c = width(); c; c--)
			write16(background());
}

inline class tft_t& tft_t::operator<<(const char c)
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
		putch(c);
		next();
	}
	return *this;
}

inline class tft_t& tft_t::operator<<(const char *str)
{
	putString(str);
	return *this;
}

inline void tft_t::point(uint16_t x, uint16_t y, uint16_t c)
{
	if (x >= width() || y >= height())
		return;
	area(x, y, 1, 1);
	start();
	write16(c);
}

inline void tft_t::newline(void)
{
	setX(0);
	setY(y() + FONT_HEIGHT * zoom());
	if (y() + FONT_HEIGHT * zoom() > height()) {
#ifdef TFT_SCROLL
		*this ^= FONT_HEIGHT * zoom;
		y -= FONT_HEIGHT * zoom;
#else
		fill(background());
		setY(0);
#endif
	}
}

inline void tft_t::fill(uint16_t clr)
{
	uint8_t ch = clr / 0x0100, cl = clr % 0x0100;
	uint16_t x = width(), y;
	all();
	start();
	while (x--) {
		y = height();
		while (y--) {
			data(ch);
			data(cl);
		}
	}
}

inline void tft_t::area(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	cmd(0x2A);			// Column Address Set
	write16(x);
	write16(x + w - 1);
	cmd(0x2B);			// Page Address Set
	write16(y);
	write16(y + h - 1);
}

inline void tft_t::next(void)
{
#ifdef TFT_VERTICALSCROLLING
	if (transform() && !portrait()) {
		uint16_t xt = vsTransformBack(x());
		setX(vsTransform(xt + FONT_WIDTH * zoom()));
	} else {
#endif
		setX(x() + FONT_WIDTH * zoom());
		if (x() + FONT_WIDTH * zoom() > width())
			newline();
#ifdef TFT_VERTICALSCROLLING
	}
#endif
}

inline void tft_t::tab(void)
{
	if (x() % (FONT_WIDTH * zoom()))
		setX(x() - x() % (FONT_WIDTH * zoom()));
	do
		next();
	while ((x() / (FONT_WIDTH * zoom())) % tabSize());
}

#endif
