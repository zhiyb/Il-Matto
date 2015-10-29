#include <avr/io.h>
#include "uart0.h"

void uart0_init()
{
	// GPIO initialisation
	DDRD &= ~(_BV(0) | _BV(1));
	PORTD |= _BV(0) | _BV(1);

	// UART 0 initialisation
	#define BAUD	UART0_BAUD
	#include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0A = USE_2X << U2X0;
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

int uart0_read_unblocked()
{
	if (!uart0_available())
		return -1;
	return UDR0;
}

// Read 1 character, blocking
char uart0_read()
{
	while (!uart0_available());
	return UDR0;
}

int uart0_write_unblocked(const char data)
{
	if (!uart0_ready())
		return -1;
	UDR0 = data;
	return data;
}

void uart0_write(const char data)
{
	while (!uart0_ready());
	UDR0 = data;
}

// For fdevopen
static int putch(char ch, FILE *stream)
{
	if (ch == '\n')
		putch('\r', stream);
	uart0_write(ch);
	return ch;
}

// For fdevopen
static int getch(FILE *stream)
{
	return uart0_read();
}

FILE *uart0_fd()
{
	static FILE *dev = NULL;
	if (dev == NULL)
		dev = fdevopen(putch, getch);
	return dev;
}
