// Author: Yubo Zhi (normanzyb@gmail.com)

#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <stdio.h>

void uart_init();
FILE *uart_io();
static inline uint8_t uart_rx_available() {return UCSR0A & (1 << RXC0);}
int uart_read_unblocked();

#ifdef __cplusplus
}
#endif

#endif
