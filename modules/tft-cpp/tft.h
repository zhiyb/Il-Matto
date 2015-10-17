/*
 * Author: Yubo Zhi (normanzyb@gmail.com)
 */

#ifndef TFT_H
#define TFT_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "tft_defs.h"
#include "tft_hw.h"
#include "ascii.h"

namespace tft
{
	void init();

	void setOrient(uint8_t o);
	static inline void bmp(bool e) {tfthw::setOrient(orient | (e ? BMPMode : 0));}
	static inline void setBGLight(bool e) {tfthw::setBGLight(e);}
	static inline void putChar(const char c);
	void putString(const char *str, bool progMem = false);
	FILE *devout();

	// Vertical scrolling related functions
#ifdef TFT_VERTICAL_SCROLLING
	// Vertical scrolling pointer
	void setVerticalScrolling(const uint16_t vsp);
	// Top fixed area, bottom fixed area
	void setVerticalScrollingArea(const uint16_t tfa, const uint16_t bfa);
	// Vertical scrolling mode drawing auto transform
	static inline bool transform() {return d.tf;}
	static inline void setTransform(const bool on) {d.tf = on;}
	// Vertical scrolling mode helper functions
	static inline uint16_t vsMaximum() {return TFT_SIZE_WIDTH;}
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

	void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t c);
	void frame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t s, uint16_t c);
	void fill(uint16_t clr);
	static inline void clean() {fill(background); x = 0; y = 0;}
	void drawChar(char ch);
	void rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c);
	void drawImage2(const uint8_t *ptr, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool progMem = false);
	void drawImage2Aligned(const uint8_t *ptr, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool progMem = false);
	static inline void point(uint16_t x, uint16_t y, uint16_t c);
#ifdef TFT_READ_AVAILABLE
	void shiftUp(const uint16_t l);
#endif

	// Private functions
	void newline();
	static inline void next();
	static inline void tab();
}

namespace tfthw
{
	static inline void area(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	static inline void all() {area(0, 0, tft::width, tft::height);}
}

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
		if (x + FONT_WIDTH * zoom > width)
			newline();
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
		//if (x + FONT_WIDTH * zoom > width)
		//	newline();
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
