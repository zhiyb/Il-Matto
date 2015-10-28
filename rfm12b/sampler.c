#include <avr/io.h>
#include "sampler.h"

#define SAMPLE_RATE	(8000UL)

void sampler_init()
{
	sampler_enable(0);
	// Setup trigger timer (timer 0)
	TCCR0A = _BV(WGM01);
	TCCR0B = 0;
	// Prescaler of 64
	OCR0A = F_CPU / SAMPLE_RATE / 64;
	TIMSK0 = 0;
	TIFR0 = _BV(OCF0A);

	// Setup ADC
	// AREF, Left-adjust, Channel 0
	ADMUX = _BV(ADLAR);
	// Prescaler of 64
	ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1);
	// Auto trigger on timer 0
	ADCSRB = _BV(ADTS1) | _BV(ADTS0);
	DIDR0 = 0xff;
}

void sampler_enable(const uint8_t e)
{
	// Prescaler of 64
	if (e) {
		TCCR0B = _BV(CS01) | _BV(CS00);
	} else {
		TCCR0B = 0;
		TIFR0 = _BV(OCF0A);
	}
}

uint8_t sampler_available()
{
	return ADCSRA & _BV(ADIF);
}

uint8_t sampler_get()
{
	ADCSRA |= _BV(ADIF);
	return ADCH;
}

uint8_t sampler_tick()
{
	uint8_t tick = TIFR0 & _BV(OCF0A);
	if (tick)
		TIFR0 |= _BV(OCF0A);
	return tick;
}
