/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef PCINT_H
#define PCINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

// Pin change interrupt
#define pcintEnable(group)	PCICR |= _BV(group)
#define pcintDisable(group)	PCICR &= ~_BV(group)

static inline void pcintSet(uint8_t group, uint8_t pins)
{
	uint8_t gp = _BV(group);
	// Disable interrupt
	pcintDisable(group);
	// Clear interrupt flag
	PCIFR |= gp;
	// Enable interrupt
	PCICR |= gp;
	if (group == 0)
		PCMSK0 = pins;
	else if (group == 1)
		PCMSK1 = pins;
	else if (group == 2)
		PCMSK2 = pins;
	else if (group == 3)
		PCMSK3 = pins;
}

#ifdef __cplusplus
}
#endif

#endif
