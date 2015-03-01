#ifndef ASCII_H
#define ASCII_H

#ifdef __cplusplus
extern "C" {
#endif

#define FONT_WIDTH 6
#define FONT_HEIGHT 8

#include <avr/pgmspace.h>

extern const unsigned char ascii[96][8] PROGMEM;

#ifdef __cplusplus
}
#endif

#endif
