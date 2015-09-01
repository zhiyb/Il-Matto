/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef TFT_H
#define TFT_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "tft_hw.h"
#include "ascii.h"

namespace tft
{
	void init();

	// Orientation should only be set with setOrient()
	extern uint8_t zoom, orient, tabSize;
	// Width & height should only be manipulated by setOrient()
	extern uint16_t x, y, width, height;
	extern uint16_t foreground, background;	// Colours

#ifdef TFT_VERTICAL_SCROLLING
	extern struct tft_vs_t {
		uint16_t vsp, tfa, bfa, topMask, bottomMask;
		bool tf;
	} d;
#endif

	void bmp(bool e);
	void setOrient(uint8_t o);
	static inline void setBGLight(bool e) {tfthw::setBGLight(e);}
	static inline bool flipped() {return orient == FlipPortrait || orient == FlipLandscape;}
	static inline bool portrait() {return orient == Portrait || orient == FlipPortrait;}
	static inline bool landscape() {return !portrait();}
	static inline void putChar(const char c);
	void putString(const char *str, bool progMem = false);
	void drawImage2(const uint8_t *ptr, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool progMem = false);

#ifdef TFT_VERTICAL_SCROLLING
	// Vertical scrolling related functions
	// Vertical scrolling pointer
	void setVerticalScrolling(const uint16_t vsp);
	// Top fixed area, bottom fixed area
	void setVerticalScrollingArea(const uint16_t tfa, const uint16_t bfa);
	// Vertical scrolling mode drawing auto transform
	static inline bool transform() {return d.tf;}
	static inline void setTransform(const bool on) {d.tf = on;}
	// Vertical scrolling mode helper functions
	static inline uint16_t vsMaximum() {return TFT_SIZE_HEIGHT;}
	static inline uint16_t topFixedArea() {return d.tfa;}
	static inline uint16_t bottomFixedArea() {return d.bfa;}
	static inline uint16_t topEdge() {return topFixedArea();}
	static inline uint16_t bottomEdge() {return vsMaximum() - bottomFixedArea();}
	static inline uint16_t upperEdge() {return d.vsp;}
	static inline uint16_t lowerEdge() {return upperEdge() == topFixedArea() ? bottomEdge() : upperEdge();}
	static inline uint16_t vsHeight() {return bottomEdge() - topFixedArea();}
	// Vertical scrolling mode drawing coordinate transform
	uint16_t vsTransform(uint16_t y);
	uint16_t vsTransformBack(uint16_t y);
	// Refresh region mask, not transformed
	static inline uint16_t topMask() {return d.topMask;}
	static inline void setTopMask(const uint16_t lm) {d.topMask = lm;}
	static inline uint16_t bottomMask() {return d.bottomMask;}
	static inline void setBottomMask(const uint16_t lm) {d.bottomMask = lm;}
	// Return to normal mode (disable transform, vertical scrolling)
	static inline void vsNormal() {setTransform(false); setVerticalScrolling(topEdge());}
#endif

	void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, \
		uint16_t c);
	void frame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint8_t s, uint16_t c);
	void fill(uint16_t clr);
	static inline void clean() {fill(background); x = 0; y = 0;}
	void drawChar(char ch);
	void rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint16_t c);
	static inline void point(uint16_t x, uint16_t y, uint16_t c);
#ifdef TFT_READ_AVAILABLE
	void shiftUp(const uint16_t l);
#endif

	// Private functions
	inline void newline();
	inline void next();
	inline void tab();
}

namespace tfthw
{
	static inline void area(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	static inline void all() {area(0, 0, tft::width, tft::height);}
}

FILE *tftout();

// Defined as inline to execute faster

static inline void tft::putChar(const char c)
{
	switch (c) {
	case '\n':
		newline();
		break;
	case '\t':
		tab();
		break;
	default:
		if ((unsigned)c < ' ')
			break;
		drawChar(c);
		next();
	}
}

inline void tft::point(uint16_t x, uint16_t y, uint16_t c)
{
	using namespace tfthw;
	if (x >= width || y >= height)
		return;
	area(x, y, 1, 1);
	memWrite();
	write16(c);
}

inline void tft::newline()
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

inline void tfthw::area(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	using namespace tfthw;
	setColumn(x, x + w - 1);
	setPage(y, y + h - 1);
}

inline void tft::next()
{
#ifdef TFT_VERTICAL_SCROLLING
	if (transform() && !portrait()) {
		uint16_t xt = vsTransformBack(x);
		x = vsTransform(xt + FONT_WIDTH * zoom);
	} else {
#endif
		x += FONT_WIDTH * zoom;
		if (x + FONT_WIDTH * zoom > width)
			newline();
#ifdef TFT_VERTICAL_SCROLLING
	}
#endif
}

inline void tft::tab()
{
	if (x % (FONT_WIDTH * zoom))
		x -= x % (FONT_WIDTH * zoom);
	do
		next();
	while (x / (FONT_WIDTH * zoom) % tabSize);
}

#endif
