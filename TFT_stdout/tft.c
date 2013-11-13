#include <avr/io.h>
#include "ascii.h"
#include "ili9341.h"
#include "tft.h"

#define WIDTH 6
#define HEIGHT 8
#define SIZE_W 240
#define SIZE_H 320
#define FC 0xFFFF
#define BC 0x0000

uint8_t x = 0;
uint16_t y = 0;

void next(void);
void newline(void);
int tftputch(const char c, FILE *stream);

FILE *tftout(void)
{
	static FILE *out = NULL;
	if (out == NULL)
		out = fdevopen(tftputch, NULL);
	return out;
}

int tftputch(const char c, FILE *stream)
{
	if (c == '\n')
		newline();
	else {
		TFT_putchar(x, y, c, FC, BC);
		next();
	}
	return 0;
}

void next(void)
{
	x += WIDTH;
	if (x + WIDTH > SIZE_W)
		newline();
}

void newline(void)
{
	x = 0;
	y += HEIGHT;
	if (y + HEIGHT > SIZE_H) {
		TFT_fill(BC);
		y = 0;
	}
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
