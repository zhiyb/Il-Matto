/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef MACROS_H
#define MACROS_H

#define CONCAT(a,b)	a ## b
#define CONCAT_E(a,b)	CONCAT(a, b)

#define STRINGIFY(x)	#x
#define TOSTRING(x)	STRINGIFY(x)

#define COLOUR888(r, g, b)	(((r) << 16) | ((g) << 8) | ((b) << 0))
#define RED888(r)		(((r) >> 16) & 0xff)
#define GREEN888(g)		(((g) >> 8) & 0xff)
#define BLUE888(b)		(((b) >> 0) & 0xff)

#endif
