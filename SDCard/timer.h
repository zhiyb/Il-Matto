#ifndef TIMER_H
#define TIMER_H

namespace timer0
{
	static inline void init(void);
	static inline void set(uint16_t freq);
	static inline bool flag(void);
}

static inline void timer0::init(void)
{
	TCCR0A = _BV(WGM01);			// No output, CTC mode (OCRA)
	TCCR0B = _BV(CS01);			// clk/8 prescaler
	TCNT0 = 0;				// Counter 0
	OCR0A = 0;				// Compare register
	TIMSK0 = 0;				// No interrupt for timer 0
	TIFR0 = 0;				// Clean interrupt flags
}

static inline void timer0::set(uint16_t freq)
{
	OCR0A = (F_CPU / 8) / freq - 1;
}

static inline bool timer0::flag(void)
{
	if (TIFR0 & _BV(OCF0A)) {
		TIFR0 = _BV(OCF0A);
		return true;
	}
	return false;
}

#endif
