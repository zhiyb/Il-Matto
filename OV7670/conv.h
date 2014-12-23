#ifndef CONV_H
#define CONV_H

#include <stdio.h>

namespace conv
{
	static inline uint32_t uint24(uint8_t block[3]);
	static inline uint32_t uint32(uint8_t block[4]);

	static inline uint32_t uint24(FILE *f);
	static inline uint32_t uint32(FILE *f);
}

// Inline functions
static inline uint32_t conv::uint24(uint8_t block[3])
{
	return (uint32_t)block[0] | \
		((uint32_t)block[1] * 0x0100) | \
		((uint32_t)block[2] * 0x00010000);
}

static inline uint32_t conv::uint32(uint8_t block[4])
{
	return (uint32_t)block[0] | \
		((uint32_t)block[1] * 0x0100) | \
		((uint32_t)block[2] * 0x00010000) | \
		((uint32_t)block[3] * 0x01000000);
}

static inline uint32_t conv::uint24(FILE *f)
{
	uint32_t res = (uint8_t)fgetc(f);
	res |= (uint32_t)(uint8_t)fgetc(f) * 0x0100;
	res |= (uint32_t)(uint8_t)fgetc(f) * 0x00010000;
	return res;
}

static inline uint32_t conv::uint32(FILE *f)
{
	uint32_t res = (uint8_t)fgetc(f);
	res |= (uint32_t)(uint8_t)fgetc(f) * 0x0100;
	res |= (uint32_t)(uint8_t)fgetc(f) * 0x00010000;
	res |= (uint32_t)(uint8_t)fgetc(f) * 0x01000000;
	return res;
}

#endif
