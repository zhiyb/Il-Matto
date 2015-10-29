// Author: Yubo Zhi (normanzyb@gmail.com)

#ifndef UART0_H
#define UART0_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <stdio.h>

// Initialise UART 0
void uart0_init();

// Return file descriptor
FILE *uart0_fd();

// If characters available for read
static inline uint8_t uart0_available() {return UCSR0A & _BV(RXC0);}

// If UART tx buffer is available
static inline uint8_t uart0_ready() {return UCSR0A & _BV(UDRE0);}

// Read 1 character, unblocking
// Return -1 for unavailable
int uart0_read_unblocked();

// Read 1 character, blocking
char uart0_read();

// Write 1 character, unblocking
// Return -1 for not ready
int uart0_write_unblocked(const char data);

// Write 1 character, blocking
void uart0_write(const char data);

#ifdef __cplusplus
}
#endif

#endif
