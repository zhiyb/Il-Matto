#ifndef CONN_H
#define CONN_H

#define CONN_NULL	0
#define CONN_DETECT	'D'
#define CONN_READY	'R'

#define CONN_LEFT	'l'
#define CONN_RIGHT	'r'
#define CONN_START	's'
#define CONN_REPORT	'-'
#define CONN_GAMEOVER	'g'

#define CONN_WAIT	200

#include <avr/io.h>
#include <stdio.h>

static inline void connect_init(void)
{
	UCSR0A = _BV(U2X0);
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
	UBRR0L = 0;				// Fastest
	UBRR0H = 0;
}

static inline void connect_put(uint8_t c)
{
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = c;
}

static inline uint8_t connect_get(void)
{
	uint8_t w = CONN_WAIT;
	while(!(UCSR0A & _BV(RXC0)) && w--);
	if (UCSR0A & _BV(RXC0))
		return UDR0;
	else
		return CONN_NULL;
}

static inline uint8_t connect_detect(void)
{
	connect_put(CONN_DETECT);
	return connect_get() == CONN_READY;
}

#endif
