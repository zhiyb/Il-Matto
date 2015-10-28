#include <avr/io.h>
#include "uart.h"

void uart_init()
{
	#include <util/setbaud.h>
	DDRD &= ~(_BV(0) | _BV(1));
	PORTD |= _BV(0) | _BV(1);
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0A = USE_2X << U2X0;
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

int uart_read_unblocked()
{
	if (!uart_rx_available())
		return -1;
	return UDR0;
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
	while (uart_rx_available());
	return UDR0;
}

FILE *uart_io()
{
	static FILE *dev = NULL;
	if (dev == NULL)
		dev = fdevopen(putch, getch);
	return dev;
}
