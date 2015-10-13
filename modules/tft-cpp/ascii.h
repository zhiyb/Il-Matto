/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef ASCII_H
#define ASCII_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/pgmspace.h>

//#define ASCII_STRIPPED

struct font_t {
	const unsigned char width, height, size, offset;
	const unsigned char *ptr;
	const struct font_t *next;
};

// Better not use PROGMEM?
extern const struct font_t fonts;

#ifdef __cplusplus
}
#endif

#endif
