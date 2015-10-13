/*
 * Author: Yubo Zhi (normanzyb@gmail.com)
 */

#ifndef TFT_DEFS_H
#define TFT_DEFS_H

#include <stdint.h>
#include "ascii.h"

namespace tft
{
	enum Orientation {Landscape = 0x00, Portrait = 0x01, \
		Flipped = 0x02, BMPMode = 0x04, Orientations = 8
	};

	// Orientation should only be set with setOrient()
	extern uint8_t zoom, orient, tabSize;
	// Width & height should only be manipulated by setOrient()
	extern uint16_t x, y, width, height;
	extern uint16_t foreground, background;	// Colours
	extern const struct font_t *font;

#ifdef TFT_VERTICAL_SCROLLING
	extern struct tft_vs_t {
		uint16_t vsp, tfa, bfa, topMask, bottomMask;
		bool tf;
	} d;
#endif

	static inline uint8_t orientation() {return orient & ~BMPMode;}
	static inline bool flipped() {return orient & Flipped;}
	static inline bool portrait() {return orient & Portrait;}
	static inline bool landscape() {return !portrait();}
	bool setFont(uint8_t w, uint8_t h);
}

#define FONT_WIDTH	(tft::font->width)
#define FONT_HEIGHT	(tft::font->height)

#endif
