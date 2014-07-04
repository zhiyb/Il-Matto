#ifndef UART_H
#define UART_H

#define UART_DDR	DDRD
#define UART_PORT	PORTD
#define UART_RXD	_BV(PD0)
#define UART_TXD	_BV(PD1)

#include <stdio.h>

namespace uart
{
	void init(void);
	FILE *out(void);
}

#endif
