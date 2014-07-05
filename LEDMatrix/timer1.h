#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

//#define TIMER1_MAX	(F_CPU / 256)		// 1Hz
#define TIMER1_MAX	(F_CPU / 256 / 5)	// 5Hz

namespace timer1
{
	static void init(void);
	static inline void start(void);
	static inline void stop(void);
	static inline bool over(void);
	static inline void clear(void);
}

static inline void timer1::stop(void)
{
	TCCR1B = 0;
	TCNT1 = 0;
	clear();
}

static inline void timer1::start(void)
{
	TCCR1B = _BV(WGM12) | _BV(CS12);
}

static inline bool timer1::over(void)
{
	return TIFR1 & _BV(OCF1A);
}

static inline void timer1::clear(void)
{
	TIFR1 = 0xFF;
}

static void timer1::init(void)
{
	TCCR1A = 0;
	TCCR1C = 0;
	TIMSK1 = 0;
	OCR1A = TIMER1_MAX;
	stop();
}

#endif
