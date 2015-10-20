#include <avr/io.h>
#include "uart.h"

void uart_init()
{
	#include <util/setbaud.h>
	DDRD &= ~0x03;
	PORTD |= 0x03;
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0A = USE_2X << U2X0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

static int putch(char ch, FILE *stream)
{
	if (ch == '\n')
		putch('\r', stream);
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = ch;
	return ch;
}

static int getch(FILE *stream)
{
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

FILE *uart_io()
{
	static FILE *dev = NULL;
	if (dev == NULL)
		dev = fdevopen(putch, getch);
	return dev;
}
