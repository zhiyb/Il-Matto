/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include "pcint.h"

inline void pcintSet(uint8_t group, uint8_t pins)
{
	uint8_t gp = _BV(group);
	// Disable interrupt
	pcint_disable(group);
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
