#ifndef _LED_H
#define _LED_H

#define LED_DDDR	DDRA
#define LED_DPORT	PORTA
#define LED_DPIN	PINA
#define LED_L(d)	(d)
#define LED_LMASK	(0x0F)
#define LED_D(d)	(d << 4)
#define LED_DMASK	(0xF0)
#define LED_LA		_BV(PB0)
#define LED_LB		_BV(PB1)
#define LED_LC		_BV(PB2)
#define LED_LD		_BV(PB3)
#define LED_GU		_BV(PB4)
#define LED_GD		_BV(PB5)
#define LED_RU		_BV(PB6)
#define LED_RD		_BV(PB7)

#define LED_CDDR	DDRC
#define LED_CPORT	PORTC
#define LED_CPIN	PINC
#define LED_EN		_BV(PC2)
#define LED_STB		_BV(PC3)
#define LED_CLK		_BV(PC4)

#define LED_W	64
#define LED_H	32

#include <inttypes.h>

typedef int8_t int_t;
typedef uint8_t uint_t;

namespace led
{
	enum Levels {Foreground = 0, Background = 2};
	enum Colours {None = 0, Red = 1, Green = 2, Orange = Red | Green, Yellow = Orange, \
		AllRed = Red << Foreground | Red << Background, AllGreen = Green << Foreground | Green << Background, \
		FGC = ~(0xFF << Background), BGC = ~FGC};
	enum BuffColours {BuffRed = 0, BuffGreen = 1};

	void init(void);

	void fill(const uint_t clr = None);
	void drawPoint(const uint_t x, const uint_t y, const uint_t clr);
	void drawChar(const uint_t x, const uint_t y, const uint_t zoom, const uint_t clr, const char ch);
	void drawString(const uint_t x, const uint_t y, const uint_t zoom, const uint_t clr, const char *str);
	void drawEllipse(uint_t xx, uint_t yy, int_t w, int_t h, const uint_t clr);
	void drawLine(uint_t x1, uint_t y1, uint_t x2, uint_t y2, const uint_t clr);

	// Row, Column, Colour
	extern volatile uint_t buff[LED_H][LED_W / 8][2];
};

#endif
