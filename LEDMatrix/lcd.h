#ifndef _LCD_H
#define _LCD_H

#define LCD_DDDR	DDRA
#define LCD_DPORT	PORTA
#define LCD_DPIN	PINA

#define LCD_CDDR	DDRC
#define LCD_CPORT	PORTC
#define LCD_CPIN	PINC
#define LCD_RS		_BV(PC4)
#define LCD_RW		_BV(PC5)
#define LCD_E		_BV(PC6)
#define LCD_RST		_BV(PC7)

#include "display.h"

namespace lcd
{
	void init(void);
	void update(void);
}

#endif
