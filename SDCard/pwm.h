#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

namespace pwm
{
	static inline void init(void);
	static inline void set(uint8_t l, uint8_t r);
}

static inline void pwm::init(void)
{
	TCCR2A = _BV(COM2A1);			// Clear OC2A on Compare Match
	TCCR2A |= _BV(COM2B1);			// Clear OC2B on Compare Match
	TCCR2A |= _BV(WGM21) | _BV(WGM20);	// PWM, Fast, 0xFF, BOTTOM
	TCCR2B = _BV(CS20);			// clk/1 prescaler
	ASSR = 0;				// I/O clock
	TIMSK2 = 0;				// No interrupts
	TCNT2 = 0x00;				// Counter 1
	OCR2A = 0x00;				// Compare register
	DDRD |= _BV(6) | _BV(7);		// Output
}

static inline void pwm::set(uint8_t l, uint8_t r)
{
	OCR2A = l;				// Duty cycle
	OCR2B = r;				// Duty cycle
}

#endif
