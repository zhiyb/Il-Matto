#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

//#define TIMER1_MAX	(F_CPU / 256)		// 1Hz
#define TIMER1_FREQ5	(F_CPU / 64 / 5)	// 5Hz
//#define TIMER1_FREQ8000	(F_CPU / 8000)		// 8000Hz
#define TIMER1_FREQ8000	(1300)		// 8000Hz

namespace timer1
{
	static inline void init(void);
	//static inline void start(void);
	static inline void stop(void);
	static inline bool over(void);
	static inline void clear(void);
	static inline uint16_t value(void) {return TCNT1;}
	static inline void freq5(void);
	static inline void freq8000(void);
}

static inline void timer1::freq5(void)
{
	OCR1A = TIMER1_FREQ5;
	TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);
}

static inline void timer1::freq8000(void)
{
	OCR1A = TIMER1_FREQ8000;
	TCCR1B = _BV(WGM12) | _BV(CS10);
}

static inline void timer1::stop(void)
{
	TCCR1B = 0;
	TCNT1 = 0;
	clear();
}

/*static inline void timer1::start(void)
{
	TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);
}*/

static inline bool timer1::over(void)
{
	return TIFR1 & _BV(OCF1A);
}

static inline void timer1::clear(void)
{
	TIFR1 |= _BV(OCF1A);
}

static inline void timer1::init(void)
{
	TCCR1A = 0;
	TCCR1C = 0;
	TIMSK1 = 0;
	stop();
}

#endif
