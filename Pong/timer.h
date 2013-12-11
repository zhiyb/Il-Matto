#ifndef TIMER_H
#define TIMER_H

#include <stdlib.h>
#include <avr/io.h>

static inline void timer0_init(void)
{
	// For movement
	TCCR0A = _BV(WGM01);			// No output, CTC mode
	TCCR0B = _BV(CS02);			// CTC, clk/256
	OCR0A = 255;				// Compare register A
	TIMSK0 = 0;				// No interrupt
	TIFR0 |= _BV(OCF0A);			// Clear interrupt flag
}

static inline uint8_t timer0(void)
{
	if (TIFR0 & _BV(OCF0A)) {		// Interrupt flag
		TIFR0 |= _BV(OCF0A);		// Clear interrupt flag
		return 1;
	}
	return 0;
}

static inline void timer0_srand(void)
{
	srand(TCNT0);
}

static inline void timer1_init(void)
{
	TCCR1A = 0;				// No output, CTC mode
	TCCR1B = _BV(WGM12);			// CTC, Top: OCR1A
	// Initial: disable timer1
	//TCCR1B |= _BV(CS11) /*| _BV(CS10)*/;	// clk/8 prescaler
	TCCR1C = 0;				// No force output
	TIMSK1 = _BV(OCIE1A);			// Compare A interrupt
}

static inline void timer1_enable(void)
{
	TCCR1B |= _BV(CS11) /*| _BV(CS10)*/;	// clk/8 prescaler
}

static inline void timer1_disable(void)
{
	TCCR1B &= ~_BV(CS11) /*| _BV(CS10)*/;	// clk/8 prescaler
}

static inline void timer2_init(void)
{
	// For debouncing
	TCCR2A = _BV(WGM21);			// No output, CTC mode
	TCCR2B = 0;				// Disabled (clk / 256)
	OCR2A = 100;				// Compare register A
	ASSR = 0;				// Async status register
	TIFR2 |= _BV(OCF2A);			// Clear interrupt flag
	TIMSK2 = _BV(OCIE2A);			// Compare match A interrupt
}

static inline void timer2_disable(void)
{
	TCCR2B = 0;				// Disabled
	TCNT2 = 0;
}

static inline void timer2_enable(void)
{
	TCCR2B = _BV(CS22) | _BV(CS21);		// clk/256
}

#endif
