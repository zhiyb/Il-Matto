#include <avr/io.h>
#include "uart.h"

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

void uart::init(void)
{
	#include <util/setbaud.h>
	UART_DDR &= ~UART_RXD;
	UART_DDR |= UART_TXD;
	UART_PORT |= UART_RXD | UART_TXD;
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0A = USE_2X << U2X0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

FILE *uart::out(void)
{
	static FILE *out = NULL;
	if (out == NULL)
		out = fdevopen(putch, getch);
	return out;
}
