#include <avr/io.h>
#include <avr/interrupt.h>
#include "led.h"

#define DS	_BV(0)
#define STCP	_BV(7)
#define SHCP	_BV(6)

namespace led
{
}

void led::init(void)
{
	PORTD &= ~(SHCP | STCP);
	DDRD |= SHCP | STCP;
	PORTB &= ~DS;
	DDRB |= DS;
}
