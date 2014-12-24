#ifndef CONV_H
#define CONV_H

#include <ctype.h>
#include <inttypes.h>

namespace conv
{
	static inline uint32_t uint24(uint8_t block[3]);
	static inline uint32_t uint32(uint8_t block[4]);

	static inline uint32_t uint24(FILE *f);
	static inline uint32_t uint32(FILE *f);

	static inline char ucs2decode(const uint8_t a, const uint8_t b);
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

static inline char conv::ucs2decode(const uint8_t a, const uint8_t b)
{
	if (a == '\0' && b == '\0')
		return '\0';
	if (!isprint(a) || b != 0)
		return '?';
	return a;
}

#endif
