/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef MACROS_H
#define MACROS_H

#include <stdint.h>

#define _NOP() __asm__ __volatile__("nop")

#define CONCAT(a,b)	a ## b
#define CONCAT_E(a,b)	CONCAT(a, b)

#define STRINGIFY(x)	#x
#define TOSTRING(x)	STRINGIFY(x)

#define COLOUR888(r, g, b)	(((uint32_t)(r) << 16) | \
				((uint32_t)(g) << 8) | \
				((uint32_t)(b) << 0))
#define RED888(r)		(((uint32_t)(r) >> 16) & 0xff)
#define GREEN888(g)		(((uint32_t)(g) >> 8) & 0xff)
#define BLUE888(b)		(((uint32_t)(b) >> 0) & 0xff)

#endif
