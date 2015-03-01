#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

namespace pwm
{
	static inline void init(void);
	static inline void enable(const bool e);
	static inline void set(const uint8_t d) {OCR2A = d;}
}

static inline void pwm::init(void)
{
	DDRD |= _BV(PD7);

	//TCCR2A = _BV(COM2A1) | _BV(WGM20);
	enable(false);
	TCCR2B = 0;
	TCNT2 = 0;
	OCR2A = 0;
	ASSR = 0;
	TIMSK2 = 0;
	TIFR2 = 0xFF;
	TCCR2B = _BV(CS20);
}

static inline void pwm::enable(const bool e)
{
	if (e)
		TCCR2A = _BV(COM2A1) | _BV(WGM20);
	else
		TCCR2A = _BV(WGM20);
}

#endif
