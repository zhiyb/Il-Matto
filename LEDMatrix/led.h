#ifndef _LED_H
#define _LED_H

#define LED_DDDR	DDRA
#define LED_DPORT	PORTA
#define LED_DPIN	PINA
#define LED_L(d)	(d)
#define LED_LMASK	(0x0F)
#define LED_D(d)	(d << 4)
#define LED_DMASK	(0xF0)
#define LED_LA		_BV(PA0)
#define LED_LB		_BV(PA1)
#define LED_LC		_BV(PA2)
#define LED_LD		_BV(PA3)
#define LED_RU		_BV(PA4)
#define LED_RD		_BV(PA5)
#define LED_GU		_BV(PA6)
#define LED_GD		_BV(PA7)

#define LED_CDDR	DDRC
#define LED_CPORT	PORTC
#define LED_CPIN	PINC
#define LED_EN		_BV(PC5)
#define LED_STB		_BV(PC7)
#define LED_CLK		_BV(PC6)

#include "display.h"

namespace led
{
	void init(void);
}

#endif
