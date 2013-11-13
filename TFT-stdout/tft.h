#ifndef _TFT_H
#define _TFT_H

#include <stdio.h>
#include "ili9341.h"

FILE *tftout(void);
void TFT_fill(uint16_t clr);
void TFT_area(uint8_t x, uint16_t y, uint8_t w, uint16_t h);
void TFT_putchar(uint8_t x, uint16_t y, char ch, \
		uint16_t fgc, uint16_t bgc);

#endif
