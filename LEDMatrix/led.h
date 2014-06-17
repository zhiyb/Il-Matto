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
#define LED_RU		_BV(PB4)
#define LED_RD		_BV(PB5)
#define LED_GU		_BV(PB6)
#define LED_GD		_BV(PB7)

#define LED_CDDR	DDRC
#define LED_CPORT	PORTC
#define LED_CPIN	PINC
#define LED_EN		_BV(PC2)
#define LED_STB		_BV(PC3)
#define LED_CLK		_BV(PC4)

#define LED_W	64
#define LED_H	32

#include <inttypes.h>

namespace led
{
	enum Colours {Red, Green};
	enum States {On = true, Off = false};

	void init(void);
	void fill(bool r = Off, bool g = Off);

	// Row, Column, Colour
	extern volatile uint8_t buff[LED_H][LED_W / 8][2];
};

#endif
