#include <avr/io.h>
#include "ascii.h"
#include "ili9341.h"
#include "tft.h"

#define WIDTH 6
#define HEIGHT 8

void TFT_putbigint16(uint8_t x, uint16_t y, uint8_t z, uint16_t i, \
		uint16_t fgc, uint16_t bgc)
{
	uint16_t p = 10000;
	while ((p != 1) && (i / p == 0))
		p /= 10;
	while (p != 0) {
		TFT_putbigchar(x, y, z, (i / p) % 10 + '0', fgc, bgc);
		x += WIDTH * z;
		p /= 10;
	}
}

void TFT_putint16(uint8_t x, uint16_t y, uint16_t i, \
		uint16_t fgc, uint16_t bgc)
{
	uint16_t p = 10000;
	while ((p != 1) && (i / p == 0))
		p /= 10;
	while (p != 0) {
		TFT_putchar(x, y, (i / p) % 10 + '0', fgc, bgc);
		x += WIDTH;
		p /= 10;
	}
}

void TFT_frame(uint8_t x, uint16_t y, uint16_t w, uint16_t h, \
		uint8_t s, uint16_t c)
{
	TFT_rectangle(x, y, w - s, s, c);
	TFT_rectangle(x + w - s, y, s, h - s, c);
	TFT_rectangle(x, y + s, s, h - s, c);
	TFT_rectangle(x + s, y + h - s, w - s, s, c);
}

void TFT_fill(uint16_t clr)
{
	uint8_t c;
	uint16_t r;
	TFT_area(0, 0, 240, 320);
	TFT_send(1, 0x2C);			// Memory Write
	for (r = 0; r < 320; r++)
		for (c = 0; c < 240; c++) {
			TFT_send(0, clr / 0x0100);
			TFT_send(0, clr % 0x0100);
		}
}

void TFT_point(uint8_t x, uint16_t y, uint16_t c)
{
	TFT_area(x, y, 1, 1);
	TFT_send(1, 0x2C);			// Memory Write
	TFT_send(0, c / 0x0100);
	TFT_send(0, c % 0x0100);
}

void TFT_area(uint8_t x, uint16_t y, uint8_t w, uint16_t h)
{
	TFT_send(1, 0x2A);			// Column Address Set
	TFT_send(0, 0x00);
	TFT_send(0, x);
	TFT_send(0, 0x00);
	TFT_send(0, x + w - 1);
	TFT_send(1, 0x2B);			// Page Address Set
	TFT_send(0, y / 0x0100);
	TFT_send(0, y % 0x0100);
	TFT_send(0, (y + h - 1) / 0x0100);
	TFT_send(0, (y + h - 1) % 0x0100);
}

void TFT_rectangle(uint8_t x, uint16_t y, uint8_t w, uint16_t h, uint16_t c)
{
	TFT_area(x, y, w, h);
	TFT_send(1, 0x2C);			// Memory Write
	while (w--)
		for (y = 0; y < h; y++) {
			TFT_send(0, c / 0x0100);
			TFT_send(0, c % 0x0100);
		}
}

void TFT_putchar(uint8_t x, uint16_t y, char ch, \
		uint16_t fgc, uint16_t bgc)
{
	unsigned char c;
	uint8_t i, j;
	TFT_area(x, y, WIDTH, HEIGHT);
	TFT_send(1, 0x2C);			// Memory Write
	for (i = 0; i < HEIGHT; i++) {
		c = pgm_read_byte(&(ascii[ch - ' '][i]));
		for (j = 0; j < WIDTH; j++) {
			if (c & 0x80) {
				TFT_send(0, fgc / 0x0100);
				TFT_send(0, fgc % 0x0100);
			} else {
				TFT_send(0, bgc / 0x0100);
				TFT_send(0, bgc % 0x0100);
			}
			c <<= 1;
		}
	}
}

void TFT_putbigchar(uint8_t x, uint16_t y, uint8_t z, char ch, \
		uint16_t fgc, uint16_t bgc)
{
	unsigned char c;
	uint8_t i, j;
	TFT_area(x, y, WIDTH * z, HEIGHT * z);
	TFT_send(1, 0x2C);			// Memory Write
	for (i = 0; i < HEIGHT * z; i++) {
		c = pgm_read_byte(&(ascii[ch - ' '][i / z]));
		for (j = 0; j < WIDTH * z; j++) {
			if (c & 0x80) {
				TFT_send(0, fgc / 0x0100);
				TFT_send(0, fgc % 0x0100);
			} else {
				TFT_send(0, bgc / 0x0100);
				TFT_send(0, bgc % 0x0100);
			}
			if (j % z == z - 1)
				c <<= 1;
		}
	}
}

void TFT_puts(uint8_t x, uint16_t y, char *str, \
		uint16_t fgc, uint16_t bgc)
{
	while (*str != '\0') {
		TFT_putchar(x, y, *str++, fgc, bgc);
		x += WIDTH;
	}
}

void TFT_putbigs(uint8_t x, uint16_t y, uint8_t z, char *str, \
		uint16_t fgc, uint16_t bgc)
{
	while (*str != '\0') {
		TFT_putbigchar(x, y, z, *str++, fgc, bgc);
		x += WIDTH * z;
	}
}
