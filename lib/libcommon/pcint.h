#ifndef PCINT_H
#define PCINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

// Pin change interrupt
void pcint_set(uint8_t group, uint8_t pins);
#define pcint_enable(group)	PCICR |= _BV(group)
#define pcint_disable(group)	PCICR &= ~_BV(group)

#ifdef __cplusplus
}
#endif

#endif
