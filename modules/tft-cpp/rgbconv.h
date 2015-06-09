/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef RGBCONV_H
#define RGBCONV_H

#include <inttypes.h>

namespace conv
{
	static inline uint16_t c16i(uint16_t c16);
	static inline uint32_t c32i(uint32_t c32);
	static inline uint16_t c32to16(uint32_t c32);
	static inline uint16_t c32to16i(uint32_t c32);
	static inline uint32_t c32(uint8_t red, uint8_t green, uint8_t blue);
}

// Inline functions
static inline uint16_t conv::c16i(uint16_t c16)
{
	// 16-bit: rrrr rggg gggb bbbb
	return ((c16 & 0xF800) >> 11) + \
		(c16 & 0x07E0) + \
		((c16 & 0x001F) << 11);
}

static inline uint32_t conv::c32i(uint32_t c32)
{
	return ((c32 & 0x00FF0000) >> 16) + \
		(c32 & 0x0000FF00) + \
		((c32 & 0x000000FF) << 16);
}

static inline uint16_t conv::c32to16(uint32_t c32)
{
	// 16-bit: rrrr rggg gggb bbbb
	return ((c32 & 0x00F80000) >> 8) + \
		((c32 & 0x0000FC00) >> 5) + \
		((c32 & 0x000000F8) >> 3);
}

static inline uint16_t conv::c32to16i(uint32_t c32)
{
	return c32to16(c32i(c32));
}

static inline uint32_t conv::c32(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((uint32_t)red << 16) | \
		((uint32_t)green << 8) | \
		((uint32_t)blue << 0);
}

#endif
