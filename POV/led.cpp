#include <avr/io.h>
#include <avr/interrupt.h>
#include "led.h"

#define DS	_BV(0)
#define STCP	_BV(7)
#define SHCP	_BV(6)

#define DS_H	PORTB |= DS
#define DS_L	PORTB &= ~DS
#define STCP_H	PORTD |= STCP
#define STCP_L	PORTD &= ~STCP
#define SHCP_H	PORTD |= SHCP
#define SHCP_L	PORTD &= ~SHCP

void led::init(void)
{
	PORTD &= ~(SHCP | STCP);
	DDRD |= SHCP | STCP;
	PORTB &= ~DS;
	DDRB |= DS;
}

void led::send(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++) {
		if (data & 0x80)
			DS_H;
		else
			DS_L;
		SHCP_H;
		SHCP_L;
		data <<= 1;
	}
	STCP_H;
	STCP_L;
}
