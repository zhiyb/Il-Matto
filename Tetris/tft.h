#ifndef _TFT_H
#define _TFT_H

#include "ili9341.h"

void TFT_frame(uint8_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint8_t s, uint16_t c);
void TFT_fill(uint16_t clr);
void TFT_area(uint8_t x, uint16_t y, uint8_t w, uint16_t h);
void TFT_putchar(uint8_t x, uint16_t y, char ch, \
		uint16_t fgc, uint16_t bgc);
void TFT_putbigchar(uint8_t x, uint16_t y, uint8_t z, char ch, \
		uint16_t fgc, uint16_t bgc);
void TFT_puts(uint8_t x, uint16_t y, char *str, \
		uint16_t fgc, uint16_t bgc);
void TFT_putbigs(uint8_t x, uint16_t y, uint8_t z, char *str, \
		uint16_t fgc, uint16_t bgc);
void TFT_rectangle(uint8_t x, uint16_t y, uint8_t w, uint16_t h, uint16_t c);
void TFT_point(uint8_t x, uint16_t y, uint16_t c);
void TFT_putint16(uint8_t x, uint16_t y, uint16_t i, \
		uint16_t fgc, uint16_t bgc);
void TFT_putbigint16(uint8_t x, uint16_t y, uint8_t z, uint16_t i, \
		uint16_t fgc, uint16_t bgc);

#endif
